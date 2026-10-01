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

#ifndef OHOS_ACELITE_TRANSITION_PARSER_H
#define OHOS_ACELITE_TRANSITION_PARSER_H

#include "easing_equation.h"
#include "jerryscript-core.h"
#include "non_copyable.h"
#include "transition_executor.h"
#include "view_transition.h"

namespace OHOS {
namespace ACELite {

/**
 * @brief Stateless parsers for the scene transition feature: converts CSS transition-*
 *        strings and startTransition() JS arguments into structured data.
 */
class TransitionParser final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(TransitionParser);
    TransitionParser() = delete;
    ~TransitionParser() = delete;

    /**
     * @brief Parse transition type string to ViewTransition::Type enum value.
     *         "fade"=0, "slide-left"=1, "slide-right"=2, "slide-up"=3,
     *         "slide-down"=4, "scale"=5, "shared-element"=6, default FADE.
     */
    static ViewTransition::Type ParseTransitionEffect(const char *effectStr);

    /**
     * @brief Parse CSS timing-function string to an easing function pointer.
     *        Supports "linear", "ease-in", "ease-out" and "ease-in-out";
     *        nullptr or any unknown value falls back to LinearEaseNone,
     *        aligned with the animation-timing-function convention.
     */
    static EasingFunc ParseEasingFunc(const char *easingStr);

    /**
     * @brief Parse the options object passed to startTransition({ incoming, sharedElement,
     *        sharedStartRect, sharedEndRect }) into staged trigger parameters.
     *        Fields absent from the object are left at their default (empty) values.
     */
    static TransitionTriggerParams ParseTransitionArgs(jerry_value_t arg);

private:
    /**
     * @brief Parse a rect sub-object { x, y, w, h } from the options object into rect.
     */
    static void ParseRect(jerry_value_t arg, const char *name, Rect &rect);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TRANSITION_PARSER_H
