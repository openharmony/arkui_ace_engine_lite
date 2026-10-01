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

#ifndef OHOS_ACELITE_TRANSITION_IMPL_H
#define OHOS_ACELITE_TRANSITION_IMPL_H
#include <ctime>

#include "animator.h"
#include "js_fwk_common.h"
#include "non_copyable.h"
#if FEATURE_PATH_ANIMATOR
#include "app_style_path_parser.h"
#include "path_animator_callback.h"
#endif // FEATURE_PATH_ANIMATOR

namespace OHOS {
namespace ACELite {
enum EasingType : uint8_t { LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT, EEND };

enum OptionsFill : uint8_t { FNONE, FORWARDS };

struct TransitionParams {
    /* animation execution time (ms), default is 0, means no animation effect. */
    int32_t during;
    /* the time interval (ms) between requesting an animation operation and executing it. */
    int32_t delay;
    /* animation execution times, default is 1. */
    int8_t iterations;
    /* specify whether to return to the initial state after the animation finishes,
    default is 'none', means to return to the initial state; 'forwards' means not return to. */
    OptionsFill fill;
    /* specify the speed curve of the animation */
    EasingType easing;

    /* transformType include "translateX/translateY", you can only specify one of these once a time. */
    char* transformType;
    /* transform from source (px) to target (px) */
    int16_t transform_from;
    int16_t transform_to;

    int16_t height_from;
    int16_t height_to;
    int16_t width_from;
    int16_t width_to;
    int16_t opacity_from;
    int16_t opacity_to;
#if FEATURE_PATH_ANIMATOR
    /* offset-distance progress range (0~100), default is 0 -> 100 */
    int16_t offsetDistanceFrom;
    int16_t offsetDistanceTo;
    /* offset-rotate: FIXED = constant angle (deg), 0 means no rotation;
       AUTO = additive offset on the path tangent angle (deg), e.g. auto=0, reverse=180 */
    int16_t offsetRotate;
    /* offset-rotate mode: OFFSET_ROTATE_FIXED (default) / OFFSET_ROTATE_AUTO */
    uint8_t offsetRotateMode;
    /* sampled polyline of offset-path (defined in ui_lite offset_path_parser.h, namespace OHOS;
        fully qualified to avoid silent shadowing if a same-name type appears in ACELite) */
    OHOS::PathPolyline pathPoly;
#endif // FEATURE_PATH_ANIMATOR
#if FEATURE_TRANSITION_ANIMATOR
    /* staged animation-name for the keyframes path; the plan is built at record/start
       time when all animation-* styles have been applied. Owned by this struct's holder
       (freed before the struct is deleted). */
    char *keyframesName;
#endif // FEATURE_TRANSITION_ANIMATOR
    uint32_t background_color_from;
    uint32_t background_color_to;

    TransitionParams()
        : during(0),
          delay(0),
          iterations(1),
          fill(OptionsFill::FNONE),
          easing(EasingType::LINEAR),
          transformType(nullptr),
          transform_from(0),
          transform_to(0),
          height_from(-1),
          height_to(-1),
          width_from(-1),
          width_to(-1),
          opacity_from(-1),
          opacity_to(-1),
#if FEATURE_PATH_ANIMATOR
          offsetDistanceFrom(0),
          offsetDistanceTo(100), // default is full progress 0% -> 100%
          offsetRotate(0),
          offsetRotateMode(OFFSET_ROTATE_FIXED),
          pathPoly(),
#endif // FEATURE_PATH_ANIMATOR
#if FEATURE_TRANSITION_ANIMATOR
          keyframesName(nullptr),
#endif // FEATURE_TRANSITION_ANIMATOR
          background_color_from(RGB_COLOR_VALUE_MAX),
          background_color_to(RGB_COLOR_VALUE_MAX) {}
};

struct ViewStatus {
    int16_t x;
    int16_t y;
    int16_t height;
    int16_t width;
    int16_t rectOpacity;
    int16_t imageOpacity;
    int16_t lineOpacity;
    int16_t textOpacity;
    Rect oriRect;
    ColorType background_color;

    ACE_DISALLOW_COPY_AND_MOVE(ViewStatus);
    ViewStatus() : x(0), y(0), height(0), width(0), rectOpacity(0), imageOpacity(0), lineOpacity(0), textOpacity(0),
                   oriRect(), background_color() {}
};

enum TransformType : uint8_t {
    TRANSLATE_X,
    TRANSLATE_Y,
    ROTATE, // rotate only support image
#if FEATURE_PATH_ANIMATOR
    TRANSLATE_OFFSET_PATH, // CSS Motion Path
#endif // FEATURE_PATH_ANIMATOR
    NONE
};

enum GeneralType : uint8_t {
    IS_HEIGHT_TRANSITION_SET,
    IS_WIDTH_TRANSITION_SET,
    IS_BACKGROUND_COLOR_TRANSITION_SET,
    IS_OPACITY_TRANSITION_SET,
    END
};

enum TransitionType : uint8_t {
    TTRANSLATE_X,
    TTRANSLATE_Y,
    TROTATE,
    HEIGHT,
    WIDTH,
    BACKGROUND_COLOR,
    OPACITY
};

/**
 * @brief: animation callback implement.
 *         supported parameters can refer to struct TransitionParams
 */
class TransitionImpl final : public AnimatorCallback {
public:
    /**
     * @brief: Construct function
     *
     * @param: params animation transition effect
     * @param: view target view which run the animation
     */
    ACE_DISALLOW_COPY_AND_MOVE(TransitionImpl);
    TransitionImpl(TransitionParams& params, UIView* view)
        : view_(view),
          params_(params),
          oriIteration_(1),
          animator_(nullptr),
          xSrc_(0), ySrc_(0), rotateSrc_(0), widthSrc_(0), heightSrc_(0), opacitySrc_(0), bgcolorTimeSrc_(0)
    {
    }
    TransitionImpl() = delete;
    ~TransitionImpl()
    {
        if (animator_ != nullptr) {
            delete (animator_);
            animator_ = nullptr;
        }
#if FEATURE_PATH_ANIMATOR
        if (pathCallback_ != nullptr) {
            delete pathCallback_;
            pathCallback_ = nullptr;
        }
#endif // FEATURE_PATH_ANIMATOR
    }

