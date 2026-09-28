/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "js_router.h"

#include "ace_log.h"
#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
#include "dft_impl.h"
#endif
#include "async_task_manager.h"
#include "js_page_state_machine.h"
#include "js_profiler.h"
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include "page_transition.h"
#endif

namespace OHOS {
namespace ACELite {
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
static void ReleaseTransitionAsync(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "release transition failed with null input parameter");
        return;
    }
    auto transition = static_cast<PageTransition *>(data);
    delete transition;
    HILOG_DEBUG(HILOG_MODULE_ACE, "page transition released");
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT

static void ReplaceAsync(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "replace async failed with null input parameter");
        return;
    }
    // void* can not be dynamically_casted from
    auto router = static_cast<Router *>(data);
    router->ReplaceSync();
    OUTPUT_TRACE();
}

jerry_value_t Router::Replace(jerry_value_t object, bool async)
{
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    // While a transition is still running (old page kept alive or animator active, including the
    // synchronous new-page render phase), ignore this request so the running transition is not
    // interrupted; animation-less replaces (e.g. a back navigation) are ignored as well.
    if ((oldSm_ != nullptr) || (transition_ != nullptr)) {
        HILOG_INFO(HILOG_MODULE_ACE,
                   "router.replace: page transition is running, ignore this second-trigger request");
        // drop the config written by RouterModule::Replace for this request, otherwise the next
        // animation-less replace (e.g. a back navigation) would pick it up as a stale transition
        transitionConfig_ = PageTransitionConfig();
        return UNDEFINED;
    }
#endif

    if (newSm_ != nullptr) {
        if (taskID_ != DISPATCH_FAILURE) {
            AsyncTaskManager::GetInstance().Cancel(taskID_);
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "router is replacing, can not handle the new request");
            return UNDEFINED;
        }
    }

    jerry_value_t jsRes = jerry_create_undefined();
    if (newSm_ == nullptr) {
        StateMachine *newSm = new StateMachine();
        if (newSm == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc state machine memory heap failed.");
            return UNDEFINED;
        }

        // init new state machine
        bool res = newSm->Init(object, jsRes);
        if (!res) {
            delete newSm;
            return jsRes;
        }
        newSm_ = newSm;
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "consume asynchronous tasks in the task queue first.async:%d", async);
    }

    // dispatch the new page rendering to the async handling as the current context of
    // router.replace need to be released, which need to return out from the scope
    if (async) {
        taskID_ = AsyncTaskManager::GetInstance().Dispatch(ReplaceAsync, this);
        if (DISPATCH_FAILURE == taskID_) {
            // request replacing failed, no chance to do it, release the new state machine
            HILOG_ERROR(HILOG_MODULE_ACE, "dispatch replacing request failed");
            delete newSm_;
            newSm_ = nullptr;
        }
    } else {
        // for the first startup of application, no need to do the async replace
        ReplaceSync();
    }
    return jsRes;
}

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
bool Router::StartPageTransition(const PageTransitionConfig &config)
{
    if ((oldSm_ == nullptr) || (currentSm_ == nullptr)) {
        return false;
    }
    UIView *oldView = oldSm_->GetPageRootView();
    UIView *newView = currentSm_->GetPageRootView();
    if ((oldView == nullptr) || (newView == nullptr)) {
        return false;
    }
    PageTransition *transition = new PageTransition();
    if (transition == nullptr) {
        return false;
    }
    transition_ = transition;
    if (!transition->Start(config, oldView, newView, Router::OnTransitionCompleted, this)) {
        transition_ = nullptr;
        delete transition;
        return false;
    }
    return true;
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
void Router::ShowOrBackgroundCurrentPage()
{
    if (hidden_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "the whole application is in background, move to HIDE state directly");
        currentSm_->ChangeState(BACKGROUND_STATE);
    } else {
        // the page finished INIT/READY rendering, so make it visible now
        currentSm_->ChangeState(SHOW_STATE);
    }
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
void Router::ReplaceSyncWithTransition()
{
    // the transition configuration is only valid for this switch, consume it once
    PageTransitionConfig config = transitionConfig_;
    transitionConfig_ = PageTransitionConfig();
    // animate only when the old page is actually visible and there is something to animate from;
    // a background app or a missing old page keeps the original no-animation behavior
    bool withTransition = config.IsValid() && !hidden_ && (currentSm_ != nullptr) && (oldSm_ == nullptr);

    if (currentSm_ != nullptr) {
        if (withTransition) {
            // keep the old page (and its mounted view) alive until the animation ends
            oldSm_ = currentSm_;
            // The old page is no longer the page the process-wide bindings belong to, but it is
            // still alive: hand them over before the new page's onInit can register its own, the
            // same order the plain replace path gets by releasing the old page first.
            oldSm_->ReleaseProcessWideBindings();
        } else {
            delete currentSm_;
        }
        currentSm_ = nullptr;
    }
    // the new state machine becomes the current one.
    currentSm_ = newSm_;
    newSm_ = nullptr;
    currentSm_->SetHiddenFlag(hidden_);
    // run the state machine to the init state (eval + render, ends in READY_STATE; the page is
    // not mounted to the RootView yet)
    currentSm_->ChangeState(INIT_STATE);

    if (withTransition && StartPageTransition(config)) {
        // Start() applied the initial frame synchronously; mounting the new page in the same
        // synchronous block shows it from its initial state, no opaque flash
        currentSm_->ChangeState(SHOW_STATE);
        return;
    }
    if (withTransition) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "router.replace: page transition setup failed (type=%{public}s), fall back to no animation",
                    PageTransitionTypeToString(config.type));
        // release the old page kept alive for the transition, then show normally
        FinishTransition();
    }
    ShowOrBackgroundCurrentPage();
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT

