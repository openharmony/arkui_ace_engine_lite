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

#ifndef OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H
#define OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H

#include "acelite_config.h"

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
#include "js_fwk_common.h"
#include "js_page_state_machine.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class RouterModuleTddTest : public testing::Test {
#else
class RouterModuleTddTest {
#endif
public:
    RouterModuleTddTest() = default;
    virtual ~RouterModuleTddTest() = default;
    void SetUp();
    void TearDown();
    void RouterModuleTest001();
    void RouterModuleTest002();
    void RouterModuleTest003();
    void RouterModuleTest004();
    void RouterModuleTest005();
    void RouterModuleTest006();
    void RouterModuleTest007();
    void RouterModuleTest008();
    void RouterModuleTest009();
    void RouterModuleTest010();
    void RouterModuleTest011();
    void RouterModuleTest012();
    void RouterModuleTest013();
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    /**
     * @brief Page-transition animation cases 014-041, compiled only when the feature gate is ON.
     *        test/ace_test_config.gni has to re-add the macro, or the whole group compiles out
     *        and the suite loses these cases without failing.
     */
    void RouterModuleTest014();
    void RouterModuleTest015();
    void RouterModuleTest016();
    void RouterModuleTest017();
    void RouterModuleTest018();
    void RouterModuleTest019();
    void RouterModuleTest020();
    void RouterModuleTest021();
    void RouterModuleTest022();
    void RouterModuleTest023();
    void RouterModuleTest024();
    void RouterModuleTest025();
    void RouterModuleTest026();
    void RouterModuleTest027();
    void RouterModuleTest028();
    void RouterModuleTest029();
    void RouterModuleTest030();
    void RouterModuleTest031();
    void RouterModuleTest032();
    void RouterModuleTest033();
    void RouterModuleTest034();
    void RouterModuleTest035();
    void RouterModuleTest036();
    void RouterModuleTest037();
    void RouterModuleTest038();
    void RouterModuleTest039();
    void RouterModuleTest040();
    void RouterModuleTest041();
    void RouterModuleTest042();
    void RouterModuleTest043();
    void RouterModuleTest044();
    void RouterModuleTest045();
    /**
     * @brief Drop a minimal page bundle on disk and point the ability path at it.
     *
     * StateMachine::Init() refuses a uri whose js file does not exist (CheckJSSourceFile),
     * so Router::Replace() can only reach the replace/transition path with a real file.
     * Returns false when the test process has no writable directory to use.
     */
    bool PrepareRealPage();
    /**
     * @brief Same as PrepareRealPage(), with the content of both pages under the caller's control:
     *        the default page (uri "/") and the second one ("pages/other/other"). A nullptr bundle
     *        leaves that page out, which is what a case needing a single page passes.
     */
    bool PrepareRealPageWithBundles(const char *indexBundle, const char *otherBundle);
#endif
    void ReleaseJSValue(JSValue &value);
    void HookViewModel(StateMachine &sm, const char *jsBundle, size_t bundleLength);
    void PrepareRouterStateMachine(StateMachine &sm, JSValue &routerParam);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_ROUTER_MODULE_TDD_TEST_H
