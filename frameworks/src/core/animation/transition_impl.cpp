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

#include "transition_impl.h"
#include <stdlib.h>
#include "ace_log.h"
#include "ace_mem_base.h"
#include "easing_equation.h"
#include "root_view.h"
#include "securec.h"
#if FEATURE_PATH_ANIMATOR
#include <cmath>
#include "path_animator_callback.h"
#endif // FEATURE_PATH_ANIMATOR

namespace OHOS {
namespace ACELite {
void TransitionImpl::Init()
{
    if (animator_ != nullptr) {
        return;
    }
    animator_ = new Animator(this, view_, 0, true);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animator create failed");
        return;
    }
}

void TransitionImpl::Callback(UIView *view)
{
    (void)(view);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animator is nullptr");
        return;
    }
    int32_t elapsedTime = animator_->GetRunTime() - params_.delay; // animation execution time
    if (elapsedTime <= 0) {
        return;
    }
    if (elapsedTime >= params_.during) {
        timeArrivaled_ = true;
    }

    Perform(elapsedTime);
}

void TransitionImpl::Start()
{
    if (animator_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "animator not initial or has been started");
        return;
    }
    uint8_t state = animator_->GetState();
    if (((state == Animator::STOP) || (state == Animator::PAUSE)) && (params_.during > 0)) {
        RecordViewStatus();
        InitTransitionParams();
        animator_->Start();
    }
}

void TransitionImpl::Stop() const
{
    if (animator_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "animator not initial or is not running");
        return;
    }
    uint8_t state = animator_->GetState();
    if (state == Animator::START || state == Animator::RUNNING) {
        animator_->Stop();
    }
}

void TransitionImpl::InitTransitionParams()
{
    InitTransitionParamsStyle();
    InitTransitionParamsTransform();
    InitTransitionParamsEasing();
}

void TransitionImpl::InitTransitionParamsStyle()
{
    params_.delay = (params_.delay <= 0) ? 0 : params_.delay;
    params_.during = (params_.during <= 0) ? 0 : params_.during;
    oriIteration_ = params_.iterations;

    if (params_.fill == OptionsFill::FORWARDS) {
        fill_ = OptionsFill::FORWARDS;
    }

    if (params_.height_from >= 0 && params_.height_to >= 0) {
        isTransitionSet_[GeneralType::IS_HEIGHT_TRANSITION_SET] = true;
        heightSrc_ = params_.height_from;
    }
    if (params_.width_from >= 0 && params_.width_to >= 0) {
        isTransitionSet_[GeneralType::IS_WIDTH_TRANSITION_SET] = true;
        widthSrc_ = params_.width_from;
    }
    if (params_.opacity_from >= 0 && params_.opacity_to >= 0) {
        isTransitionSet_[GeneralType::IS_OPACITY_TRANSITION_SET] = true;
        opacitySrc_ = (params_.opacity_from <= OPA_OPAQUE) ? params_.opacity_from : OPA_OPAQUE;
        params_.opacity_to = (params_.opacity_to <= OPA_OPAQUE) ? params_.opacity_to : OPA_OPAQUE;
    }

    const uint32_t rgbValueMax = 0xFFFFFF;
    if ((params_.background_color_from <= rgbValueMax) && (params_.background_color_to <= rgbValueMax)) {
        isTransitionSet_[GeneralType::IS_BACKGROUND_COLOR_TRANSITION_SET] = true;
        GetRGB(params_.background_color_from, rSrc_, gSrc_, bSrc_);
        GetRGB(params_.background_color_to, rTo_, gTo_, bTo_);
        steps_ = params_.during / INTERVAL;
    }
}

void TransitionImpl::InitTransitionParamsTransform()
{
    if (params_.transformType == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "transformType not set");
        return;
    }
    if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_X)) {
        transformType_ = TransformType::TRANSLATE_X;
        xSrc_ = viewStatus_.x + params_.transform_from;
    } else if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_Y)) {
        transformType_ = TransformType::TRANSLATE_Y;
        ySrc_ = viewStatus_.y + params_.transform_from;
    } else if (!strcmp(params_.transformType, TRANSITION_ROTATE)) {
        transformType_ = TransformType::ROTATE;
        rotateSrc_ = params_.transform_from;
    }
