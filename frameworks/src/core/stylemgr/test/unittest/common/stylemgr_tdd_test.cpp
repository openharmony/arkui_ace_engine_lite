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

#include "stylemgr_tdd_test.h"
#include <cstdio>
#include "component.h"
#include "div_component.h"
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
#include "flex_layout.h"
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#include "js_ability.h"
#include "js_debugger_config.h"
#include "stylemgr/app_style_manager.h"
#include "stylemgr/app_style_sheet.h"

namespace OHOS {
namespace ACELite {
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
namespace {
constexpr uint16_t FLEX_GROW_VALUE = 2;
constexpr int16_t FLEX_SHRINK_VALUE = 0;
constexpr int16_t FLEX_BASIS_VALUE = 100;
constexpr int32_t SIZE_CONSTRAINT_WIDTH_VALUE = 160;
constexpr int32_t SIZE_CONSTRAINT_HEIGHT_VALUE = 90;
constexpr int32_t SIZE_CONSTRAINT_MIN_WIDTH_VALUE = 80;
constexpr int32_t SIZE_CONSTRAINT_MAX_WIDTH_VALUE = 240;
constexpr int32_t SIZE_CONSTRAINT_MIN_HEIGHT_VALUE = 60;
constexpr int32_t SIZE_CONSTRAINT_MAX_HEIGHT_VALUE = 180;
constexpr double SIZE_CONSTRAINT_ASPECT_RATIO_STYLE_VALUE = 1.5;
constexpr int32_t SIZE_CONSTRAINT_ASPECT_RATIO_VALUE = 150;
constexpr const char SIZE_CONSTRAINT_ASPECT_RATIO_PERCENT[] = "150%";
} // namespace
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

StyleMgrTddTest::StyleMgrTddTest()
{}

void StyleMgrTddTest::SetUp()
{
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
    if (g_testStyleMgr == nullptr) {
        g_testStyleMgr = new AppStyleManager();
        g_testStyleMgr->Prepare();
    }
    rootComponentMock_.PrepareRootContainer();
}

void StyleMgrTddTest::TearDown()
{
    if (g_testStyleMgr != nullptr) {
        delete g_testStyleMgr;
        g_testStyleMgr = nullptr;
    }
    jerry_cleanup();
    Debugger::GetInstance().ReleaseJSContext();
}

void StyleMgrTddTest::TestCaseB(const char* name) const
{
    g_assertRecord = 0;
    g_currentTestCaseName = const_cast<char *>(name);
}

void StyleMgrTddTest::JudeTestCaseResult() const
{
    if (g_assertRecord > 0) {
        printf("[Test Case] [%s]   FAILED \n", g_currentTestCaseName);
    } else {
        printf("[Test Case] [%s]   PASSED \n", g_currentTestCaseName);
    }
}

void StyleMgrTddTest::ReleaseTestResources(const jerry_value_t* values,
                                           uint8_t valueCount,
                                           Component** components,
                                           uint8_t componentCount) const
{
    // release JS values
    if ((values != nullptr)  && (valueCount != 0)) {
        for (int i = 0; i < valueCount; i++) {
            jerry_release_value(values[i]);
        }
    }

    // release component instances
    if ((components != nullptr) && (componentCount != 0)) {
        for (int i = 0; ((i < componentCount) && (components[i] != nullptr)); i++) {
            components[i]->Release();
            delete components[i];
            components[i] = nullptr;
        }
    }
}

/**
 * prepare valid style sheet with given selectors and key value paire
 */
JSValue StyleMgrTddTest::PrepareStyleSheet(const char* selectors,
                                           const char* selectorName,
                                           const char* propName,
                                           uint16_t propValue) const
{
    if ((selectors == nullptr) || (strlen(selectors) == 0) || (selectorName == nullptr) ||
        (strlen(selectorName) == 0) || (propName == nullptr) || (strlen(propName) == 0)) {
        return jerry_create_undefined();
    }

    /**
     * create one style sheet
     */
    JSValue styleSheet = jerry_create_object();

    /**
     * create id selector object and set it to style sheet
     */
    JSValue idSelectorKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(selectors));
    JSValue idSelectors = jerry_create_object();
    jerry_release_value(jerry_set_property(styleSheet, idSelectorKey, idSelectors));
    jerry_release_value(idSelectorKey);

    /**
     * create one id selector
     */
    JSValue sampleSelectorKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(selectorName));
    JSValue sampleSelector = jerry_create_object();
    jerry_release_value(jerry_set_property(idSelectors, sampleSelectorKey, sampleSelector));
    jerry_release_value(sampleSelectorKey);