void Router::ReplaceSync()
{
    taskID_ = DISPATCH_FAILURE;
    if (newSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "replace sync failed, new sm should be prepared");
        return;
    }
    START_TRACING(ROUTER_REPLACE);
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    ReplaceSyncWithTransition();
#else
    // new state machine run successfully to show new page, then need to release old state machine.
    if (currentSm_ != nullptr) {
        delete currentSm_;
        currentSm_ = nullptr;
    }
    // new state machine should to be current.
    currentSm_ = newSm_;
    newSm_ = nullptr;
    currentSm_->SetHiddenFlag(hidden_);
    // run state machine and start to jump to init state.
    currentSm_->ChangeState(INIT_STATE);
    if (hidden_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "the whole application is in background, move to HIDE state directly");
        // the whole app is in background, move to HIDE state immediately
        currentSm_->ChangeState(BACKGROUND_STATE);
    } else {
        // above call will move sm into ready state, than let the page show
        currentSm_->ChangeState(SHOW_STATE);
    }
#endif
#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
    DftImpl::GetInstance()->CallbackPageReplaced(currentSm_->GetCurrentState());
#endif
    STOP_TRACING();
}

void Router::Show()
{
    if (currentSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no SM when performing show");
        return;
    }

    hidden_ = false;
    currentSm_->SetHiddenFlag(hidden_);
    currentSm_->ChangeState(SHOW_STATE);
}

void Router::Hide()
{
    if (currentSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no SM when performing background");
        return;
    }

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    // if a page transition is still running when the app goes to background, finish it first
    // so the old page is released and no view is left on the RootView
    if ((oldSm_ != nullptr) || (transition_ != nullptr)) {
        FinishTransition();
    }
    ReleasePendingTransition();
#endif
    currentSm_->ChangeState(BACKGROUND_STATE);
    hidden_ = true;
    currentSm_->SetHiddenFlag(hidden_);
}

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
void Router::OnTransitionCompleted(void *ctx)
{
    if (ctx == nullptr) {
        return;
    }
    Router *router = static_cast<Router *>(ctx);
    // the transition has already stopped its animator; take ownership of it so the object outlives
    // the animator frame it is currently inside
    PageTransition *finished = router->transition_;
    router->transition_ = nullptr;
    if (finished != nullptr) {
        // this runs inside PageTransition::Callback() <- Animator::Run() <- AnimatorManager's list
        // walk, so deleting here would leave the manager iterating over freed storage. Defer the
        // release to the main task queue, which runs after that frame has returned.
        if (DISPATCH_FAILURE == AsyncTaskManager::GetInstance().Dispatch(ReleaseTransitionAsync, finished)) {
            // the queue is unusable (engine already fatal, or the task allocation failed), so the
            // object cannot be handed over. Keep it and release it at the next non-animator point
            // instead of leaking it here.
            HILOG_ERROR(HILOG_MODULE_ACE, "dispatch transition release failed, deferring to Router teardown");
            if (router->pendingRelease_ != nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "a transition is already pending release, previous one leaks");
            }
            router->pendingRelease_ = finished;
        }
    }
    router->FinishTransition();
}

#ifdef TDD_ASSERTIONS
void Router::CompleteTransitionForTest()
{
    if (transition_ != nullptr) {
        // the animator has run to the end of the duration, which is what Callback() detects
        transition_->StopAnimator();
    }
    OnTransitionCompleted(this);
}
#endif // TDD_ASSERTIONS

void Router::ReleasePendingTransition()
{
    if (pendingRelease_ == nullptr) {
        return;
    }
    PageTransition *orphan = pendingRelease_;
    pendingRelease_ = nullptr;
    // The animator was already stopped when the transition completed, and StopAnimatorOnly() is
    // idempotent, so the invariant is kept without touching anything. StopAnimator() must not be
    // used here: it writes the final frame, and the views of this transition are already released.
    orphan->StopAnimatorOnly();
    delete orphan;
}

void Router::FinishTransition()
{
    if ((transition_ == nullptr) && (oldSm_ == nullptr)) {
        // nothing in progress, idempotent
        return;
    }
    // stop and destroy the running transition first (interrupt path). The normal completion path
    // has already unregistered the transition (transition_ == nullptr) before calling this.
    if (transition_ != nullptr) {
        PageTransition *transitionToDestroy = transition_;
        transition_ = nullptr;
        // interrupt: snap to the final state, then release. Deleting synchronously is safe here -
        // this path is Hide()/~Router(), never an animator frame.
        transitionToDestroy->StopAnimator();
        delete transitionToDestroy;
    }
    if (oldSm_ == nullptr) {
        return;
    }
    StateMachine *old = oldSm_;
    oldSm_ = nullptr;
    // the new page is already live and owns the process-wide bindings
    old->SetReleasedAfterTransition(true);
    // StateMachine destructor releases the page resources and detaches its root view from the
    // RootView (ReleaseHistoryPageResource -> DetachFromRootView), no manual Remove needed.
    delete old;
}

void Router::SetTransitionConfig(const PageTransitionConfig &config)
{
    transitionConfig_ = config;
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT
} // namespace ACELite
} // namespace OHOS
