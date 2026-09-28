/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "router_module_tdd_test.h"

#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_debugger_config.h"
#include "js_fwk_common.h"
#include "js_page_state_machine.h"
#include "js_router.h"
#include "router_module.h"
#include "securec.h"
#include "wrapper/js.h"

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <sys/stat.h>
#include "ace_log.h"
#include "component.h"
#include "digital_crown_module.h"
#include "fatal_handler.h"
#include "rotate_manager.h"
#endif

namespace OHOS {
namespace ACELite {
static const char PAGE_VIEW_MODEL_BUNDLE[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'] }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    }\n"
    "  });\n"
    "})();";

void RouterModuleTddTest::SetUp()
{
    // set sample ability info
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("root", "bundleName", 0);
    JsAppEnvironment::GetInstance()->InitJsFramework();
}

void RouterModuleTddTest::TearDown()
{
    // clear sample ability info
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(nullptr, nullptr, 0);
    JsAppEnvironment::GetInstance()->Cleanup();
}

void RouterModuleTddTest::ReleaseJSValue(JSValue &value)
{
    jerry_release_value(value);
    value = UNDEFINED;
}

/**
 * @tc.name: RouterModuleTestTest001
 * @tc.desc: router replace with invalid router param
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with undefined
     */
    JSValue routerParam = UNDEFINED;
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(replaceResult);
}

/**
 * @tc.name: RouterModuleTestTest002
 * @tc.desc: Verify the router replace process with valid uri but invalid path
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with only uri
     */
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "invalid path");
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(replaceResult);
}

/**
 * @tc.name: RouterModuleTestTest003
 * @tc.desc: Verify the params handling
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "path");
    JSValue extraParam = jerry_create_object();
    JSObject::SetNumber(extraParam, "count", 0);
    JSObject::Set(routerParam, "params", extraParam);
    /**
     * @tc.steps: step2. trigger router replace
     */
    Router router;
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(extraParam);
    ReleaseJSValue(routerParam);
    ReleaseJSValue(replaceResult);
}

void RouterModuleTddTest::PrepareRouterStateMachine(StateMachine &sm, JSValue &routerParam)
{
    routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "sample path");
    JSValue extraParam = jerry_create_object();
    JSObject::SetNumber(extraParam, "size", 0);
    JSObject::Set(routerParam, "params", extraParam);
    ReleaseJSValue(extraParam);

    JSValue jsResult = UNDEFINED;
    bool initResult = sm.Init(routerParam, jsResult);
    EXPECT_FALSE(initResult);
    EXPECT_TRUE(jerry_value_is_error(jsResult));
    ReleaseJSValue(jsResult);
}

void RouterModuleTddTest::HookViewModel(StateMachine &sm, const char *jsBundle, size_t bundleLength)
{
    if (jsBundle == nullptr || strlen(jsBundle) == 0 || bundleLength == 0) {
        return;
    }

    // clear ability info to avoid page evaling
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(nullptr, nullptr, 0);
    // eval our own view model, and hook it into state machine
    JSValue viewModel = jerry_eval(reinterpret_cast<const jerry_char_t *>(PAGE_VIEW_MODEL_BUNDLE),
                                   strlen(PAGE_VIEW_MODEL_BUNDLE), JERRY_PARSE_NO_OPTS);
    sm.SetViewModel(viewModel);
    // initialize state machine to set correct app root path
    JSValue result = UNDEFINED;
    sm.Init(UNDEFINED, result);
}

/**
 * @tc.name: RouterModuleTestTest004
 * @tc.desc: Verify the page statemachie processing
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = UNDEFINED;
    StateMachine pageSM;
    PrepareRouterStateMachine(pageSM, routerParam);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageSM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state
     */
    pageSM.ChangeState(INIT_STATE);

    /**
     * @tc.steps: step4. check the result
     */
    int8_t currentState = pageSM.GetCurrentState();
    EXPECT_EQ(currentState, READY_STATE);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest005
 * @tc.desc: Verify the cache distribution process and overflow checking
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue routerParam = UNDEFINED;
    StateMachine pageStateMachine;
    PrepareRouterStateMachine(pageStateMachine, routerParam);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageStateMachine, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to ready directly
     */
    pageStateMachine.ChangeState(READY_STATE);

    /**
     * @tc.steps: step4. check the result, the state changing will be refused
     */
    int8_t currentState = pageStateMachine.GetCurrentState();
    EXPECT_EQ(currentState, UNDEFINED_STATE);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest006
 * @tc.desc: Verify the cache distribution process, considering the magic number length
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue prameter = UNDEFINED;
    StateMachine stateMachine;
    PrepareRouterStateMachine(stateMachine, prameter);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateMachine, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateMachine.ChangeState(SHOW_STATE);

    /**
     * @tc.steps: step4. check the result, the state changing will be refused
     */
    int8_t currentState = stateMachine.GetCurrentState();
    EXPECT_EQ(currentState, UNDEFINED_STATE);
    ReleaseJSValue(prameter);
}

/**
 * @tc.name: RouterModuleTestTest007
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue prameterObject = UNDEFINED;
    StateMachine pageStateM;
    PrepareRouterStateMachine(pageStateM, prameterObject);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(pageStateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    pageStateM.ChangeState(BACKGROUND_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(pageStateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(prameterObject);
}

/**
 * @tc.name: RouterModuleTestTest008
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(DESTROY_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest009
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with uri and params bothly
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(BACKGROUND_STATE);
    stateM.ChangeState(DESTROY_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest010
 * @tc.desc: change state without sequence will be refused
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters
     */
    JSValue object = UNDEFINED;
    StateMachine stateM;
    PrepareRouterStateMachine(stateM, object);
    /**
     * @tc.steps: step2. hook view model to one
     */
    HookViewModel(stateM, PAGE_VIEW_MODEL_BUNDLE, strlen(PAGE_VIEW_MODEL_BUNDLE));
    /**
     * @tc.steps: step3. trigger change state to show state directly
     */
    stateM.ChangeState(DESTROY_STATE);
    stateM.ChangeState(SHOW_STATE);

    /**
     * @tc.steps: step4. check the result, state changing will be refused
     */
    EXPECT_EQ(stateM.GetCurrentState(), UNDEFINED_STATE);
    ReleaseJSValue(object);
}

