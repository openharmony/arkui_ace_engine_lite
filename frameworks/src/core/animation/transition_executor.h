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

#ifndef OHOS_ACELITE_TRANSITION_EXECUTOR_H
#define OHOS_ACELITE_TRANSITION_EXECUTOR_H

#include "easing_equation.h"
#include "gfx_utils/rect.h"
#include "non_copyable.h"
#include "view_transition.h"

namespace OHOS {
namespace ACELite {

/**
 * @brief Style settings of a scene-level transition, parsed from the CSS transition-*
 *        properties. Persistent across triggers. Pure value struct, shallow-copy safe.
 */
struct TransitionStyle {
    int32_t duration;                     // transition duration (ms), matches SetDuration(int32_t)
    int32_t delay;                        // delay before start (ms), matches SetDelay(int32_t)
    EasingFunc easing;                    // easing function pointer, matches SetEasingFunc(EasingFunc)
    ViewTransition::Type transitionEffect;  // transition type enum
    bool hasType;                         // whether transitionEffect has been explicitly set

    TransitionStyle()
        : duration(0),
          delay(0),
          easing(EasingEquation::LinearEaseNone),
          transitionEffect(ViewTransition::TRANSITION_FADE),
          hasType(false) {}
};

/**
 * @brief Parameters staged by a startTransition() JS call and consumed by the next trigger.
 *        The component does not own the views; the caller must ensure they outlive the
 *        transition (clear the references via ViewTransition::InvalidateTargets before
 *        destroying them). Single-use: call Reset() after each trigger attempt.
 */
struct TransitionTriggerParams {
    UIView *incoming;        // incoming view (entering)
    UIView *sharedElement;   // shared element view (shared-element transition only)
    Rect sharedStartRect;    // shared element start rect (parent coordinate system)
    Rect sharedEndRect;      // shared element end rect (parent coordinate system)

    TransitionTriggerParams() : incoming(nullptr), sharedElement(nullptr), sharedStartRect(), sharedEndRect() {}

    void Reset()
    {
        incoming = nullptr;
        sharedElement = nullptr;
        sharedStartRect.SetPosition(0, 0);
        sharedStartRect.Resize(0, 0);
        sharedEndRect.SetPosition(0, 0);
        sharedEndRect.Resize(0, 0);
    }
};

/**
 * @brief Context for a scene-level transition, holding the style, the selected outgoing
 *        view and the staged trigger parameters.
 */
struct TransitionContext {
    TransitionStyle style;              // duration, easing, transitionEffect, etc.
    UIView* outgoing;                   // outgoing view (leaving)
    TransitionTriggerParams trigger;    // incoming view, shared element and its rects

    TransitionContext() : style(), outgoing(nullptr), trigger() {}
};

/**
 * @brief Executor that owns and drives the component's current ViewTransition: creates and
 *        starts it from TransitionContext, and releases it (invalidate + delete)
 *        on the next Execute() or on Release(). One instance per component.
 */
class TransitionExecutor final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(TransitionExecutor);
    TransitionExecutor() = default;
    ~TransitionExecutor();

    /**
     * @brief Release the previously running transition (if any), then create and
     *        start a new ViewTransition from the context.
     * @return The started transition (owned by this executor, do not delete manually).
     */
    ViewTransition* Execute(TransitionContext& ctx);

    /**
     * @brief Cancel the current transition (restoring view snapshots), clear its view
     *        references and delete it. Safe to call when nothing is running.
     */
    void Release();

private:
    /**
     * @brief Dual-view transition (Fade / Slide / Scale).
     */
    static ViewTransition* ExecuteDualView(TransitionContext& ctx);

    /**
     * @brief Shared-element transition.
     */
    static ViewTransition* ExecuteSharedElement(TransitionContext& ctx);

    ViewTransition* currentTrans_ = nullptr;  // owned running/finished transition
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TRANSITION_EXECUTOR_H
