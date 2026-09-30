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

#include "div_flex_layout_tdd_test.h"

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
#include <memory>

#include "component_factory.h"
#include "component_utils.h"
#include "flex_layout.h"
#include "key_parser.h"

namespace OHOS {
namespace ACELite {
namespace {
constexpr int16_t FLEX_TEST_CONTAINER_WIDTH = 400;
constexpr int16_t FLEX_TEST_CONTAINER_HEIGHT = 300;
constexpr int16_t FLEX_TEST_SHRINK_CONTAINER_WIDTH = 200;
constexpr int16_t FLEX_TEST_GROW_CONTAINER_WIDTH = 600;
constexpr int16_t FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH = 250;
constexpr int16_t FLEX_TEST_CHILD_WIDTH = 100;
constexpr int16_t FLEX_TEST_CHILD_HEIGHT = 50;
constexpr int16_t FLEX_TEST_CHILD_WIDE_WIDTH = 300;
constexpr int16_t FLEX_TEST_CHILD_MIN_WIDTH = 80;
constexpr int16_t FLEX_TEST_CHILD_MAX_WIDTH = 250;
constexpr int16_t FLEX_TEST_CHILD_MIN_HEIGHT = 40;
constexpr int16_t FLEX_TEST_FIXED_CHILD_WIDTH = 120;
constexpr int16_t FLEX_TEST_FIXED_CHILD_HEIGHT = 60;
constexpr int16_t FLEX_TEST_MIN_HEIGHT_CHILD_HEIGHT = 20;
constexpr int16_t FLEX_TEST_MULTI_LINE_CHILD_WIDTH = 80;
constexpr double FLEX_TEST_ASPECT_RATIO = 2.0;
constexpr int16_t FLEX_TEST_ASPECT_RATIO_HEIGHT =
    static_cast<int16_t>(FLEX_TEST_CHILD_WIDTH / FLEX_TEST_ASPECT_RATIO);
constexpr uint8_t FLEX_TEST_CHILD_COUNT_ONE = 1;
constexpr uint8_t FLEX_TEST_CHILD_COUNT_TWO = 2;
constexpr uint8_t FLEX_TEST_CHILD_COUNT_FOUR = 4;
constexpr uint8_t FLEX_TEST_CHILD_INDEX_FIRST = 0;
constexpr uint8_t FLEX_TEST_CHILD_INDEX_SECOND = 1;
constexpr uint8_t FLEX_TEST_CHILD_INDEX_THIRD = 2;
constexpr uint8_t FLEX_TEST_CHILD_INDEX_FOURTH = 3;

constexpr int16_t FLEX_TEST_GAP_ROW = 10;
constexpr int16_t FLEX_TEST_GAP_COL = 20;
constexpr int16_t FLEX_TEST_GAP_PIXEL = 15;
constexpr int16_t FLEX_TEST_GAP_NEGATIVE = -5;
constexpr int16_t FLEX_TEST_GAP_CLAMPED = 0;
constexpr int16_t FLEX_TEST_COLUMN_GAP_PIXEL = 30;
constexpr int16_t FLEX_TEST_ROW_GAP_PERCENT = 50;
constexpr int16_t FLEX_TEST_EXPECTED_SECOND_CHILD_Y = 200;
constexpr int16_t FLEX_TEST_EXPECTED_SECOND_CHILD_X = 280;
constexpr int16_t FLEX_TEST_ALIGN_ITEMS_CENTER_Y = 125;
constexpr int16_t FLEX_TEST_ALIGN_ITEMS_END_Y = 250;
constexpr int16_t FLEX_TEST_ALIGN_CONTENT_CENTER_OFFSET = 100;
constexpr int16_t FLEX_TEST_LEFT_PIXEL = 10;
constexpr int16_t FLEX_TEST_TOP_PIXEL = 5;
constexpr int16_t FLEX_TEST_LEFT_PERCENT = 20;
constexpr int16_t FLEX_TEST_RIGHT_PIXEL = 50;
constexpr int16_t FLEX_TEST_BOTTOM_PIXEL = 25;
constexpr int16_t FLEX_TEST_BOTTOM_PERCENT = 25;
constexpr int16_t FLEX_TEST_ASPECT_RATIO_STRING_RESULT = 178;
constexpr uint16_t FLEX_TEST_FLEX_GROW = 1;
constexpr uint16_t FLEX_TEST_FLEX_SHRINK = 1;
constexpr int16_t FLEX_TEST_FLEX_BASIS = 100;
constexpr int16_t FLEX_TEST_MIN_WIDTH = 80;
constexpr int16_t FLEX_TEST_MAX_WIDTH = 300;
constexpr int16_t FLEX_TEST_MIN_HEIGHT = 40;
constexpr int16_t FLEX_TEST_MAX_HEIGHT = 200;
constexpr double FLEX_TEST_ASPECT_RATIO_INPUT = 2.0;
constexpr int16_t FLEX_TEST_ASPECT_RATIO_SCALED = 200;
constexpr int16_t FLEX_TEST_INVALID_NEGATIVE = -1;
constexpr int16_t FLEX_TEST_INVALID_NEGATIVE_SMALL = -5;
constexpr int32_t FLEX_TEST_OVERSIZE_UINT16 = 70000;
constexpr int32_t FLEX_TEST_OVERSIZE_INT16 = 40000;

class StyleBuilder {
public:
    StyleBuilder() : styleObj_(jerry_create_object()) {}
    ~StyleBuilder()
    {
        if (styleObj_ != 0) {
            jerry_release_value(styleObj_);
        }
    }
    StyleBuilder(const StyleBuilder&) = delete;
    StyleBuilder& operator=(const StyleBuilder&) = delete;

    StyleBuilder& SetNumber(const char* key, double value)
    {
        JerrySetNumberProperty(styleObj_, key, value);
        return *this;
    }

    StyleBuilder& SetString(const char* key, const char* value)
    {
        if (value != nullptr) {
            JerrySetStringProperty(styleObj_, key, value);
        }
        return *this;
    }

    jerry_value_t Detach()
    {
        jerry_value_t ret = styleObj_;
        styleObj_ = 0;
        return ret;
    }

    jerry_value_t Get() const
    {
        return styleObj_;
    }

private:
    jerry_value_t styleObj_;
};

class ComponentHandle {
public:
    explicit ComponentHandle(Component* comp = nullptr) : comp_(comp) {}
    ~ComponentHandle()
    {
        Reset();
    }
    ComponentHandle(const ComponentHandle&) = delete;
    ComponentHandle& operator=(const ComponentHandle&) = delete;

    void Reset(Component* comp = nullptr)
    {
        if (comp_ != nullptr) {
            ComponentUtils::ReleaseComponents(comp_);
        }
        comp_ = comp;
    }

    UIViewGroup* GetContainer() const
    {
        if (comp_ == nullptr) {
            return nullptr;
        }
        return static_cast<UIViewGroup*>(comp_->GetComponentRootView());
    }

    UIView* GetChildAt(uint8_t index) const
    {
        UIViewGroup* container = GetContainer();
        if (container == nullptr) {
            return nullptr;
        }
        UIView* child = container->GetChildrenHead();
        for (uint8_t i = 0; i < index && child != nullptr; i++) {
            child = child->GetNextSibling();
        }
        return child;
    }

private:
    Component* comp_;
};

class DivComponentBuilder {
public:
    explicit DivComponentBuilder(RootComponentMock& root)
        : root_(root), options_(0), style_(0), children_(0), childCount_(0), currentChildIndex_(0)
    {}

    ~DivComponentBuilder()
    {
        Reset();
    }

    DivComponentBuilder(const DivComponentBuilder&) = delete;
    DivComponentBuilder& operator=(const DivComponentBuilder&) = delete;

    DivComponentBuilder& SetContainerStyle(StyleBuilder& containerStyle)
    {
        if (style_ != 0) {
            jerry_release_value(style_);
        }
        style_ = containerStyle.Detach();
        return *this;
    }

    DivComponentBuilder& SetChildCount(uint8_t count)
    {
        if (children_ != 0) {
            jerry_release_value(children_);
        }
        childCount_ = count;
        children_ = jerry_create_array(count);
        currentChildIndex_ = 0;
        return *this;
    }