/**
 * @tc.name: RouterModuleTestTest011
 * @tc.desc: pass wrong parameters into router API
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue thisVal = JSI::CreateUndefined();
    const uint8_t argsNum = 2;
    JSIValue args[argsNum] = {JSI::CreateUndefined(), JSI::CreateUndefined()};
    /**
     * @tc.steps: step2. call router module function to simulator the API using
     */
    JSIValue callResult = RouterModule::Replace(thisVal, args, argsNum);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(callResult));
    JSI::ReleaseValue(callResult);
}

/**
 * @tc.name: RouterModuleTestTest012
 * @tc.desc: pass wrong parameters into router API
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue context = JSI::CreateUndefined();
    const uint8_t argsNumber = 1;
    /**
     * @tc.steps: step2. pass null args to API
     */
    JSIValue result = RouterModule::Replace(context, nullptr, argsNumber);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(result));
    JSI::ReleaseValue(result);
}

/**
 * @tc.name: RouterModuleTestTest013
 * @tc.desc: the router replace will fail before the js ability is launched
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router parameters
     */
    JSIValue context = JSI::CreateObject();
    const uint8_t argsNumber = 1;
    JSIValue args[argsNumber] = {JSI::CreateObject()};
    /**
     * @tc.steps: step2. pass null args to API
     */
    JSIValue result = RouterModule::Replace(context, nullptr, argsNumber);
    /**
     * @tc.steps: step4. check the result
     */
    EXPECT_TRUE(JSI::ValueIsError(result));
    JSI::ReleaseValue(result);
    JSI::ReleaseValue(args[0]);
    JSI::ReleaseValue(context);
}

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
// ---- page-transition animation tests, compiled only when the feature gate is ON ----

/**
 * The same page as PAGE_VIEW_MODEL_BUNDLE, plus one animated component: the 'pane' class runs an
 * animation named paneAnim whose keyframes are declared in the style sheet. The engine records
 * exactly one animation for this component on every render of the page, which the cases below use
 * to observe which page owns the animations of the process-wide animation list.
 */
static const char PAGE_VIEW_MODEL_BUNDLE_ANIMATED[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'] }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%',\n"
    "          animationName: 'paneAnim',\n"
    "          animationDuration: 1000\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      },\n"
    "      '@keyframes': {\n"
    "        paneAnim: [\n"
    "          { opacity: '0' },\n"
    "          { opacity: '1' }\n"
    "        ]\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    }\n"
    "  });\n"
    "})();";

/**
 * A page that registers a global digital crown listener in onShow, the way a watch page does. The
 * crown monitor is a single process-wide slot, so a page replaced by this one must not take it away.
 */
static const char PAGE_VIEW_MODEL_BUNDLE_CROWN[] =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function (vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'] }, [\n"
    "        _c('div', { staticClass: ['pane'] }, [\n"
    "          _c('input', {\n"
    "            staticClass: ['button'],\n"
    "            attrs: { type: 'button', value: 'CLICK ME' }\n"
    "          })\n"
    "        ])\n"
    "      ]);\n"
    "    },\n"
    "    onShow: function () {\n"
    "      setMonitorForCrownEvents(function () { return true; });\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          backgroundColor: '#f00',\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        pane: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          width: '70%',\n"
    "          height: '70%'\n"
    "        },\n"
    "        button: {\n"
    "          width: 240\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      logs: []\n"
    "    }\n"
    "  });\n"
    "})();";

// Durations shared by the cases below
constexpr uint16_t TDD_DURATION_NONE = 0;          // no animation
constexpr uint16_t TDD_DURATION_DEFAULT = 300;     // RouterModule's fallback when duration is absent
constexpr uint16_t TDD_DURATION_MAX = 5000;        // TRANSITION_DURATION_MAX; larger values are clamped
constexpr uint16_t TDD_DURATION_OVER_MAX = 10000;  // above the ceiling, to exercise the clamp
/**
 * @tc.name: RouterModuleTestTest014
 * @tc.desc: ParseAnimationConfig parses a valid animation option
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with valid animation
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("fade");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumber(TDD_DURATION_DEFAULT);
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.type, PageTransitionType::FADE);
    EXPECT_EQ(config.duration, 300);
    EXPECT_TRUE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest015
 * @tc.desc: No animation option means no transition
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters without animation
     */
    JSIValue routerParam = JSI::CreateObject();
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.type, PageTransitionType::UNKNOWN);
    EXPECT_EQ(config.duration, 0);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest016
 * @tc.desc: Duration defaults to 300 when omitted
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with only type
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("scale");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.type, PageTransitionType::SCALE);
    EXPECT_EQ(config.duration, 300);
    EXPECT_TRUE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest017
 * @tc.desc: Duration 0 disables the transition
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with duration 0
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("scale");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumber(0);
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.type, PageTransitionType::SCALE);
    EXPECT_EQ(config.duration, 0);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest018
 * @tc.desc: Negative duration is treated as 0, no transition
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with negative duration
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("scale");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumber(-100);
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.duration, 0);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest019
 * @tc.desc: Duration above 5000 is clamped to 5000
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with duration 10000
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("scale");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumber(TDD_DURATION_OVER_MAX);
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.duration, 5000);
    EXPECT_TRUE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest020
 * @tc.desc: Invalid type disables the transition
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with invalid type
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("bounce");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_EQ(config.type, PageTransitionType::UNKNOWN);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest021
 * @tc.desc: All 10 transition types map to the right enum
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. iterate all 10 legal type names
     */
    struct TypeMap {
        const char *name;
        PageTransitionType type;
    };
    const TypeMap typeMap[] = {
        {"fade", PageTransitionType::FADE},
        {"scale", PageTransitionType::SCALE},
        {"slide_left", PageTransitionType::SLIDE_LEFT},
        {"slide_right", PageTransitionType::SLIDE_RIGHT},
        {"slide_up", PageTransitionType::SLIDE_UP},
        {"slide_down", PageTransitionType::SLIDE_DOWN},
        {"slide_over_left", PageTransitionType::SLIDE_OVER_LEFT},
        {"slide_over_right", PageTransitionType::SLIDE_OVER_RIGHT},
        {"slide_over_up", PageTransitionType::SLIDE_OVER_UP},
        {"slide_over_down", PageTransitionType::SLIDE_OVER_DOWN},
    };
    constexpr uint8_t typeMapSize = static_cast<uint8_t>(sizeof(typeMap) / sizeof(typeMap[0]));
    for (uint8_t i = 0; i < typeMapSize; i++) {
        /**
         * @tc.steps: step2. build JS parameters for each type
         */
        JSIValue routerParam = JSI::CreateObject();
        JSIValue animation = JSI::CreateObject();
        JSIValue typeValue = JSI::CreateString(typeMap[i].name);
        JSI::SetNamedProperty(animation, "type", typeValue);
        JSI::ReleaseValue(typeValue);
        JSIValue durationValue = JSI::CreateNumber(100);
        JSI::SetNamedProperty(animation, "duration", durationValue);
        JSI::ReleaseValue(durationValue);
        JSI::SetNamedProperty(routerParam, "animation", animation);
        JSI::ReleaseValue(animation);
        /**
         * @tc.steps: step3. check the mapping
         */
        PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
        EXPECT_EQ(config.type, typeMap[i].type);
        EXPECT_EQ(config.duration, 100);
        EXPECT_TRUE(config.IsValid());
        JSI::ReleaseValue(routerParam);
    }
}

