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

#include "transition_parser.h"

#include <climits>
#include <cmath>
#include <cstring>
#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "handler.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {

using std::strcmp;

namespace {
int16_t ClampToInt16(double value)
{
    // NaN defense: a missing or non-numeric rect field arrives as NaN, which slips
    // through both range checks (every comparison with NaN is false) and hits
    // static_cast as undefined behavior; clamp it to 0 so the rect degrades to empty
    if (std::isnan(value)) {
        return 0;
    }
    if (value >= INT16_MAX) {
        return INT16_MAX;
    }
    if (value <= INT16_MIN) {
        return INT16_MIN;
    }
    return static_cast<int16_t>(value);
}

// returns true only when the named shared-element rect field exists in the options object
// and the parsed rect is non-empty (a zero/negative-sized rect is as meaningless as a
// missing one for shared-element animation)
bool IsSharedRectUsable(jerry_value_t obj, const char *name, const Rect &rect)
{
    jerry_value_t value = jerryx_get_property_str(obj, name);
    bool present = !jerry_value_is_undefined(value);
    jerry_release_value(value);
    return present && rect.GetWidth() > 0 && rect.GetHeight() > 0;
}
} // namespace

ViewTransition::Type TransitionParser::ParseTransitionEffect(const char *effectStr)
{
    if (effectStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ParseTransitionEffect: effectStr null, default FADE");
        return ViewTransition::TRANSITION_FADE;
    }
    if (!strcmp(effectStr, "fade")) {
        return ViewTransition::TRANSITION_FADE;
    }
    if (!strcmp(effectStr, "slide-left")) {
        return ViewTransition::TRANSITION_SLIDE_LEFT;
    }
    if (!strcmp(effectStr, "slide-right")) {
        return ViewTransition::TRANSITION_SLIDE_RIGHT;
    }
    if (!strcmp(effectStr, "slide-up")) {
        return ViewTransition::TRANSITION_SLIDE_UP;
    }
    if (!strcmp(effectStr, "slide-down")) {
        return ViewTransition::TRANSITION_SLIDE_DOWN;
    }
    if (!strcmp(effectStr, "scale")) {
        return ViewTransition::TRANSITION_SCALE;
    }
    if (!strcmp(effectStr, "shared-element")) {
        return ViewTransition::TRANSITION_SHARED_ELEMENT;
    }
    HILOG_ERROR(HILOG_MODULE_ACE, "ParseTransitionEffect: unknown effect '%s', default FADE", effectStr);
    return ViewTransition::TRANSITION_FADE;
}

EasingFunc TransitionParser::ParseEasingFunc(const char *easingStr)
{
    if (easingStr == nullptr) {
        return EasingEquation::LinearEaseNone;
    }
    if (!strcmp(easingStr, "linear")) {
        return EasingEquation::LinearEaseNone;
    }
    if (!strcmp(easingStr, "ease-in")) {
        return EasingEquation::CubicEaseIn;
    }
    if (!strcmp(easingStr, "ease-out")) {
        return EasingEquation::CubicEaseOut;
    }
    if (!strcmp(easingStr, "ease-in-out")) {
        return EasingEquation::CubicEaseInOut;
    }
    return EasingEquation::LinearEaseNone;
}

TransitionTriggerParams TransitionParser::ParseTransitionArgs(jerry_value_t arg)
{
    TransitionTriggerParams params;
    jerry_value_t incoming = jerryx_get_property_str(arg, "incoming");
    if (!jerry_value_is_undefined(incoming)) {
        Component *incomingComponent = ComponentUtils::GetComponentFromBindingObject(incoming);
        if (incomingComponent != nullptr) {
            params.incoming = incomingComponent->GetComponentRootView();
        }
    }
    jerry_release_value(incoming);

    jerry_value_t shared = jerryx_get_property_str(arg, "sharedElement");
    if (!jerry_value_is_undefined(shared)) {
        Component *sharedComponent = ComponentUtils::GetComponentFromBindingObject(shared);
        if (sharedComponent != nullptr) {
            params.sharedElement = sharedComponent->GetComponentRootView();
        }
    }
    jerry_release_value(shared);

    ParseRect(arg, "sharedStartRect", params.sharedStartRect);
    ParseRect(arg, "sharedEndRect", params.sharedEndRect);

    // a shared-element transition is meaningless without two usable rects: the shared
    // element would be scaled to zero and vanish. Degrade to a dual-view transition
    // instead, leaving the shared element untouched.
    if (params.sharedElement != nullptr &&
        (!IsSharedRectUsable(arg, "sharedStartRect", params.sharedStartRect) ||
         !IsSharedRectUsable(arg, "sharedEndRect", params.sharedEndRect))) {
        HILOG_WARN(HILOG_MODULE_ACE, "sharedElement with missing/empty shared rects, degrade to dual-view transition");
        params.sharedElement = nullptr;
    }
    return params;
}

void TransitionParser::ParseRect(jerry_value_t arg, const char *name, Rect &rect)
{
    jerry_value_t rectObj = jerryx_get_property_str(arg, name);
    if (!jerry_value_is_undefined(rectObj)) {
        int16_t x = ClampToInt16(JSObject::GetNumber(rectObj, "x"));
        int16_t y = ClampToInt16(JSObject::GetNumber(rectObj, "y"));
        int16_t w = ClampToInt16(JSObject::GetNumber(rectObj, "w"));
        int16_t h = ClampToInt16(JSObject::GetNumber(rectObj, "h"));
        rect.SetPosition(x, y);
        rect.Resize(w, h);
    }
    jerry_release_value(rectObj);
}
} // namespace ACELite
} // namespace OHOS
