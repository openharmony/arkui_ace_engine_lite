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

/**
 * @file keyframes_transition_impl.cpp
 * @brief Implementation of the keyframes animation executor: plan assembly
 *        (static) and the engine-driven execution.
 */

#include "keyframes_transition_impl.h"

#if FEATURE_TRANSITION_ANIMATOR

#include <cstdlib>
#include <cstring>

#include "ace_log.h"
#include "ace_mem_base.h"
#include "gfx_utils/color.h"
#include "js_fwk_common.h"
#include "keys.h"
#include "securec.h"
#include "stylemgr/app_style.h"
#include "stylemgr/app_style_item.h"
#include "stylemgr/app_style_sheet.h"

namespace OHOS {
namespace ACELite {
namespace {
constexpr uint8_t PERCENT_FULL = 100;
constexpr uint8_t DEG_PER_RAD = 57; // aligned with the classic Component::GetAnimatorValue
constexpr const char *ANIMATION_NAME_NONE = "none";
} // namespace

/* ---------------- static factory ---------------- */

KeyframesTransitionImpl *KeyframesTransitionImpl::Build(const AppStyleSheet &styleSheet,
                                                        const char *animationName,
                                                        const TransitionParams &base, UIView *view)
{
    if (view == nullptr) {
        return nullptr;
    }
    KeyframeAnimationPlan plan;
    if (!BuildPlan(styleSheet, animationName, base, plan) || !plan.IsValid()) {
        return nullptr;
    }
    KeyframesTransitionImpl *executor = new KeyframesTransitionImpl(plan, view);
    if (executor == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create keyframes transition error");
        return nullptr;
    }
    if (!executor->Init()) {
        delete executor;
        return nullptr;
    }
    return executor;
}

/* ---------------- execution ---------------- */

KeyframesTransitionImpl::KeyframesTransitionImpl(const KeyframeAnimationPlan &plan, UIView *view)
    : animator_(nullptr),
      headCallback_(nullptr),
      tailCallback_(nullptr),
      nodes_{nullptr},
      nodeCount_(0),
      plan_(plan),
      view_(view),
      oriIteration_(plan.iterations),
      timeArrivaled_(false)
{
}

bool KeyframesTransitionImpl::Init()
{
    if (animator_ != nullptr) {
        return true;
    }
    RecordViewStatus();
    if (!BuildChain()) {
        return false;
    }
    animator_ = new Animator(this, view_, 0, true);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "keyframes animator create failed");
        ReleaseChain();
        return false;
    }
    return true;
}

void KeyframesTransitionImpl::Start()
{
    /* iterations==0 means "do not play" (feature-path semantics) */
    if (plan_.iterations == 0) {
        return;
    }
    uint8_t state = animator_->GetState();
    if (((state == Animator::STOP) || (state == Animator::PAUSE)) && (plan_.duration > 0)) {
        animator_->SetTime(headCallback_->GetTotalTime());
        animator_->SetRunTime(0);
        animator_->Start();
    }
}

void KeyframesTransitionImpl::Stop()
{
    if (animator_ == nullptr) {
        return;
    }
    uint8_t state = animator_->GetState();
    if ((state == Animator::START) || (state == Animator::RUNNING)) {
        animator_->Stop();
    }
}

void KeyframesTransitionImpl::Callback(UIView *view)
{
    (void)(view);
    if ((animator_ == nullptr) || (headCallback_ == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "keyframes animator is nullptr");
        return;
    }
    const uint32_t elapsedTime = animator_->GetRunTime();
    const uint32_t totalTime = headCallback_->GetTotalTime();
    if (elapsedTime >= totalTime) {
        timeArrivaled_ = true;
    }
    headCallback_->ApplyFrame(view_, elapsedTime, totalTime);

    if (!timeArrivaled_) {
        return;
    }
    if (!RepeatAnimator()) {
        Rect invalidatedAreaBefore = view_->GetRect();
        Stop();
        if (plan_.fill == OptionsFill::FNONE) {
            RecoveryViewStatus(invalidatedAreaBefore);
        }
        return;
    }
    ResetRepeatParam();
}

bool KeyframesTransitionImpl::RepeatAnimator()
{
    if (oriIteration_ != ITERATIONS_INFINITY) {
        oriIteration_--;
        if (oriIteration_ == 0) {
            oriIteration_ = plan_.iterations;
            return false;
        }
    }
    return true;
}

void KeyframesTransitionImpl::ResetRepeatParam()
{
    if (animator_ != nullptr) {
        animator_->SetRunTime(0);
    }
    timeArrivaled_ = false;
}