/**
 * @tc.name: RouterModuleTestTest022
 * @tc.desc: PageTransitionConfig validity rule: valid type and positive duration
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. default config is invalid
     */
    PageTransitionConfig defaultConfig;
    EXPECT_FALSE(defaultConfig.IsValid());
    /**
     * @tc.steps: step2. valid type + positive duration is valid
     */
    PageTransitionConfig fadeConfig;
    fadeConfig.type = PageTransitionType::FADE;
    fadeConfig.duration = TDD_DURATION_DEFAULT;
    EXPECT_TRUE(fadeConfig.IsValid());
    /**
     * @tc.steps: step3. zero duration is invalid
     */
    PageTransitionConfig zeroDurationConfig;
    zeroDurationConfig.type = PageTransitionType::FADE;
    zeroDurationConfig.duration = TDD_DURATION_NONE;
    EXPECT_FALSE(zeroDurationConfig.IsValid());
    /**
     * @tc.steps: step4. unknown type is invalid
     */
    PageTransitionConfig unknownTypeConfig;
    unknownTypeConfig.type = PageTransitionType::UNKNOWN;
    unknownTypeConfig.duration = TDD_DURATION_DEFAULT;
    EXPECT_FALSE(unknownTypeConfig.IsValid());
}

/**
 * @tc.name: RouterModuleTestTest023
 * @tc.desc: FinishTransition is idempotent when no transition is running
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create a router with no transition in progress
     */
    Router router;
    /**
     * @tc.steps: step2. call FinishTransition multiple times, must not crash
     */
    router.FinishTransition();
    router.FinishTransition();
    router.SetTransitionConfig(PageTransitionConfig());
    router.FinishTransition();
}

/**
 * @tc.name: RouterModuleTestTest024
 * @tc.desc: PageTransition fade frame logic: opacity interpolates and reaches the final state
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. initialise a fade transition with two views
     */
    UIView oldView;
    UIView newView;
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    PageTransition transition;
    transition.InitForTest(config, &oldView, &newView);
    /**
     * @tc.steps: step2. at elapsed 0 the new page is transparent and the old page opaque
     */
    transition.ApplyFrameForTest(0);
    EXPECT_EQ(newView.GetOpaScale(), OPA_TRANSPARENT);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    /**
     * @tc.steps: step3. midway both pages are in the intermediate alpha range
     */
    transition.ApplyFrameForTest(150);
    EXPECT_GT(newView.GetOpaScale(), OPA_TRANSPARENT);
    EXPECT_LT(oldView.GetOpaScale(), OPA_OPAQUE);
    /**
     * @tc.steps: step4. at the end the new page is fully opaque and the old page transparent
     */
    transition.ApplyFrameForTest(300);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_TRANSPARENT);
}

/**
 * @tc.name: RouterModuleTestTest025
 * @tc.desc: PageTransition scale keeps both pages fully opaque on every frame
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. initialise a scale transition with two views
     */
    UIView oldView;
    UIView newView;
    PageTransitionConfig config;
    config.type = PageTransitionType::SCALE;
    config.duration = TDD_DURATION_DEFAULT;
    PageTransition transition;
    transition.InitForTest(config, &oldView, &newView);
    /**
     * @tc.steps: step2. scale keeps both pages fully opaque on the first frame (no opacity cross-fade)
     */
    transition.ApplyFrameForTest(0);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    /**
     * @tc.steps: step3. both pages remain fully opaque midway
     */
    transition.ApplyFrameForTest(150);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    /**
     * @tc.steps: step4. both pages remain fully opaque on the final frame
     */
    transition.ApplyFrameForTest(300);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
}

