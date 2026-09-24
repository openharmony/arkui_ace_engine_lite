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

#include "component_tdd_test.h"

#include <memory>

#include "div_component.h"

namespace OHOS {
namespace ACELite {
ComponentTddTest::ComponentTddTest() : BaseTest()
{
    uint8_t componentNameLength = 3;
    // a div is used as the vehicle to render the base Component under test
    componentNameId_ = KeyParser::ParseKeyId("div", componentNameLength);
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
namespace {
constexpr int16_t COMPONENT_TEST_WIDTH = 100;
constexpr int16_t COMPONENT_TEST_ASPECT_RATIO_PLAIN_RESULT = 178; // 1.78 * 100
constexpr int16_t COMPONENT_TEST_ASPECT_RATIO_PERCENT_RESULT = 50; // 50% -> ratio 0.5 -> scaled 50
constexpr double COMPONENT_TEST_ASPECT_RATIO_NEAR_ZERO = 0.0001;
constexpr int32_t COMPONENT_TEST_ASPECT_RATIO_OVERSIZE = 70000; // scaled 7,000,000 > UINT16_MAX
} // namespace

void ComponentTddTest::ExplicitFlagAutoAndPixel001()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. do not set width/height style, cross-axis sizes are auto,
     *                   HasExplicitWidth/HasExplicitHeight should be false
     */
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    UIViewGroup *rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_FALSE(rootView->HasExplicitWidth());
    EXPECT_FALSE(rootView->HasExplicitHeight());

    /* *
     * @tc.steps: step2. set width = 100, height = 50,
     *                   HasExplicitWidth/HasExplicitHeight should be true
     */
    int16_t width = 100;
    int16_t height = 50;
    JerrySetNumberProperty(styleObj_, "width", width);
    JerrySetNumberProperty(styleObj_, "height", height);
    component.reset(GetRenderedComponent(componentNameId_));
    rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(rootView->HasExplicitWidth());
    EXPECT_TRUE(rootView->HasExplicitHeight());
    TDD_CASE_END();
}

void ComponentTddTest::NegativePixelNotExplicit002()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set valid pixel width but negative pixel height
     */
    int16_t width = 100;
    int16_t negativeHeight = -1;
    JerrySetNumberProperty(styleObj_, "width", width);
    JerrySetNumberProperty(styleObj_, "height", negativeHeight); // negative pixel height
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    UIViewGroup *rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. valid pixel width is explicit, negative pixel height is NOT explicit,
     *                      so that stretch can still apply on the cross axis
     */
    EXPECT_TRUE(rootView->HasExplicitWidth());
    EXPECT_FALSE(rootView->HasExplicitHeight());
    TDD_CASE_END();
}

void ComponentTddTest::PercentDimensionExplicit003()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set percent height
     */
    JerrySetStringProperty(styleObj_, "height", "50%"); // 50%: percent height
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    UIViewGroup *rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. percent dimension should be marked explicit, not stretched
     */
    EXPECT_TRUE(rootView->HasExplicitHeight());
    TDD_CASE_END();
}

void ComponentTddTest::AspectRatioPlainString001()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set width = 100 and aspectRatio = "1.78" (plain string without '/')
     * @tc.expected: step1. ParseAspectRatioString plain-string branch converts it to 178
     */
    JerrySetNumberProperty(styleObj_, "width", COMPONENT_TEST_WIDTH);
    JerrySetStringProperty(styleObj_, "aspectRatio", "1.78");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    UIViewGroup *rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(rootView != nullptr);
    if (rootView != nullptr) {
        EXPECT_EQ(rootView->GetAspectRatio(), COMPONENT_TEST_ASPECT_RATIO_PLAIN_RESULT);
    }
    TDD_CASE_END();
}