#if FEATURE_PATH_ANIMATOR
    if ((params_.transformType != nullptr) && (transformType_ == TransformType::NONE) &&
        !strcmp(params_.transformType, TRANSITION_OFFSET_PATH)) {
        transformType_ = TransformType::TRANSLATE_OFFSET_PATH;
    }
#endif // FEATURE_PATH_ANIMATOR
}

void TransitionImpl::InitTransitionParamsEasing()
{
    switch (params_.easing) {
        case EasingType::EASE_IN:
            easingType_[EasingType::EASE_IN] = true;
            break;
        case EasingType::EASE_OUT:
            easingType_[EasingType::EASE_OUT] = true;
            break;
        case EasingType::EASE_IN_OUT:
            easingType_[EasingType::EASE_IN_OUT] = true;
            break;
        default:
            easingType_[EasingType::LINEAR] = true;
            break;
    }
}

void TransitionImpl::GetRGB(const uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) const
{
    r = uint8_t((color & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
    g = uint8_t((color & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
    b = uint8_t((color & TEXT_BLUE_COLOR_MASK));
}

int16_t TransitionImpl::GetNextFrameValue(int16_t from, int16_t to, int32_t elapsedTime) const
{
    int16_t nextPoint;
    if (easingType_[EasingType::EASE_IN]) {
        nextPoint = EasingEquation::CubicEaseIn(from, to, elapsedTime, params_.during);
    } else if (easingType_[EasingType::EASE_OUT]) {
        nextPoint = EasingEquation::CubicEaseOut(from, to, elapsedTime, params_.during);
    } else if (easingType_[EasingType::EASE_IN_OUT]) {
        nextPoint = EasingEquation::CubicEaseInOut(from, to, elapsedTime, params_.during);
    } else {
        nextPoint = EasingEquation::LinearEaseNone(from, to, elapsedTime, params_.during);
    }

    return nextPoint;
}

void TransitionImpl::SetTransformSrcPosition()
{
    if (params_.transformType == nullptr) {
        return;
    }
    if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_X)) {
        view_->SetPosition(xSrc_, viewStatus_.y);
        view_->GetParent()->Invalidate();
    } else if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_Y)) {
        view_->SetPosition(viewStatus_.x, ySrc_);
        view_->GetParent()->Invalidate();
    } else if (!strcmp(params_.transformType, TRANSITION_ROTATE)) {
        RotateAroundCenterPoint(rotateSrc_);
    } else {
        // do nothing
    }
}

void TransitionImpl::RotateAroundCenterPoint(int16_t angle)
{
    TransformMap transMap(view_->GetOrigRect());
    const int circleRate = 360;
    angle = angle % circleRate;
    float halfVal = 2.0f;
    pivot_.x_ = (view_->GetWidth() - 1) / halfVal;
    pivot_.y_ = (view_->GetHeight() - 1) / halfVal;
    transMap.Rotate((angle), pivot_);
    view_->SetTransformMap(transMap);
}

void TransitionImpl::Perform(int32_t elapsedTime)
{
    if (timeArrivaled_) {
        if (!RepeatAnimator()) {
            this->Stop();
        }
    }

    if (!isTransformSrcSet_) {
        SetTransformSrcPosition();
        isTransformSrcSet_ = true;
    }

    if (transformType_ == TransformType::TRANSLATE_X) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TTRANSLATE_X, xSrc_,
                          elapsedTime);
    } else if (transformType_ == TransformType::TRANSLATE_Y) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TTRANSLATE_Y, ySrc_,
                          elapsedTime);
    } else if (transformType_ == TransformType::ROTATE) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TROTATE, rotateSrc_,
                          elapsedTime);