    /**
     * add height key value into sample
     */
    JSValue heightKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    JSValue heightValue = jerry_create_number(propValue);
    jerry_release_value(jerry_set_property(sampleSelector, heightKey, heightValue));
    jerry_release_value(heightKey);

    return styleSheet;
}

JSValue StyleMgrTddTest::PrepareStyleSheetWithKeyValues(const char* selector,
                                                        JSValue selectorObj,
                                                        const char* selectorName,
                                                        const uint8_t itemCount,
                                                        char* keys[],
                                                        const uint16_t values[]) const
{
    if ((itemCount == 0) || (selectorName == nullptr) || (strlen(selectorName) == 0) ||
        (keys == nullptr) || (values == nullptr)) {
        return jerry_create_undefined();
    }

    /**
     * create one style sheet
     */
    JSValue styleSheet = jerry_create_object();

    /**
    * create selector object and set it to style sheet
    */
    JSValue selectorObjectTarget = jerry_create_object();
    JSValue selectorKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(selector));
    jerry_release_value(jerry_set_property(styleSheet, selectorKey, selectorObjectTarget));
    jerry_release_value(selectorKey);

    /**
     * set selector into style sheet object
     */
    JSValue selectorNameValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(selectorName));
    jerry_release_value(jerry_set_property(selectorObjectTarget, selectorNameValue, selectorObj));
    jerry_release_value(selectorNameValue);

    /**
     * add value into selector
     */
    for (uint8_t i = 0; i < itemCount; i++) {
        JSValue widthValue = jerry_create_number(values[i]);
        JSValue keyNameValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(keys[i]));
        jerry_release_value(jerry_set_property(selectorObj, keyNameValue, widthValue));
        jerry_release_value(keyNameValue);
    }

    return styleSheet;
}

/**
 * prepare valid style option
 */
JSValue StyleMgrTddTest::PrepareStyleOption(const char* styleType, const char* propName, uint16_t propValue) const
{
    if ((styleType == nullptr) || (strlen(styleType) == 0) || (propName == nullptr) || (strlen(propName) == 0)) {
        return jerry_create_undefined();
    }

    /**
     * create one style sheet
     */
    JSValue styleOption = jerry_create_object();

    /**
     * create style object
     */
    JSValue styleTypeKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(styleType));
    JSValue styleObj = jerry_create_object();
    jerry_release_value(jerry_set_property(styleOption, styleTypeKey, styleObj));
    jerry_release_value(styleTypeKey);

    /**
     * add height key value into style
     */
    JSValue propKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    JSValue propValueObj = jerry_create_number(propValue);
    jerry_release_value(jerry_set_property(styleObj, propKey, propValueObj));
    jerry_release_value(propKey);

    return styleOption;
}

/**
 * prepare valid attr option
 */
JSValue StyleMgrTddTest::PrepareAttrOption(const char* propName, const char* propValue) const
{
    if ((propName == nullptr) || (strlen(propName) == 0)) {
        return jerry_create_undefined();
    }

    /**
     * create one style sheet
     */
    JSValue option = jerry_create_object();

    /**
     * create style object
     */
    JSValue attrsKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("attrs"));
    JSValue attrsObj = jerry_create_object();
    jerry_release_value(jerry_set_property(option, attrsKey, attrsObj));
    jerry_release_value(attrsKey);

    /**
     * add height key value into style
     */
    JSValue propKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    JSValue propValueObj = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propValue));
    jerry_release_value(jerry_set_property(attrsObj, propKey, propValueObj));
    jerry_release_value(propKey);

    return option;
}

void StyleMgrTddTest::AddValueToAttrOption(jerry_value_t attrOption, const char* propName, const char* propValue) const
{
    if ((propName == nullptr) || (strlen(propName) == 0)) {
        return;
    }

    JSValue attrsKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("attrs"));
    JSValue attrsObj = jerry_create_object();
    jerry_release_value(jerry_set_property(attrOption, attrsKey, attrsObj));
    jerry_release_value(attrsKey);
    /**
     * add height key value into style
     */
    JSValue propKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    JSValue propValueObj = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propValue));
    jerry_release_value(jerry_set_property(attrsObj, propKey, propValueObj));
    jerry_release_value(propKey);
}

