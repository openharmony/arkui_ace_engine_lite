/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACELITE_JS_ROUTER_H
#define OHOS_ACELITE_JS_ROUTER_H

#include "js_page_state_machine.h"
#include "non_copyable.h"
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include "page_transition.h"
#endif

namespace OHOS {
namespace ACELite {
class Router final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(Router);
#ifndef ENABLE_PAGE_TRANSITION_EFFECT
    Router() : currentSm_(nullptr), newSm_(nullptr), hidden_(false), taskID_(DISPATCH_FAILURE) {}
#else
    // transitionConfig_ is not listed: PageTransitionConfig default-constructs itself
    Router() : currentSm_(nullptr), newSm_(nullptr), oldSm_(nullptr), transition_(nullptr),
        pendingRelease_(nullptr), hidden_(false), taskID_(DISPATCH_FAILURE) {}
#endif

    ~Router()
    {
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
        // stop the running transition first, otherwise AnimatorManager keeps driving freed views
        FinishTransition();
        ReleasePendingTransition();
#endif
        if (currentSm_ != nullptr) {
            delete currentSm_;
        }
    }
    jerry_value_t Replace(jerry_value_t object, bool async = true);
    void ReplaceSync();
    void Show();
    void Hide();
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    /**
     * @brief Set the transition configuration parsed from JS animation parameter.
     *        Must be called before Replace() and is consumed by ReplaceSync().
     */
    void SetTransitionConfig(const PageTransitionConfig &config);
    /**
     * @brief Finish the running page transition: stop the animator and release the old page.
     *        Idempotent, can be triggered by normal completion, interruption and app background.
     */
    void FinishTransition();
#ifdef TDD_ASSERTIONS
    /**
     * @brief Test-only: run the running transition to its normal end, exactly like
     *        PageTransition::Callback() does once the configured duration has elapsed. Production
     *        code reaches that point from the animator, which a unit test can not tick.
     */
    void CompleteTransitionForTest();
#endif // TDD_ASSERTIONS
#endif

private:
#ifndef ENABLE_PAGE_TRANSITION_EFFECT
    StateMachine *currentSm_; // current state machine for current shown page
    StateMachine *newSm_;     // current state machine for target showing page
    bool hidden_;             // the flag representing whether the whole app is moved to background
    uint16_t taskID_;
#else
    /**
     * @brief Completion callback invoked by PageTransition when the animation finishes.
     *        Runs inside the animator's callback, so it must not delete the PageTransition:
     *        it unregisters it, schedules its release on the main task queue and releases
     *        the old page.
     */
    static void OnTransitionCompleted(void *ctx);
    // start a page transition between the old page (kept in oldSm_) and the newly rendered
    // currentSm_ page; return false when it cannot be set up, the caller then releases the old
    // page and shows the new one without animation
    bool StartPageTransition(const PageTransitionConfig &config);
    // bring the freshly rendered page to SHOW or BACKGROUND according to the foreground state,
    // used when no page transition is running
    void ShowOrBackgroundCurrentPage();
    // release a transition that OnTransitionCompleted could not hand to the task queue. Must only
    // be called from a context outside an animator frame (FinishTransition() itself is also reached
    // from inside PageTransition::Callback(), so it is not a valid drain point).
    void ReleasePendingTransition();
    // run the synchronous page switch for ReplaceSync(): consume the pending transition
    // configuration, swap in the newly rendered page, then either animate the old page out or
    // show the new one directly
    void ReplaceSyncWithTransition();
    StateMachine *currentSm_;      // current state machine for current shown page
    StateMachine *newSm_;          // current state machine for target showing page
    StateMachine *oldSm_;          // old page kept alive during the transition, released on FinishTransition
    PageTransition *transition_;   // the running page transition, used for interruption
    // a transition whose deferred release could not be queued (Dispatch failure); released at the
    // next point that is guaranteed to be outside an animator frame, see ReleasePendingTransition()
    PageTransition *pendingRelease_;
    PageTransitionConfig transitionConfig_; // transition configuration of the next ReplaceSync
    bool hidden_;                  // the flag representing whether the whole app is moved to background
    uint16_t taskID_;
#endif
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_JS_ROUTER_H