    DivComponentBuilder& AddChild(const StyleBuilder& childStyle)
    {
        EnsureChildrenArray();
        jerry_value_t childOptions = jerry_create_object();
        JerrySetNamedProperty(childOptions, "staticStyle", childStyle.Get());
        jerry_value_t nullChildren = jerry_create_null();
        Component* child = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("div"), childOptions, nullChildren);
        if (child != nullptr) {
            child->Render();
            jerry_release_value(jerry_set_property_by_index(children_, currentChildIndex_, child->GetNativeElement()));
            currentChildIndex_++;
        }
        jerry_release_value(nullChildren);
        jerry_release_value(childOptions);
        return *this;
    }

    Component* Build()
    {
        options_ = jerry_create_object();
        if (style_ != 0) {
            JerrySetNamedProperty(options_, "staticStyle", style_);
        }
        Component* comp = ComponentFactory::CreateComponent(KeyParser::ParseKeyId("div"), options_, children_);
        if (comp != nullptr) {
            root_.RenderComponent(*comp);
        }
        Reset();
        return comp;
    }

private:
    void Reset()
    {
        if (options_ != 0) {
            jerry_release_value(options_);
            options_ = 0;
        }
        if (style_ != 0) {
            jerry_release_value(style_);
            style_ = 0;
        }
        if (children_ != 0) {
            jerry_release_value(children_);
            children_ = 0;
        }
        childCount_ = 0;
        currentChildIndex_ = 0;
    }

    void EnsureChildrenArray()
    {
        if (children_ == 0) {
            children_ = jerry_create_array(childCount_);
        }
    }

    RootComponentMock& root_;
    jerry_value_t options_;
    jerry_value_t style_;
    jerry_value_t children_;
    uint8_t childCount_;
    uint8_t currentChildIndex_;
};

void VerifyFixedSizeChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder fixedContainerStyle;
    fixedContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                       .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                       .SetString("flexDirection", "row");
    StyleBuilder fixedChildStyle;
    fixedChildStyle.SetNumber("width", FLEX_TEST_FIXED_CHILD_WIDTH)
                   .SetNumber("height", FLEX_TEST_FIXED_CHILD_HEIGHT);
    DivComponentBuilder fixedBuilder(rootComponentMock);
    fixedBuilder.SetContainerStyle(fixedContainerStyle)
                .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                .AddChild(fixedChildStyle);
    ComponentHandle fixedHandle(fixedBuilder.Build());
    UIView *fixedChild = fixedHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(fixedChild != nullptr);
    if (fixedChild != nullptr) {
        EXPECT_EQ(fixedChild->GetWidth(), FLEX_TEST_FIXED_CHILD_WIDTH);
        EXPECT_EQ(fixedChild->GetHeight(), FLEX_TEST_FIXED_CHILD_HEIGHT);
    }
}

void VerifyMinWidthChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CHILD_MIN_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder minWidthContainerStyle;
    minWidthContainerStyle.SetNumber("width", FLEX_TEST_CHILD_MIN_WIDTH)
                          .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                          .SetString("flexDirection", "row");
    StyleBuilder minWidthChildStyle;
    minWidthChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDE_WIDTH)
                      .SetNumber("height", FLEX_TEST_CHILD_HEIGHT)
                      .SetNumber("flexShrink", 1)
                      .SetNumber("minWidth", FLEX_TEST_CHILD_MIN_WIDTH);
    DivComponentBuilder minWidthBuilder(rootComponentMock);
    minWidthBuilder.SetContainerStyle(minWidthContainerStyle)
                   .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                   .AddChild(minWidthChildStyle);
    ComponentHandle minWidthHandle(minWidthBuilder.Build());
    UIView *minWidthChild = minWidthHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(minWidthChild != nullptr);
    if (minWidthChild != nullptr) {
        EXPECT_EQ(minWidthChild->GetWidth(), FLEX_TEST_CHILD_MIN_WIDTH);
    }
}

void VerifyMaxWidthChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_GROW_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder maxWidthContainerStyle;
    maxWidthContainerStyle.SetNumber("width", FLEX_TEST_GROW_CONTAINER_WIDTH)
                          .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                          .SetString("flexDirection", "row");
    StyleBuilder maxWidthChildStyle;
    maxWidthChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDE_WIDTH)
                      .SetNumber("height", FLEX_TEST_CHILD_HEIGHT)
                      .SetNumber("flexGrow", 1)
                      .SetNumber("maxWidth", FLEX_TEST_CHILD_MAX_WIDTH);
    DivComponentBuilder maxWidthBuilder(rootComponentMock);
    maxWidthBuilder.SetContainerStyle(maxWidthContainerStyle)
                   .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                   .AddChild(maxWidthChildStyle);
    ComponentHandle maxWidthHandle(maxWidthBuilder.Build());
    UIView *maxWidthChild = maxWidthHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(maxWidthChild != nullptr);
    if (maxWidthChild != nullptr) {
        EXPECT_EQ(maxWidthChild->GetWidth(), FLEX_TEST_CHILD_MAX_WIDTH);
    }
}

void VerifyMinHeightChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder minHeightContainerStyle;
    minHeightContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                           .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                           .SetString("flexDirection", "row");
    StyleBuilder minHeightChildStyle;
    minHeightChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDE_WIDTH)
                       .SetNumber("height", FLEX_TEST_MIN_HEIGHT_CHILD_HEIGHT)
                       .SetNumber("minHeight", FLEX_TEST_CHILD_MIN_HEIGHT);
    DivComponentBuilder minHeightBuilder(rootComponentMock);
    minHeightBuilder.SetContainerStyle(minHeightContainerStyle)
                    .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                    .AddChild(minHeightChildStyle);
    ComponentHandle minHeightHandle(minHeightBuilder.Build());
    UIView *minHeightChild = minHeightHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(minHeightChild != nullptr);
    if (minHeightChild != nullptr) {
        EXPECT_EQ(minHeightChild->GetHeight(), FLEX_TEST_CHILD_MIN_HEIGHT);
    }
}

void VerifyMaxHeightChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder maxHeightContainerStyle;
    maxHeightContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                           .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                           .SetString("flexDirection", "row");
    StyleBuilder maxHeightChildStyle;
    maxHeightChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
                       .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                       .SetNumber("maxHeight", FLEX_TEST_CHILD_HEIGHT);
    DivComponentBuilder maxHeightBuilder(rootComponentMock);
    maxHeightBuilder.SetContainerStyle(maxHeightContainerStyle)
                    .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                    .AddChild(maxHeightChildStyle);
    ComponentHandle maxHeightHandle(maxHeightBuilder.Build());
    UIView *maxHeightChild = maxHeightHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(maxHeightChild != nullptr);
    if (maxHeightChild != nullptr) {
        EXPECT_EQ(maxHeightChild->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
    }
}

void VerifyAspectRatioChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder aspectRatioContainerStyle;
    aspectRatioContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                             .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                             .SetString("flexDirection", "row");
    StyleBuilder aspectRatioChildStyle;
    aspectRatioChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
                         .SetNumber("aspectRatio", FLEX_TEST_ASPECT_RATIO);
    DivComponentBuilder aspectRatioBuilder(rootComponentMock);
    aspectRatioBuilder.SetContainerStyle(aspectRatioContainerStyle)
                      .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                      .AddChild(aspectRatioChildStyle);
    ComponentHandle aspectRatioHandle(aspectRatioBuilder.Build());
    UIView *aspectRatioChild = aspectRatioHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(aspectRatioChild != nullptr);
    if (aspectRatioChild != nullptr) {
        EXPECT_EQ(aspectRatioChild->GetHeight(), FLEX_TEST_ASPECT_RATIO_HEIGHT);
    }
}

