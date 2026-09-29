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

#include "element_transition_tdd_test.h"
#include "component_utils.h"
#include "element_transition.h"
#include "handler.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "transition_executor.h"
#include "transition_parser.h"

namespace OHOS {
namespace ACELite {
namespace {
const int32_t LONG_DURATION_MS = 400;
const int32_t NEGATIVE_DURATION_MS = -100;
const int32_t TEST_WIDTH_VALUE = 100;
// shared element rects used across cases
const int16_t STAR_START_X = 30;
const int16_t STAR_START_Y = 30;
const int16_t STAR_SIZE = 40;
const int16_t STAR_END_X = 100;
const int16_t STAR_END_Y = 110;
const int16_t STAR_END_SIZE = 80;
const float STAR_END_SCALE = 2.0f; // 80 / 40

// the transition test page: a stage with two switchable panels and a shared element star.
// star is after the panels, so everyday scenarios select panelA as the outgoing view
const char BUNDLE_TRANSITION[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['stage'], attrs: { ref: 'stage' } }, [\n"
    "        _c('div', { staticClass: ['panelA'], attrs: { ref: 'panelA' } }),\n"
    "        _c('div', { staticClass: ['panelB'], attrs: { ref: 'panelB' } }),\n"
    "        _c('div', { staticClass: ['star'], attrs: { ref: 'star' } })\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        stage: { width: 300, height: 170 },\n"
    "        star: { width: 40, height: 40, backgroundColor: '#ff0' },\n"
    "        panelA: { width: 300, height: 170, backgroundColor: '#f00' },\n"
    "        panelB: { width: 300, height: 170, backgroundColor: '#0f0' }\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";

// same page but with the star before the panels: exercises that the shared element is
// never selected as the outgoing view (SharedElementNotOutgoingTest001)
const char BUNDLE_TRANSITION_STAR_FIRST[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['stage'], attrs: { ref: 'stage' } }, [\n"
    "        _c('div', { staticClass: ['star'], attrs: { ref: 'star' } }),\n"
    "        _c('div', { staticClass: ['panelA'], attrs: { ref: 'panelA' } }),\n"
    "        _c('div', { staticClass: ['panelB'], attrs: { ref: 'panelB' } })\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        stage: { width: 300, height: 170 },\n"
    "        star: { width: 40, height: 40, backgroundColor: '#ff0' },\n"
    "        panelA: { width: 300, height: 170, backgroundColor: '#f00' },\n"
    "        panelB: { width: 300, height: 170, backgroundColor: '#0f0' }\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";

void SetNumberProp(jerry_value_t obj, const char *name, double value)
{
    jerry_value_t propValue = jerry_create_number(value);
    jerryx_set_property_str(obj, name, propValue);
    jerry_release_value(propValue);
}

jerry_value_t BuildRectObj(int16_t x, int16_t y, int16_t w, int16_t h)
{
    jerry_value_t rect = jerry_create_object();
    SetNumberProp(rect, "x", x);
    SetNumberProp(rect, "y", y);
    SetNumberProp(rect, "w", w);
    SetNumberProp(rect, "h", h);
    return rect;
}

// builds the startTransition options object; rects are attached only when withRects is set
jerry_value_t BuildOptions(jerry_value_t incoming, jerry_value_t shared, bool withRects)
{
    jerry_value_t options = jerry_create_object();
    if (!jerry_value_is_undefined(incoming)) {
        jerryx_set_property_str(options, "incoming", incoming);
    }
    if (!jerry_value_is_undefined(shared)) {
        jerryx_set_property_str(options, "sharedElement", shared);
        if (withRects) {
            jerry_value_t startRect = BuildRectObj(STAR_START_X, STAR_START_Y, STAR_SIZE, STAR_SIZE);
            jerryx_set_property_str(options, "sharedStartRect", startRect);
            jerry_release_value(startRect);
            jerry_value_t endRect = BuildRectObj(STAR_END_X, STAR_END_Y, STAR_END_SIZE, STAR_END_SIZE);
            jerryx_set_property_str(options, "sharedEndRect", endRect);
            jerry_release_value(endRect);
        }
    }
    return options;
}
} // namespace

JSValue ElementTransitionTddTest::GetRefDom(JSValue page, const char *ref) const
{
    JSValue refs = JSObject::Get(page, "$refs");
    JSValue dom = JSObject::Get(refs, ref);
    JSRelease(refs);
    return dom; // caller releases
}

Component *ElementTransitionTddTest::GetRefComponent(JSValue page, const char *ref) const
{
    JSValue dom = GetRefDom(page, ref);
    Component *component = ComponentUtils::GetComponentFromBindingObject(dom);
    JSRelease(dom);
    return component;
}

void ElementTransitionTddTest::SetCharStyle(Component *component, const char *name, const char *value) const
{
    jerry_value_t attrValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    component->UpdateView(KeyParser::ParseKeyId(name), attrValue);
    jerry_release_value(attrValue);
}

void ElementTransitionTddTest::SetNumStyle(Component *component, const char *name, int32_t value) const
{
    jerry_value_t attrValue = jerry_create_number(value);
    component->UpdateView(KeyParser::ParseKeyId(name), attrValue);
    jerry_release_value(attrValue);
}

void ElementTransitionTddTest::TriggerWithArg(JSValue stageDom, jerry_value_t arg) const
{
    jerry_value_t args[1] = { arg };
    ElementTransition::StartTransitionHandler(UNDEFINED, stageDom, args, 1);
}

/**
 * @tc.name: ParseTransitionEffectTest001
 * @tc.desc: Verify every transition-effect string maps to its ViewTransition::Type, and that
 *           unknown or null input falls back to TRANSITION_FADE.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::ParseTransitionEffectTest001()
{
    TDD_CASE_BEGIN();
    EXPECT_EQ(ViewTransition::TRANSITION_FADE, TransitionParser::ParseTransitionEffect("fade"));
    EXPECT_EQ(ViewTransition::TRANSITION_SLIDE_LEFT, TransitionParser::ParseTransitionEffect("slide-left"));
    EXPECT_EQ(ViewTransition::TRANSITION_SLIDE_RIGHT, TransitionParser::ParseTransitionEffect("slide-right"));
    EXPECT_EQ(ViewTransition::TRANSITION_SLIDE_UP, TransitionParser::ParseTransitionEffect("slide-up"));
    EXPECT_EQ(ViewTransition::TRANSITION_SLIDE_DOWN, TransitionParser::ParseTransitionEffect("slide-down"));
    EXPECT_EQ(ViewTransition::TRANSITION_SCALE, TransitionParser::ParseTransitionEffect("scale"));
    EXPECT_EQ(ViewTransition::TRANSITION_SHARED_ELEMENT,
              TransitionParser::ParseTransitionEffect("shared-element"));
    EXPECT_EQ(ViewTransition::TRANSITION_FADE, TransitionParser::ParseTransitionEffect("unknown-x"));
    EXPECT_EQ(ViewTransition::TRANSITION_FADE, TransitionParser::ParseTransitionEffect(nullptr));
    TDD_CASE_END();
}

/**
 * @tc.name: ParseEasingFuncTest001
 * @tc.desc: Verify every timing-function string maps to its easing function, and that
 *           unknown or null input falls back to LinearEaseNone.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::ParseEasingFuncTest001()
{
    TDD_CASE_BEGIN();
    EXPECT_EQ(EasingEquation::LinearEaseNone, TransitionParser::ParseEasingFunc("linear"));
    EXPECT_EQ(EasingEquation::CubicEaseIn, TransitionParser::ParseEasingFunc("ease-in"));
    EXPECT_EQ(EasingEquation::CubicEaseOut, TransitionParser::ParseEasingFunc("ease-out"));
    EXPECT_EQ(EasingEquation::CubicEaseInOut, TransitionParser::ParseEasingFunc("ease-in-out"));
    EXPECT_EQ(EasingEquation::LinearEaseNone, TransitionParser::ParseEasingFunc("unknown-x"));
    EXPECT_EQ(EasingEquation::LinearEaseNone, TransitionParser::ParseEasingFunc(nullptr));
    TDD_CASE_END();
}

/**
 * @tc.name: ParseTransitionArgsFullTest001
 * @tc.desc: Verify a full options object is parsed into TransitionTriggerParams, with rect
 *           fields readable in x/y/w/h semantics.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::ParseTransitionArgsFullTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    JSValue incomingDom = GetRefDom(page, "panelB");
    JSValue sharedDom = GetRefDom(page, "star");
    jerry_value_t options = BuildOptions(incomingDom, sharedDom, true);
    TransitionTriggerParams params = TransitionParser::ParseTransitionArgs(options);
    EXPECT_EQ(GetViewByRef(page, "panelB"), params.incoming);
    EXPECT_EQ(GetViewByRef(page, "star"), params.sharedElement);
    EXPECT_EQ(STAR_START_X, params.sharedStartRect.GetX());
    EXPECT_EQ(STAR_START_Y, params.sharedStartRect.GetY());
    EXPECT_EQ(STAR_SIZE, params.sharedStartRect.GetWidth());
    EXPECT_EQ(STAR_SIZE, params.sharedStartRect.GetHeight());
    EXPECT_EQ(STAR_END_X, params.sharedEndRect.GetX());
    EXPECT_EQ(STAR_END_Y, params.sharedEndRect.GetY());
    EXPECT_EQ(STAR_END_SIZE, params.sharedEndRect.GetWidth());
    EXPECT_EQ(STAR_END_SIZE, params.sharedEndRect.GetHeight());
    jerry_release_value(options);
    JSRelease(incomingDom);
    JSRelease(sharedDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ParseTransitionArgsPartialTest001
 * @tc.desc: Verify options carrying only incoming leave the rest at defaults, and that an
 *           incoming value without a bound component parses to nullptr.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::ParseTransitionArgsPartialTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    JSValue incomingDom = GetRefDom(page, "panelB");
    jerry_value_t options = BuildOptions(incomingDom, JSUndefined::Create(), false);
    TransitionTriggerParams params = TransitionParser::ParseTransitionArgs(options);
    EXPECT_EQ(GetViewByRef(page, "panelB"), params.incoming);
    EXPECT_EQ(nullptr, params.sharedElement);
    jerry_release_value(options);

    jerry_value_t plainValue = jerry_create_object();
    jerry_value_t plainOptions = BuildOptions(plainValue, JSUndefined::Create(), false);
    jerry_release_value(plainValue);
    TransitionTriggerParams plainParams = TransitionParser::ParseTransitionArgs(plainOptions);
    EXPECT_EQ(nullptr, plainParams.incoming);
    EXPECT_EQ(nullptr, plainParams.sharedElement);
    jerry_release_value(plainOptions);
    JSRelease(incomingDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: ParseTransitionArgsSharedNoRectsTest001
 * @tc.desc: Verify an options object carrying sharedElement but with missing or empty
 *           (zero-sized) shared rects degrades to a dual-view transition: the shared
 *           element is dropped so it stays untouched instead of being scaled to zero.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::ParseTransitionArgsSharedNoRectsTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    JSValue incomingDom = GetRefDom(page, "panelB");
    JSValue sharedDom = GetRefDom(page, "star");

    // case 1: sharedElement present, rects absent
    jerry_value_t options = BuildOptions(incomingDom, sharedDom, false);
    TransitionTriggerParams params = TransitionParser::ParseTransitionArgs(options);
    EXPECT_EQ(GetViewByRef(page, "panelB"), params.incoming);
    EXPECT_EQ(nullptr, params.sharedElement);
    jerry_release_value(options);

    // case 2: sharedElement present, rects present but zero-sized
    jerry_value_t zeroOptions = BuildOptions(incomingDom, sharedDom, false);
    jerry_value_t zeroRect = BuildRectObj(0, 0, 0, 0);
    jerryx_set_property_str(zeroOptions, "sharedStartRect", zeroRect);
    jerryx_set_property_str(zeroOptions, "sharedEndRect", zeroRect);
    jerry_release_value(zeroRect);
    TransitionTriggerParams zeroParams = TransitionParser::ParseTransitionArgs(zeroOptions);
    EXPECT_EQ(GetViewByRef(page, "panelB"), zeroParams.incoming);
    EXPECT_EQ(nullptr, zeroParams.sharedElement);
    jerry_release_value(zeroOptions);

    JSRelease(sharedDom);
    JSRelease(incomingDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: TransitionEffectRegisterTest001
 * @tc.desc: Verify startTransition is registered on demand: absent before the style is set,
 *           present after transition-effect is applied, and absent on unrelated elements.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::TransitionEffectRegisterTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    JSValue stageDom = GetRefDom(page, "stage");
    Component *stage = GetRefComponent(page, "stage");
    jerry_value_t before = jerryx_get_property_str(stageDom, "startTransition");
    EXPECT_TRUE(jerry_value_is_undefined(before));
    jerry_release_value(before);

    SetCharStyle(stage, "transitionEffect", "fade");
    jerry_value_t func = jerryx_get_property_str(stageDom, "startTransition");
    EXPECT_TRUE(jerry_value_is_function(func));
    jerry_release_value(func);

    JSValue plainDom = GetRefDom(page, "panelA");
    jerry_value_t missing = jerryx_get_property_str(plainDom, "startTransition");
    EXPECT_TRUE(jerry_value_is_undefined(missing));
    jerry_release_value(missing);
    JSRelease(plainDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: TransitionDurationSyncTest001
 * @tc.desc: Verify a transition without transition-duration (default 0) finishes
 *           synchronously: outgoing hidden, incoming fully shown.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::TransitionDurationSyncTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);

    TriggerWithArg(stageDom, incomingDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(OPA_OPAQUE, GetViewByRef(page, "panelB")->GetOpaScale());
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: TransitionDurationAsyncTest001
 * @tc.desc: Verify a positive transition-duration takes effect: the transition starts but
 *           does not finish synchronously, and views stay in the start-of-transition state.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::TransitionDurationAsyncTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    SetNumStyle(stage, "transitionDuration", LONG_DURATION_MS);
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);

    TriggerWithArg(stageDom, incomingDom);
    // still running: outgoing not yet hidden, incoming already prepared visible.
    // visibility is frame-independent here, unlike opacity which changes every frame
    EXPECT_TRUE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());

    // stop the running transition before teardown: a zero-duration retrigger makes the
    // executor release the running one and finish synchronously
    SetNumStyle(stage, "transitionDuration", 0);
    TriggerWithArg(stageDom, incomingDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: TransitionDurationNegativeTest001
 * @tc.desc: Verify a negative transition-duration is clamped to 0 so the transition
 *           finishes synchronously.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::TransitionDurationNegativeTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    SetNumStyle(stage, "transitionDuration", NEGATIVE_DURATION_MS);
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);

    TriggerWithArg(stageDom, incomingDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StartTransitionDirectElementTest001
 * @tc.desc: Verify the direct element form startTransition(incomingElement): the visible
 *           child is selected as outgoing and the transition lands on the final state.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartTransitionDirectElementTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "slide-left");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);
    int16_t outStartX = GetViewByRef(page, "panelA")->GetX();

    TriggerWithArg(stageDom, incomingDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    // slide-left finish: the incoming view takes over the outgoing start position
    EXPECT_EQ(outStartX, GetViewByRef(page, "panelB")->GetX());
    EXPECT_EQ(outStartX, GetViewByRef(page, "panelA")->GetX());
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StartTransitionOptionsSharedTest001
 * @tc.desc: Verify the options form startTransition({incoming, sharedElement, sharedStartRect,
 *           sharedEndRect}): panels cross-fade and the shared element snaps to the end rect
 *           with the end scale.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartTransitionOptionsSharedTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "shared-element");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    JSValue sharedDom = GetRefDom(page, "star");
    GetViewByRef(page, "panelB")->SetVisible(false);

    jerry_value_t options = BuildOptions(incomingDom, sharedDom, true);
    TriggerWithArg(stageDom, options);
    jerry_release_value(options);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(STAR_END_X, GetViewByRef(page, "star")->GetX());
    EXPECT_EQ(STAR_END_Y, GetViewByRef(page, "star")->GetY());
    const float *scale = GetViewByRef(page, "star")->GetTransformMap().GetScaleMatrix().GetData();
    EXPECT_FLOAT_EQ(STAR_END_SCALE, scale[0]); // 0: x scale index
    EXPECT_FLOAT_EQ(STAR_END_SCALE, scale[5]); // 5: y scale index
    JSRelease(sharedDom);
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StartTransitionWithoutEffectTest001
 * @tc.desc: Verify calling startTransition on a component without transition-effect is a
 *           safe no-op.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartTransitionWithoutEffectTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);

    TriggerWithArg(stageDom, incomingDom);
    EXPECT_TRUE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_FALSE(GetViewByRef(page, "panelB")->IsVisible());
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StaleParamsResetTest001
 * @tc.desc: Verify staged parameters never leak across calls: an options call without
 *           incoming aborts, and a following direct call runs a plain dual-view transition
 *           instead of reusing the stale shared element.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StaleParamsResetTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    JSValue sharedDom = GetRefDom(page, "star");
    GetViewByRef(page, "panelB")->SetVisible(false);
    int16_t starX = GetViewByRef(page, "star")->GetX();

    // first call: options without incoming, aborts without touching anything
    jerry_value_t options = BuildOptions(JSUndefined::Create(), sharedDom, true);
    TriggerWithArg(stageDom, options);
    jerry_release_value(options);
    EXPECT_TRUE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_FALSE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(starX, GetViewByRef(page, "star")->GetX());

    // second call: direct element; must run a dual-view transition and leave the star alone
    TriggerWithArg(stageDom, incomingDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(starX, GetViewByRef(page, "star")->GetX());
    JSRelease(sharedDom);
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StartTransitionTwiceTest001
 * @tc.desc: Verify toggling twice works: the executor releases the previous transition and
 *           single-use parameters are reset between triggers.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartTransitionTwiceTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue panelADom = GetRefDom(page, "panelA");
    JSValue panelBDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);

    TriggerWithArg(stageDom, panelBDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    TriggerWithArg(stageDom, panelADom);
    EXPECT_TRUE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_FALSE(GetViewByRef(page, "panelB")->IsVisible());
    JSRelease(panelBDom);
    JSRelease(panelADom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: StartAnimationNoAutoTriggerTest001
 * @tc.desc: Verify style updates do not auto-trigger the scene transition: the transition
 *           only starts from an explicit startTransition() call.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartAnimationNoAutoTriggerTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    GetViewByRef(page, "panelB")->SetVisible(false);
    // any style update ends in StartAnimation(); nothing transition-related may happen
    SetNumStyle(stage, "width", TEST_WIDTH_VALUE);
    EXPECT_TRUE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_FALSE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(OPA_OPAQUE, GetViewByRef(page, "panelA")->GetOpaScale());
    DestroyPage(page);
    TDD_CASE_END();
}

/**
 * @tc.name: SharedElementNotOutgoingTest001
 * @tc.desc: Verify the shared element is never selected as the outgoing view even when it
 *           precedes the panels in the child list.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::SharedElementNotOutgoingTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION_STAR_FIRST, strlen(BUNDLE_TRANSITION_STAR_FIRST));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "shared-element");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue incomingDom = GetRefDom(page, "panelB");
    JSValue sharedDom = GetRefDom(page, "star");
    GetViewByRef(page, "panelB")->SetVisible(false);

    jerry_value_t options = BuildOptions(incomingDom, sharedDom, true);
    TriggerWithArg(stageDom, options);
    jerry_release_value(options);
    // panelA must have been the outgoing view (hidden after finish); had the star been
    // selected instead, panelA would stay visible
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(STAR_END_X, GetViewByRef(page, "star")->GetX());
    JSRelease(sharedDom);
    JSRelease(incomingDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}
/**
 * @tc.name: StartTransitionSameTargetSkipTest001
 * @tc.desc: Verify calling startTransition with the already-visible target is a no-op:
 *           the hidden sibling must not be used as a fake outgoing view.
 * @tc.type: FUNC
 */
void ElementTransitionTddTest::StartTransitionSameTargetSkipTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE_TRANSITION, strlen(BUNDLE_TRANSITION));
    Component *stage = GetRefComponent(page, "stage");
    SetCharStyle(stage, "transitionEffect", "fade");
    JSValue stageDom = GetRefDom(page, "stage");
    JSValue panelBDom = GetRefDom(page, "panelB");
    GetViewByRef(page, "panelB")->SetVisible(false);
    // hide the star as well: with a visible third child the first pass of
    // SelectOutgoingView picks it as the outgoing view and the skip guard is
    // never reached, making this case indistinguishable from a normal transition
    GetViewByRef(page, "star")->SetVisible(false);