/**
 * @tc.name: RouterModuleTestTest026
 * @tc.desc: PageTransitionTypeToString maps all 10 valid types and UNKNOWN to readable strings
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. all 10 valid types map to readable lower-case strings
     */
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::FADE), "fade");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SCALE), "scale");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_LEFT), "slide_left");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_RIGHT), "slide_right");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_UP), "slide_up");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_DOWN), "slide_down");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_OVER_LEFT), "slide_over_left");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_OVER_RIGHT), "slide_over_right");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_OVER_UP), "slide_over_up");
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::SLIDE_OVER_DOWN), "slide_over_down");
    /**
     * @tc.steps: step2. an invalid/unset type degrades to "unknown"
     */
    EXPECT_STREQ(PageTransitionTypeToString(PageTransitionType::UNKNOWN), "unknown");
}

/**
 * @tc.name: RouterModuleTestTest027
 * @tc.desc: Duration NaN must not reach integer truncation (UB), degrades to no animation
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest027, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with duration NaN
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("fade");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumberNaN();
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result: NaN falls back to no animation
     */
    EXPECT_EQ(config.type, PageTransitionType::FADE);
    EXPECT_EQ(config.duration, 0);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest028
 * @tc.desc: Duration +Infinity is still caught by the >5000 clamp, no UB
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters with duration +Infinity
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSIValue typeValue = JSI::CreateString("fade");
    JSI::SetNamedProperty(animation, "type", typeValue);
    JSI::ReleaseValue(typeValue);
    JSIValue durationValue = JSI::CreateNumber(std::numeric_limits<double>::infinity());
    JSI::SetNamedProperty(animation, "duration", durationValue);
    JSI::ReleaseValue(durationValue);
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. check the result: +Infinity is clamped to 5000
     */
    EXPECT_EQ(config.type, PageTransitionType::FADE);
    EXPECT_EQ(config.duration, 5000);
    EXPECT_TRUE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest029
 * @tc.desc: PageTransition slide keeps both pages fully opaque on every frame (no cross-fade)
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. run the same frame sweep for each of the four slide directions
     */
    const PageTransitionType slideTypes[] = {
        PageTransitionType::SLIDE_LEFT,
        PageTransitionType::SLIDE_RIGHT,
        PageTransitionType::SLIDE_UP,
        PageTransitionType::SLIDE_DOWN,
    };
    constexpr uint8_t slideTypeCount = static_cast<uint8_t>(sizeof(slideTypes) / sizeof(slideTypes[0]));
    for (uint8_t i = 0; i < slideTypeCount; i++) {
        UIView oldView;
        UIView newView;
        PageTransitionConfig config;
        config.type = slideTypes[i];
        config.duration = TDD_DURATION_DEFAULT;
        PageTransition transition;
        transition.InitForTest(config, &oldView, &newView);
        /**
         * @tc.steps: step2. a slide never cross-fades, so both pages stay opaque on every frame
         */
        transition.ApplyFrameForTest(0);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
        transition.ApplyFrameForTest(150);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
        transition.ApplyFrameForTest(300);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    }
}

/**
 * @tc.name: RouterModuleTestTest030
 * @tc.desc: PageTransition slide-over keeps both pages fully opaque on every frame
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest030, TestSize.Level1)
{
    /**
     * @tc.steps: step1. run the same frame sweep for each of the four slide-over directions
     */
    const PageTransitionType slideOverTypes[] = {
        PageTransitionType::SLIDE_OVER_LEFT,
        PageTransitionType::SLIDE_OVER_RIGHT,
        PageTransitionType::SLIDE_OVER_UP,
        PageTransitionType::SLIDE_OVER_DOWN,
    };
    constexpr uint8_t slideOverTypeCount = static_cast<uint8_t>(sizeof(slideOverTypes) / sizeof(slideOverTypes[0]));
    for (uint8_t i = 0; i < slideOverTypeCount; i++) {
        UIView oldView;
        UIView newView;
        PageTransitionConfig config;
        config.type = slideOverTypes[i];
        config.duration = TDD_DURATION_DEFAULT;
        PageTransition transition;
        transition.InitForTest(config, &oldView, &newView);
        /**
         * @tc.steps: step2. the overlay slide only moves the new page, both stay opaque
         */
        transition.ApplyFrameForTest(0);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
        transition.ApplyFrameForTest(150);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
        transition.ApplyFrameForTest(300);
        EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
        EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    }
}

/**
 * @tc.name: RouterModuleTestTest031
 * @tc.desc: animation option without a "type" member degrades to no transition
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare router JS parameters whose animation object carries no type
     */
    JSIValue routerParam = JSI::CreateObject();
    JSIValue animation = JSI::CreateObject();
    JSI::SetNamedProperty(routerParam, "animation", animation);
    JSI::ReleaseValue(animation);
    /**
     * @tc.steps: step2. parse the animation config
     */
    PageTransitionConfig config = RouterModule::ParseAnimationConfig(routerParam);
    /**
     * @tc.steps: step3. a missing type is not a valid transition, duration falls back to default
     */
    EXPECT_EQ(config.type, PageTransitionType::UNKNOWN);
    EXPECT_EQ(config.duration, 300);
    EXPECT_FALSE(config.IsValid());
    JSI::ReleaseValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest032
 * @tc.desc: PageTransition skips the frame when the configured duration is zero
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. initialise a fade transition whose duration disables the animation
     */
    UIView oldView;
    UIView newView;
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_NONE;
    PageTransition transition;
    transition.InitForTest(config, &oldView, &newView);
    /**
     * @tc.steps: step2. the frame must be skipped entirely, so a fade frame is never applied and
     *                   the new page is left opaque instead of being made transparent
     */
    transition.ApplyFrameForTest(0);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
}