void ComponentTddTest::AspectRatioInvalidValues002()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set width = 100 and aspectRatio = -1 (ratio <= 0)
     * @tc.expected: step1. ConvertAspectRatioValue rejects it, aspect ratio stays 0
     */
    JerrySetNumberProperty(styleObj_, "width", COMPONENT_TEST_WIDTH);
    JerrySetNumberProperty(styleObj_, "aspectRatio", -1);
    std::unique_ptr<Component> negativeComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *negativeView = reinterpret_cast<UIViewGroup *>(negativeComponent->GetComponentRootView());
    EXPECT_TRUE(negativeView != nullptr);
    if (negativeView != nullptr) {
        EXPECT_EQ(negativeView->GetAspectRatio(), 0);
    }

    /* *
     * @tc.steps: step2. set aspectRatio = 0 (ratio <= 0)
     */
    JerrySetNumberProperty(styleObj_, "aspectRatio", 0);
    std::unique_ptr<Component> zeroComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *zeroView = reinterpret_cast<UIViewGroup *>(zeroComponent->GetComponentRootView());
    EXPECT_TRUE(zeroView != nullptr);
    if (zeroView != nullptr) {
        EXPECT_EQ(zeroView->GetAspectRatio(), 0);
    }

    /* *
     * @tc.steps: step3. set aspectRatio = 70000 (scaled > UINT16_MAX, oversized ratio)
     */
    JerrySetNumberProperty(styleObj_, "aspectRatio", COMPONENT_TEST_ASPECT_RATIO_OVERSIZE);
    std::unique_ptr<Component> oversizeComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *oversizeView = reinterpret_cast<UIViewGroup *>(oversizeComponent->GetComponentRootView());
    EXPECT_TRUE(oversizeView != nullptr);
    if (oversizeView != nullptr) {
        EXPECT_EQ(oversizeView->GetAspectRatio(), 0);
    }

    /* *
     * @tc.steps: step4. set aspectRatio = 0.0001 (scaled rounds to 0, near-zero ratio)
     */
    JerrySetNumberProperty(styleObj_, "aspectRatio", COMPONENT_TEST_ASPECT_RATIO_NEAR_ZERO);
    std::unique_ptr<Component> nearZeroComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *nearZeroView = reinterpret_cast<UIViewGroup *>(nearZeroComponent->GetComponentRootView());
    EXPECT_TRUE(nearZeroView != nullptr);
    if (nearZeroView != nullptr) {
        EXPECT_EQ(nearZeroView->GetAspectRatio(), 0);
    }
    TDD_CASE_END();
}

void ComponentTddTest::AspectRatioInvalidStringsAndTypes003()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set aspectRatio = "abc" (invalid plain string)
     * @tc.expected: step1. ParseAspectRatioString rejects it, aspect ratio stays 0
     */
    JerrySetNumberProperty(styleObj_, "width", COMPONENT_TEST_WIDTH);
    JerrySetStringProperty(styleObj_, "aspectRatio", "abc");
    std::unique_ptr<Component> plainInvalidComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *plainInvalidView = reinterpret_cast<UIViewGroup *>(plainInvalidComponent->GetComponentRootView());
    EXPECT_TRUE(plainInvalidView != nullptr);
    if (plainInvalidView != nullptr) {
        EXPECT_EQ(plainInvalidView->GetAspectRatio(), 0);
    }

    /* *
     * @tc.steps: step2. set aspectRatio = "16/0" (invalid slash denominator)
     */
    JerrySetStringProperty(styleObj_, "aspectRatio", "16/0");
    std::unique_ptr<Component> slashInvalidComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *slashInvalidView = reinterpret_cast<UIViewGroup *>(slashInvalidComponent->GetComponentRootView());
    EXPECT_TRUE(slashInvalidView != nullptr);
    if (slashInvalidView != nullptr) {
        EXPECT_EQ(slashInvalidView->GetAspectRatio(), 0);
    }

    /* *
     * @tc.steps: step3. set aspectRatio = true (boolean value hits the default value-type arm)
     */
    jerry_value_t boolVal = jerry_create_boolean(true);
    jerry_value_t propName = jerry_create_string(reinterpret_cast<const jerry_char_t *>("aspectRatio"));
    jerry_set_property(styleObj_, propName, boolVal);
    jerry_release_value(propName);
    jerry_release_value(boolVal);
    std::unique_ptr<Component> boolInvalidComponent(GetRenderedComponent(componentNameId_));
    UIViewGroup *boolInvalidView = reinterpret_cast<UIViewGroup *>(boolInvalidComponent->GetComponentRootView());
    EXPECT_TRUE(boolInvalidView != nullptr);
    if (boolInvalidView != nullptr) {
        EXPECT_EQ(boolInvalidView->GetAspectRatio(), 0);
    }
    TDD_CASE_END();
}