    // first trigger: switch from panelA to panelB (duration 0, finishes synchronously)
    TriggerWithArg(stageDom, panelBDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(OPA_OPAQUE, GetViewByRef(page, "panelB")->GetOpaScale());

    // second trigger with the same (already visible) incoming: must be skipped.
    // a positive duration makes a fake transition observable: without the skip guard
    // the hidden fallback panelA would be snapshot-shown and panelB reset to opa 0,
    // so the assertions below would fail
    SetNumStyle(stage, "transitionDuration", LONG_DURATION_MS);
    TriggerWithArg(stageDom, panelBDom);
    EXPECT_FALSE(GetViewByRef(page, "panelA")->IsVisible());
    EXPECT_TRUE(GetViewByRef(page, "panelB")->IsVisible());
    EXPECT_EQ(OPA_OPAQUE, GetViewByRef(page, "panelB")->GetOpaScale());

    JSRelease(panelBDom);
    JSRelease(stageDom);
    DestroyPage(page);
    TDD_CASE_END();
}

void ElementTransitionTddTest::RunTests()
{
    ParseTransitionEffectTest001();
    ParseEasingFuncTest001();
    ParseTransitionArgsFullTest001();
    ParseTransitionArgsPartialTest001();
    ParseTransitionArgsSharedNoRectsTest001();
    TransitionEffectRegisterTest001();
    TransitionDurationSyncTest001();
    TransitionDurationAsyncTest001();
    TransitionDurationNegativeTest001();
    StartTransitionDirectElementTest001();
    StartTransitionOptionsSharedTest001();
    StartTransitionWithoutEffectTest001();
    StaleParamsResetTest001();
    StartTransitionTwiceTest001();
    StartTransitionSameTargetSkipTest001();
    StartAnimationNoAutoTriggerTest001();
    SharedElementNotOutgoingTest001();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ParseTransitionEffectTest001
 * @tc.desc: Verify every transition-effect string maps to its ViewTransition::Type, and that
 *           unknown or null input falls back to TRANSITION_FADE.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, ParseTransitionEffectTest001, TestSize.Level1)
{
    ElementTransitionTddTest::ParseTransitionEffectTest001();
}

/**
 * @tc.name: ParseEasingFuncTest001
 * @tc.desc: Verify every timing-function string maps to its easing function, and that
 *           unknown or null input falls back to LinearEaseNone.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, ParseEasingFuncTest001, TestSize.Level1)
{
    ElementTransitionTddTest::ParseEasingFuncTest001();
}

/**
 * @tc.name: ParseTransitionArgsFullTest001
 * @tc.desc: Verify a full options object is parsed into TransitionTriggerParams, with rect
 *           fields readable in x/y/w/h semantics.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, ParseTransitionArgsFullTest001, TestSize.Level1)
{
    ElementTransitionTddTest::ParseTransitionArgsFullTest001();
}

/**
 * @tc.name: ParseTransitionArgsPartialTest001
 * @tc.desc: Verify options carrying only incoming leave the rest at defaults, and that an
 *           incoming value without a bound component parses to nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, ParseTransitionArgsPartialTest001, TestSize.Level1)
{
    ElementTransitionTddTest::ParseTransitionArgsPartialTest001();
}

/**
 * @tc.name: ParseTransitionArgsSharedNoRectsTest001
 * @tc.desc: Verify an options object carrying sharedElement but with missing or empty
 *           (zero-sized) shared rects degrades to a dual-view transition: the shared
 *           element is dropped so it stays untouched instead of being scaled to zero.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, ParseTransitionArgsSharedNoRectsTest001, TestSize.Level1)
{
    ElementTransitionTddTest::ParseTransitionArgsSharedNoRectsTest001();
}

/**
 * @tc.name: TransitionEffectRegisterTest001
 * @tc.desc: Verify startTransition is registered on demand: absent before the style is set,
 *           present after transition-effect is applied, and absent on unrelated elements.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, TransitionEffectRegisterTest001, TestSize.Level1)
{
    ElementTransitionTddTest::TransitionEffectRegisterTest001();
}

/**
 * @tc.name: TransitionDurationSyncTest001
 * @tc.desc: Verify a transition without transition-duration (default 0) finishes
 *           synchronously: outgoing hidden, incoming fully shown.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, TransitionDurationSyncTest001, TestSize.Level1)
{
    ElementTransitionTddTest::TransitionDurationSyncTest001();
}

/**
 * @tc.name: TransitionDurationAsyncTest001
 * @tc.desc: Verify a positive transition-duration takes effect: the transition starts but
 *           does not finish synchronously, and views stay in the start-of-transition state.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, TransitionDurationAsyncTest001, TestSize.Level1)
{
    ElementTransitionTddTest::TransitionDurationAsyncTest001();
}

/**
 * @tc.name: TransitionDurationNegativeTest001
 * @tc.desc: Verify a negative transition-duration is clamped to 0 so the transition
 *           finishes synchronously.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, TransitionDurationNegativeTest001, TestSize.Level1)
{
    ElementTransitionTddTest::TransitionDurationNegativeTest001();
}

/**
 * @tc.name: StartTransitionDirectElementTest001
 * @tc.desc: Verify the direct element form startTransition(incomingElement): the visible
 *           child is selected as outgoing and the transition lands on the final state.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartTransitionDirectElementTest001, TestSize.Level1)
{
    ElementTransitionTddTest::StartTransitionDirectElementTest001();
}

/**
 * @tc.name: StartTransitionOptionsSharedTest001
 * @tc.desc: Verify the options form startTransition({incoming, sharedElement, sharedStartRect,
 *           sharedEndRect}): panels cross-fade and the shared element snaps to the end rect
 *           with the end scale.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartTransitionOptionsSharedTest001, TestSize.Level1)
{
    ElementTransitionTddTest::StartTransitionOptionsSharedTest001();
}

/**
 * @tc.name: StartTransitionWithoutEffectTest001
 * @tc.desc: Verify calling startTransition on a component without transition-effect is a
 *           safe no-op.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartTransitionWithoutEffectTest001, TestSize.Level0)
{
    ElementTransitionTddTest::StartTransitionWithoutEffectTest001();
}

/**
 * @tc.name: StaleParamsResetTest001
 * @tc.desc: Verify staged parameters never leak across calls: an options call without
 *           incoming aborts, and a following direct call runs a plain dual-view transition
 *           instead of reusing the stale shared element.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StaleParamsResetTest001, TestSize.Level1)
{
    ElementTransitionTddTest::StaleParamsResetTest001();
}

/**
 * @tc.name: StartTransitionTwiceTest001
 * @tc.desc: Verify toggling twice works: the executor releases the previous transition and
 *           single-use parameters are reset between triggers.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartTransitionTwiceTest001, TestSize.Level1)
{
    ElementTransitionTddTest::StartTransitionTwiceTest001();
}

/**
 * @tc.name: StartAnimationNoAutoTriggerTest001
 * @tc.desc: Verify style updates do not auto-trigger the scene transition: the transition
 *           only starts from an explicit startTransition() call.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartAnimationNoAutoTriggerTest001, TestSize.Level0)
{
    ElementTransitionTddTest::StartAnimationNoAutoTriggerTest001();
}

/**
 * @tc.name: SharedElementNotOutgoingTest001
 * @tc.desc: Verify the shared element is never selected as the outgoing view even when it
 *           precedes the panels in the child list.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, SharedElementNotOutgoingTest001, TestSize.Level1)
{
    ElementTransitionTddTest::SharedElementNotOutgoingTest001();
}

/**
 * @tc.name: StartTransitionSameTargetSkipTest001
 * @tc.desc: Verify calling startTransition with the already-visible target is a no-op:
 *           the hidden sibling must not be used as a fake outgoing view.
 * @tc.type: FUNC
 */
HWTEST_F(ElementTransitionTddTest, StartTransitionSameTargetSkipTest001, TestSize.Level1)
{
    ElementTransitionTddTest::StartTransitionSameTargetSkipTest001();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