void StyleMgrTddTest::AddStrValueToOption(jerry_value_t option, const char* propName, const char* propValue) const
{
    if ((propName == nullptr) || (strlen(propName) == 0)) {
        return;
    }

    /**
    * add height key value into style
    */
    JSValue propKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    JSValue propValueObj = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propValue));
    jerry_release_value(jerry_set_property(option, propKey, propValueObj));
    jerry_release_value(propKey);
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
namespace {
JSValue CreateStyleOption()
{
    JSValue option = jerry_create_object();
    JSValue styleKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("staticStyle"));
    JSValue style = jerry_create_object();
    jerry_release_value(jerry_set_property(option, styleKey, style));
    jerry_release_value(styleKey);
    jerry_release_value(style);
    return option;
}

JSValue GetStaticStyle(JSValue option)
{
    JSValue styleKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("staticStyle"));
    JSValue style = jerry_get_property(option, styleKey);
    jerry_release_value(styleKey);
    return style;
}

void SetStyleNumber(JSValue style, const char *name, double value)
{
    JSValue key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    JSValue val = jerry_create_number(value);
    jerry_release_value(jerry_set_property(style, key, val));
    jerry_release_value(key);
    jerry_release_value(val);
}

void SetStyleString(JSValue style, const char *name, const char *value)
{
    JSValue key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    JSValue val = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_release_value(jerry_set_property(style, key, val));
    jerry_release_value(key);
    jerry_release_value(val);
}
} // namespace
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

