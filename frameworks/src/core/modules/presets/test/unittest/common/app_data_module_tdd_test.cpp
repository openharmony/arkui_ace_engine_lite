/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "app_data_module_tdd_test.h"

#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_fwk_common.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
void AppDataModuleTddTest::SetUp()
{
    JsAppEnvironment *environment = JsAppEnvironment::GetInstance();
    if (environment == nullptr) {
        return;
    }
    environment->InitJsFramework();
}

void AppDataModuleTddTest::TearDown()
{
    JsAppEnvironment *environment = JsAppEnvironment::GetInstance();
    JsAppContext *appContext = JsAppContext::GetInstance();
    if (environment == nullptr || appContext == nullptr) {
        return;
    }
    appContext->ReleaseStyles();
    environment->Cleanup();
}

/**
 * @tc.name: AceliteFrameworkGetAppData001
 * @tc.desc: Verify the getApp module
 */
HWTEST_F(AppDataModuleTddTest, GetAppData001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetMethodName[] = "getApp";
    jerry_value_t globalObject = jerry_get_global_object();
    jerry_value_t getAppModule = jerryx_get_property_str(globalObject, targetMethodName);

    /**
     * @tc.steps: step2. check method on global
     */
    bool result = jerry_value_is_undefined(getAppModule);
    jerry_release_value(globalObject);
    jerry_release_value(getAppModule);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: AceliteFrameworkGetAppData002
 * @tc.desc: Verify the getApp module
 */
HWTEST_F(AppDataModuleTddTest, GetAppData002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char codeToEval[] = "getApp()";
    const char codeToEval2[] = "getApp().data";

    /**
     * @tc.steps: step2. check return data
     */
    jerry_value_t evalResult =
        jerry_eval(reinterpret_cast<const jerry_char_t *>(codeToEval), strlen(codeToEval), JERRY_PARSE_NO_OPTS);
    bool result = jerry_value_is_undefined(evalResult);
    EXPECT_EQ(result, true);
    jerry_release_value(evalResult);
    evalResult =
            jerry_eval(reinterpret_cast<const jerry_char_t *>(codeToEval2), strlen(codeToEval2), JERRY_PARSE_NO_OPTS);
    result = jerry_value_is_error(evalResult);
    EXPECT_EQ(result, true);
    jerry_release_value(evalResult);
}
} // namespace ACELite
} // namespace OHOS
