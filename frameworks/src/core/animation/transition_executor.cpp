/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "transition_executor.h"
#include "ace_log.h"
#include "time_util.h"

namespace OHOS {
namespace ACELite {

TransitionExecutor::~TransitionExecutor()
{
    Release();
}

ViewTransition* TransitionExecutor::Execute(TransitionContext& ctx)
{
    Release();
    if (ctx.trigger.sharedElement != nullptr) {
        currentTrans_ = ExecuteSharedElement(ctx);
    } else {
        currentTrans_ = ExecuteDualView(ctx);
    }
    return currentTrans_;
}

void TransitionExecutor::Release()
{
    if (currentTrans_ == nullptr) {
        return;
    }
    // InvalidateTargets cancels the transition (restoring snapshots if it is running)
    // and clears the view references before deleting; the transition never owns the views.
    currentTrans_->InvalidateTargets();
    delete currentTrans_;
    currentTrans_ = nullptr;
}

void TransitionExecutor::Abort()
{
    if (currentTrans_ == nullptr) {
        return;
    }
    // Teardown path: the views may already have been destroyed, so only stop the animator
    // and clear the references; do NOT restore snapshots (that would touch dead views).
    currentTrans_->Abort();
    delete currentTrans_;
    currentTrans_ = nullptr;
}

ViewTransition* TransitionExecutor::ExecuteDualView(TransitionContext& ctx)
{
    ViewTransition* trans = new ViewTransition();
    if (trans == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ExecuteDualView: create ViewTransition failed");
        return nullptr;
    }
    trans->SetOutgoingView(ctx.outgoing);
    trans->SetIncomingView(ctx.trigger.incoming);
    trans->SetType(ctx.style.transitionEffect);
    trans->SetDuration(ctx.style.duration);
    trans->SetDelay(ctx.style.delay);
    if (ctx.style.easing != nullptr) {
        trans->SetEasingFunc(ctx.style.easing);
    }
    trans->Start();
    return trans;
}

ViewTransition* TransitionExecutor::ExecuteSharedElement(TransitionContext& ctx)
{
    ViewTransition* trans = new ViewTransition();
    if (trans == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ExecuteSharedElement: create ViewTransition failed");
        return nullptr;
    }
    const Rect& startRect = ctx.trigger.sharedStartRect;
    const Rect& endRect = ctx.trigger.sharedEndRect;
    // shared-element transition also includes background cross-fade
    trans->SetOutgoingView(ctx.outgoing);
    trans->SetIncomingView(ctx.trigger.incoming);
    trans->SetSharedElement(ctx.trigger.sharedElement);
    trans->SetSharedStartRect(startRect.GetX(), startRect.GetY(), startRect.GetWidth(), startRect.GetHeight());
    trans->SetSharedEndRect(endRect.GetX(), endRect.GetY(), endRect.GetWidth(), endRect.GetHeight());
    trans->SetType(ViewTransition::TRANSITION_SHARED_ELEMENT);
    trans->SetDuration(ctx.style.duration);
    trans->SetDelay(ctx.style.delay);
    if (ctx.style.easing != nullptr) {
        trans->SetEasingFunc(ctx.style.easing);
    }
    trans->Start();
    return trans;
}
} // namespace ACELite
} // namespace OHOS
