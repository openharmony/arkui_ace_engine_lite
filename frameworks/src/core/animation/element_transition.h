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
 * @file element_transition.h
 * @brief Element-transition feature: per-component state and its operations.
 *
 * The Component only owns an ElementTransitionState value member; all logic
 * (CSS transition-* style application, the startTransition JS callback body,
 * and resource release) lives in the ElementTransition free functions below.
 * The whole feature is compiled only when the
 * ace_engine_lite_enable_element_transition_capability GN feature is enabled.
 */

#ifndef OHOS_ACELITE_ELEMENT_TRANSITION_H
#define OHOS_ACELITE_ELEMENT_TRANSITION_H

#include "jerryscript-core.h"
#include "transition_executor.h"

#if FEATURE_ELEMENT_TRANSITION

namespace OHOS {
namespace ACELite {

class AppStyleItem;
class Component;

/**
 * @brief Aggregated per-component state of the element-transition feature.
 *        Owned directly by the Component; manipulated only through the
 *        ElementTransition functions below. Not copyable (the executor owns
 *        a running ViewTransition).
 */
struct ElementTransitionState {
    TransitionStyle *style;           // lazy-allocated; nullptr means no transition configured
    TransitionTriggerParams trigger;  // single-use params staged by a startTransition() call
    TransitionExecutor executor;      // owns the currently running ViewTransition

    ElementTransitionState() : style(nullptr), trigger(), executor() {}
};

namespace ElementTransition {

/**
 * @brief Parse a transition-* style item and update the owner's transition state.
 *
 * When transition-effect is set for the first time, the startTransition JS method
 * is registered on the component's native element.
 */
void ApplyTransitionConfig(Component &owner, const AppStyleItem *style);

/**
 * @brief JS callback body of element.startTransition(...).
 *
 * Matches jerry_external_handler_t; registered on demand by ApplyTransitionConfig.
 * Accepts either a bound component (treated as the incoming view) or an options
 * object describing incoming/sharedElement/sharedStartRect/sharedEndRect.
 */
jerry_value_t StartTransitionHandler(const jerry_value_t func, const jerry_value_t context,
                                     const jerry_value_t args[], const jerry_length_t argsNum);

/**
 * @brief Release the transition style and stop any running transition.
 *        Safe to call when nothing was configured.
 */
void Release(ElementTransitionState &state);

} // namespace ElementTransition
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_ELEMENT_TRANSITION

#endif // OHOS_ACELITE_ELEMENT_TRANSITION_H
