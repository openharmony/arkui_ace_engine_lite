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

#include "page_transition.h"

#include "ace_log.h"
#include "ace_mem_base.h"
#include "easing_equation.h"

// Feature engine: compiled only when the page-transition gate is ON. With the
// gate OFF this translation unit contributes nothing (header is empty too).
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
namespace OHOS {
namespace ACELite {
namespace {
// scale interpolation uses integer easing in thousandths, 500 means 0.5, 1000 means 1.0
constexpr uint16_t SCALE_INTERP_MIN = 500;
constexpr uint16_t SCALE_INTERP_MAX = 1000;
#ifdef TDD_ASSERTIONS
// test-only frame counter, see PageTransition::GetAppliedFrameCountForTest()
uint32_t g_appliedFrameCount = 0;
#endif
} // namespace

const char *PageTransitionTypeToString(PageTransitionType type)
{
    switch (type) {
        case PageTransitionType::FADE:
            return "fade";
        case PageTransitionType::SCALE:
            return "scale";
        case PageTransitionType::SLIDE_LEFT:
            return "slide_left";
        case PageTransitionType::SLIDE_RIGHT:
            return "slide_right";
        case PageTransitionType::SLIDE_UP:
            return "slide_up";
        case PageTransitionType::SLIDE_DOWN:
            return "slide_down";
        case PageTransitionType::SLIDE_OVER_LEFT:
            return "slide_over_left";
        case PageTransitionType::SLIDE_OVER_RIGHT:
            return "slide_over_right";
        case PageTransitionType::SLIDE_OVER_UP:
            return "slide_over_up";
        case PageTransitionType::SLIDE_OVER_DOWN:
            return "slide_over_down";
        default:
            return "unknown";
    }
}

void *PageTransition::operator new(size_t size) noexcept
{
    return ace_malloc(size);
}

void PageTransition::operator delete(void *p) noexcept
{
    // null is a no-op, as with the standard delete
    if (p != nullptr) {
        ace_free(p);
    }
}

PageTransition::PageTransition()
    : oldView_(nullptr),
      newView_(nullptr),
      config_(),
      animator_(nullptr),
      completeCallback_(nullptr),
      completeCtx_(nullptr),
      screenW_(0),
      screenH_(0)
{
}

PageTransition::~PageTransition()
{
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
}

bool PageTransition::Start(const PageTransitionConfig &config, UIView *oldView, UIView *newView,
                           CompleteCallback completeCallback, void *completeCtx)
{
    // Start() is one-shot: a second call would overwrite animator_ and leak the previous Animator,
    // which would still be registered with AnimatorManager and keep ticking this object
    if (animator_ != nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "page transition already started, ignore");
        return false;
    }
    oldView_ = oldView;
    newView_ = newView;
    config_ = config;
    completeCallback_ = completeCallback;
    completeCtx_ = completeCtx;
    screenW_ = GetHorizontalResolution();
    screenH_ = GetVerticalResolution();

    // repeat=true + time=0: the animator never auto-stops, the duration is managed inside Callback()
    //
    // Plain new, as everywhere else in this engine: Animator derives from HeapBase, which supplies
    // a class-scope operator new when the graphics memory hooks are enabled, and a class-scope
    // allocation function hides the global overloads - "new (std::nothrow)" would not compile in
    // that configuration. With the hooks on the class-scope operator new already reports failure
    // by returning nullptr, which is what the check below is for; with them off this is the global
    // operator new, and the matching delete is picked by the compiler in both cases.
    animator_ = new Animator(this, newView_, 0, true);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create page transition animator failed");
        return false;
    }
    HILOG_INFO(HILOG_MODULE_ACE,
               "page transition start: type=%{public}s duration=%{public}d",
               PageTransitionTypeToString(config_.type), config_.duration);
    // apply the initial frame immediately so the new page starts from the configured state
    // (off-screen / transparent / scaled) instead of a single opaque frame before the first tick
    ApplyFrame(0);
    animator_->Start();
    return true;
}

