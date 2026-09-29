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
 * @file element_transition.cpp
 * @brief Implementation of the element-transition feature logic.
 */

#include "element_transition.h"

#if FEATURE_ELEMENT_TRANSITION

#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "js_fwk_common.h"
#include "keys.h"
#include "time_util.h"
#include "transition_parser.h"

namespace OHOS {
namespace ACELite {
namespace {
constexpr char FUNC_START_TRANSITION[] = "startTransition";

// Upper bound for transition duration / delay (ms). Absurdly large values are treated as
// illegal input and clamped to avoid runaway animations and uint32 overflow.
constexpr uint32_t MAX_TRANSITION_DURATION_MS = 10000;
constexpr uint32_t MAX_TRANSITION_DELAY_MS = 10000;

int32_t ClampTransitionTime(int32_t value, uint32_t maxMs, const char *name)
{
    if (value < 0) {
        return 0;
    }
    if (static_cast<uint32_t>(value) > maxMs) {
        HILOG_WARN(HILOG_MODULE_ACE, "%{public}s=%{public}d exceeds max %{public}u, clamped",
                   name, value, maxMs);
        return static_cast<int32_t>(maxMs);
    }
    return value;
}

int32_t GetMillisecondsFromStyleItem(const AppStyleItem *styleItem)
{
    if (styleItem == nullptr) {
        return 0;
    }
    if (styleItem->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        return styleItem->GetNumValue();
    }
    if (styleItem->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        const char *strValue = styleItem->GetStrValue();
        if (strValue == nullptr) {
            return 0;
        }
        return ParseToMilliseconds(strValue);
    }
    return 0;
}

bool HasTransitionType(const ElementTransitionState &state)
{
    return (state.style != nullptr) && state.style->hasType;
}

void ParseStartTransitionArgs(jerry_value_t arg, TransitionTriggerParams &trigger)
{
    Component *directIncoming = ComponentUtils::GetComponentFromBindingObject(arg);
    if (directIncoming != nullptr) {
        trigger.incoming = directIncoming->GetComponentRootView();
    } else {
        trigger = TransitionParser::ParseTransitionArgs(arg);
    }
}

UIView *SelectOutgoingView(const Component &owner, const TransitionTriggerParams &trigger)
{
    // find outgoing view: a currently visible child that is not the incoming view
    // and not the shared element (the shared element is animated separately and
    // should never be treated as the outgoing scene panel).
    Component *child = owner.GetChildHead();
    while (child != nullptr) {
        UIView *childView = child->GetComponentRootView();
        if (childView != nullptr && childView->IsVisible() &&
            childView != trigger.incoming && childView != trigger.sharedElement) {
            return childView;
        }
        child = child->GetNextSibling();
    }

    // fallback: use the first child that is not the incoming/shared view, even if it
    // is currently hidden (e.g. just hidden by a synchronous show-toggle before this call)
    child = owner.GetChildHead();
    while (child != nullptr) {
        UIView *childView = child->GetComponentRootView();
        if (childView != nullptr && childView != trigger.incoming && childView != trigger.sharedElement) {
            return childView;
        }
        child = child->GetNextSibling();
    }
    return nullptr;
}

void StartTransition(Component &owner, ElementTransitionState &state)
{
    if (!HasTransitionType(state)) {
        HILOG_WARN(HILOG_MODULE_ACE, "StartTransition skip: transition style invalid");
        return;
    }

    UIView *outView = SelectOutgoingView(owner, state.trigger);
    if (outView == nullptr || state.trigger.incoming == nullptr) {
        return;
    }

    // Guard: the incoming view is already on screen and the outgoing candidate comes
    // from the hidden-sibling fallback (no currently visible sibling). The target state
    // is already reached, skip to avoid a fake transition from a hidden panel.
    if (state.trigger.incoming->IsVisible() && !outView->IsVisible()) {
        state.trigger.Reset();
        return;
    }

    // build TransitionContext
    TransitionContext ctx;
    ctx.style = *(state.style);
    ctx.outgoing = outView;
    ctx.trigger = state.trigger;

    // execute transition (the executor releases the previous one internally)
    state.executor.Execute(ctx);

    // reset single-use parameters after attempting to start; even if Execute fails,
    // the consumed arguments must not leak into the next transition
    state.trigger.Reset();
}
} // namespace

namespace ElementTransition {

void ApplyTransitionConfig(Component &owner, const AppStyleItem *style)
{
    ElementTransitionState &state = owner.GetElementTransitionState();
    bool hadType = HasTransitionType(state);
    if (state.style == nullptr) {
        state.style = new TransitionStyle();
        if (state.style == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create TransitionStyle failed");
            return;
        }
    }
    uint16_t keyId = style->GetPropNameId();
    switch (keyId) {
        case K_TRANSITION_EFFECT: {
            const char *typeStr = style->GetStrValue();
            if (typeStr == nullptr) {
                break;
            }
            state.style->transitionEffect = TransitionParser::ParseTransitionEffect(typeStr);
            state.style->hasType = true;
            break;
        }
        case K_TRANSITION_DURATION: {
            state.style->duration = ClampTransitionTime(
                GetMillisecondsFromStyleItem(style), MAX_TRANSITION_DURATION_MS, "transition-duration");
            break;
        }
        case K_TRANSITION_DELAY: {
            state.style->delay = ClampTransitionTime(
                GetMillisecondsFromStyleItem(style), MAX_TRANSITION_DELAY_MS, "transition-delay");
            break;
        }
        case K_TRANSITION_TIMING_FUNCTION: {
            const char *easingStr = style->GetStrValue();
            if (easingStr != nullptr) {
                state.style->easing = TransitionParser::ParseEasingFunc(easingStr);
            }
            break;
        }
        default:
            break;
    }
    // register on demand: only elements declaring transition-effect own
    // this JS function, other components are not affected
    if (!hadType && HasTransitionType(state)) {
        JerrySetFuncProperty(owner.GetNativeElement(), FUNC_START_TRANSITION, StartTransitionHandler);
    }
}

jerry_value_t StartTransitionHandler(const jerry_value_t func, const jerry_value_t context,
                                     const jerry_value_t args[], const jerry_length_t argsNum)
{
    (void)func;
    Component *component = ComponentUtils::GetComponentFromBindingObject(context);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "StartTransitionHandler: component null");
        return UNDEFINED;
    }
    ElementTransitionState &state = component->GetElementTransitionState();
    // staged parameters are single-use: always start from a clean state, so that leftovers
    // from a previously aborted trigger (e.g. an options object without incoming) cannot
    // leak into this call
    state.trigger.Reset();
    if (argsNum > 0) {
        ParseStartTransitionArgs(args[0], state.trigger);
    }
    StartTransition(*component, state);
    return UNDEFINED;
}

void Release(ElementTransitionState &state)
{
    if (state.style != nullptr) {
        delete state.style;
        state.style = nullptr;
    }
    state.executor.Release();
}

} // namespace ElementTransition
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_ELEMENT_TRANSITION