    /**
     * @brief: must call this to do initialization after create TransitionImpl instance
     */
    void Init();
    /**
     * brief: start the animation
     */
    void Start();
    static int8_t GetNumIterations(const char* iterations);
    static bool IsEndWith(const char* src, const char *end);
    void Stop() const;

private:
    void Callback(UIView* view) override;
    bool RepeatAnimator();
    void ResetRepeatParam();
    void RecordViewStatus();
    void RecoveryViewStatus(Rect invalidatedAreaBefore) const;
    void InitTransitionParams();
    void InitTransitionParamsStyle();
    void InitTransitionParamsTransform();
    void InitTransitionParamsEasing();
    void GetRGB(const uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b) const;
    int16_t GetNextFrameValue(int16_t from, int16_t to, int32_t elapsedTime) const;
    void SetTransformSrcPosition();
    void RotateAroundCenterPoint(int16_t angle);
    void Perform(int32_t elapsedTime);
    void PerformTransitionBgColorLinear(int32_t elapsedTime);
    void PerformTransition(int16_t from,
                           int16_t to,
                           TransitionType transitionType,
                           int16_t& updateAttrValue,
                           int32_t elapsedTime);
#if FEATURE_PATH_ANIMATOR
    void PerformOffsetPathTransition(int32_t elapsedTime);
    /* path validity check + path-base sync for "if" mounting (skip 1st frame, sync layout
       position at the 2nd frame); returns false when the current frame should be skipped */
    bool SyncOffsetPathBase();
    /* eased offset-distance in permille (0~1000), clamped; full progress at time arrival */
    int16_t EvaluateOffsetDist(int32_t elapsedTime) const;
    /* one-time configuration of pathCallback_ from CSS params (path polyline + rotate mode);
       the view-effect application is then delegated to PathAnimatorCallback::ApplyFrame */
    void ConfigurePathCallbackIfNeeded();
#endif // FEATURE_PATH_ANIMATOR

    UIView* view_;
    TransitionParams& params_;
    int8_t oriIteration_;
    Animator* animator_;
    Vector2<float> pivot_;
    int16_t xSrc_;
    int16_t ySrc_;
    int16_t rotateSrc_;
    int16_t widthSrc_;
    int16_t heightSrc_;
    int16_t opacitySrc_;
    ViewStatus viewStatus_;
    bool isTransformSrcSet_ = false;
    bool timeArrivaled_ = false;
    bool easingType_[EasingType::EEND] = {0};
    bool isTransitionSet_[GeneralType::END] = {0};
    OptionsFill fill_ = OptionsFill::FNONE;
    TransformType transformType_ = TransformType::NONE;
#if FEATURE_PATH_ANIMATOR
    enum class OffsetPathSyncState : uint8_t {
        WAIT_FIRST_FRAME = 0,
        WAIT_LAYOUT = 1,
        SYNCED = 2,
    };
    OffsetPathSyncState offsetPathBaseSyncState_ = OffsetPathSyncState::WAIT_FIRST_FRAME;
    /* view-effect executor for offset-path: lazily created on first use so that
       transitions without offset-path pay only the pointer size */
    OHOS::PathAnimatorCallback *pathCallback_ = nullptr;
    bool pathCallbackConfigured_ = false;
    const static uint16_t POINTS_MIN = 2;
#endif // FEATURE_PATH_ANIMATOR

    /* used for background-color */
    uint8_t rSrc_ = 0; // used to record the last time red value
    uint8_t gSrc_ = 0;
    uint8_t bSrc_ = 0;
    uint8_t rTo_ = 0;
    uint8_t gTo_ = 0;
    uint8_t bTo_ = 0;
    int8_t count_ = 1;             // used to record the current number of times of updating the bg-color
    int8_t steps_ = 1;             // total target number of times of updating the bg-color (during_/INTERVAL)
    int32_t bgcolorTimeSrc_ = 0;       // used to record the last time bg-color
    const static int8_t ITERATIONS_INFINITY = -1;
    const static int16_t INTERVAL = 150; // update the bg-color every INTERVAL (ms)
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TRANSITION_IMPL_H