void PageTransition::StopAnimator()
{
    if (animator_ == nullptr) {
        return;
    }
    // snap the new page to its final state so an interrupted transition never leaves the page
    // stuck at an intermediate animated position/opacity (jump-to-final on interrupt); the normal
    // completion path lands on the same state, so applying it here is idempotent
    ApplyFrame(config_.duration);
    StopAnimatorOnly();
}

void PageTransition::StopAnimatorOnly()
{
    if (animator_ == nullptr) {
        return;
    }
    // Stop() only unregisters from AnimatorManager and sets STOP - it frees nothing, so this is
    // safe even when reached from inside Callback() while Animator::Run() is on the stack
    animator_->Stop();
}

void PageTransition::Callback(UIView *view)
{
    (void)(view);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "page transition animator is nullptr");
        return;
    }
    uint32_t runTime = animator_->GetRunTime();
    // runs until the full duration elapses (no early finish); a second replace is ignored by the Router
    if (runTime >= config_.duration) {
        HILOG_INFO(HILOG_MODULE_ACE,
                   "page transition [%{public}s] finished normally, duration=%{public}d",
                   PageTransitionTypeToString(config_.type), config_.duration);
        // CubicEaseInOut does not clamp curTime, so the duration is applied exactly to reach the
        // final state: the new page is fully opaque / in-place / scale 1.0. StopAnimator() is free
        // of any deallocation, which is what makes it safe inside this animator frame.
        StopAnimator();
        // hand ownership to the Router. It must not touch this object after the callback returns;
        // it only records that the transition ended and schedules the release.
        if (completeCallback_ != nullptr) {
            completeCallback_(completeCtx_);
        }
        return;
    }
    ApplyFrame(static_cast<uint16_t>(runTime));
}

void PageTransition::ApplyFrame(uint16_t elapsed)
{
#ifdef TDD_ASSERTIONS
    g_appliedFrameCount++;
#endif
    if ((newView_ == nullptr) || (oldView_ == nullptr)) {
        return;
    }
    uint16_t duration = config_.duration;
    if (duration == 0) {
        return;
    }
    // normalized easing progress in [0, 1]: CubicEaseInOut over a 0~1000 integer scale
    float progress = static_cast<float>(
        EasingEquation::CubicEaseInOut(0, 1000, elapsed, duration)) / 1000.0f;
    switch (config_.type) {
        case PageTransitionType::FADE:
            ApplyFade(progress);
            break;
        case PageTransitionType::SCALE:
            ApplyScale(progress);
            break;
        case PageTransitionType::SLIDE_LEFT:
        case PageTransitionType::SLIDE_RIGHT:
        case PageTransitionType::SLIDE_UP:
        case PageTransitionType::SLIDE_DOWN:
            ApplySlide(progress);
            break;
        case PageTransitionType::SLIDE_OVER_LEFT:
        case PageTransitionType::SLIDE_OVER_RIGHT:
        case PageTransitionType::SLIDE_OVER_UP:
        case PageTransitionType::SLIDE_OVER_DOWN:
            ApplySlideOver(progress);
            break;
        default:
            break;
    }
}

void PageTransition::ApplyFade(float progress)
{
    uint8_t newOpa = static_cast<uint8_t>(OPA_OPAQUE * progress);
    uint8_t oldOpa = static_cast<uint8_t>(OPA_OPAQUE * (1.0f - progress));
    newView_->SetOpaScale(newOpa);
    oldView_->SetOpaScale(oldOpa);
    // SetOpaScale does not invalidate the view, mark it dirty explicitly
    newView_->Invalidate();
    oldView_->Invalidate();
}