/**
 * @tc.name: RouterModuleTestTest033
 * @tc.desc: PageTransition skips the frame when either page view is missing
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest033, TestSize.Level1)
{
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    /**
     * @tc.steps: step1. new view missing: the frame is skipped instead of dereferencing nullptr
     */
    UIView oldView;
    PageTransition missingNew;
    missingNew.InitForTest(config, &oldView, nullptr);
    missingNew.ApplyFrameForTest(0);
    EXPECT_EQ(oldView.GetOpaScale(), OPA_OPAQUE);
    /**
     * @tc.steps: step2. old view missing: same, the remaining view is untouched
     */
    UIView newView;
    PageTransition missingOld;
    missingOld.InitForTest(config, nullptr, &newView);
    missingOld.ApplyFrameForTest(0);
    EXPECT_EQ(newView.GetOpaScale(), OPA_OPAQUE);
}

namespace {
// bind a $root property on the global object, the way the page renderer publishes the
// page view model. Used to observe whether a page teardown clears the shared binding.
void BindGlobalRootProperty()
{
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_ROOT));
    jerry_value_t dummy = jerry_create_object();
    jerry_release_value(jerry_set_property(global, key, dummy));
    jerry_release_value(dummy);
    jerry_release_value(key);
    jerry_release_value(global);
}

bool GlobalRootPropertyExists()
{
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_ROOT));
    jerry_value_t hasProperty = jerry_has_property(global, key);
    bool exists = jerry_get_boolean_value(hasProperty);
    jerry_release_value(hasProperty);
    jerry_release_value(key);
    jerry_release_value(global);
    return exists;
}
} // namespace

/**
 * @tc.name: RouterModuleTestTest034
 * @tc.desc: A page torn down without a running transition still releases the shared $root binding
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest034, TestSize.Level1)
{
    /**
     * @tc.steps: step1. publish the shared $root binding, as a rendered page would
     */
    BindGlobalRootProperty();
    EXPECT_TRUE(GlobalRootPropertyExists());
    /**
     * @tc.steps: step2. tear the page down without marking it as replaced by a transition
     */
    StateMachine stateMachine;
    stateMachine.ReleaseHistoryPageResource();
    /**
     * @tc.steps: step3. the plain teardown releases the shared binding as before
     */
    EXPECT_FALSE(GlobalRootPropertyExists());
}

/**
 * @tc.name: RouterModuleTestTest035
 * @tc.desc: A page replaced during a transition must not release the shared $root binding
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest035, TestSize.Level1)
{
    /**
     * @tc.steps: step1. publish the shared $root binding, as the newly shown page would
     */
    BindGlobalRootProperty();
    EXPECT_TRUE(GlobalRootPropertyExists());
    /**
     * @tc.steps: step2. mark the page as replaced by a transition, then tear it down
     */
    StateMachine stateMachine;
    stateMachine.SetReleasedAfterTransition(true);
    stateMachine.ReleaseHistoryPageResource();
    /**
     * @tc.steps: step3. the newer page is still live and owns $root, so it must survive
     */
    EXPECT_TRUE(GlobalRootPropertyExists());
}

