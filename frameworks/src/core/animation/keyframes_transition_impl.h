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
 * @file keyframes_transition_impl.h
 * @brief Keyframes animation executor for the FEATURE_TRANSITION_ANIMATOR capability.
 *
 * One class owns the whole feature path:
 *   - Build(): assembles a KeyframeAnimationPlan from the keyframes definitions
 *     (multi-segment / multi-property) plus the component's animation-* values,
 *     and creates the executor from it;
 *   - instance side: expands the plan into a ui_lite TransitionAnimatorCallback
 *     chain (one node per segment) and drives it with a ui_lite Animator.
 *
 * Classic single-property transitions keep using TransitionImpl; this class is
 * standalone (TransitionImpl is not modified). Compiled only when
 * ace_engine_lite_enable_animation_capability is enabled.
 */

#ifndef OHOS_ACELITE_KEYFRAMES_TRANSITION_IMPL_H
#define OHOS_ACELITE_KEYFRAMES_TRANSITION_IMPL_H

#if FEATURE_TRANSITION_ANIMATOR

#include "animator.h"
#include "animator/transition_animator_callback.h"
#include "memory_heap.h"
#include "non_copyable.h"
#include "transition_impl.h"

namespace OHOS {
namespace ACELite {

class AppStyle;
class AppStyleSheet;

/* bounce demo carries 7 segments (8 keyframe points); one extra slot for safety */
constexpr uint8_t MAX_KEYFRAME_SEGMENTS = 8;
/* properties animated in parallel within one segment (translateX/Y, rotate, scale, opacity) */
constexpr uint8_t MAX_SEGMENT_EFFECTS = 5;
/* animation duration / delay clamp (ms): absurdly large input is treated as illegal */
constexpr int32_t MAX_KEYFRAMES_DURATION_MS = 60000;

/**
 * @brief One animated property within a keyframe segment, from->to values.
 */
struct SegmentEffect {
    uint16_t attrKeyId;   // K_TRANSLATE_X / K_TRANSLATE_Y / K_ROTATE / K_SCALE / K_OPACITY
    int16_t fromValue;    // px / degree / scale base (ANIMATION_SCALE_BASE) / opacity(0~255)
    int16_t toValue;
};

/**
 * @brief One keyframe segment: a time window (percent) plus the effects animated in it.
 */
struct KeyframeSegmentPlan {
    uint8_t timeFrom;   // start percent (0~100)
    uint8_t timeTo;     // end percent (0~100)
    uint8_t effectCount;
    SegmentEffect effects[MAX_SEGMENT_EFFECTS];
};

/**
 * @brief The assembled animation plan: a sequence of segments sharing one time axis.
 *        background-color is a single-shot channel (only applied for single-segment
 *        animations so the color ramps over the whole duration); width/height and
 *        offset-distance keyframes are NOT supported by the engine and reject the plan.
 */
struct KeyframeAnimationPlan : public MemoryHeap {
    KeyframeSegmentPlan segments[MAX_KEYFRAME_SEGMENTS];
    uint8_t segmentCount;
    uint32_t backgroundColorFrom;
    uint32_t backgroundColorTo;
    bool hasBackgroundColor;
    /* properties the engine cannot drive were found: plan rejected, classic path fallback */
    bool hasUnsupportedEffects;
    int32_t duration;        // ms
    int32_t delay;           // ms
    int8_t iterations;       // 1~127; 0 means "do not play"; -1 means infinite
    OptionsFill fill;
    EasingType easing;

    KeyframeAnimationPlan()
        : segmentCount(0), backgroundColorFrom(0), backgroundColorTo(0),
          hasBackgroundColor(false), hasUnsupportedEffects(false),
          duration(0), delay(0), iterations(1), fill(OptionsFill::FNONE), easing(EasingType::LINEAR) {}