void PageTransition::ApplyScale(float progress)
{
    Vector2<float> pivot{screenW_ / 2.0f, screenH_ / 2.0f};
    // the new page scales up 0.5 -> 1.0 while the old page scales down 1.0 -> 0.5
    float newScale = (SCALE_INTERP_MIN + (SCALE_INTERP_MAX - SCALE_INTERP_MIN) * progress) /
                     static_cast<float>(SCALE_INTERP_MAX);
    float oldScale = (SCALE_INTERP_MAX - (SCALE_INTERP_MAX - SCALE_INTERP_MIN) * progress) /
                     static_cast<float>(SCALE_INTERP_MAX);
    newView_->Scale(Vector2<float>(newScale, newScale), pivot);
    oldView_->Scale(Vector2<float>(oldScale, oldScale), pivot);
    newView_->SetOpaScale(OPA_OPAQUE);
    oldView_->SetOpaScale(OPA_OPAQUE);
}

void PageTransition::ApplySlide(float progress)
{
    int16_t newOffsetX = 0;
    int16_t newOffsetY = 0;
    int16_t oldOffsetX = 0;
    int16_t oldOffsetY = 0;
    switch (config_.type) {
        case PageTransitionType::SLIDE_LEFT:
            newOffsetX = static_cast<int16_t>(-screenW_ * (1.0f - progress));
            oldOffsetX = static_cast<int16_t>(screenW_ * progress);
            break;
        case PageTransitionType::SLIDE_RIGHT:
            newOffsetX = static_cast<int16_t>(screenW_ * (1.0f - progress));
            oldOffsetX = static_cast<int16_t>(-screenW_ * progress);
            break;
        case PageTransitionType::SLIDE_UP:
            newOffsetY = static_cast<int16_t>(-screenH_ * (1.0f - progress));
            oldOffsetY = static_cast<int16_t>(screenH_ * progress);
            break;
        case PageTransitionType::SLIDE_DOWN:
            newOffsetY = static_cast<int16_t>(screenH_ * (1.0f - progress));
            oldOffsetY = static_cast<int16_t>(-screenH_ * progress);
            break;
        default:
            break;
    }
    newView_->Translate(Vector2<int16_t>(newOffsetX, newOffsetY));
    oldView_->Translate(Vector2<int16_t>(oldOffsetX, oldOffsetY));
}

void PageTransition::ApplySlideOver(float progress)
{
    int16_t offsetX = 0;
    int16_t offsetY = 0;
    switch (config_.type) {
        case PageTransitionType::SLIDE_OVER_LEFT:
            offsetX = static_cast<int16_t>(-screenW_ * (1.0f - progress));
            break;
        case PageTransitionType::SLIDE_OVER_RIGHT:
            offsetX = static_cast<int16_t>(screenW_ * (1.0f - progress));
            break;
        case PageTransitionType::SLIDE_OVER_UP:
            offsetY = static_cast<int16_t>(-screenH_ * (1.0f - progress));
            break;
        case PageTransitionType::SLIDE_OVER_DOWN:
            offsetY = static_cast<int16_t>(screenH_ * (1.0f - progress));
            break;
        default:
            break;
    }
    // overlay slide: only the new page slides in, the old page stays put
    newView_->Translate(Vector2<int16_t>(offsetX, offsetY));
}

#ifdef TDD_ASSERTIONS
// TDD-only hooks. TDD_ASSERTIONS is set only by unit-test builds (test/ace_test_config.gni),
// so these implementations are compiled out of product binaries. See page_transition.h.
void PageTransition::InitForTest(const PageTransitionConfig &config, UIView *oldView, UIView *newView)
{
    config_ = config;
    oldView_ = oldView;
    newView_ = newView;
}

void PageTransition::ApplyFrameForTest(uint16_t elapsed)
{
    ApplyFrame(elapsed);
}

void PageTransition::CallbackForTest(UIView *view)
{
    Callback(view);
}

uint32_t PageTransition::GetAppliedFrameCountForTest()
{
    return g_appliedFrameCount;
}
#endif
} // namespace ACELite
} // namespace OHOS
#endif // ENABLE_PAGE_TRANSITION_EFFECT