void StyleMgrTddTest::ACELiteStyleManagerInlineStyle001()
{
    TestCaseB("ACELiteStyleManagerInlineStyle001");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. create undefine for style sheet object
     */
    JSValue undefined = jerry_create_undefined();

    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(undefined);

    /**
     * @tc.steps: step4. check if the style sheet object is generated
     */
    EXPECT_TRUE(g_testStyleMgr->GetStyleSheet() == nullptr);

    /**
     * @tc.steps: step4. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerInlineStyle002()
{
    TestCaseB("ACELiteStyleManagerInlineStyle002");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. create undefine for style sheet object
     */
    JSValue styleSheet = jerry_create_object();

    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheet);

    /**
     * @tc.steps: step4. check if the style sheet object is generated, style sheet instance will be created out
     */
    EXPECT_FALSE(g_testStyleMgr->GetStyleSheet() == nullptr);

    /**
     * @tc.steps: step5. release style sheet object
     */
    jerry_release_value(styleSheet);

    /**
     * @tc.steps: step6. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerInlineStyle003()
{
    TestCaseB("ACELiteStyleManagerInlineStyle003");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet
     */
    constexpr char id[] = "idSelectors";
    const char *sample = "sample";
    constexpr char heightKey[] = "height";
    constexpr int32_t heightValue = 50;
    JSValue styleSheetObj = PrepareStyleSheet(id, sample, heightKey, heightValue);

    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step4. check the result
     */
    AppStyleSheet* styleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_FALSE(styleSheet == nullptr);
    if (styleSheet == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    AppStyle* sampleSelector = styleSheet->GetStyleFromIDSelectors(sample);
    EXPECT_FALSE(sampleSelector == nullptr);
    if (sampleSelector == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    AppStyleItem* sampleSelectorItem = const_cast<AppStyleItem *>(sampleSelector->GetFirst());
    EXPECT_FALSE(sampleSelectorItem == nullptr);
    if (sampleSelectorItem == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    EXPECT_EQ(sampleSelectorItem->GetNumValue(), heightValue);

    /**
     * @tc.steps: step5. release the style sheet JS object
     */
    jerry_release_value(styleSheetObj);

    /**
     * @tc.steps: step6. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerInlineStyle004()
{
    TestCaseB("ACELiteStyleManagerInlineStyle004");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char classSelector[] = "classSelectors";
    const char *sample = "sampleClass";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 200;
    JSValue styleSheetObj = PrepareStyleSheet(classSelector, sample, widthKey, widthValue);

    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step4. check the result
     */
    AppStyleSheet* styleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_FALSE(styleSheet == nullptr);
    if (styleSheet == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    AppStyle* sampleSelector = styleSheet->GetStyleFromClassSelectors(sample);
    EXPECT_FALSE(sampleSelector == nullptr);
    if (sampleSelector == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    AppStyleItem* sampleSelectorItem = const_cast<AppStyleItem *>(sampleSelector->GetFirst());
    EXPECT_FALSE(sampleSelectorItem == nullptr);
    if (sampleSelectorItem == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    EXPECT_EQ(sampleSelectorItem->GetNumValue(), widthValue);

    /**
     * @tc.steps: step5. release the style sheet JS object
     */
    jerry_release_value(styleSheetObj);

    /**
     * @tc.steps: step6. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerInlineStyle005()
{
    TestCaseB("ACELiteStyleManagerInlineStyle005");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char classSelector[] = "classSelectors";
    // give an very long selector name
    constexpr char sample[] = "sampleClassSelectorWithVeryLongNameOverSixtyFourCharatersWhichIsTooLong";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 200;
    JSValue styleSheetObj = PrepareStyleSheet(classSelector, sample, widthKey, widthValue);

    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step4. check the result
     */
    AppStyleSheet* styleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_FALSE(styleSheet == nullptr);
    if (styleSheet == nullptr) {
        jerry_release_value(styleSheetObj);
        TDD_CASE_END();
        return;
    }

    /**
     * @tc.steps: step5. release the style sheet JS object
     */
    jerry_release_value(styleSheetObj);

    /**
     * @tc.steps: step6. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerIDSelector001()
{
    TestCaseB("ACELiteStyleManagerIDSelector001");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    DivComponent* divComponent = new DivComponent(UNDEFINED, UNDEFINED, g_testStyleMgr);

    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(g_testStyleMgr->GetStyleSheet() == nullptr);

    divComponent->Release();
    delete divComponent;
    divComponent = nullptr;

    /**
     * @tc.steps: step4. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerIDSelector002()
{
    TestCaseB("ACELiteStyleManagerIDSelector002");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char styleType[] = "staticStyle";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 200;
    jerry_value_t styleOption = PrepareStyleOption(styleType, widthKey, widthValue);
    DivComponent* divComponent = new DivComponent(styleOption, UNDEFINED, g_testStyleMgr);
    divComponent->Render();
    /**
     * @tc.steps: step3. check the result
     */
    EXPECT_TRUE(g_testStyleMgr->GetStyleSheet() == nullptr);

    /**
     * @tc.steps: step4. check the result
     */
    jerry_release_value(styleOption);
    divComponent->Release();
    delete divComponent;
    divComponent = nullptr;

    /**
     * @tc.steps: step5. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerIDSelector003()
{
    TestCaseB("ACELiteStyleManagerIDSelector003");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char idKey[] = "id";
    constexpr char idValue[] = "test";
    jerry_value_t styleOption = PrepareAttrOption(idKey, idValue);
    DivComponent* divComponent = new DivComponent(styleOption, UNDEFINED, g_testStyleMgr);
    divComponent->Render();

    /**
     * @tc.steps: step3. check the result, no style sheet, so no one will be added into combined style
     */
    EXPECT_TRUE(g_testStyleMgr->GetStyleSheet() == nullptr);

    /**
     * @tc.steps: step4. check the result
     */
    jerry_release_value(styleOption);
    divComponent->Release();
    delete divComponent;
    divComponent = nullptr;

    /**
     * @tc.steps: step5. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerIDSelector004()
{
    TestCaseB("ACELiteStyleManagerIDSelector004");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char classSelector[] = "classSelectors";
    constexpr char sample[] = "sample";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 200;
    JSValue styleSheetObj = PrepareStyleSheet(classSelector, sample, widthKey, widthValue);
    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step4. prepare valid style sheet with class selector
     */
    constexpr char styleType[] = "staticStyle";
    constexpr char heightKey[] = "height";
    constexpr int32_t heightValue = 100;
    jerry_value_t option = PrepareStyleOption(styleType, heightKey, heightValue);

    AddValueToAttrOption(option, "staticClass", "sample");
    DivComponent* divComponent = new DivComponent(option, UNDEFINED, g_testStyleMgr);

    const uint8_t valueCount = 2;
    const jerry_value_t jsValues[valueCount] = {option, styleSheetObj};
    const uint8_t componentCount = 1;
    Component* components[1] = {divComponent};

    /**
     * @tc.steps: step5. check the result, no style sheet, so no one will be added into combined style
     */
    EXPECT_FALSE(g_testStyleMgr->GetStyleSheet() == nullptr);
    if (g_testStyleMgr->GetStyleSheet() == nullptr) {
        ReleaseTestResources(jsValues, valueCount, components, componentCount);
        TDD_CASE_END();
        return;
    }

    /**
     * @tc.steps: step6. release test resources
     */
    ReleaseTestResources(jsValues, valueCount, components, componentCount);

    /**
     * @tc.steps: step7. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerIDSelector005()
{
    TestCaseB("ACELiteStyleManagerIDSelector005");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char styleType[] = "staticStyle";
    constexpr char widthKey2[] = "width";
    constexpr int32_t widthValue2 = 100;
    jerry_value_t styleOption = PrepareStyleOption(styleType, widthKey2, widthValue2);
    DivComponent* divComponent = new DivComponent(styleOption, UNDEFINED, g_testStyleMgr);

    constexpr char classSelector[] = "classSelectors";
    constexpr char sample[] = "sample";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 500;
    JSValue styleSheetObj = PrepareStyleSheet(classSelector, sample, widthKey, widthValue);
    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);
    rootComponentMock_.RenderComponent(*divComponent);

    /**
     * @tc.steps: step4. prepare valid style sheet with class selector
     */
    Component* component = reinterpret_cast<Component *>(divComponent);
    UIViewGroup* uiView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());

    const uint8_t jsValueCount = 2;
    const jerry_value_t jerryValues[jsValueCount] = {styleOption, styleSheetObj};
    const uint8_t componentNumber = 1;
    Component* componentInstances[1] = {divComponent};

    /**
     * @tc.steps: step5. check the result, no style sheet, so no one will be added into combined style
     */
    AppStyleSheet* appStyleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_FALSE(appStyleSheet == nullptr);
    if (appStyleSheet == nullptr) {
        ReleaseTestResources(jerryValues, jsValueCount, componentInstances, componentNumber);
        TDD_CASE_END();
        return;
    }

    uint16_t width = uiView->GetWidth();
    EXPECT_EQ(width, widthValue2);

    /**
     * @tc.steps: step6. release test resources
     */
    ReleaseTestResources(jerryValues, jsValueCount, componentInstances, componentNumber);

    /**
     * @tc.steps: step7. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerClassSelector001()
{
    TestCaseB("ACELiteStyleManagerClassSelector001");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    DivComponent* divComponent = new DivComponent(UNDEFINED, UNDEFINED, g_testStyleMgr);
    divComponent->Render();

    /**
     * @tc.steps: step5. check the result, no style sheet, so no one will be added into combined style
     */
    EXPECT_TRUE(const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet()) == nullptr);

    divComponent->Release();
    delete divComponent;
    divComponent = nullptr;

    /**
     * @tc.steps: step4. clean up test environment
     */
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerClassSelector002()
{
    TestCaseB("ACELiteStyleManagerClassSelector002");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style
     */
    constexpr int32_t heightValue = 100;
    constexpr char heightKey[] = "height";
    constexpr char styleType[] = "staticStyle";
    jerry_value_t styleOption = PrepareStyleOption(styleType, heightKey, heightValue);
    DivComponent* divComponent = new DivComponent(styleOption, UNDEFINED, g_testStyleMgr);
    divComponent->Render();

    /**
     * @tc.steps: step3. check the result
     */
    const AppStyleSheet* appStyleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_TRUE(appStyleSheet == nullptr);

    /**
     * @tc.steps: step4. clean up test environment
     */
    jerry_release_value(styleOption);
    divComponent->Release();
    delete divComponent;
    divComponent = nullptr;
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerClassSelector003()
{
    TestCaseB("ACELiteStyleManagerClassSelector003");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step2. prepare valid style sheet with class selector
     */
    constexpr char classSelector[] = "classSelectors";
    constexpr char sample[] = "sample";
    constexpr char widthKey[] = "width";
    constexpr int32_t widthValue = 200;
    JSValue styleSheetObj = PrepareStyleSheet(classSelector, sample, widthKey, widthValue);
    /**
     * @tc.steps: step3. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step4. prepare valid style sheet with class selector
     */
    constexpr char styleType[] = "staticStyle";
    constexpr char widthKey2[] = "width";
    constexpr int32_t widthValue2 = 100;
    jerry_value_t styleOption = PrepareStyleOption(styleType, widthKey2, widthValue2);
    DivComponent* divComponent = new DivComponent(styleOption, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*divComponent);

    const uint8_t jsCount = 2;
    const jerry_value_t jsObjects[jsCount] = {styleOption, styleSheetObj};
    const uint8_t divNumber = 1;
    Component* divComponents[1] = {divComponent};

    /**
     * @tc.steps: step5. check the result, no style sheet, so no one will be added into combined style
     */
    Component* component = reinterpret_cast<Component *>(divComponent);
    UIViewGroup* uiView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    AppStyleSheet* appStyleSheet = const_cast<AppStyleSheet *>(g_testStyleMgr->GetStyleSheet());
    EXPECT_FALSE(appStyleSheet == nullptr);
    if (appStyleSheet == nullptr) {
        ReleaseTestResources(jsObjects, jsCount, divComponents, divNumber);
        TDD_CASE_END();
        return;
    }

    uint16_t width = uiView->GetWidth();
    EXPECT_EQ(width, widthValue2);

    /**
     * @tc.steps: step6. release test resources
     */
    ReleaseTestResources(jsObjects, jsCount, divComponents, divNumber);

    /**
     * @tc.steps: step7. clean up test environment
     */
    TDD_CASE_END();
}

JSValue StyleMgrTddTest::PreparePesudoObjects(uint16_t value) const
{
    constexpr char classSelector[] = "classSelectors";
    constexpr char sample[] = "sample";
    constexpr int32_t activeWidthValue = 300;
    JSValue classSelectorObj = jerry_create_object();
    const uint8_t itemCount = 2;
    char* keys[itemCount];
    uint16_t const values[itemCount] = {value, activeWidthValue};
    keys[0] = const_cast<char *>("width");
    keys[1] = const_cast<char *>("width:active");
    JSValue styleSheetObj =
        PrepareStyleSheetWithKeyValues(classSelector, classSelectorObj, sample, itemCount, keys, values);
    return styleSheetObj;
}

void StyleMgrTddTest::ACELiteStyleManagerCSSPesudo001()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. prepare pesudo class items
     */
    constexpr uint16_t widthValue = 200;
    JSValue styleSheetObj = PreparePesudoObjects(widthValue);

    /**
     * @tc.steps: step2. trigger style sheet initialization
     */
    g_testStyleMgr->InitStyleSheet(styleSheetObj);

    /**
     * @tc.steps: step3. prepare valid style sheet with class selector
     */
    constexpr uint16_t heightValue = 50;
    jerry_value_t option = PrepareStyleOption("staticStyle", "height", heightValue);
    AddStrValueToOption(option, "staticClass", "sample");
    DivComponent* divComponent = new DivComponent(option, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*divComponent);
    const uint8_t jsObjCount = 2;
    const jerry_value_t jsObjs[jsObjCount] = {option, styleSheetObj};
    const uint8_t divComponentNumber = 1;
    Component* divComponentInstances[1] = {divComponent};

    /**
     * @tc.steps: step4. check the result, no style sheet, so no one will be added into combined style
     */
    uint16_t index = KeyParser::ParseKeyId("width");
    EXPECT_TRUE(g_testStyleMgr->isPseudoExist(index, 0x01));
    Component* component = reinterpret_cast<Component *>(divComponent);
    UIViewGroup* uiView = reinterpret_cast<UIViewGroup *>(component->GetComponentRootView());
    if (uiView != nullptr) {
        EXPECT_EQ(uiView->GetWidth(), widthValue);
        EXPECT_EQ(uiView->GetHeight(), heightValue);
    }

    ReleaseTestResources(jsObjs, jsObjCount, divComponentInstances, divComponentNumber);
    TDD_CASE_END();
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
void StyleMgrTddTest::ACELiteStyleManagerFlexItemSizingStyle001()
{
    TDD_CASE_BEGIN();

    JSValue option = CreateStyleOption();
    JSValue style = GetStaticStyle(option);
    SetStyleNumber(style, "flexGrow", FLEX_GROW_VALUE);
    SetStyleNumber(style, "flexShrink", FLEX_SHRINK_VALUE);
    SetStyleNumber(style, "flexBasis", FLEX_BASIS_VALUE);
    SetStyleString(style, "alignSelf", "stretch");

    DivComponent* divComponent = new DivComponent(option, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*divComponent);
    Component* component = static_cast<Component *>(divComponent);
    UIView* view = component->GetComponentRootView();

    EXPECT_NE(view, nullptr);
    if (view != nullptr) {
        EXPECT_EQ(view->GetFlexGrow(), FLEX_GROW_VALUE);
        EXPECT_EQ(view->GetFlexShrink(), FLEX_SHRINK_VALUE);
        EXPECT_EQ(view->GetFlexBasis(), FLEX_BASIS_VALUE);
        EXPECT_EQ(view->GetAlignSelf(), UIView::ALIGN_SELF_STRETCH);
    }

    jerry_release_value(style);
    Component* components[1] = {divComponent};
    const jerry_value_t values[1] = {option};
    ReleaseTestResources(values, 1, components, 1);
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerAlignContentStyle001()
{
    TDD_CASE_BEGIN();

    JSValue option = CreateStyleOption();
    JSValue style = GetStaticStyle(option);
    SetStyleString(style, "alignContent", "space-between");

    DivComponent* divComponent = new DivComponent(option, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*divComponent);
    Component* component = static_cast<Component *>(divComponent);
    FlexLayout* view = static_cast<FlexLayout *>(component->GetComponentRootView());

    EXPECT_NE(view, nullptr);
    if (view != nullptr) {
        EXPECT_EQ(view->GetAlignContent(), ALIGN_CONTENT_BETWEEN);
    }

    jerry_release_value(style);
    Component* components[1] = {divComponent};
    const jerry_value_t values[1] = {option};
    ReleaseTestResources(values, 1, components, 1);
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerSizeConstraintsStyle001()
{
    TDD_CASE_BEGIN();

    JSValue option = CreateStyleOption();
    JSValue style = GetStaticStyle(option);
    SetStyleNumber(style, "width", SIZE_CONSTRAINT_WIDTH_VALUE);
    SetStyleNumber(style, "height", SIZE_CONSTRAINT_HEIGHT_VALUE);
    SetStyleNumber(style, "minWidth", SIZE_CONSTRAINT_MIN_WIDTH_VALUE);
    SetStyleNumber(style, "maxWidth", SIZE_CONSTRAINT_MAX_WIDTH_VALUE);
    SetStyleNumber(style, "minHeight", SIZE_CONSTRAINT_MIN_HEIGHT_VALUE);
    SetStyleNumber(style, "maxHeight", SIZE_CONSTRAINT_MAX_HEIGHT_VALUE);
    SetStyleNumber(style, "aspectRatio", SIZE_CONSTRAINT_ASPECT_RATIO_STYLE_VALUE);

    DivComponent* divComponent = new DivComponent(option, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*divComponent);
    Component* component = static_cast<Component*>(divComponent);
    UIView* view = component->GetComponentRootView();

    EXPECT_NE(view, nullptr);
    if (view != nullptr) {
        EXPECT_EQ(view->GetWidth(), SIZE_CONSTRAINT_WIDTH_VALUE);
        EXPECT_EQ(view->GetHeight(), SIZE_CONSTRAINT_HEIGHT_VALUE);
        EXPECT_EQ(view->GetMinWidth(), SIZE_CONSTRAINT_MIN_WIDTH_VALUE);
        EXPECT_EQ(view->GetMaxWidth(), SIZE_CONSTRAINT_MAX_WIDTH_VALUE);
        EXPECT_EQ(view->GetMinHeight(), SIZE_CONSTRAINT_MIN_HEIGHT_VALUE);
        EXPECT_EQ(view->GetMaxHeight(), SIZE_CONSTRAINT_MAX_HEIGHT_VALUE);
        EXPECT_EQ(view->GetAspectRatio(), SIZE_CONSTRAINT_ASPECT_RATIO_VALUE);
    }

    jerry_release_value(style);
    Component* components[1] = {divComponent};
    const jerry_value_t values[1] = {option};
    ReleaseTestResources(values, 1, components, 1);
    TDD_CASE_END();
}

void StyleMgrTddTest::ACELiteStyleManagerSizeConstraintsPercentStyle001()
{
    TDD_CASE_BEGIN();

    JSValue percentOption = CreateStyleOption();
    JSValue percentStyle = GetStaticStyle(percentOption);
    SetStyleString(percentStyle, "aspectRatio", SIZE_CONSTRAINT_ASPECT_RATIO_PERCENT);
    DivComponent* percentComponent = new DivComponent(percentOption, UNDEFINED, g_testStyleMgr);
    rootComponentMock_.RenderComponent(*percentComponent);
    Component* percentBaseComponent = static_cast<Component*>(percentComponent);
    UIView* percentView = percentBaseComponent->GetComponentRootView();
    EXPECT_NE(percentView, nullptr);
    if (percentView != nullptr) {
        EXPECT_EQ(percentView->GetAspectRatio(), SIZE_CONSTRAINT_ASPECT_RATIO_VALUE);
    }

    jerry_release_value(percentStyle);
    Component* components[1] = {percentComponent};
    const jerry_value_t values[1] = {percentOption};
    ReleaseTestResources(values, 1, components, 1);
    TDD_CASE_END();
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

void StyleMgrTddTest::RunTests()
{
    ACELiteStyleManagerInlineStyle001();
    ACELiteStyleManagerInlineStyle002();
    ACELiteStyleManagerInlineStyle003();
    ACELiteStyleManagerInlineStyle004();
    ACELiteStyleManagerInlineStyle005();
    ACELiteStyleManagerIDSelector001();
    ACELiteStyleManagerIDSelector002();
    ACELiteStyleManagerIDSelector003();
    ACELiteStyleManagerIDSelector004();
    ACELiteStyleManagerIDSelector005();
    ACELiteStyleManagerClassSelector001();
    ACELiteStyleManagerClassSelector002();
    ACELiteStyleManagerClassSelector003();
    ACELiteStyleManagerCSSPesudo001();
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    ACELiteStyleManagerFlexItemSizingStyle001();
    ACELiteStyleManagerAlignContentStyle001();
    ACELiteStyleManagerSizeConstraintsStyle001();
    ACELiteStyleManagerSizeConstraintsPercentStyle001();
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ACELiteStyleManagerInlineStyle001
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, InlineStyle001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerInlineStyle001();
}

/**
 * @tc.name: ACELiteStyleManagerInlineStyle002
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, InlineStyle002, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerInlineStyle002();
}

/**
 * @tc.name: ACELiteStyleManagerInlineStyle003
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, InlineStyle003, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerInlineStyle003();
}

/**
 * @tc.name: ACELiteStyleManagerInlineStyle004
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, InlineStyle004, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerInlineStyle004();
}

/**
 * @tc.name: ACELiteStyleManagerInlineStyle005
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, InlineStyle005, TestSize.Level0)
{
    StyleMgrTddTest::ACELiteStyleManagerInlineStyle005();
}

/**
 * @tc.name: ACELiteStyleManagerIDSelector001
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, IDSelector001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerIDSelector001();
}

/**
 * @tc.name: ACELiteStyleManagerIDSelector002
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, IDSelector002, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerIDSelector002();
}

/**
 * @tc.name: ACELiteStyleManagerIDSelector003
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, IDSelector003, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerIDSelector003();
}

/**
 * @tc.name: ACELiteStyleManagerIDSelector004
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, IDSelector004, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerIDSelector004();
}

/**
 * @tc.name: ACELiteStyleManagerIDSelector005
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, IDSelector005, TestSize.Level0)
{
    StyleMgrTddTest::ACELiteStyleManagerIDSelector005();
}

/**
 * @tc.name: ACELiteStyleManagerClassSelector001
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, ClassSelector001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerClassSelector001();
}

/**
 * @tc.name: ACELiteStyleManagerClassSelector002
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, ClassSelector002, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerClassSelector002();
}

/**
 * @tc.name: ACELiteStyleManagerClassSelector003
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, ClassSelector003, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerClassSelector003();
}

/**
 * @tc.name: ACELiteStyleManagerCSSPesudo001
 * @tc.desc: Verify the style sheet initialization.
 */
HWTEST_F(StyleMgrTddTest, CSSPesudo001, TestSize.Level0)
{
    StyleMgrTddTest::ACELiteStyleManagerCSSPesudo001();
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
/**
 * @tc.name: ACELiteStyleManagerFlexItemSizingStyle001
 * @tc.desc: Verify flex item sizing styles are applied.
 */
HWTEST_F(StyleMgrTddTest, FlexItemSizingStyle001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerFlexItemSizingStyle001();
}

/**
 * @tc.name: ACELiteStyleManagerAlignContentStyle001
 * @tc.desc: Verify alignContent style is applied to flex layout.
 */
HWTEST_F(StyleMgrTddTest, AlignContentStyle001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerAlignContentStyle001();
}

/**
 * @tc.name: ACELiteStyleManagerSizeConstraintsStyle001
 * @tc.desc: Verify fixed sizes, min/max constraints, and aspect ratio styles are applied.
 */
HWTEST_F(StyleMgrTddTest, SizeConstraintsStyle001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerSizeConstraintsStyle001();
}

/**
 * @tc.name: ACELiteStyleManagerSizeConstraintsPercentStyle001
 * @tc.desc: Verify percent aspect ratio style is applied.
 */
HWTEST_F(StyleMgrTddTest, SizeConstraintsPercentStyle001, TestSize.Level1)
{
    StyleMgrTddTest::ACELiteStyleManagerSizeConstraintsPercentStyle001();
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#endif
} // ACELite
} // OHOS