#if FEATURE_PATH_ANIMATOR
    } else if (transformType_ == TransformType::TRANSLATE_OFFSET_PATH) {
        PerformOffsetPathTransition(elapsedTime);
#endif // FEATURE_PATH_ANIMATOR
    }

    if (isTransitionSet_[GeneralType::IS_HEIGHT_TRANSITION_SET]) {
        PerformTransition(params_.height_from, params_.height_to, TransitionType::HEIGHT, heightSrc_, elapsedTime);
    }
    if (isTransitionSet_[GeneralType::IS_WIDTH_TRANSITION_SET]) {
        PerformTransition(params_.width_from, params_.width_to, TransitionType::WIDTH, widthSrc_, elapsedTime);
    }

    if (isTransitionSet_[GeneralType::IS_OPACITY_TRANSITION_SET]) {
        PerformTransition(params_.opacity_from, params_.opacity_to, TransitionType::OPACITY, opacitySrc_, elapsedTime);
    }

    if (isTransitionSet_[GeneralType::IS_BACKGROUND_COLOR_TRANSITION_SET]) {
        PerformTransitionBgColorLinear(elapsedTime);
    }

    if ((fill_ == OptionsFill::FNONE) && timeArrivaled_) {
        RecoveryViewStatus(view_->GetRect());
    }

    if (timeArrivaled_) {
        ResetRepeatParam();
    }
}

void TransitionImpl::PerformTransitionBgColorLinear(int32_t elapsedTime)
{
    if (timeArrivaled_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "time arrived");
        view_->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(rTo_, gTo_, bTo_).full);
        return;
    }
    if ((steps_ != 0) && (count_ <= steps_) && (elapsedTime <= params_.during)) {
        if ((elapsedTime - bgcolorTimeSrc_) > INTERVAL) {
            ColorType color = Color::GetColorFromRGB(rSrc_ + (rTo_ - rSrc_) * count_ / steps_,
                                                     gSrc_ + (gTo_ - gSrc_) * count_ / steps_,
                                                     bSrc_ + (bTo_ - bSrc_) * count_ / steps_);
            view_->SetStyle(STYLE_BACKGROUND_COLOR, color.full);
            view_->Invalidate();
            bgcolorTimeSrc_ = elapsedTime;
            count_++;
        }
    }
}

void TransitionImpl::PerformTransition(int16_t from,
                                       int16_t to,
                                       TransitionType transitionType,
                                       int16_t &updateAttrValue,
                                       int32_t elapsedTime)
{
    if (timeArrivaled_) {
        updateAttrValue = to;
    } else {
        int16_t prefetchedValue = 0;
        prefetchedValue = GetNextFrameValue(from, to, elapsedTime);
        int16_t diffDistance = prefetchedValue - updateAttrValue;
        if (((diffDistance < 1) && (diffDistance > -1)) || (elapsedTime > params_.during)) {
            return;
        }
        updateAttrValue = static_cast<int16_t>(prefetchedValue);
    }

    Rect invalidatedArea = view_->GetRect();
    switch (transitionType) {
        case TransitionType::TTRANSLATE_X:
            view_->SetPosition((updateAttrValue + viewStatus_.x), viewStatus_.y);
            break;
        case TransitionType::TTRANSLATE_Y:
            view_->SetPosition(viewStatus_.x, (updateAttrValue + viewStatus_.y));
            break;
        case TransitionType::TROTATE: {
            RotateAroundCenterPoint(updateAttrValue);
            break;
        }
        case TransitionType::HEIGHT:
            view_->SetHeight(updateAttrValue);
            break;
        case TransitionType::WIDTH:
            view_->SetWidth(updateAttrValue);
            break;
        case TransitionType::OPACITY: {
            double rate = (double)updateAttrValue / ALPHA_MAX;
            view_->SetStyle(STYLE_BACKGROUND_OPA, static_cast<int64_t>(viewStatus_.rectOpacity * rate));
            view_->SetStyle(STYLE_IMAGE_OPA, static_cast<int64_t>(viewStatus_.imageOpacity * rate));
            view_->SetStyle(STYLE_LINE_OPA, static_cast<int64_t>(viewStatus_.lineOpacity * rate));
            view_->SetStyle(STYLE_TEXT_OPA, static_cast<int64_t>(viewStatus_.textOpacity * rate));
            break;
        }
        default:
            HILOG_INFO(HILOG_MODULE_ACE, "animation nothing to do.");
            break;
    }

    invalidatedArea.Join(invalidatedArea, view_->GetRect());
    view_->InvalidateRect(invalidatedArea);
}

#if FEATURE_PATH_ANIMATOR
namespace {
constexpr int16_t PERCENT_TO_PERMILLE = 10;     // percent -> permille multiplier (100% == 1000‰)
constexpr int16_t PROGRESS_PERMILLE_MAX = 1000; // permille progress upper limit (100.0%)
} // namespace