namespace {
/**
 * Candidate roots for the throw-away page bundle, tried in order; the first one that accepts a
 * file wins. The test process is launched with different mounts depending on the product, so no
 * single hard-coded path is portable. TDD_PAGE_ROOT_ENV can prepend a product-specific root.
 */
const char * const TDD_PAGE_ROOTS[] = {
    "/storage/tdd_router_page",
    "/data/tdd_router_page",
    "tdd_router_page",
};
constexpr uint8_t TDD_PAGE_ROOT_COUNT = sizeof(TDD_PAGE_ROOTS) / sizeof(TDD_PAGE_ROOTS[0]);
/** Optional root override, tried before the built-in candidates. */
const char * const TDD_PAGE_ROOT_ENV = "TDD_ROUTER_PAGE_ROOT";
/** Intermediate directories of both bundles below, relative to the chosen root. */
const char * const TDD_PAGE_SUB_DIRS[] = { "pages", "pages/index", "pages/other" };
constexpr uint8_t TDD_PAGE_SUB_DIR_COUNT = 3;
/** The two pages a case can drop: the default one and the second one, plus the uri selecting each. */
const char * const TDD_PAGE_FILE_INDEX = "pages/index/index.js";
const char * const TDD_PAGE_FILE_OTHER = "pages/other/other.js";
const char * const TDD_PAGE_URI_INDEX = "/";
const char * const TDD_PAGE_URI_OTHER = "pages/other/other";

bool CreateDirIfNeeded(const char *path)
{
    if (mkdir(path, S_IRWXU) == 0) {
        return true;
    }
    if (errno != EEXIST) {
        return false;
    }
    // EEXIST only means the name is taken, not that it is usable: make sure it is a directory.
    // A root we cannot write to is caught by WritePageBundle() and the caller moves on.
    struct stat pathStat;
    if (stat(path, &pathStat) != 0) {
        return false;
    }
    return S_ISDIR(pathStat.st_mode);
}

bool JoinPath(char *buffer, size_t bufferSize, const char *root, const char *relative)
{
    // sprintf_s returns a negative value when the result would not fit, so it fails on
    // truncation instead of silently cutting the path short.
    return sprintf_s(buffer, bufferSize, "%s/%s", root, relative) >= 0;
}

/** Writes one page file below the root, the intermediate directories are created on demand. */
bool WritePageFile(const char *root, const char *relative, const char *bundle)
{
    if ((bundle == nullptr) || (relative == nullptr)) {
        return true;
    }
    char path[PATH_LENGTH_MAX] = { 0 };
    for (uint8_t i = 0; i < TDD_PAGE_SUB_DIR_COUNT; i++) {
        if (!JoinPath(path, sizeof(path), root, TDD_PAGE_SUB_DIRS[i]) || !CreateDirIfNeeded(path)) {
            return false;
        }
    }
    if (!JoinPath(path, sizeof(path), root, relative)) {
        return false;
    }
    FILE *pageFile = fopen(path, "w");
    if (pageFile == nullptr) {
        return false;
    }
    size_t length = strlen(bundle);
    size_t written = fwrite(bundle, 1, length, pageFile);
    // a failed flush can leave a truncated file even when fwrite reported a full write
    bool closedOk = (fclose(pageFile) == 0);
    if (!closedOk) {
        HILOG_ERROR(HILOG_MODULE_ACE, "tdd router page could not be flushed to [%{public}s]", path);
    }
    return closedOk && (written == length);
}

/** Populates `root` with the throw-away page bundles and points the ability path at it. */
bool TryPreparePageRoot(const char *root, const char *indexBundle, const char *otherBundle)
{
    if (!CreateDirIfNeeded(root)) {
        return false;
    }
    if (!WritePageFile(root, TDD_PAGE_FILE_INDEX, indexBundle) ||
        !WritePageFile(root, TDD_PAGE_FILE_OTHER, otherBundle)) {
        return false;
    }
    HILOG_INFO(HILOG_MODULE_ACE, "tdd router page prepared under [%{public}s]", root);
    // StateMachine::Init resolves the js file against the ability path, so point it at the root we
    // just populated. TearDown clears this again after the case.
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(root, "bundleName", 0);
    return true;
}
} // namespace

bool RouterModuleTddTest::PrepareRealPage()
{
    return PrepareRealPageWithBundles(PAGE_VIEW_MODEL_BUNDLE, nullptr);
}

bool RouterModuleTddTest::PrepareRealPageWithBundles(const char *indexBundle, const char *otherBundle)
{
    // a root supplied through the environment takes precedence over the built-in candidates
    const char *envRoot = getenv(TDD_PAGE_ROOT_ENV);
    if ((envRoot != nullptr) && (envRoot[0] != '\0') && TryPreparePageRoot(envRoot, indexBundle, otherBundle)) {
        return true;
    }
    for (uint8_t i = 0; i < TDD_PAGE_ROOT_COUNT; i++) {
        if (TryPreparePageRoot(TDD_PAGE_ROOTS[i], indexBundle, otherBundle)) {
            return true;
        }
    }
    HILOG_ERROR(HILOG_MODULE_ACE, "tdd router page could not be prepared, no writable root");
    return false;
}

/**
 * @tc.name: RouterModuleTestTest036
 * @tc.desc: Router::Replace succeeds against a real page file and lands on the current page
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest036, TestSize.Level1)
{
    /**
     * @tc.steps: step1. drop a real page bundle so StateMachine::Init can resolve the js file
     */
    ASSERT_TRUE(PrepareRealPage());
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "/");
    /**
     * @tc.steps: step2. replace synchronously, no animation configured
     */
    JSValue replaceResult = router.Replace(routerParam, false);
    /**
     * @tc.steps: step3. the replace is accepted, the page becomes the current one
     */
    EXPECT_FALSE(jerry_value_is_error(replaceResult));
    ReleaseJSValue(replaceResult);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest037
 * @tc.desc: Router::Replace with a valid animation keeps the outgoing page alive until it ends
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest037, TestSize.Level1)
{
    ASSERT_TRUE(PrepareRealPage());
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "/");
    /**
     * @tc.steps: step1. install a first page, so the next replace has one to animate away
     */
    JSValue firstResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(firstResult));
    ReleaseJSValue(firstResult);
    /**
     * @tc.steps: step2. ask for a valid transition and replace again
     */
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSValue secondResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(secondResult));
    ReleaseJSValue(secondResult);
    /**
     * @tc.steps: step3. stop whatever is still running so the router tears down cleanly
     */
    router.FinishTransition();
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest038
 * @tc.desc: Backgrounding the app while a transition runs finishes it first
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest038, TestSize.Level1)
{
    ASSERT_TRUE(PrepareRealPage());
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "/");
    JSValue firstResult = router.Replace(routerParam, false);
    ReleaseJSValue(firstResult);
    PageTransitionConfig config;
    config.type = PageTransitionType::SCALE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSValue secondResult = router.Replace(routerParam, false);
    ReleaseJSValue(secondResult);
    /**
     * @tc.steps: the whole app moves to background while the animation is still on screen
     */
    router.Hide();
    router.Hide();
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest039
 * @tc.desc: PageTransition::Start and StopAnimator drive a real animator over two views
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest039, TestSize.Level1)
{
    /**
     * @tc.steps: step1. start a transition on two real views, no completion callback
     */
    UIView oldView;
    UIView newView;
    PageTransitionConfig config;
    config.type = PageTransitionType::SLIDE_LEFT;
    config.duration = TDD_DURATION_DEFAULT;
    PageTransition *transition = new PageTransition();
    ASSERT_TRUE(transition != nullptr);
    EXPECT_TRUE(transition->Start(config, &oldView, &newView, nullptr, nullptr));
    /**
     * @tc.steps: step2. an interrupted transition snaps to the final frame and stops its animator;
     *                   releasing the object stays with the owner (the Router in production)
     */
    transition->StopAnimator();
    delete transition;
}