void VerifyFlexBasisChild(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder flexBasisContainerStyle;
    flexBasisContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                           .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                           .SetString("flexDirection", "row");
    StyleBuilder flexBasisChildStyle;
    flexBasisChildStyle.SetNumber("flexBasis", FLEX_TEST_FIXED_CHILD_WIDTH)
                       .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);
    DivComponentBuilder flexBasisBuilder(rootComponentMock);
    flexBasisBuilder.SetContainerStyle(flexBasisContainerStyle)
                    .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                    .AddChild(flexBasisChildStyle);
    ComponentHandle flexBasisHandle(flexBasisBuilder.Build());
    UIView *flexBasisChild = flexBasisHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(flexBasisChild != nullptr);
    if (flexBasisChild != nullptr) {
        EXPECT_EQ(flexBasisChild->GetWidth(), FLEX_TEST_FIXED_CHILD_WIDTH);
    }
}

void VerifyDynamicAspectRatioRelayout(RootComponentMock &rootComponentMock)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row");
    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
              .SetNumber("aspectRatio", FLEX_TEST_ASPECT_RATIO);
    DivComponentBuilder builder(rootComponentMock);
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
           .AddChild(childStyle);
    ComponentHandle handle(builder.Build());
    UIView *child = handle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(child != nullptr);
    if (child != nullptr) {
        EXPECT_EQ(child->GetHeight(), FLEX_TEST_ASPECT_RATIO_HEIGHT);
    }
}

void VerifyAlignContentPosition(RootComponentMock &rootComponentMock, const char *alignContent, int16_t firstY,
                                int16_t fourthY)
{
    rootComponentMock.PrepareRootContainer(FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("flexWrap", "wrap")
                  .SetString("alignContent", alignContent);
    DivComponentBuilder builder(rootComponentMock);
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_FOUR)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle);
    ComponentHandle handle(builder.Build());
    UIView *firstChild = handle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *fourthChild = handle.GetChildAt(FLEX_TEST_CHILD_INDEX_FOURTH);
    EXPECT_TRUE(firstChild != nullptr);
    EXPECT_TRUE(fourthChild != nullptr);
    if (firstChild != nullptr) {
        EXPECT_EQ(firstChild->GetY(), firstY);
    }
    if (fourthChild != nullptr) {
        EXPECT_EQ(fourthChild->GetY(), fourthY);
    }
}
} // namespace

DivFlexLayoutTddTest::DivFlexLayoutTddTest()
{
    componentNameId_ = KeyParser::ParseKeyId("div");
}

bool DivFlexLayoutTddTest::VerifyOverflowStyle(const char *overflowValue, OverflowMode expectedMode)
{
    if (overflowValue != nullptr) {
        JerrySetStringProperty(styleObj_, "overflow", overflowValue);
    }
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        return false;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_EQ(divView->GetOverflow(), expectedMode);
    return true;
}