void TransitionImpl::PerformOffsetPathTransition(int32_t elapsedTime)
{
    if (!SyncOffsetPathBase()) {
        return;
    }
    ConfigurePathCallbackIfNeeded();
    if (pathCallback_ == nullptr) {
        return; // allocation failed: skip the path frame, other effects unaffected
    }
    /* delegate the view-effect application (locate/SetPosition/rotate/invalidate) to
       PathAnimatorCallback — TransitionImpl only keeps the CSS progress semantics */
    pathCallback_->ApplyFrame(view_, static_cast<uint16_t>(EvaluateOffsetDist(elapsedTime)));
}

bool TransitionImpl::SyncOffsetPathBase()
{
    if (params_.pathPoly.count < POINTS_MIN) {
        return false;
    }
    /* for "if" mounting, Start runs before layout finished and viewStatus_ recorded in
       Start() is (0,0,0,0): skip the 1st frame to wait for layout, then sync the real
       layout position & size at the 2nd frame as the path base */
    if (offsetPathBaseSyncState_ == OffsetPathSyncState::WAIT_FIRST_FRAME) {
        offsetPathBaseSyncState_ = OffsetPathSyncState::WAIT_LAYOUT;
        return false;
    }
    if (offsetPathBaseSyncState_ == OffsetPathSyncState::WAIT_LAYOUT) {
        offsetPathBaseSyncState_ = OffsetPathSyncState::SYNCED;
        viewStatus_.x = view_->GetX();
        viewStatus_.y = view_->GetY();
        viewStatus_.height = view_->GetHeight();
        viewStatus_.width = view_->GetWidth();
    }
    return true;
}

int16_t TransitionImpl::EvaluateOffsetDist(int32_t elapsedTime) const
{
    int16_t dist;
    if (timeArrivaled_) {
        dist = params_.offsetDistanceTo * PERCENT_TO_PERMILLE; // percent -> permille
    } else {
        /* interpolate in permille (0~1000) instead of percent: the int16 easing output
           then yields ~10x finer position steps (0.1% instead of 1% per step) */
        dist = GetNextFrameValue(params_.offsetDistanceFrom * PERCENT_TO_PERMILLE,
                                 params_.offsetDistanceTo * PERCENT_TO_PERMILLE, elapsedTime);
    }
    const int16_t distMin = 0;
    return (dist < distMin) ? distMin : ((dist > PROGRESS_PERMILLE_MAX) ? PROGRESS_PERMILLE_MAX : dist);
}

void TransitionImpl::ConfigurePathCallbackIfNeeded()
{
    if (pathCallbackConfigured_) {
        return;
    }
    pathCallbackConfigured_ = true;
    if (pathCallback_ == nullptr) {
        pathCallback_ = new OHOS::PathAnimatorCallback();
        if (pathCallback_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create PathAnimatorCallback object error");
            return;
        }
    }
    /* path data: the sampled polyline parsed from the CSS offset-path value */
    pathCallback_->SetPath(params_.pathPoly);
    /* rotation: map the CSS offset-rotate result (fixed angle / auto + offset) */
    if (params_.offsetRotateMode == OFFSET_ROTATE_AUTO) {
        pathCallback_->SetAutoRotate(params_.offsetRotate); // auto=0, reverse=180, "auto Xdeg"=X
    } else if (params_.offsetRotate != 0) {
        pathCallback_->SetFixedRotate(params_.offsetRotate);
    }
}
#endif // FEATURE_PATH_ANIMATOR

int8_t TransitionImpl::GetNumIterations(const char *iterations)
{
    int8_t min = 1;
    int8_t max = 127;
    if (iterations == nullptr) {
        return min;
    }
    if (!strcmp(iterations, "infinite")) {
        return TransitionImpl::ITERATIONS_INFINITY;
    }
#if FEATURE_TRANSITION_ANIMATOR
    char *end = nullptr;
    long value = strtol(iterations, &end, DEC);
    // Support the transition animation scenario where iterations is set to 0.
    if ((end == iterations) || (end == nullptr) || (*end != '\0') || (value < 0) || (value > max)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animation iterations should set between 0 and 127");
        return min;
    }
    return static_cast<int8_t>(value);
#else
    long value = strtol(iterations, nullptr, DEC);
    if ((value < min) || (value > max)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animation iterations should set between 1 and 127");
        return min;
    }
    return (int8_t)value;
#endif // FEATURE_TRANSITION_ANIMATOR
}