bool KeyframesTransitionImpl::BuildChain()
{
    /* running offsets: a segment that does not animate translateX/Y keeps the value
       reached by the previous segments (continuous motion path) */
    int16_t currentOffsetX = 0;
    int16_t currentOffsetY = 0;
    for (uint8_t i = 0; i < plan_.segmentCount; i++) {
        const KeyframeSegmentPlan &segment = plan_.segments[i];
        TransitionAnimatorCallback *node = BuildSegmentNode(segment, (i == 0));
        if (node == nullptr) {
            ReleaseChain();
            return false;
        }
        /* translateX and translateY must be merged into ONE SetPosition call per node
           (the engine holds a single position effect per node) */
        int16_t fromX = currentOffsetX;
        int16_t fromY = currentOffsetY;
        int16_t toX = currentOffsetX;
        int16_t toY = currentOffsetY;
        bool hasPosition = false;
        for (uint8_t eff = 0; eff < segment.effectCount; eff++) {
            const SegmentEffect &effect = segment.effects[eff];
            if (effect.attrKeyId == K_TRANSLATE_X) {
                fromX = effect.fromValue;
                toX = effect.toValue;
                hasPosition = true;
            } else if (effect.attrKeyId == K_TRANSLATE_Y) {
                fromY = effect.fromValue;
                toY = effect.toValue;
                hasPosition = true;
            } else {
                ApplyEffectToNode(effect, *node);
            }
        }
        if (hasPosition) {
            /* keyframes offsets are relative to the view's original position */
            node->SetPosition(static_cast<int16_t>(viewStatus_.x + fromX),
                              static_cast<int16_t>(viewStatus_.y + fromY),
                              static_cast<int16_t>(viewStatus_.x + toX),
                              static_cast<int16_t>(viewStatus_.y + toY));
            currentOffsetX = toX;
            currentOffsetY = toY;
        }
        nodes_[nodeCount_++] = node;
        if (headCallback_ == nullptr) {
            headCallback_ = node;
            tailCallback_ = node;
        } else {
            /* segments always play sequentially along the keyframes time axis */
            tailCallback_->AddTransitionAnimatorCallback(node, true);
            tailCallback_ = node;
        }
    }
    return (headCallback_ != nullptr);
}

TransitionAnimatorCallback *KeyframesTransitionImpl::BuildSegmentNode(const KeyframeSegmentPlan &segment,
                                                                      bool isFirstSegment)
{
    TransitionAnimatorCallback *node = new TransitionAnimatorCallback();
    if (node == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create transition callback node error");
        return nullptr;
    }
    uint32_t segmentDuration =
        static_cast<uint32_t>(segment.timeTo - segment.timeFrom) * plan_.duration / PERCENT_FULL;
    node->SetDuration(segmentDuration);
    node->SetDelay(static_cast<uint32_t>((isFirstSegment && (plan_.delay > 0)) ? plan_.delay : 0));
    node->SetEasingFunc(ResolveEasingFunc());
    /* background-color ramps over the whole duration on the single-segment path */
    if (plan_.hasBackgroundColor && isFirstSegment) {
        uint8_t rFrom;
        uint8_t gFrom;
        uint8_t bFrom;
        uint8_t rTo;
        uint8_t gTo;
        uint8_t bTo;
        GetRGB(plan_.backgroundColorFrom, rFrom, gFrom, bFrom);
        GetRGB(plan_.backgroundColorTo, rTo, gTo, bTo);
        node->SetColor(Color::GetColorFromRGB(rFrom, gFrom, bFrom), Color::GetColorFromRGB(rTo, gTo, bTo));
    }
    return node;
}

void KeyframesTransitionImpl::ApplyEffectToNode(const SegmentEffect &effect,
                                                TransitionAnimatorCallback &node) const
{
    switch (effect.attrKeyId) {
        case K_ROTATE:
            node.SetRotation(effect.fromValue, effect.toValue);
            break;
        case K_SCALE: {
            float base = ANIMATION_SCALE_BASE;
            node.SetScale(static_cast<float>(effect.fromValue) / base,
                          static_cast<float>(effect.fromValue) / base,
                          static_cast<float>(effect.toValue) / base,
                          static_cast<float>(effect.toValue) / base);
            break;
        }
        case K_OPACITY:
            node.SetOpacity(static_cast<uint8_t>(effect.fromValue), static_cast<uint8_t>(effect.toValue));
            break;
        default:
            break;
    }
}

EasingFunc KeyframesTransitionImpl::ResolveEasingFunc() const
{
    switch (plan_.easing) {
        case EasingType::EASE_IN:
            return EasingEquation::CubicEaseIn;
        case EasingType::EASE_OUT:
            return EasingEquation::CubicEaseOut;
        case EasingType::EASE_IN_OUT:
            return EasingEquation::CubicEaseInOut;
        default:
            return EasingEquation::LinearEaseNone;
    }
}