/**
 * @tc.name: RouterModuleTestTest040
 * @tc.desc: A second replace issued while a transition runs is ignored
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest040, TestSize.Level1)
{
    ASSERT_TRUE(PrepareRealPage());
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", "/");
    JSValue firstResult = router.Replace(routerParam, false);
    ReleaseJSValue(firstResult);
    PageTransitionConfig config;
    config.type = PageTransitionType::SLIDE_RIGHT;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSValue secondResult = router.Replace(routerParam, false);
    ReleaseJSValue(secondResult);
    /**
     * @tc.steps: the second-trigger request must be dropped, not queued
     */
    JSValue ignoredResult = router.Replace(routerParam, false);
    EXPECT_TRUE(jerry_value_is_undefined(ignoredResult));
    ReleaseJSValue(ignoredResult);
    router.FinishTransition();
    ReleaseJSValue(routerParam);
}

namespace {
// Completion count carried through the callback's ctx pointer, so each case owns its own copy
struct TransitionCompletionCounter {
    int count;
};

void OnTddTransitionCompleted(void *ctx)
{
    TransitionCompletionCounter *counter = static_cast<TransitionCompletionCounter *>(ctx);
    if (counter != nullptr) {
        counter->count++;
    }
}
} // namespace

/**
 * @tc.name: RouterModuleTestTest041
 * @tc.desc: PageTransition::Callback handles the no-animator, in-progress and finished frames
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest041, TestSize.Level1)
{
    UIView oldView;
    UIView newView;
    /**
     * @tc.steps: step1. a transition with no animator must bail out and leave itself alone
     */
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    {
        PageTransition noAnimator;
        noAnimator.InitForTest(config, &oldView, &newView);
        noAnimator.CallbackForTest(nullptr);
    }
    /**
     * @tc.steps: step2. while the duration is still running the callback only applies a frame
     */
    PageTransitionConfig longConfig;
    longConfig.type = PageTransitionType::SLIDE_LEFT;
    longConfig.duration = TDD_DURATION_MAX;
    TransitionCompletionCounter runningCount = { 0 };
    PageTransition *running = new PageTransition();
    ASSERT_TRUE(running != nullptr);
    ASSERT_TRUE(running->Start(longConfig, &oldView, &newView, OnTddTransitionCompleted, &runningCount));
    running->CallbackForTest(nullptr);
    EXPECT_EQ(runningCount.count, 0);
    running->StopAnimator();
    delete running;
    /**
     * @tc.steps: step3. once the duration has elapsed the callback finishes and notifies the router
     */
    PageTransitionConfig elapsedConfig;
    elapsedConfig.type = PageTransitionType::SCALE;
    elapsedConfig.duration = TDD_DURATION_NONE;
    TransitionCompletionCounter finishingCount = { 0 };
    PageTransition *finishing = new PageTransition();
    ASSERT_TRUE(finishing != nullptr);
    ASSERT_TRUE(finishing->Start(elapsedConfig, &oldView, &newView, OnTddTransitionCompleted, &finishingCount));
    finishing->CallbackForTest(nullptr);   // stops itself; the object stays owned by the caller
    EXPECT_EQ(finishingCount.count, 1);
    delete finishing;
    /**
     * @tc.steps: step4. the same finish without a completion callback
     */
    TransitionCompletionCounter silentCount = { 0 };
    PageTransition *silent = new PageTransition();
    ASSERT_TRUE(silent != nullptr);
    ASSERT_TRUE(silent->Start(elapsedConfig, &oldView, &newView, nullptr, &silentCount));
    silent->CallbackForTest(nullptr);      // stops itself; the object stays owned by the caller
    EXPECT_EQ(silentCount.count, 0);
    delete silent;
}

/**
 * @tc.name: RouterModuleTestTest042
 * @tc.desc: A page replaced during a transition releases its own component animations and keeps the
 *           animations of the page that replaced it
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest042, TestSize.Level1)
{
    /**
     * @tc.steps: step1. install a page with one animated component
     */
    ASSERT_TRUE(PrepareRealPageWithBundles(PAGE_VIEW_MODEL_BUNDLE_ANIMATED, nullptr));
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_INDEX);
    uint16_t nodesBefore = Component::GetAnimationNodeCountForTest();
    JSValue firstResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(firstResult));
    ReleaseJSValue(firstResult);
    /**
     * @tc.steps: step2. the rendered page recorded its animated component
     */
    uint16_t nodesPerPage = Component::GetAnimationNodeCountForTest() - nodesBefore;
    ASSERT_EQ(nodesPerPage, 1);
    /**
     * @tc.steps: step3. replace with a transition: both pages are alive while the animation runs
     */
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSValue secondResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(secondResult));
    ReleaseJSValue(secondResult);
    EXPECT_EQ(Component::GetAnimationNodeCountForTest(), nodesBefore + (nodesPerPage * 2));
    /**
     * @tc.steps: step4. the transition ends and the replaced page is released. Its animation must
     *                  go with it, otherwise the next page's render restarts it on freed views
     */
    router.FinishTransition();
    EXPECT_EQ(Component::GetAnimationNodeCountForTest(), nodesBefore + nodesPerPage);
    /**
     * @tc.steps: step5. a further replace proves the list is still usable and leaves one page only
     */
    router.Replace(routerParam, false);
    router.FinishTransition();
    EXPECT_EQ(Component::GetAnimationNodeCountForTest(), nodesBefore + nodesPerPage);
    ReleaseJSValue(routerParam);
}