void DivFlexLayoutTddTest::ComponentDivStyleSetTest033()
{
    TDD_CASE_BEGIN();
    bool result = VerifyOverflowStyle(nullptr, OverflowMode::OVERFLOW_VISIBLE);
    result = result && VerifyOverflowStyle("hidden", OverflowMode::OVERFLOW_HIDDEN);
    result = result && VerifyOverflowStyle("visible", OverflowMode::OVERFLOW_VISIBLE);
    EXPECT_TRUE(result);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivStyleSetTest034()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style width = 100 and marginLeft = "auto"
     */
    uint8_t width = 100;
    JerrySetNumberProperty(styleObj_, "width", width);
    JerrySetStringProperty(styleObj_, "marginLeft", "auto");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. marginLeft dimension type is TYPE_AUTO, stale pixel margin is 0,
     *                      view auto flag is set
     * @tc.note: the auto margin position shift is produced by the flex layout of the parent
     *           container, which is not triggered in this render flow, so the x coordinate
     *           is not asserted here (covered by OverflowMarginTddTest.MarginAutoRow002).
     */
    bool dimensionAuto = (component->GetDimension(K_MARGIN_LEFT).type == DimensionType::TYPE_AUTO);
    bool pixelMarginZero = (divView->GetStyle(STYLE_MARGIN_LEFT) == 0);
    bool autoFlagSet = divView->IsMarginLeftAuto();
    EXPECT_TRUE(dimensionAuto);
    EXPECT_TRUE(pixelMarginZero);
    EXPECT_TRUE(autoFlagSet);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivStyleSetTest035()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. set style marginLeft = "20" (string value which is not auto)
     */
    JerrySetStringProperty(styleObj_, "marginLeft", "20");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    /* *
     * @tc.expected: step1. marginLeft falls back to pixel 20, view auto flag is not set
     */
    uint8_t marginLeft = 20;
    Dimension margin = component->GetDimension(K_MARGIN_LEFT);
    bool isPixel = (margin.type == DimensionType::TYPE_PIXEL) && (margin.value.pixel == marginLeft);
    EXPECT_TRUE(isPixel);
    EXPECT_EQ(divView->GetStyle(STYLE_MARGIN_LEFT), marginLeft);
    EXPECT_FALSE(divView->IsMarginLeftAuto());
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivStyleSetTest036()
{
    TDD_CASE_BEGIN();
    JerrySetStringProperty(styleObj_, "gap", "10 20");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_ROW); // 10: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_COL); // 20: column gap
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::StretchAlignApplyTest037()
{
    TDD_CASE_BEGIN();
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    /* *
     * @tc.steps: step1. set align-items to stretch via UpdateView
     * @tc.expected: step1. stretch is recognized and applied, UpdateView returns true
     */
    jerry_value_t stretchVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("stretch"));
    EXPECT_TRUE(component->UpdateView(K_ALIGN_ITEMS, stretchVal));
    jerry_release_value(stretchVal);
    /* *
     * @tc.steps: step2. set align-items to an illegal value
     * @tc.expected: step2. illegal value is rejected, UpdateView returns false
     */
    jerry_value_t invalidVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("stretchX"));
    EXPECT_FALSE(component->UpdateView(K_ALIGN_ITEMS, invalidVal));
    jerry_release_value(invalidVal);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivStyleSetTest038()
{
    TDD_CASE_BEGIN();
    JerrySetStringProperty(styleObj_, "position", "absolute");
    JerrySetStringProperty(styleObj_, "right", "50%");
    JerrySetStringProperty(styleObj_, "bottom", "25%");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetPositionType(), POSITION_ABSOLUTE);
    EXPECT_TRUE(divView->HasFlexRight());
    EXPECT_TRUE(divView->IsFlexRightPercent());
    EXPECT_FLOAT_EQ(divView->GetFlexRightPercent(), static_cast<float>(FLEX_TEST_ROW_GAP_PERCENT)); // 50: right percent
    EXPECT_TRUE(divView->HasFlexBottom());
    EXPECT_TRUE(divView->IsFlexBottomPercent());
    EXPECT_FLOAT_EQ(divView->GetFlexBottomPercent(),
                    static_cast<float>(FLEX_TEST_BOTTOM_PERCENT)); // 25: bottom percent
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexAlignSelfTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("alignItems", "flex-start");

    StyleBuilder stretchChildStyle;
    stretchChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
                     .SetString("alignSelf", "stretch");

    StyleBuilder normalChildStyle;
    normalChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
                    .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
           .AddChild(stretchChildStyle)
           .AddChild(normalChildStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *firstChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *secondChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(firstChild != nullptr);
    EXPECT_TRUE(secondChild != nullptr);
    if (firstChild != nullptr) {
        EXPECT_EQ(firstChild->GetHeight(), FLEX_TEST_CONTAINER_HEIGHT);
        EXPECT_EQ(firstChild->GetAlignSelf(), UIView::ALIGN_SELF_STRETCH);
    }
    if (secondChild != nullptr) {
        EXPECT_EQ(secondChild->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexAlignItemsCenterEndTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    StyleBuilder centerContainerStyle;
    centerContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                        .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                        .SetString("flexDirection", "row")
                        .SetString("alignItems", "center");

    DivComponentBuilder centerBuilder(GetRootComponentMock());
    centerBuilder.SetContainerStyle(centerContainerStyle)
                 .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                 .AddChild(childStyle);
    ComponentHandle centerHandle(centerBuilder.Build());
    UIView *centerChild = centerHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(centerChild != nullptr);
    if (centerChild != nullptr) {
        EXPECT_EQ(centerChild->GetY(), FLEX_TEST_ALIGN_ITEMS_CENTER_Y);
    }

    StyleBuilder endContainerStyle;
    endContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                     .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                     .SetString("flexDirection", "row")
                     .SetString("alignItems", "flex-end");
    DivComponentBuilder endBuilder(GetRootComponentMock());
    endBuilder.SetContainerStyle(endContainerStyle)
              .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
              .AddChild(childStyle);
    ComponentHandle endHandle(endBuilder.Build());
    UIView *endChild = endHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(endChild != nullptr);
    if (endChild != nullptr) {
        EXPECT_EQ(endChild->GetY(), FLEX_TEST_ALIGN_ITEMS_END_Y);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexDirectionReverseTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    /* row-reverse: first child should be placed after the second child */
    StyleBuilder rowReverseContainerStyle;
    rowReverseContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                            .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                            .SetString("flexDirection", "row-reverse");
    DivComponentBuilder rowReverseBuilder(GetRootComponentMock());
    rowReverseBuilder.SetContainerStyle(rowReverseContainerStyle)
                     .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
                     .AddChild(childStyle)
                     .AddChild(childStyle);
    ComponentHandle rowReverseHandle(rowReverseBuilder.Build());
    UIView *rowReverseFirst = rowReverseHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *rowReverseSecond = rowReverseHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(rowReverseFirst != nullptr);
    EXPECT_TRUE(rowReverseSecond != nullptr);
    if (rowReverseFirst != nullptr && rowReverseSecond != nullptr) {
        EXPECT_GT(rowReverseFirst->GetX(), rowReverseSecond->GetX());
    }

    /* column-reverse: first child should be placed after the second child */
    StyleBuilder columnReverseContainerStyle;
    columnReverseContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                               .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                               .SetString("flexDirection", "column-reverse");
    DivComponentBuilder columnReverseBuilder(GetRootComponentMock());
    columnReverseBuilder.SetContainerStyle(columnReverseContainerStyle)
                        .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
                        .AddChild(childStyle)
                        .AddChild(childStyle);
    ComponentHandle columnReverseHandle(columnReverseBuilder.Build());
    UIView *columnReverseFirst = columnReverseHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *columnReverseSecond = columnReverseHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(columnReverseFirst != nullptr);
    EXPECT_TRUE(columnReverseSecond != nullptr);
    if (columnReverseFirst != nullptr && columnReverseSecond != nullptr) {
        EXPECT_GT(columnReverseFirst->GetY(), columnReverseSecond->GetY());
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexDirectionInvalidTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    /* invalid direction falls back to default row layout */
    StyleBuilder invalidContainerStyle;
    invalidContainerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                         .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                         .SetString("flexDirection", "invalid-direction");
    DivComponentBuilder invalidBuilder(GetRootComponentMock());
    invalidBuilder.SetContainerStyle(invalidContainerStyle)
                  .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
                  .AddChild(childStyle)
                  .AddChild(childStyle);
    ComponentHandle invalidHandle(invalidBuilder.Build());
    UIView *invalidFirst = invalidHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *invalidSecond = invalidHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(invalidFirst != nullptr);
    EXPECT_TRUE(invalidSecond != nullptr);
    if (invalidFirst != nullptr && invalidSecond != nullptr) {
        EXPECT_EQ(invalidFirst->GetX(), 0);
        EXPECT_EQ(invalidSecond->GetX(), FLEX_TEST_CHILD_WIDTH);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexShrinkGrowTest()
{
    TDD_CASE_BEGIN();

    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_SHRINK_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder shrinkContainerStyle;
    shrinkContainerStyle.SetNumber("width", FLEX_TEST_SHRINK_CONTAINER_WIDTH)
                        .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                        .SetString("flexDirection", "row");
    StyleBuilder shrinkChildStyle;
    shrinkChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDE_WIDTH)
                    .SetNumber("height", FLEX_TEST_CHILD_HEIGHT)
                    .SetNumber("flexShrink", 1)
                    .SetNumber("minWidth", FLEX_TEST_CHILD_MIN_WIDTH);

    DivComponentBuilder shrinkBuilder(GetRootComponentMock());
    shrinkBuilder.SetContainerStyle(shrinkContainerStyle)
                 .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
                 .AddChild(shrinkChildStyle);
    ComponentHandle shrinkHandle(shrinkBuilder.Build());
    UIView *shrinkChild = shrinkHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(shrinkChild != nullptr);
    if (shrinkChild != nullptr) {
        EXPECT_EQ(shrinkChild->GetWidth(), FLEX_TEST_SHRINK_CONTAINER_WIDTH);
    }

    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_GROW_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);
    StyleBuilder growContainerStyle;
    growContainerStyle.SetNumber("width", FLEX_TEST_GROW_CONTAINER_WIDTH)
                      .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                      .SetString("flexDirection", "row");
    StyleBuilder growChildStyle;
    growChildStyle.SetNumber("width", FLEX_TEST_CHILD_WIDE_WIDTH)
                  .SetNumber("height", FLEX_TEST_CHILD_HEIGHT)
                  .SetNumber("flexGrow", 1)
                  .SetNumber("maxWidth", FLEX_TEST_CHILD_MAX_WIDTH);

    DivComponentBuilder growBuilder(GetRootComponentMock());
    growBuilder.SetContainerStyle(growContainerStyle)
               .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
               .AddChild(growChildStyle);
    ComponentHandle growHandle(growBuilder.Build());
    UIView *growChild = growHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(growChild != nullptr);
    if (growChild != nullptr) {
        EXPECT_EQ(growChild->GetWidth(), FLEX_TEST_CHILD_MAX_WIDTH);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexWrapCenterTest()
{
    TDD_CASE_BEGIN();
    RootComponentMock &rootComponentMock = GetRootComponentMock();
    VerifyAlignContentPosition(rootComponentMock, "center",
                               FLEX_TEST_ALIGN_CONTENT_CENTER_OFFSET,
                               FLEX_TEST_ALIGN_CONTENT_CENTER_OFFSET + FLEX_TEST_CHILD_HEIGHT);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexWrapSpaceBetweenTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("flexWrap", "wrap")
                  .SetString("alignContent", "space-between");

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_FOUR)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *firstChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *thirdChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_THIRD);
    UIView *fourthChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FOURTH);
    EXPECT_TRUE(firstChild != nullptr);
    EXPECT_TRUE(thirdChild != nullptr);
    EXPECT_TRUE(fourthChild != nullptr);
    if (firstChild != nullptr) {
        EXPECT_EQ(firstChild->GetY(), 0);
    }
    if (thirdChild != nullptr) {
        EXPECT_EQ(thirdChild->GetY(), 0);
    }
    if (fourthChild != nullptr) {
        EXPECT_EQ(fourthChild->GetY(), FLEX_TEST_CONTAINER_HEIGHT - FLEX_TEST_CHILD_HEIGHT);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexWrapStartTest()
{
    TDD_CASE_BEGIN();
    RootComponentMock &rootComponentMock = GetRootComponentMock();
    VerifyAlignContentPosition(rootComponentMock, "flex-start", 0, FLEX_TEST_CHILD_HEIGHT);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexWrapEndTest()
{
    TDD_CASE_BEGIN();
    RootComponentMock &rootComponentMock = GetRootComponentMock();
    VerifyAlignContentPosition(rootComponentMock, "flex-end",
                               FLEX_TEST_CONTAINER_HEIGHT - FLEX_TEST_CHILD_HEIGHT * FLEX_TEST_CHILD_COUNT_TWO,
                               FLEX_TEST_CONTAINER_HEIGHT - FLEX_TEST_CHILD_HEIGHT);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexConstraintsTest()
{
    TDD_CASE_BEGIN();
    RootComponentMock &rootComponentMock = GetRootComponentMock();
    VerifyFixedSizeChild(rootComponentMock);
    VerifyMinWidthChild(rootComponentMock);
    VerifyMaxWidthChild(rootComponentMock);
    VerifyMinHeightChild(rootComponentMock);
    VerifyMaxHeightChild(rootComponentMock);
    VerifyAspectRatioChild(rootComponentMock);
    VerifyFlexBasisChild(rootComponentMock);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexWrapStretchTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("flexWrap", "wrap")
                  .SetString("alignContent", "stretch");

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CHILD_WIDTH)
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_FOUR)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *firstChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    UIView *thirdChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_THIRD);
    UIView *fourthChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FOURTH);
    EXPECT_TRUE(firstChild != nullptr);
    EXPECT_TRUE(thirdChild != nullptr);
    EXPECT_TRUE(fourthChild != nullptr);
    if (firstChild != nullptr) {
        EXPECT_EQ(firstChild->GetY(), 0);
        EXPECT_EQ(firstChild->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
    }
    if (thirdChild != nullptr) {
        EXPECT_EQ(thirdChild->GetY(), 0);
        EXPECT_EQ(thirdChild->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
    }
    if (fourthChild != nullptr) {
        EXPECT_EQ(fourthChild->GetY(), FLEX_TEST_CONTAINER_HEIGHT / FLEX_TEST_CHILD_COUNT_TWO);
        EXPECT_EQ(fourthChild->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexDynamicAspectRatioTest()
{
    TDD_CASE_BEGIN();
    VerifyDynamicAspectRatioRelayout(GetRootComponentMock());
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivGapSinglePropsTest()
{
    TDD_CASE_BEGIN();
    /* rowGap/columnGap single properties with pixel values */
    JerrySetStringProperty(styleObj_, "rowGap", "10");
    JerrySetStringProperty(styleObj_, "columnGap", "30");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_ROW); // 10: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_COLUMN_GAP_PIXEL); // 30: column gap

    /* negative values are clamped to zero */
    jerry_value_t negVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("-5"));
    EXPECT_TRUE(component->UpdateView(K_ROW_GAP, negVal));
    jerry_release_value(negVal);
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_CLAMPED); // 0: clamped

    /* oversize values are clamped to INT16_MAX */
    jerry_value_t bigRowGap = jerry_create_number(FLEX_TEST_OVERSIZE_INT16); // 40000
    EXPECT_TRUE(component->UpdateView(K_ROW_GAP, bigRowGap));
    jerry_release_value(bigRowGap);
    EXPECT_EQ(divView->GetRowGap(), INT16_MAX);

    jerry_value_t bigColumnGap = jerry_create_number(FLEX_TEST_OVERSIZE_INT16); // 40000
    EXPECT_TRUE(component->UpdateView(K_COLUMN_GAP, bigColumnGap));
    jerry_release_value(bigColumnGap);
    EXPECT_EQ(divView->GetColumnGap(), INT16_MAX);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivGapNumberVariantsTest()
{
    TDD_CASE_BEGIN();
    /* number value sets both row and column gap */
    JerrySetNumberProperty(styleObj_, "gap", FLEX_TEST_GAP_PIXEL);
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_PIXEL); // 15: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_PIXEL); // 15: column gap

    /* negative number is clamped to zero */
    jerry_value_t negVal = jerry_create_number(FLEX_TEST_GAP_NEGATIVE);
    EXPECT_TRUE(component->UpdateView(K_GAP, negVal));
    jerry_release_value(negVal);
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_CLAMPED); // 0: clamped
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_CLAMPED); // 0: clamped

    /* oversize number is clamped to INT16_MAX */
    jerry_value_t bigVal = jerry_create_number(FLEX_TEST_OVERSIZE_INT16); // 40000
    EXPECT_TRUE(component->UpdateView(K_GAP, bigVal));
    jerry_release_value(bigVal);
    EXPECT_EQ(divView->GetRowGap(), INT16_MAX);
    EXPECT_EQ(divView->GetColumnGap(), INT16_MAX);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivGapStringVariantsTest()
{
    TDD_CASE_BEGIN();
    /* single string value applies to both row and column gap */
    JerrySetStringProperty(styleObj_, "gap", "10");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_ROW); // 10: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_ROW); // 10: column gap

    /* leading spaces before the first token */
    jerry_value_t leadVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>(" 10 20"));
    EXPECT_TRUE(component->UpdateView(K_GAP, leadVal));
    jerry_release_value(leadVal);
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_ROW); // 10: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_COL); // 20: column gap

    /* px suffix stops the digit scan */
    jerry_value_t pxVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("10px 20px"));
    EXPECT_TRUE(component->UpdateView(K_GAP, pxVal));
    jerry_release_value(pxVal);
    EXPECT_EQ(divView->GetRowGap(), FLEX_TEST_GAP_ROW); // 10: row gap
    EXPECT_EQ(divView->GetColumnGap(), FLEX_TEST_GAP_COL); // 20: column gap

    /* oversize values are clamped to INT16_MAX */
    jerry_value_t bigVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("99999 88888"));
    EXPECT_TRUE(component->UpdateView(K_GAP, bigVal));
    jerry_release_value(bigVal);
    EXPECT_EQ(divView->GetRowGap(), INT16_MAX);
    EXPECT_EQ(divView->GetColumnGap(), INT16_MAX);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexRowGapPercentTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("flexWrap", "wrap")
                  .SetString("alignContent", "flex-start")
                  .SetString("rowGap", "50%"); // 50% of container height 300 -> 150

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CONTAINER_WIDTH) // 250: force one child per line
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT); // 50: child height

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
           .AddChild(childStyle)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *secondChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(secondChild != nullptr);
    if (secondChild != nullptr) {
        EXPECT_EQ(secondChild->GetY(), FLEX_TEST_EXPECTED_SECOND_CHILD_Y); // 200 = 50 (line height) + 150 (row gap)
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexColumnGapPercentTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "row")
                  .SetString("flexWrap", "wrap")
                  .SetString("alignContent", "flex-start")
                  .SetString("columnGap", "50%"); // 50% of container width 400 -> 200

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_MULTI_LINE_CHILD_WIDTH) // 80: two children fit in one line
              .SetNumber("height", FLEX_TEST_CHILD_HEIGHT);

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_TWO)
           .AddChild(childStyle)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *secondChild = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_SECOND);
    EXPECT_TRUE(secondChild != nullptr);
    if (secondChild != nullptr) {
        EXPECT_EQ(secondChild->GetX(), FLEX_TEST_EXPECTED_SECOND_CHILD_X); // 280 = 80 + 200
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivAspectRatioStringTest()
{
    TDD_CASE_BEGIN();
    /* "width/height" string form, e.g. 16/9 -> ratio about 1.78 -> scaled 177 */
    JerrySetStringProperty(styleObj_, "aspectRatio", "16/9");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetAspectRatio(), FLEX_TEST_ASPECT_RATIO_STRING_RESULT);
    // 178 = round(16 / 9 * 100), the engine rounds with +0.5
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivAbsoluteInsetsPixelClearTest()
{
    TDD_CASE_BEGIN();
    JerrySetStringProperty(styleObj_, "position", "absolute");
    JerrySetStringProperty(styleObj_, "left", "10px");
    JerrySetStringProperty(styleObj_, "top", "5px");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_TRUE(divView->HasFlexLeft());
    EXPECT_EQ(divView->GetFlexLeft(), FLEX_TEST_LEFT_PIXEL); // 10: left pixel inset
    EXPECT_TRUE(divView->HasFlexTop());
    EXPECT_EQ(divView->GetFlexTop(), FLEX_TEST_TOP_PIXEL); // 5: top pixel inset

    /* percent left via UpdateView, then clear with a non-dimension value */
    jerry_value_t percentVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("20%"));
    EXPECT_TRUE(component->UpdateView(K_LEFT, percentVal));
    jerry_release_value(percentVal);
    EXPECT_TRUE(divView->IsFlexLeftPercent());
    EXPECT_FLOAT_EQ(divView->GetFlexLeftPercent(), static_cast<float>(FLEX_TEST_LEFT_PERCENT)); // 20: left percent

    /* a non-dimension value (e.g. boolean) resolves to TYPE_UNKNOWN and clears the inset.
     * Note: the string "auto" parses to pixel 0 via strtol leniency, so it does NOT clear. */
    jerry_value_t clearVal = jerry_create_boolean(true);
    EXPECT_TRUE(component->UpdateView(K_LEFT, clearVal));
    jerry_release_value(clearVal);
    EXPECT_FALSE(divView->HasFlexLeft());

    /* right/bottom pixel insets */
    jerry_value_t rightVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("50px"));
    EXPECT_TRUE(component->UpdateView(K_RIGHT, rightVal));
    jerry_release_value(rightVal);
    EXPECT_TRUE(divView->HasFlexRight());
    EXPECT_EQ(divView->GetFlexRight(), FLEX_TEST_RIGHT_PIXEL); // 50: right pixel inset

    jerry_value_t bottomVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("25px"));
    EXPECT_TRUE(component->UpdateView(K_BOTTOM, bottomVal));
    jerry_release_value(bottomVal);
    EXPECT_TRUE(divView->HasFlexBottom());
    EXPECT_EQ(divView->GetFlexBottom(), FLEX_TEST_BOTTOM_PIXEL); // 25: bottom pixel inset

    /* clear right/bottom with a non-dimension value to cover the TYPE_UNKNOWN path */
    jerry_value_t clearRightVal = jerry_create_boolean(true);
    EXPECT_TRUE(component->UpdateView(K_RIGHT, clearRightVal));
    jerry_release_value(clearRightVal);
    EXPECT_FALSE(divView->HasFlexRight());

    jerry_value_t clearBottomVal = jerry_create_boolean(true);
    EXPECT_TRUE(component->UpdateView(K_BOTTOM, clearBottomVal));
    jerry_release_value(clearBottomVal);
    EXPECT_FALSE(divView->HasFlexBottom());
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivUpdateViewOverflowAlignFlexTest()
{
    TDD_CASE_BEGIN();
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }

    jerry_value_t hiddenVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("hidden"));
    EXPECT_TRUE(component->UpdateView(K_OVERFLOW, hiddenVal));
    jerry_release_value(hiddenVal);
    EXPECT_EQ(divView->GetOverflow(), OVERFLOW_HIDDEN);

    jerry_value_t centerVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("center"));
    EXPECT_TRUE(component->UpdateView(K_ALIGN_SELF, centerVal));
    jerry_release_value(centerVal);
    EXPECT_EQ(divView->GetAlignSelf(), ALIGN_CENTER);

    jerry_value_t growVal = jerry_create_number(FLEX_TEST_FLEX_GROW);
    EXPECT_TRUE(component->UpdateView(K_FLEX_GROW, growVal));
    jerry_release_value(growVal);
    EXPECT_EQ(divView->GetFlexGrow(), FLEX_TEST_FLEX_GROW);

    jerry_value_t shrinkVal = jerry_create_number(FLEX_TEST_FLEX_SHRINK);
    EXPECT_TRUE(component->UpdateView(K_FLEX_SHRINK, shrinkVal));
    jerry_release_value(shrinkVal);
    EXPECT_EQ(divView->GetFlexShrink(), FLEX_TEST_FLEX_SHRINK);

    jerry_value_t basisVal = jerry_create_number(FLEX_TEST_FLEX_BASIS);
    EXPECT_TRUE(component->UpdateView(K_FLEX_BASIS, basisVal));
    jerry_release_value(basisVal);
    EXPECT_EQ(divView->GetFlexBasis(), FLEX_TEST_FLEX_BASIS); // 100: flex basis

    jerry_value_t contentVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("center"));
    EXPECT_TRUE(component->UpdateView(K_ALIGN_CONTENT, contentVal));
    jerry_release_value(contentVal);
    EXPECT_EQ(divView->GetAlignContent(), ALIGN_CONTENT_CENTER);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivUpdateViewConstraintKeysTest()
{
    TDD_CASE_BEGIN();
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }

    jerry_value_t minWidthVal = jerry_create_number(FLEX_TEST_MIN_WIDTH);
    EXPECT_TRUE(component->UpdateView(K_MIN_WIDTH, minWidthVal));
    jerry_release_value(minWidthVal);
    EXPECT_EQ(divView->GetMinWidth(), FLEX_TEST_MIN_WIDTH); // 80: min width

    jerry_value_t maxWidthVal = jerry_create_number(FLEX_TEST_MAX_WIDTH);
    EXPECT_TRUE(component->UpdateView(K_MAX_WIDTH, maxWidthVal));
    jerry_release_value(maxWidthVal);
    EXPECT_EQ(divView->GetMaxWidth(), FLEX_TEST_MAX_WIDTH); // 300: max width

    jerry_value_t minHeightVal = jerry_create_number(FLEX_TEST_MIN_HEIGHT);
    EXPECT_TRUE(component->UpdateView(K_MIN_HEIGHT, minHeightVal));
    jerry_release_value(minHeightVal);
    EXPECT_EQ(divView->GetMinHeight(), FLEX_TEST_MIN_HEIGHT); // 40: min height

    jerry_value_t maxHeightVal = jerry_create_number(FLEX_TEST_MAX_HEIGHT);
    EXPECT_TRUE(component->UpdateView(K_MAX_HEIGHT, maxHeightVal));
    jerry_release_value(maxHeightVal);
    EXPECT_EQ(divView->GetMaxHeight(), FLEX_TEST_MAX_HEIGHT); // 200: max height

    jerry_value_t ratioVal = jerry_create_number(FLEX_TEST_ASPECT_RATIO_INPUT);
    EXPECT_TRUE(component->UpdateView(K_ASPECT_RATIO, ratioVal));
    jerry_release_value(ratioVal);
    EXPECT_EQ(divView->GetAspectRatio(), FLEX_TEST_ASPECT_RATIO_SCALED); // 200 = 2.0 * 100
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivUpdateViewPositionKeysTest()
{
    TDD_CASE_BEGIN();
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }

    jerry_value_t rightVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("10%"));
    EXPECT_TRUE(component->UpdateView(K_RIGHT, rightVal));
    jerry_release_value(rightVal);
    EXPECT_TRUE(divView->IsFlexRightPercent());

    jerry_value_t bottomVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("20%"));
    EXPECT_TRUE(component->UpdateView(K_BOTTOM, bottomVal));
    jerry_release_value(bottomVal);
    EXPECT_TRUE(divView->IsFlexBottomPercent());

    jerry_value_t positionVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("absolute"));
    EXPECT_TRUE(component->UpdateView(K_POSITION, positionVal));
    jerry_release_value(positionVal);
    EXPECT_EQ(divView->GetPositionType(), POSITION_ABSOLUTE);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexInvalidFactorTest()
{
    TDD_CASE_BEGIN();
    JerrySetNumberProperty(styleObj_, "flexGrow", FLEX_TEST_INVALID_NEGATIVE); // negative grow is rejected
    JerrySetNumberProperty(styleObj_, "flexShrink", FLEX_TEST_OVERSIZE_UINT16); // 70000: beyond UINT16_MAX, rejected
    JerrySetNumberProperty(styleObj_, "flexBasis", FLEX_TEST_INVALID_NEGATIVE_SMALL); // negative basis is rejected
    JerrySetNumberProperty(styleObj_, "minWidth", FLEX_TEST_INVALID_NEGATIVE_SMALL); // negative min-width is rejected
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    UIViewGroup *divView = static_cast<UIViewGroup *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetFlexGrow(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged
    EXPECT_EQ(divView->GetFlexShrink(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged
    EXPECT_EQ(divView->GetFlexBasis(), FLEX_TEST_INVALID_NEGATIVE); // -1: unchanged
    EXPECT_EQ(divView->GetMinWidth(), FLEX_TEST_INVALID_NEGATIVE); // -1: unchanged

    /* non-number flexGrow is rejected as well */
    jerry_value_t strVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("abc"));
    EXPECT_FALSE(component->UpdateView(K_FLEX_GROW, strVal));
    jerry_release_value(strVal);
    EXPECT_EQ(divView->GetFlexGrow(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged

    /* aspectRatio zero/negative/oversize are rejected; flexBasis oversize is rejected */
    jerry_value_t zeroVal = jerry_create_number(FLEX_TEST_GAP_CLAMPED);
    EXPECT_FALSE(component->UpdateView(K_ASPECT_RATIO, zeroVal));
    jerry_release_value(zeroVal);
    EXPECT_EQ(divView->GetAspectRatio(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged

    jerry_value_t negRatioVal = jerry_create_number(FLEX_TEST_INVALID_NEGATIVE);
    EXPECT_FALSE(component->UpdateView(K_ASPECT_RATIO, negRatioVal));
    jerry_release_value(negRatioVal);
    EXPECT_EQ(divView->GetAspectRatio(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged

    jerry_value_t bigRatioVal = jerry_create_number(FLEX_TEST_OVERSIZE_UINT16);
    EXPECT_FALSE(component->UpdateView(K_ASPECT_RATIO, bigRatioVal));
    jerry_release_value(bigRatioVal);
    EXPECT_EQ(divView->GetAspectRatio(), FLEX_TEST_GAP_CLAMPED); // 0: unchanged

    jerry_value_t bigBasisVal = jerry_create_number(FLEX_TEST_OVERSIZE_INT16); // 40000: beyond INT16_MAX, rejected
    EXPECT_FALSE(component->UpdateView(K_FLEX_BASIS, bigBasisVal));
    jerry_release_value(bigBasisVal);
    EXPECT_EQ(divView->GetFlexBasis(), FLEX_TEST_INVALID_NEGATIVE); // -1: unchanged

    jerry_value_t bigMinWidthVal = jerry_create_number(FLEX_TEST_OVERSIZE_INT16); // 40000: beyond INT16_MAX, rejected
    EXPECT_FALSE(component->UpdateView(K_MIN_WIDTH, bigMinWidthVal));
    jerry_release_value(bigMinWidthVal);
    EXPECT_EQ(divView->GetMinWidth(), FLEX_TEST_INVALID_NEGATIVE); // -1: unchanged
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivPositionAlignInvalidTest()
{
    TDD_CASE_BEGIN();
    JerrySetStringProperty(styleObj_, "alignSelf", "flex-end");
    std::unique_ptr<Component> component(GetRenderedComponent(componentNameId_));
    EXPECT_TRUE(component != nullptr);
    if (component == nullptr) {
        TDD_CASE_END();
        return;
    }
    FlexLayout *divView = static_cast<FlexLayout *>(component->GetComponentRootView());
    EXPECT_TRUE(divView != nullptr);
    if (divView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_EQ(divView->GetAlignSelf(), ALIGN_END);

    /* flex-start is also accepted */
    jerry_value_t startVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("flex-start"));
    EXPECT_TRUE(component->UpdateView(K_ALIGN_SELF, startVal));
    jerry_release_value(startVal);
    EXPECT_EQ(divView->GetAlignSelf(), ALIGN_START);

    /* center is also accepted */
    jerry_value_t centerVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("center"));
    EXPECT_TRUE(component->UpdateView(K_ALIGN_SELF, centerVal));
    jerry_release_value(centerVal);
    EXPECT_EQ(divView->GetAlignSelf(), ALIGN_CENTER);

    /* illegal alignSelf is rejected and the previous value is kept */
    jerry_value_t invalidVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("invalid"));
    EXPECT_FALSE(component->UpdateView(K_ALIGN_SELF, invalidVal));
    jerry_release_value(invalidVal);
    EXPECT_EQ(divView->GetAlignSelf(), ALIGN_CENTER);

    /* non-absolute position falls back to static */
    jerry_value_t staticVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("static"));
    EXPECT_TRUE(component->UpdateView(K_POSITION, staticVal));
    jerry_release_value(staticVal);
    EXPECT_EQ(divView->GetPositionType(), POSITION_STATIC);

    /* illegal align-content is rejected; default (stretch) is kept */
    jerry_value_t invalidContentVal = jerry_create_string(reinterpret_cast<const jerry_char_t *>("invalid"));
    EXPECT_FALSE(component->UpdateView(K_ALIGN_CONTENT, invalidContentVal));
    jerry_release_value(invalidContentVal);
    EXPECT_EQ(divView->GetAlignContent(), ALIGN_CONTENT_STRETCH);
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivFlexColumnAspectRatioTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "column");

    StyleBuilder childStyle;
    childStyle.SetNumber("width", FLEX_TEST_CHILD_WIDTH) // 100: explicit width
              .SetNumber("aspectRatio", FLEX_TEST_ASPECT_RATIO); // 2.0

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *child = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(child != nullptr);
    if (child != nullptr) {
        EXPECT_EQ(child->GetHeight(), FLEX_TEST_ASPECT_RATIO_HEIGHT); // 50 = 100 / 2.0
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::ComponentDivAttachViewColumnAspectRatioTest()
{
    TDD_CASE_BEGIN();
    GetRootComponentMock().PrepareRootContainer(FLEX_TEST_CONTAINER_WIDTH, FLEX_TEST_CONTAINER_HEIGHT, true);

    StyleBuilder containerStyle;
    containerStyle.SetNumber("width", FLEX_TEST_CONTAINER_WIDTH)
                  .SetNumber("height", FLEX_TEST_CONTAINER_HEIGHT)
                  .SetString("flexDirection", "column");

    /* child has explicit height and aspectRatio, but no explicit width.
     * AttachView column path should see K_WIDTH unknown and HasAspectRatioMainSize true,
     * skipping the default width-stretch-to-container logic. */
    StyleBuilder childStyle;
    childStyle.SetNumber("height", FLEX_TEST_CHILD_HEIGHT)
              .SetNumber("aspectRatio", FLEX_TEST_ASPECT_RATIO);

    DivComponentBuilder builder(GetRootComponentMock());
    builder.SetContainerStyle(containerStyle)
           .SetChildCount(FLEX_TEST_CHILD_COUNT_ONE)
           .AddChild(childStyle);

    ComponentHandle componentHandle(builder.Build());
    UIView *child = componentHandle.GetChildAt(FLEX_TEST_CHILD_INDEX_FIRST);
    EXPECT_TRUE(child != nullptr);
    if (child != nullptr) {
        EXPECT_EQ(child->GetHeight(), FLEX_TEST_CHILD_HEIGHT);
        EXPECT_NE(child->GetWidth(), FLEX_TEST_CONTAINER_WIDTH);
    }
    TDD_CASE_END();
}

void DivFlexLayoutTddTest::RunTests()
{
    ComponentDivStyleSetTest033();
    ComponentDivStyleSetTest034();
    ComponentDivStyleSetTest035();
    ComponentDivStyleSetTest036();
    StretchAlignApplyTest037();
    ComponentDivStyleSetTest038();
    ComponentDivFlexAlignSelfTest();
    ComponentDivFlexAlignItemsCenterEndTest();
    ComponentDivFlexDirectionReverseTest();
    ComponentDivFlexDirectionInvalidTest();
    ComponentDivFlexShrinkGrowTest();
    ComponentDivFlexWrapStartTest();
    ComponentDivFlexWrapEndTest();
    ComponentDivFlexWrapCenterTest();
    ComponentDivFlexWrapSpaceBetweenTest();
    ComponentDivFlexConstraintsTest();
    ComponentDivFlexWrapStretchTest();
    ComponentDivFlexDynamicAspectRatioTest();
    ComponentDivGapSinglePropsTest();
    ComponentDivGapNumberVariantsTest();
    ComponentDivGapStringVariantsTest();
    ComponentDivFlexRowGapPercentTest();
    ComponentDivFlexColumnGapPercentTest();
    ComponentDivAspectRatioStringTest();
    ComponentDivAbsoluteInsetsPixelClearTest();
    ComponentDivUpdateViewOverflowAlignFlexTest();
    ComponentDivUpdateViewConstraintKeysTest();
    ComponentDivUpdateViewPositionKeysTest();
    ComponentDivFlexInvalidFactorTest();
    ComponentDivPositionAlignInvalidTest();
    ComponentDivFlexColumnAspectRatioTest();
    ComponentDivAttachViewColumnAspectRatioTest();
}

#if defined(TDD_ASSERTIONS)

/* *
 * @tc.name: ComponentDivStyleSetTest033
 * @tc.desc: Verify overflow style default/hidden/visible can set nomally.
 */
HWTEST_F(DivFlexLayoutTddTest, div033, TestSize.Level1)
{
    DivFlexLayoutTddTest::ComponentDivStyleSetTest033();
}

/* *
 * @tc.name: ComponentDivStyleSetTest034
 * @tc.desc: Verify marginLeft auto is parsed to TYPE_AUTO and clears the stale pixel margin.
 */
HWTEST_F(DivFlexLayoutTddTest, div034, TestSize.Level1)
{
    DivFlexLayoutTddTest::ComponentDivStyleSetTest034();
}

/* *
 * @tc.name: ComponentDivStyleSetTest035
 * @tc.desc: Verify marginLeft with non-auto string value falls back to pixel.
 */
HWTEST_F(DivFlexLayoutTddTest, div035, TestSize.Level1)
{
    DivFlexLayoutTddTest::ComponentDivStyleSetTest035();
}

/* *
 * @tc.name: ComponentDivStyleSetTest036
 * @tc.desc: Verify gap shorthand can set row-gap and column-gap.
 */
HWTEST_F(DivFlexLayoutTddTest, div036, TestSize.Level1)
{
    DivFlexLayoutTddTest::ComponentDivStyleSetTest036();
}

/* *
 * @tc.name: StretchAlignApplyTest037
 * @tc.desc: Verify align-items stretch is recognized and applied, illegal value is rejected.
 */
HWTEST_F(DivFlexLayoutTddTest, div037, TestSize.Level1)
{
    DivFlexLayoutTddTest::StretchAlignApplyTest037();
}

/* *
 * @tc.name: ComponentDivStyleSetTest038
 * @tc.desc: Verify absolute right/bottom percent styles are kept as percent in native view.
 */
HWTEST_F(DivFlexLayoutTddTest, div038, TestSize.Level1)
{
    DivFlexLayoutTddTest::ComponentDivStyleSetTest038();
}

/* *
 * @tc.name: ComponentDivFlexAlignSelfTest
 * @tc.desc: Verify alignSelf stretch works in flex layout.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexAlignSelf, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexAlignSelfTest();
}

/* *
 * @tc.name: ComponentDivFlexAlignItemsCenterEndTest
 * @tc.desc: Verify alignItems center and flex-end place the child on the cross axis.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexAlignItemsCenterEnd, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexAlignItemsCenterEndTest();
}

/* *
 * @tc.name: ComponentDivFlexDirectionReverseTest
 * @tc.desc: Verify flexDirection row-reverse and column-reverse place children in reverse order.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexDirectionReverse, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexDirectionReverseTest();
}

/* *
 * @tc.name: ComponentDivFlexDirectionInvalidTest
 * @tc.desc: Verify invalid flexDirection falls back to default row layout.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexDirectionInvalid, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexDirectionInvalidTest();
}

/* *
 * @tc.name: ComponentDivFlexShrinkGrowTest
 * @tc.desc: Verify flexShrink and flexGrow with min/max constraints.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexShrinkGrow, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexShrinkGrowTest();
}

/* *
 * @tc.name: ComponentDivFlexWrapStartTest
 * @tc.desc: Verify flexWrap with alignContent flex-start.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexWrapStart, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexWrapStartTest();
}

/* *
 * @tc.name: ComponentDivFlexWrapEndTest
 * @tc.desc: Verify flexWrap with alignContent flex-end.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexWrapEnd, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexWrapEndTest();
}

/* *
 * @tc.name: ComponentDivFlexWrapCenterTest
 * @tc.desc: Verify flexWrap with alignContent center.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexWrapCenter, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexWrapCenterTest();
}

/* *
 * @tc.name: ComponentDivFlexWrapSpaceBetweenTest
 * @tc.desc: Verify flexWrap with alignContent space-between.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexWrapSpaceBetween, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexWrapSpaceBetweenTest();
}

/* *
 * @tc.name: ComponentDivFlexConstraintsTest
 * @tc.desc: Verify min/max width/height and aspectRatio constraints.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexConstraints, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexConstraintsTest();
}

/* *
 * @tc.name: ComponentDivFlexWrapStretchTest
 * @tc.desc: Verify flexWrap with alignContent stretch.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexWrapStretch, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexWrapStretchTest();
}

/* *
 * @tc.name: ComponentDivFlexDynamicAspectRatioTest
 * @tc.desc: Verify dynamic aspectRatio relayout.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexDynamicAspectRatio, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexDynamicAspectRatioTest();
}

/* *
 * @tc.name: ComponentDivGapSinglePropsTest
 * @tc.desc: Verify rowGap/columnGap single properties apply pixel values and clamp negatives.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexGapSingleProps, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivGapSinglePropsTest();
}

/* *
 * @tc.name: ComponentDivGapNumberVariantsTest
 * @tc.desc: Verify gap with number values applies to both gaps and clamps negatives.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexGapNumberVariants, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivGapNumberVariantsTest();
}

/* *
 * @tc.name: ComponentDivGapStringVariantsTest
 * @tc.desc: Verify gap shorthand single value, leading spaces, px suffix and oversize clamping.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexGapStringVariants, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivGapStringVariantsTest();
}

/* *
 * @tc.name: ComponentDivFlexRowGapPercentTest
 * @tc.desc: Verify rowGap percent resolves against the container height in wrapped layout.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexRowGapPercent, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexRowGapPercentTest();
}

/* *
 * @tc.name: ComponentDivFlexColumnGapPercentTest
 * @tc.desc: Verify columnGap percent resolves against the container width in wrapped layout.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexColumnGapPercent, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexColumnGapPercentTest();
}

/* *
 * @tc.name: ComponentDivAspectRatioStringTest
 * @tc.desc: Verify aspectRatio accepts the "width/height" string form.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexAspectRatioString, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivAspectRatioStringTest();
}

/* *
 * @tc.name: ComponentDivAbsoluteInsetsPixelClearTest
 * @tc.desc: Verify absolute left/top/right/bottom pixel and percent insets, and clearing left.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexAbsoluteInsetsPixelClear, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivAbsoluteInsetsPixelClearTest();
}

/* *
 * @tc.name: ComponentDivUpdateViewOverflowAlignFlexTest
 * @tc.desc: Verify overflow/alignSelf/flexGrow/flexShrink/flexBasis/alignContent via UpdateView.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexUpdateViewOverflowAlignFlex, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivUpdateViewOverflowAlignFlexTest();
}

/* *
 * @tc.name: ComponentDivUpdateViewConstraintKeysTest
 * @tc.desc: Verify min/max width/height and aspectRatio via UpdateView.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexUpdateViewConstraintKeys, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivUpdateViewConstraintKeysTest();
}

/* *
 * @tc.name: ComponentDivUpdateViewPositionKeysTest
 * @tc.desc: Verify right/bottom percent and position absolute via UpdateView.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexUpdateViewPositionKeys, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivUpdateViewPositionKeysTest();
}

/* *
 * @tc.name: ComponentDivFlexInvalidFactorTest
 * @tc.desc: Verify invalid flexGrow/flexShrink/flexBasis/minWidth values are rejected.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexInvalidFactor, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexInvalidFactorTest();
}

/* *
 * @tc.name: ComponentDivPositionAlignInvalidTest
 * @tc.desc: Verify alignSelf values, illegal alignSelf/alignContent rejection and static position fallback.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexPositionAlignInvalid, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivPositionAlignInvalidTest();
}

/* *
 * @tc.name: ComponentDivFlexColumnAspectRatioTest
 * @tc.desc: Verify aspectRatio derives the main size in column direction.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexColumnAspectRatio, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivFlexColumnAspectRatioTest();
}

/* *
 * @tc.name: ComponentDivAttachViewColumnAspectRatioTest
 * @tc.desc: Verify AttachView skips default width sizing when column child has explicit height and aspectRatio.
 */
HWTEST_F(DivFlexLayoutTddTest, divFlexAttachViewColumnAspectRatio, TestSize.Level0)
{
    DivFlexLayoutTddTest::ComponentDivAttachViewColumnAspectRatioTest();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS

#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