void KeyframesTransitionImpl::ReleaseChain()
{
    /* nodes are linked for the engine's timeline computation but owned here */
    for (uint8_t i = 0; i < nodeCount_; i++) {
        delete nodes_[i];
        nodes_[i] = nullptr;
    }
    nodeCount_ = 0;
    headCallback_ = nullptr;
    tailCallback_ = nullptr;
}

void KeyframesTransitionImpl::RecordViewStatus()
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

void KeyframesTransitionImpl::RecoveryViewStatus(Rect invalidatedAreaBefore) const
{
    view_->SetX(viewStatus_.x);
    view_->SetY(viewStatus_.y);
    view_->SetHeight(viewStatus_.height);
    view_->SetWidth(viewStatus_.width);
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

void KeyframesTransitionImpl::GetRGB(const uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) const
{
    const uint8_t bitsPerByte = 8;
    const uint8_t bitsOfTwoBytes = 16;
    b = color & 0xFF;
    g = (color >> bitsPerByte) & 0xFF;
    r = (color >> bitsOfTwoBytes) & 0xFF;
}

/* ---------------- plan assembly (static, stateless) ---------------- */

int32_t KeyframesTransitionImpl::ClampTimeValue(int32_t value)
{
    if (value < 0) {
        return 0;
    }
    return (value > MAX_KEYFRAMES_DURATION_MS) ? MAX_KEYFRAMES_DURATION_MS : value;
}

int32_t KeyframesTransitionImpl::ParseSingleValue(const char *value, bool isScale, bool isOpacity)
{
    if ((value == nullptr) || (*value == '\0')) {
        return 0;
    }
    if (isScale) {
        /* scale: float factor based on ANIMATION_SCALE_BASE, clamped like the classic parser */
        double scale = strtod(value, nullptr);
        const double minScale = 0.0;
        const double maxScaleValue = 32767.0;
        if (scale < minScale) {
            return 0;
        }
        double convertedValue = scale * ANIMATION_SCALE_BASE;
        return (convertedValue > maxScaleValue) ? static_cast<int32_t>(maxScaleValue)
                                                : static_cast<int32_t>(convertedValue);
    }
    if (isOpacity) {
        /* opacity: 0~1 float scaled to 0~ALPHA_MAX */
        return static_cast<int32_t>(strtod(value, nullptr) * ALPHA_MAX);
    }
    int32_t convertedValue = static_cast<int32_t>(strtol(value, nullptr, DEC));
    if (TransitionImpl::IsEndWith(value, "rad")) {
        convertedValue = convertedValue * DEG_PER_RAD;
    }
    return convertedValue;
}

bool KeyframesTransitionImpl::ParseFromToValues(const char *itemValue, bool isScale, bool isOpacity,
                                                int16_t &fromValue, int16_t &toValue)
{
    if (itemValue == nullptr) {
        return false;
    }
    size_t valueLen = strlen(itemValue);
    if ((valueLen == 0) || (valueLen >= UINT8_MAX)) {
        return false;
    }
    char buffer[UINT8_MAX] = {0};
    if (memcpy_s(buffer, sizeof(buffer) - 1, itemValue, valueLen) != 0) {
        return false;
    }
    char *next = nullptr;
    char *from = strtok_s(buffer, ANIMATION_VALUE_SEP, &next);
    char *to = strtok_s(nullptr, ANIMATION_VALUE_SEP, &next);
    if ((from == nullptr) || (to == nullptr)) {
        return false;
    }
    fromValue = static_cast<int16_t>(ParseSingleValue(from, isScale, isOpacity));
    toValue = static_cast<int16_t>(ParseSingleValue(to, isScale, isOpacity));
    return true;
}

bool KeyframesTransitionImpl::AddEffect(uint16_t keyId, const char *itemValue,
                                        KeyframeSegmentPlan &outSegment)
{
    if (outSegment.effectCount >= MAX_SEGMENT_EFFECTS) {
        HILOG_WARN(HILOG_MODULE_ACE, "too many effects in one keyframe segment, dropped");
        return false;
    }
    bool isScale = (keyId == K_SCALE);
    bool isOpacity = (keyId == K_OPACITY);
    int16_t fromValue = 0;
    int16_t toValue = 0;
    if (!ParseFromToValues(itemValue, isScale, isOpacity, fromValue, toValue)) {
        HILOG_WARN(HILOG_MODULE_ACE, "keyframe item value parse failed, dropped");
        return false;
    }
    SegmentEffect &effect = outSegment.effects[outSegment.effectCount];
    effect.attrKeyId = keyId;
    effect.fromValue = fromValue;
    effect.toValue = toValue;
    outSegment.effectCount++;
    return true;
}

bool KeyframesTransitionImpl::BuildSegmentEffects(const AppStyle &segmentStyle,
                                                  KeyframeSegmentPlan &outSegment,
                                                  bool &hasUnsupported)
{
    const AppStyleItem *item = segmentStyle.GetFirst();
    while (item != nullptr) {
        uint16_t keyId = item->GetPropNameId();
        switch (keyId) {
            case K_TRANSLATE_X:
            case K_TRANSLATE_Y:
            case K_ROTATE:
            case K_SCALE:
            case K_OPACITY:
                AddEffect(keyId, item->GetStrValue(), outSegment);
                break;
            case K_BACKGROUND_COLOR:
                /* the single-shot channel, collected by ApplyBackgroundColor */
                break;
            case K_HEIGHT:
            case K_WIDTH:
            case K_OFFSET_DISTANCE:
                /* the engine cannot drive these: reject the plan and fall back to
                   the classic TransitionImpl path (which supports them) */
                hasUnsupported = true;
                break;
            default:
                /* unknown / unparsable items (e.g. byproducts of transform splitting):
                   skip silently, aligned with the classic handler's default branch */
                break;
        }
        item = item->GetNext();
    }
    return (outSegment.effectCount > 0);
}

bool KeyframesTransitionImpl::BuildSegments(const AppStyle &style, KeyframeAnimationPlan &outPlan)
{
    if (style.GetKeyFrameSegmentCount() > 1) {
        /* multi-segment keyframes (e.g. 0%/50%/100%): one plan segment per style segment */
        const AppStyle *segment = style.GetFirstKeyFrameSegment();
        while (segment != nullptr) {
            if (outPlan.segmentCount >= MAX_KEYFRAME_SEGMENTS) {
                HILOG_WARN(HILOG_MODULE_ACE, "too many keyframe segments, truncated");
                break;
            }
            KeyframeSegmentPlan &outSegment = outPlan.segments[outPlan.segmentCount];
            outSegment.timeFrom = segment->GetKeyFrameTimeFrom();
            outSegment.timeTo = segment->GetKeyFrameTimeTo();
            outSegment.effectCount = 0;
            if (BuildSegmentEffects(*segment, outSegment, outPlan.hasUnsupportedEffects)) {
                outPlan.segmentCount++;
            }
            segment = segment->GetNextKeyFrameSegment();
        }
    } else {
        /* classic single-segment keyframes (from/to) */
        KeyframeSegmentPlan &outSegment = outPlan.segments[0];
        outSegment.timeFrom = 0;
        outSegment.timeTo = PERCENT_FULL;
        outSegment.effectCount = 0;
        if (BuildSegmentEffects(style, outSegment, outPlan.hasUnsupportedEffects)) {
            outPlan.segmentCount = 1;
        }
    }
    return (outPlan.segmentCount > 0) && !outPlan.hasUnsupportedEffects;
}

void KeyframesTransitionImpl::ApplyBackgroundColor(const AppStyle &style,
                                                   KeyframeAnimationPlan &outPlan)
{
    /* single-shot channel: only applied for single-segment animations so the color
       ramps over the whole duration (engine limitation: one color ramp per node) */
    if (outPlan.segmentCount != 1) {
        return;
    }
    const AppStyleItem *fromItem = style.GetStyleItemByNameId(K_BACKGROUND_COLOR);
    if (fromItem == nullptr) {
        return;
    }
    int16_t fromValue = 0;
    int16_t toValue = 0;
    /* the color value is stored as "from,to" numeric string (already converted on JS side) */
    if (!ParseFromToValues(fromItem->GetStrValue(), false, false, fromValue, toValue)) {
        return;
    }
    outPlan.backgroundColorFrom = static_cast<uint32_t>(fromValue);
    outPlan.backgroundColorTo = static_cast<uint32_t>(toValue);
    outPlan.hasBackgroundColor = true;
}

void KeyframesTransitionImpl::ApplyBaseStyle(const TransitionParams &base,
                                             KeyframeAnimationPlan &outPlan)
{
    outPlan.duration = ClampTimeValue(base.during);
    outPlan.delay = ClampTimeValue(base.delay);
    outPlan.iterations = base.iterations;
    outPlan.fill = base.fill;
    outPlan.easing = base.easing;
}

bool KeyframesTransitionImpl::BuildPlan(const AppStyleSheet &styleSheet, const char *animationName,
                                        const TransitionParams &base, KeyframeAnimationPlan &outPlan)
{
    if ((animationName == nullptr) || (strlen(animationName) == 0) ||
        (strcmp(animationName, ANIMATION_NAME_NONE) == 0)) {
        return false;
    }
    AppStyle *style = styleSheet.GetStyleFromKeyFramesSelectors(animationName);
    if (style == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "keyframes style not found");
        return false;
    }
    ApplyBaseStyle(base, outPlan);
    if (!BuildSegments(*style, outPlan)) {
        return false;
    }
    ApplyBackgroundColor(*style, outPlan);
    return outPlan.IsValid();
}

} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_TRANSITION_ANIMATOR
