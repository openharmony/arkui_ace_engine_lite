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

#ifndef OHOS_ACELITE_TEST_DIV_FLEX_LAYOUT_H
#define OHOS_ACELITE_TEST_DIV_FLEX_LAYOUT_H

#include "base_test.h"

namespace OHOS {
namespace ACELite {
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
class DivFlexLayoutTddTest : public BaseTest {
public:
    DivFlexLayoutTddTest();
    ~DivFlexLayoutTddTest() override = default;

    void ComponentDivStyleSetTest033();
    void ComponentDivStyleSetTest034();
    void ComponentDivStyleSetTest035();
    void ComponentDivStyleSetTest036();
    void StretchAlignApplyTest037();
    void ComponentDivStyleSetTest038();
    void ComponentDivFlexAlignSelfTest();
    void ComponentDivFlexShrinkGrowTest();
    void ComponentDivFlexAlignItemsCenterEndTest();
    void ComponentDivFlexDirectionReverseTest();
    void ComponentDivFlexDirectionInvalidTest();
    void ComponentDivFlexWrapStartTest();
    void ComponentDivFlexWrapEndTest();
    void ComponentDivFlexWrapCenterTest();
    void ComponentDivFlexWrapSpaceBetweenTest();
    void ComponentDivFlexColumnGapPercentTest();
    void ComponentDivFlexConstraintsTest();
    void ComponentDivFlexWrapStretchTest();
    void ComponentDivFlexDynamicAspectRatioTest();
    void ComponentDivGapSinglePropsTest();
    void ComponentDivGapNumberVariantsTest();
    void ComponentDivGapStringVariantsTest();
    void ComponentDivFlexRowGapPercentTest();
    void ComponentDivAspectRatioStringTest();
    void ComponentDivAbsoluteInsetsPixelClearTest();
    void ComponentDivUpdateViewOverflowAlignFlexTest();
    void ComponentDivUpdateViewConstraintKeysTest();
    void ComponentDivUpdateViewPositionKeysTest();
    void ComponentDivFlexInvalidFactorTest();
    void ComponentDivPositionAlignInvalidTest();
    void ComponentDivFlexColumnAspectRatioTest();
    void ComponentDivAttachViewColumnAspectRatioTest();

    void RunTests();

private:
    bool VerifyOverflowStyle(const char *overflowValue, OverflowMode expectedMode);
};
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_TEST_DIV_FLEX_LAYOUT_H