    bool IsValid() const
    {
        /* duration==0 / iterations==0 still yield a plan: the executor then refuses to
           start, so the classic path (which would play once) is never entered */
        return (!hasUnsupportedEffects) && (segmentCount > 0);
    }
};

class KeyframesTransitionImpl final : public AnimatorCallback, public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(KeyframesTransitionImpl);
    ~KeyframesTransitionImpl() override
    {
        ReleaseChain();
        if (animator_ != nullptr) {
            delete animator_;
            animator_ = nullptr;
        }
    }

    /**
     * @brief Assemble the plan from the keyframes style (animation-name) plus the
     *        component's animation-* values, and create + initialize the executor.
     *        The only way to create an executor (constructor is private).
     * @return the ready-to-start executor, or nullptr when there is no valid feature
     *         plan (unknown name / "none" / unsupported properties) - the caller then
     *         keeps the classic TransitionImpl path.
     */
    static KeyframesTransitionImpl *Build(const AppStyleSheet &styleSheet, const char *animationName,
                                          const TransitionParams &base, UIView *view);

    /**
     * @brief Start the animation; does nothing when iterations is 0 or duration
     *        is not positive.
     */
    void Start();
    void Stop();

private:
    KeyframesTransitionImpl(const KeyframeAnimationPlan &plan, UIView *view);

    /* ---------- execution ---------- */
    bool Init();
    void Callback(UIView *view) override;
    /* expand the plan into the segment callback chain */
    bool BuildChain();
    void ReleaseChain();
    TransitionAnimatorCallback *BuildSegmentNode(const KeyframeSegmentPlan &segment,
                                                 bool isFirstSegment);
    void ApplyEffectToNode(const SegmentEffect &effect, TransitionAnimatorCallback &node) const;
    void RecordViewStatus();
    void RecoveryViewStatus(Rect invalidatedAreaBefore) const;
    bool RepeatAnimator();
    void ResetRepeatParam();
    EasingFunc ResolveEasingFunc() const;
    void GetRGB(const uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) const;

    /* ---------- plan assembly (static, stateless) ---------- */
    static bool BuildPlan(const AppStyleSheet &styleSheet, const char *animationName,
                          const TransitionParams &base, KeyframeAnimationPlan &outPlan);
    static bool BuildSegments(const AppStyle &style, KeyframeAnimationPlan &outPlan);
    /* @return false when the segment carries no supported effect; hasUnsupported is set
       when an engine-unsupported property (width/height/offset-distance/...) was seen */
    static bool BuildSegmentEffects(const AppStyle &segmentStyle, KeyframeSegmentPlan &outSegment,
                                    bool &hasUnsupported);
    static bool AddEffect(uint16_t keyId, const char *itemValue, KeyframeSegmentPlan &outSegment);
    static void ApplyBaseStyle(const TransitionParams &base, KeyframeAnimationPlan &outPlan);
    static void ApplyBackgroundColor(const AppStyle &style, KeyframeAnimationPlan &outPlan);
    /* value parsers aligned with the classic Component::GetAnimatorValue rules */
    static bool ParseFromToValues(const char *itemValue, bool isScale, bool isOpacity,
                                  int16_t &fromValue, int16_t &toValue);
    static int32_t ParseSingleValue(const char *value, bool isScale, bool isOpacity);
    static int32_t ClampTimeValue(int32_t value);

    Animator *animator_;
    TransitionAnimatorCallback *headCallback_; // chain head, drives the timeline
    TransitionAnimatorCallback *tailCallback_;
    /* owning list of the chain nodes (the engine keeps no public next accessor) */
    TransitionAnimatorCallback *nodes_[MAX_KEYFRAME_SEGMENTS];
    uint8_t nodeCount_;
    KeyframeAnimationPlan plan_;
    UIView *view_;
    ViewStatus viewStatus_;
    int8_t oriIteration_;
    bool timeArrivaled_;
    const static int8_t ITERATIONS_INFINITY = -1;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_TRANSITION_ANIMATOR

#endif // OHOS_ACELITE_KEYFRAMES_TRANSITION_IMPL_H