namespace {
/**
 * @brief Makes the task queue unusable for the scope of a case, the way a runtime fatal error does,
 *        and restores the fatal handler when the case ends - also when an assertion fails early.
 */
struct TaskQueueFailureGuard {
    TaskQueueFailureGuard()
    {
        FatalHandler::GetInstance().SetFatalError(FatalHandler::ERR_EVAL_JS_FAILED);
    }
    ~TaskQueueFailureGuard()
    {
        FatalHandler::GetInstance().CleanUpFatalResource();
    }
};

/**
 * @brief Clears the process-wide digital crown registration before and after a case, so that no
 *        case sees - nor leaves behind - the registration of another one. It runs while the engine
 *        is still alive, which is what makes releasing the jerry value it holds safe.
 */
struct CrownRegistrationGuard {
    CrownRegistrationGuard()
    {
        DigitalCrownModule::GetInstance().Clear();
    }
    ~CrownRegistrationGuard()
    {
        DigitalCrownModule::GetInstance().Clear();
    }
};
} // namespace

/**
 * @tc.name: RouterModuleTestTest043
 * @tc.desc: A transition whose deferred release could not be queued is released without touching the
 *           views of the pages that are already gone
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest043, TestSize.Level1)
{
    /**
     * @tc.steps: step1. install a first page and replace it with an animated transition
     */
    ASSERT_TRUE(PrepareRealPage());
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_INDEX);
    JSValue firstResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(firstResult));
    ReleaseJSValue(firstResult);
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSValue secondResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(secondResult));
    ReleaseJSValue(secondResult);
    /**
     * @tc.steps: step2. the task queue is unusable, so the finished transition can not be handed
     *                  over for a deferred release and stays pending inside the router
     */
    TaskQueueFailureGuard queueFailure;
    uint32_t framesBeforeCompletion = PageTransition::GetAppliedFrameCountForTest();
    router.CompleteTransitionForTest();
    uint32_t framesAfterCompletion = PageTransition::GetAppliedFrameCountForTest();
    EXPECT_GT(framesAfterCompletion, framesBeforeCompletion);
    /**
     * @tc.steps: step3. the app goes to background, which drains the pending release. The old page
     *                  is already released, so nothing may write the final frame into its views
     */
    router.Hide();
    EXPECT_EQ(PageTransition::GetAppliedFrameCountForTest(), framesAfterCompletion);
    /**
     * @tc.steps: step4. the same must hold for the router teardown, and the drain is idempotent
     */
    router.Hide();
    EXPECT_EQ(PageTransition::GetAppliedFrameCountForTest(), framesAfterCompletion);
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest044
 * @tc.desc: The page that replaced another one keeps the digital crown listener it registered
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest044, TestSize.Level1)
{
    /**
     * @tc.steps: step1. install a page that registers no crown listener
     */
    // the case starts on the second page and replaces it with the default one, so the second page
    // is the plain bundle and the default page is the one registering the crown listener
    ASSERT_TRUE(PrepareRealPageWithBundles(PAGE_VIEW_MODEL_BUNDLE_CROWN, PAGE_VIEW_MODEL_BUNDLE));
    CrownRegistrationGuard crownRegistration;
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_OTHER);
    JSValue firstResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(firstResult));
    ReleaseJSValue(firstResult);
    EXPECT_TRUE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    /**
     * @tc.steps: step2. replace it with a page registering the crown listener in onShow
     */
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_INDEX);
    JSValue secondResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(secondResult));
    ReleaseJSValue(secondResult);
    EXPECT_FALSE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    /**
     * @tc.steps: step3. the transition ends and releases the replaced page, which must not take the
     *                  crown listener of the page that replaced it away
     */
    router.FinishTransition();
    EXPECT_FALSE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    ReleaseJSValue(routerParam);
}

/**
 * @tc.name: RouterModuleTestTest045
 * @tc.desc: The digital crown listener of a replaced page does not outlive it when the replacing
 *           page registers nothing
 */
HWTEST_F(RouterModuleTddTest, RouterModuleTest045, TestSize.Level1)
{
    /**
     * @tc.steps: step1. install a page that registers the crown listener in onShow
     */
    ASSERT_TRUE(PrepareRealPageWithBundles(PAGE_VIEW_MODEL_BUNDLE_CROWN, PAGE_VIEW_MODEL_BUNDLE));
    CrownRegistrationGuard crownRegistration;
    Router router;
    JSValue routerParam = jerry_create_object();
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_INDEX);
    JSValue firstResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(firstResult));
    ReleaseJSValue(firstResult);
    EXPECT_FALSE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    /**
     * @tc.steps: step2. replace it with a page registering nothing, the crown listener must be
     *                  given up before the new page runs, not left behind for a freed page
     */
    PageTransitionConfig config;
    config.type = PageTransitionType::FADE;
    config.duration = TDD_DURATION_DEFAULT;
    router.SetTransitionConfig(config);
    JSObject::SetString(routerParam, "uri", TDD_PAGE_URI_OTHER);
    JSValue secondResult = router.Replace(routerParam, false);
    EXPECT_FALSE(jerry_value_is_error(secondResult));
    ReleaseJSValue(secondResult);
    EXPECT_TRUE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    /**
     * @tc.steps: step3. releasing the replaced page at the end of the transition must not register
     *                  anything back
     */
    router.FinishTransition();
    EXPECT_TRUE(RotateManager::GetInstance().GetRegisteredListeners().IsEmpty());
    ReleaseJSValue(routerParam);
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT
} // namespace ACELite
} // namespace OHOS