bool TransitionImpl::IsEndWith(const char *src, const char *end)
{
    if ((src == nullptr) || (end == nullptr)) {
        return false;
    }

    size_t strLen = strlen(src);
    size_t endLen = strlen(end);
    if ((strLen < endLen) || (strLen == 0) || (endLen == 0)) {
        return false;
    }

    while (endLen > 0) {
        if (src[strLen - 1] != end[endLen - 1]) {
            return false;
        }
        endLen--;
        strLen--;
    }
    return true;
}

bool TransitionImpl::RepeatAnimator()
{
    if (oriIteration_ != TransitionImpl::ITERATIONS_INFINITY) {
        oriIteration_--;
        if (oriIteration_ == 0) {
            oriIteration_ = params_.iterations;
            return false;
        }
    }
    HILOG_DEBUG(HILOG_MODULE_ACE, "repeat");
    return true;
}

void TransitionImpl::ResetRepeatParam()
{
    if (animator_ != nullptr) {
        animator_->SetRunTime(0); // reset animation executing time
    }
    timeArrivaled_ = false; // reset timeArrivaled flag
    bgcolorTimeSrc_ = 0;    // reset bg-color last update time
    count_ = 1;             // reset bg-color update number of times
}

void TransitionImpl::RecordViewStatus()
{
    viewStatus_.x = view_->GetX();
    viewStatus_.y = view_->GetY();
    viewStatus_.oriRect = view_->GetOrigRect();
    viewStatus_.height = view_->GetHeight();
    viewStatus_.width = view_->GetWidth();
    viewStatus_.background_color.full = view_->GetStyle(STYLE_BACKGROUND_COLOR);
    viewStatus_.rectOpacity = view_->GetStyle(STYLE_BACKGROUND_OPA);
    viewStatus_.imageOpacity = view_->GetStyle(STYLE_IMAGE_OPA);
    viewStatus_.lineOpacity = view_->GetStyle(STYLE_LINE_OPA);
    viewStatus_.textOpacity = view_->GetStyle(STYLE_TEXT_OPA);
}

void TransitionImpl::RecoveryViewStatus(Rect invalidatedAreaBefore) const
{
    view_->SetX(viewStatus_.x);
    view_->SetY(viewStatus_.y);
#if FEATURE_PATH_ANIMATOR
    /* defense: skip size restore when width/height recorded as 0 (layout was not finished
     * at record time), avoiding restoring the view to 0x0 and making it invisible
     * (once triggered in the "if" mounting + fill none scenario).
     * Only applied for offset-path transitions to avoid affecting other transform types. */
    if (transformType_ == TransformType::TRANSLATE_OFFSET_PATH) {
        if ((viewStatus_.width > 0) && (viewStatus_.height > 0)) {
            view_->SetHeight(viewStatus_.height);
            view_->SetWidth(viewStatus_.width);
        }
    } else {
        view_->SetHeight(viewStatus_.height);
        view_->SetWidth(viewStatus_.width);
    }
#else
    view_->SetHeight(viewStatus_.height);
    view_->SetWidth(viewStatus_.width);
#endif // FEATURE_PATH_ANIMATOR
    TransformMap &transMap = view_->GetTransformMap();
    Polygon polygon(Rect(0, 0, 0, 0));
    transMap.SetPolygon(polygon);
    view_->SetStyle(STYLE_BACKGROUND_COLOR, viewStatus_.background_color.full);
    view_->SetStyle(STYLE_BACKGROUND_OPA, viewStatus_.rectOpacity);
    view_->SetStyle(STYLE_IMAGE_OPA, viewStatus_.imageOpacity);
    view_->SetStyle(STYLE_LINE_OPA, viewStatus_.lineOpacity);
    view_->SetStyle(STYLE_TEXT_OPA, viewStatus_.textOpacity);
    invalidatedAreaBefore.Join(invalidatedAreaBefore, view_->GetRect());
    view_->InvalidateRect(invalidatedAreaBefore);
}
} // namespace ACELite
} // namespace OHOS