void ComponentTddTest::AspectRatioPercent004()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set width = 100 and aspectRatio = "50%"
     * @tc.expected: step1. ApplyCommonStyle K_ASPECT_RATIO percent arm converts it to 50
     */
    JerrySetNumberProperty(styleObj_, "width", COMPONENT_TEST_WIDTH);
    JerrySetStringProperty(styleObj_, "aspectRatio", "50%");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    UIViewGroup *rootView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(rootView != nullptr);
    if (rootView != nullptr) {
        EXPECT_EQ(rootView->GetAspectRatio(), COMPONENT_TEST_ASPECT_RATIO_PERCENT_RESULT);
    }
    TDD_CASE_END();
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

void ComponentTddTest::RunTests()
{
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    ExplicitFlagAutoAndPixel001();
    NegativePixelNotExplicit002();
    PercentDimensionExplicit003();
    AspectRatioPlainString001();
    AspectRatioInvalidValues002();
    AspectRatioInvalidStringsAndTypes003();
    AspectRatioPercent004();
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
}

#ifdef TDD_ASSERTIONS
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
/* *
 * @tc.name: ExplicitFlagAutoAndPixel001
 * @tc.desc: Verify explicit pixel width/height styles set HasExplicitWidth/HasExplicitHeight true,
 *           and auto sizes leave them false.
 */
HWTEST_F(ComponentTddTest, component001, TestSize.Level1)
{
    ComponentTddTest::ExplicitFlagAutoAndPixel001();
}

/* *
 * @tc.name: NegativePixelNotExplicit002
 * @tc.desc: Verify negative pixel dimension is not marked explicit so that stretch can apply.
 */
HWTEST_F(ComponentTddTest, component002, TestSize.Level1)
{
    ComponentTddTest::NegativePixelNotExplicit002();
}

/* *
 * @tc.name: PercentDimensionExplicit003
 * @tc.desc: Verify percent dimension is marked explicit and not stretched.
 */
HWTEST_F(ComponentTddTest, component003, TestSize.Level1)
{
    ComponentTddTest::PercentDimensionExplicit003();
}

/* *
 * @tc.name: AspectRatioPlainString001
 * @tc.desc: Verify aspectRatio accepts a plain floating string without '/'.
 */
HWTEST_F(ComponentTddTest, component004, TestSize.Level1)
{
    ComponentTddTest::AspectRatioPlainString001();
}

/* *
 * @tc.name: AspectRatioInvalidValues002
 * @tc.desc: Verify ConvertAspectRatioValue rejects negative, zero, oversized and near-zero ratios.
 */
HWTEST_F(ComponentTddTest, component005, TestSize.Level1)
{
    ComponentTddTest::AspectRatioInvalidValues002();
}

/* *
 * @tc.name: AspectRatioInvalidStringsAndTypes003
 * @tc.desc: Verify ParseAspectRatioString rejects invalid strings and ApplyCommonStyle rejects boolean aspectRatio.
 */
HWTEST_F(ComponentTddTest, component006, TestSize.Level1)
{
    ComponentTddTest::AspectRatioInvalidStringsAndTypes003();
}

/* *
 * @tc.name: AspectRatioPercent004
 * @tc.desc: Verify ApplyCommonStyle K_ASPECT_RATIO percent arm converts "50%" correctly.
 */
HWTEST_F(ComponentTddTest, component007, TestSize.Level1)
{
    ComponentTddTest::AspectRatioPercent004();
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#endif
} // namespace ACELite
} // namespace OHOS
