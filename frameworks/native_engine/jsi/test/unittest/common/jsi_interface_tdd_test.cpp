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

#include "jsi_interface_tdd_test.h"
#include "acelite_config.h"
#include "jerryscript-core.h"
#include "js_debugger_config.h"
#include "jsi_types.h"
#include <cstdio>
#include <cstring>

namespace OHOS {
namespace ACELite {
#define TO_JERRY_VALUE(a) ((jerry_value_t)(uintptr_t)(a))
#define TO_JSI_VALUE(a) ((JSIValue)(uintptr_t)(a))

static int8_t g_descValue = 0;

JsiInterfaceTddTest::JsiInterfaceTddTest() {}

void JsiInterfaceTddTest::SetUp()
{
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
}

void JsiInterfaceTddTest::TearDown()
{
    jerry_cleanup();
    Debugger::GetInstance().ReleaseJSContext();
}

JSIValue JsiInterfaceTddTest::Function(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum)
{
    (void)thisVal;
    (void)args;
    (void)argsNum;
    printf("Function called\n");
    return JSI::CreateUndefined();
}

JSIValue JsiInterfaceTddTest::Setter(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum)
{
    (void)thisVal;
    (void)argsNum;
    double newValue = JSI::ValueToNumber(args[0]);
    g_descValue = (int8_t)newValue;

    printf("Setter called, setting: %d\n", g_descValue);
    return JSI::CreateUndefined();
}

JSIValue JsiInterfaceTddTest::Getter(const JSIValue thisVal, const JSIValue args[], uint8_t argsNum)
{
    (void)thisVal;
    (void)args;
    (void)argsNum;
    g_descValue++;
    printf("Getter called, returning: %d\n", g_descValue);
    return JSI::CreateNumber(g_descValue);
}

void JsiInterfaceTddTest::OnDestroy(int8_t statusCode)
{
    printf("OnDestroy called, statusCode=%d\n", statusCode);
}

void JsiInterfaceTddTest::OnTerminate(int8_t statusCode)
{
    printf("OnTerminate called, statusCode=%d\n", statusCode);
}

void JsiInterfaceTddTest::JSIInterfaceTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateObject
     */
    JSIValue object = JSI::CreateObject();
    /**
     * @tc.expected: step1. ValueIsObject return true
     */
    bool res = JSI::ValueIsObject(object);
    if (res) {
        printf("JSIInterfaceTest001 pass\n");
    } else {
        printf("JSIInterfaceTest001 fail\n");
    }
    EXPECT_TRUE(res);
    JSI::ReleaseValue(object);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI GetGlobalObject
     */
    JSIValue global = JSI::GetGlobalObject();
    /**
     * @tc.expected: step1. global is not undefined
     */
    if (JSI::ValueIsUndefined(global)) {
        printf("JSIInterfaceTest002 fail\n");
    } else {
        printf("JSIInterfaceTest002 pass\n");
    }
    EXPECT_FALSE(JSI::ValueIsUndefined(global));
    JSI::ReleaseValue(global);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI GetJSHeapStatus
     */
    JSHeapStatus status;
    bool res = JSI::GetJSHeapStatus(status);
    /**
     * @tc.expected: step1. res is true
     */
    if (res) {
        printf("JSIInterfaceTest003 JSHeapStatus:totalBytes=%d allocBytes=%d peakAllocBytes=%d\n", status.totalBytes,
               status.allocBytes, status.peakAllocBytes);
        printf("JSIInterfaceTest003 pass\n");
    } else {
        printf("JSIInterfaceTest003 fail\n");
    }
    EXPECT_TRUE(res);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateArray
     */
    uint32_t initLength = 10;
    JSIValue array = JSI::CreateArray(initLength);

    /**
     * @tc.steps: step2. call JSI ValueIsArray with Array object
     */
    bool res1 = JSI::ValueIsArray(array);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsArray with Undefined object
     */
    bool res2 = JSI::ValueIsArray(undefined);

    /**
     * @tc.steps: step4. call JSI GetArrayLength
     */
    uint32_t length = JSI::GetArrayLength(array);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and length = 10
     */
    if (res1 && !res2 && (length == initLength)) {
        printf("JSIInterfaceTest004 pass\n");
    } else {
        printf("JSIInterfaceTest004 fail\n");
    }
    EXPECT_TRUE(res1 && !res2 && (length == initLength));
    JSI::ReleaseValueList(array, undefined);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateFunction
     */
    JSIValue func = JSI::CreateFunction(Function);

    /**
     * @tc.steps: step2. call JSI ValueIsFunction with Function object
     */
    bool res1 = JSI::ValueIsFunction(func);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsFunction with Undefined object
     */
    bool res2 = JSI::ValueIsFunction(undefined);

    /**
     * @tc.steps: step4. call JSI CallFunction
     */
    JSI::CallFunction(func, undefined, nullptr, 0);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and Function was called
     */
    if (res1 && !res2) {
        printf("JSIInterfaceTest005 pass\n");
    } else {
        printf("JSIInterfaceTest005 fail\n");
    }
    EXPECT_TRUE(res1 && !res2);
    JSI::ReleaseValueList(func, undefined);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest006()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateFunction
     */
    double initNum = 123;
    JSIValue num = JSI::CreateNumber(initNum);

    /**
     * @tc.steps: step2. call JSI ValueIsNumber with Number object
     */
    bool res1 = JSI::ValueIsNumber(num);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsNumber with Undefined object
     */
    bool res2 = JSI::ValueIsNumber(undefined);

    /**
     * @tc.steps: step4. call JSI ValueToNumber
     */
    int32_t numValue = (int32_t)JSI::ValueToNumber(num);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and numValue = 123
     */
    if (res1 && !res2 && numValue == (int32_t)initNum) {
        printf("JSIInterfaceTest006 pass\n");
    } else {
        printf("JSIInterfaceTest006 fail\n");
    }
    EXPECT_TRUE(res1 && !res2 && numValue == (int32_t)initNum);
    JSI::ReleaseValueList(num, undefined);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest007()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateString
     */
    JSIValue str = JSI::CreateString("test");

    /**
     * @tc.steps: step2. call JSI ValueIsString with String object
     */
    bool res1 = JSI::ValueIsString(str);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsString with Undefined object
     */
    bool res2 = JSI::ValueIsString(undefined);

    /**
     * @tc.steps: step4. call JSI ValueToString
     */
    char *strValue = JSI::ValueToString(str);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "test")) {
        printf("JSIInterfaceTest007 pass\n");
    } else {
        printf("JSIInterfaceTest007 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "test"));
    JSI::ReleaseValueList(str, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest008()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateBoolean
     */
    JSIValue jsBool = JSI::CreateBoolean(true);

    /**
     * @tc.steps: step2. call JSI ValueIsBoolean with Boolean object
     */
    bool res1 = JSI::ValueIsBoolean(jsBool);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsBoolean with Undefined object
     */
    bool res2 = JSI::ValueIsBoolean(undefined);

    /**
     * @tc.steps: step4. call JSI ValueToBoolean
     */
    bool jsBoolValue = JSI::ValueToBoolean(jsBool);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and jsBoolValue = true
     */
    if (res1 && !res2 && jsBoolValue) {
        printf("JSIInterfaceTest008 pass\n");
    } else {
        printf("JSIInterfaceTest008 fail\n");
    }
    EXPECT_TRUE(res1 && !res2 && jsBoolValue);
    JSI::ReleaseValueList(jsBool, undefined);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest009()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateUndefined
     */
    JSIValue undef = JSI::CreateUndefined();

    /**
     * @tc.steps: step2. call JSI ValueIsUndefined with Undefined object
     */
    bool res1 = JSI::ValueIsUndefined(undef);
    JSIValue obj = JSI::CreateObject();

    /**
     * @tc.steps: step3. call JSI ValueIsUndefined with obj
     */
    bool res2 = JSI::ValueIsUndefined(obj);
    /**
     * @tc.expected: step2-step3. res1 = true, res2 = false
     */
    if (res1 && !res2) {
        printf("JSIInterfaceTest009 pass\n");
    } else {
        printf("JSIInterfaceTest009 fail\n");
    }
    EXPECT_TRUE(res1 && !res2);
    JSI::ReleaseValueList(undef, obj);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest010()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateNull
     */
    JSIValue nullObj = JSI::CreateNull();

    /**
     * @tc.steps: step2. call JSI ValueIsNull with Null object
     */
    bool res1 = JSI::ValueIsNull(nullObj);
    JSIValue obj = JSI::CreateObject();

    /**
     * @tc.steps: step3. call JSI ValueIsNull with obj
     */
    bool res2 = JSI::ValueIsNull(obj);
    /**
     * @tc.expected: step2-step3. res1 = true, res2 = false
     */
    if (res1 && !res2) {
        printf("JSIInterfaceTest010 pass\n");
    } else {
        printf("JSIInterfaceTest010 fail\n");
    }
    EXPECT_TRUE(res1 && !res2);
    JSI::ReleaseValueList(nullObj, obj);
    TDD_CASE_END();
}

#if IS_ENABLED(JS_FWK_SYMBOL)
void JsiInterfaceTddTest::JSIInterfaceTest011()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateSymbol
     */
    JSIValue strValue = JSI::CreateString("Symbol description string");
    JSIValue symbol = JSI::CreateSymbol(strValue);

    /**
     * @tc.steps: step2. call JSI ValueIsSymbol with Symbol object
     */
    bool res1 = JSI::ValueIsSymbol(symbol);
    JSIValue undef = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsSymbol with undef
     */
    bool res2 = JSI::ValueIsSymbol(undef);
    /**
     * @tc.expected: step2-step3. res1 = true, res2 = false
     */
    if (res1 && !res2) {
        printf("JSIInterfaceTest011 pass\n");
    } else {
        printf("JSIInterfaceTest011 fail\n");
    }
    EXPECT_TRUE(res1 && !res2);
    JSI::ReleaseValueList(strValue, symbol);
    TDD_CASE_END();
}
#endif // ENABLED(JS_FWK_SYMBOL)

void JsiInterfaceTddTest::JSIInterfaceTest012()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateObject
     */
    JSIValue obj = JSI::CreateObject();

    /**
     * @tc.steps: step2. call JSI ValueIsObject with obj
     */
    bool res1 = JSI::ValueIsObject(obj);
    JSIValue undef = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsObject with undef
     */
    bool res2 = JSI::ValueIsObject(undef);
    /**
     * @tc.expected: step2-step3. res1 = true, res2 = false
     */
    if (!res1 || res2) {
        printf("JSIInterfaceTest012 fail\n");
    }
    EXPECT_FALSE(!res1 || res2);

    /**
     * @tc.steps: step4. call JSI AcquireValue
     */
    JSIValue acquire = JSI::AcquireValue(obj);
    if (acquire != undef) {
        printf("JSIInterfaceTest012 pass\n");
    }
    EXPECT_TRUE(acquire != undef);
    /**
     * @tc.steps: step5. call JSI ReleaseValue on acquire
     */
    JSI::ReleaseValue(acquire);
    /**
     * @tc.expected: step5. no exception log
     */
    JSI::ReleaseValueList(obj, undef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest013()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI JsonParse
     */
    JSIValue jsonValue = JSI::JsonParse("{\"key1\":\"value1\",\"key2\":\"value2\"}");

    /**
     * @tc.steps: step2. call JSI ValueIsObject with obj
     */
    char *jsonStr = JSI::JsonStringify(jsonValue);
    /**
     * @tc.expected: step2. jsonStr = "{\"key1\":\"value1\",\"key2\":\"value2\"}"
     */
    if ((jsonStr != nullptr) && !strcmp(jsonStr, "{\"key1\":\"value1\",\"key2\":\"value2\"}")) {
        printf("JSIInterfaceTest013 pass\n");
    } else {
        printf("JSIInterfaceTest013 fail\n");
    }
    EXPECT_TRUE((jsonStr != nullptr) && !strcmp(jsonStr, "{\"key1\":\"value1\",\"key2\":\"value2\"}"));
    JSI::ReleaseValue(jsonValue);
    JSI::ReleaseString(jsonStr);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest014()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI SetNumberProperty
     */
    JSIValue obj = JSI::CreateObject();
    double initNum = 123;
    JSI::SetNumberProperty(obj, "num", initNum);

    /**
     * @tc.steps: step2. call JSI GetNumberProperty
     */
    int32_t num = (int32_t)JSI::GetNumberProperty(obj, "num");
    /**
     * @tc.expected: step2. num = 123
     */
    if (num == (int32_t)initNum) {
        printf("JSIInterfaceTest014 pass\n");
    } else {
        printf("JSIInterfaceTest014 fail\n");
    }
    EXPECT_TRUE(num == (int32_t)initNum);
    JSI::ReleaseValue(obj);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest015()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI SetBooleanProperty
     */
    JSIValue obj = JSI::CreateObject();
    JSI::SetBooleanProperty(obj, "bool", true);

    /**
     * @tc.steps: step2. call JSI GetBooleanProperty
     */
    bool boolValue = JSI::GetBooleanProperty(obj, "bool");
    /**
     * @tc.expected: step2. boolValue = true
     */
    if (boolValue) {
        printf("JSIInterfaceTest015 pass\n");
    } else {
        printf("JSIInterfaceTest015 fail\n");
    }
    EXPECT_TRUE(boolValue);
    JSI::ReleaseValue(obj);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest016()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI SetStringProperty
     */
    JSIValue obj = JSI::CreateObject();
    JSI::SetStringProperty(obj, "string", "test");

    /**
     * @tc.steps: step2. call JSI GetStringProperty
     */
    char *str = JSI::GetStringProperty(obj, "string");
    /**
     * @tc.expected: step2. str = "test"
     */
    if (str != nullptr && !strcmp(str, "test")) {
        printf("JSIInterfaceTest016 pass\n");
    } else {
        printf("JSIInterfaceTest016 fail\n");
    }
    EXPECT_TRUE(str != nullptr && !strcmp(str, "test"));
    JSI::ReleaseValue(obj);
    JSI::ReleaseString(str);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest017()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI SetPropertyByIndex
     */
    uint32_t initLength = 2;
    double initNum = 123;
    JSIValue array = JSI::CreateArray(initLength);
    JSIValue numValue = JSI::CreateNumber(initNum);
    JSIValue strValue = JSI::CreateString("test");
    JSI::SetPropertyByIndex(array, 0, numValue);
    JSI::SetPropertyByIndex(array, 1, strValue);

    /**
     * @tc.steps: step2. call JSI GetPropertyByIndex
     */
    JSIValue numValueGet = JSI::GetPropertyByIndex(array, 0);
    JSIValue strValueGet = JSI::GetPropertyByIndex(array, 1);
    int32_t num = (int32_t)JSI::ValueToNumber(numValueGet);
    char *str = JSI::ValueToString(strValueGet);
    /**
     * @tc.expected: step2. str = "test", num = 123
     */
    if ((str != nullptr) && !strcmp(str, "test") && (num == (int32_t)initNum)) {
        printf("JSIInterfaceTest017 pass\n");
    } else {
        printf("JSIInterfaceTest017 fail\n");
    }
    EXPECT_TRUE((str != nullptr) && !strcmp(str, "test") && (num == (int32_t)initNum));
    JSI::ReleaseValueList(array, numValue, strValue, numValueGet, strValueGet);
    JSI::ReleaseString(str);
    TDD_CASE_END();
}

#if IS_ENABLED(JS_FWK_TYPEDARRAY)
void JsiInterfaceTddTest::JSIInterfaceTest018()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateArrayBuffer
     */
    uint8_t *ptr = nullptr;
    size_t initLength = 2;
    uint8_t value1 = 128;
    uint8_t value2 = 255;
    JSIValue buffer = JSI::CreateArrayBuffer(initLength, ptr);
    if (!ptr) {
        JSI::ReleaseValue(buffer);
        printf("JSIInterfaceTest018 fail\n");
        EXPECT_TRUE(false);
        return;
    }
    ptr[0] = value1;
    ptr[1] = value2;
    JSIValue undef = JSI::CreateUndefined();

    /**
     * @tc.steps: step2. call JSI ValueIsArrayBuffer
     */
    bool res1 = JSI::ValueIsArrayBuffer(buffer);

    /**
     * @tc.steps: step3. call JSI ValueIsArrayBuffer with undef
     */
    bool res2 = JSI::ValueIsArrayBuffer(undef);

    /**
     * @tc.steps: step4. call JSI GetArrayBufferInfo
     */
    size_t length;
    uint8_t *ptr2 = JSI::GetArrayBufferInfo(buffer, length);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false, length = 2
     */
    if ((ptr2 != nullptr) && res1 && !res2 && (length == initLength) && (ptr2[0] == value1) && (ptr2[1] == value2)) {
        printf("JSIInterfaceTest018 pass\n");
    } else {
        printf("JSIInterfaceTest018 fail\n");
    }
    EXPECT_TRUE((ptr2 != nullptr) && res1 && !res2 && (length == initLength) && (ptr2[0] == value1) &&
                (ptr2[1] == value2));
    JSI::ReleaseValueList(buffer, undef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest019()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. create an ArrayBuffer
     */
    uint8_t *ptr = nullptr;
    size_t initLength = 4;
    uint8_t value1 = 128;
    uint8_t value2 = 255;
    size_t initOffset = 2;
    JSIValue buffer = JSI::CreateArrayBuffer(initLength, ptr);
    if (!ptr) {
        JSI::ReleaseValue(buffer);
        printf("JSIInterfaceTest019 fail\n");
        EXPECT_TRUE(false);
        return;
    }
    ptr[0] = 0;
    ptr[1] = 0;
    ptr[initOffset] = value1;
    ptr[initOffset + 1] = value2;

    /**
     * @tc.steps: step2. create an TypedArray
     */
    size_t byteLength = 2;
    JSIValue typedArray = JSI::CreateTypedArray(TypedArrayType::JSI_UINT8_ARRAY, byteLength, buffer, initOffset);
    JSI::ReleaseValue(buffer);
    JSIValue undef = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsTypedArray
     */
    bool res1 = JSI::ValueIsTypedArray(typedArray);

    /**
     * @tc.steps: step4. call JSI ValueIsTypedArray with undef
     */
    bool res2 = JSI::ValueIsTypedArray(undef);

    /**
     * @tc.steps: step5. call JSI GetTypedArrayInfo
     */
    TypedArrayType type;
    size_t length, offset;
    JSIValue arrayBuffer;
    uint8_t *ptr2 = JSI::GetTypedArrayInfo(typedArray, type, length, arrayBuffer, offset);
    /**
     * @tc.expected: step3-step5. res1 = true, res2 = false, typedArray params are correct
     */
    if (res1 && !res2 && (type == TypedArrayType::JSI_UINT8_ARRAY) && (length == byteLength) &&
        (offset == initOffset) && ptr2 != nullptr && (ptr2[offset] == value1) && (ptr2[offset + 1] == value2)) {
        printf("JSIInterfaceTest019 pass\n");
    } else {
        printf("JSIInterfaceTest019 fail\n");
    }
    EXPECT_TRUE(res1 && !res2 && (type == TypedArrayType::JSI_UINT8_ARRAY) && (length == byteLength) &&
                (offset == initOffset) && ptr2 != nullptr && (ptr2[offset] == value1) && (ptr2[offset + 1] == value2));

    JSI::ReleaseValueList(typedArray, undef, arrayBuffer);
    TDD_CASE_END();
}
#endif // ENABLED(JS_FWK_TYPEDARRAY)

void JsiInterfaceTddTest::JSIInterfaceTest020()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. initial JSPropertyDescriptor object
     */
    JSPropertyDescriptor desc;
    const char *propName = "propTest";
    desc.getter = Getter;
    desc.setter = Setter;

    /**
     * @tc.steps: step2. call JSI DefineNamedProperty
     */
    JSIValue globalObj = JSI::GetGlobalObject();
    bool res = JSI::DefineNamedProperty(globalObj, propName, desc);
    JSI::ReleaseValue(globalObj);

    /**
     * @tc.steps: step3. run an example js code to use the getter/setter
     */
    const char *jerrySrc = "this.propTest; this.propTest; this.propTest = 4; this.propTest";
    jerry_value_t evalResult =
        jerry_eval(reinterpret_cast<const jerry_char_t *>(jerrySrc), strlen(jerrySrc), JERRY_PARSE_NO_OPTS);
    int8_t num = jerry_get_number_value(evalResult);
    uint8_t expectedNum = 5;
    /**
     * @tc.expected: step3. res = true, num = 5
     */
    if (res && (num == expectedNum)) {
        printf("JSIInterfaceTest020 pass\n");
    } else {
        printf("JSIInterfaceTest020 fail\n");
    }
    EXPECT_TRUE(res && (num == expectedNum));
    jerry_release_value(evalResult);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest021()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. create an object and set properties
     */
    JSIValue object = JSI::CreateObject();
    const char *key1 = "key1";
    const char *key2 = "key2";
    JSI::SetNumberProperty(object, key1, 0);
    JSI::SetNumberProperty(object, key2, 1);

    /**
     * @tc.steps: step2. call JSI GetObjectKeys
     */
    JSIValue keys = JSI::GetObjectKeys(object);
    JSIValue keyValue1 = JSI::GetPropertyByIndex(keys, 0);
    JSIValue keyValue2 = JSI::GetPropertyByIndex(keys, 1);
    char *keyAcquired1 = JSI::ValueToString(keyValue1);
    char *keyAcquired2 = JSI::ValueToString(keyValue2);
    int32_t num1 = JSI::GetNumberProperty(object, keyAcquired1);
    int32_t num2 = JSI::GetNumberProperty(object, keyAcquired2);
    /**
     * @tc.expected: step2. keyAcquired1 = key1, num1 = 0, keyAcquired2 = key2, num1 = 1
     */
    if (keyAcquired1 != nullptr && !strcmp(keyAcquired1, key1) && num1 == 0 && keyAcquired2 != nullptr &&
        !strcmp(keyAcquired2, key2) && num2 == 1) {
        printf("JSIInterfaceTest021 pass\n");
    } else {
        printf("JSIInterfaceTest021 fail\n");
    }
    EXPECT_TRUE(keyAcquired1 != nullptr && !strcmp(keyAcquired1, key1) && num1 == 0 && keyAcquired2 != nullptr &&
                !strcmp(keyAcquired2, key2) && num2 == 1);

    JSI::ReleaseValueList(object, keys, keyValue1, keyValue2);
    JSI::ReleaseString(keyAcquired1);
    JSI::ReleaseString(keyAcquired2);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest022()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. create an object and call JSI SetOnDestroy
     */
    JSIValue object = JSI::CreateObject();
    JSI::SetOnDestroy(object, OnDestroy);

    /**
     * @tc.steps: step2. get destroy callback from object
     */
    const char *name = "onDestroyHandler";
    JSIValue funObj = JSI::GetNamedProperty(object, name);
    void *nativePtr = nullptr;
    jerry_value_t jObj = (jerry_value_t)(uintptr_t)(funObj);
    bool exist = jerry_get_object_native_pointer(jObj, &nativePtr, nullptr);
    /**
     * @tc.expected: step2. invoke callback and the OnDestroy function will be called
     */
    if (!exist || (nativePtr == nullptr)) {
        printf("JSIInterfaceTest022 fail\n");
    } else {
        JsiCallback callback = reinterpret_cast<JsiCallback>(nativePtr);
        (*callback)(JSI_ERR_OK);
        printf("JSIInterfaceTest022 pass\n");
    }
    EXPECT_TRUE(nativePtr != nullptr);
    jerry_delete_object_native_pointer(jObj, nullptr);
    JSI::ReleaseValueList(object, funObj);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest023()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. create an object and call JSI SetOnTerminate
     */
    JSIValue object = JSI::CreateObject();
    JSI::SetOnTerminate(object, OnTerminate);

    /**
     * @tc.steps: step2. get terminate callback from object
     */
    const char *name = "onTerminateHandler";
    JSIValue funObj = JSI::GetNamedProperty(object, name);
    void *nativePtr = nullptr;
    jerry_value_t jObj = (jerry_value_t)(uintptr_t)(funObj);
    bool exist = jerry_get_object_native_pointer(jObj, &nativePtr, nullptr);
    /**
     * @tc.expected: step2. invoke callback and the OnTerminate function will be called
     */
    if (!exist || (nativePtr == nullptr)) {
        printf("JSIInterfaceTest023 fail\n");
    } else {
        JsiCallback callback = reinterpret_cast<JsiCallback>(nativePtr);
        (*callback)(JSI_ERR_OK);
        printf("JSIInterfaceTest023 pass\n");
    }
    EXPECT_TRUE(nativePtr != nullptr);
    jerry_delete_object_native_pointer(jObj, nullptr);
    JSI::ReleaseValueList(object, funObj);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest024()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateString
     */
    JSIValue str = JSI::CreateString("test");

    /**
     * @tc.steps: step2. call JSI ValueIsString with String object
     */
    bool res1 = JSI::ValueIsString(str);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsString with Undefined object
     */
    bool res2 = JSI::ValueIsString(undefined);

    /**
     * @tc.steps: step4. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(str);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "test")) {
        printf("JSIInterfaceTest024 pass\n");
    } else {
        printf("JSIInterfaceTest024 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "test"));
    JSI::ReleaseValueList(str, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest025()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateString
     */
    double initNum = 123;
    JSIValue num = JSI::CreateNumber(initNum);

    /**
     * @tc.steps: step2. call JSI ValueIsNumber with Number object
     */
    bool res1 = JSI::ValueIsNumber(num);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsNumber with Undefined object
     */
    bool res2 = JSI::ValueIsNumber(undefined);

    /**
     * @tc.steps: step4. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(num);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "123")) {
        printf("JSIInterfaceTest025 pass\n");
    } else {
        printf("JSIInterfaceTest025 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "123"));
    JSI::ReleaseValueList(num, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest026()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateBoolean
     */
    JSIValue jsBool = JSI::CreateBoolean(true);

    /**
     * @tc.steps: step2. call JSI ValueIsBoolean with Boolean object
     */
    bool res1 = JSI::ValueIsBoolean(jsBool);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsBoolean with Undefined object
     */
    bool res2 = JSI::ValueIsBoolean(undefined);

    /**
     * @tc.steps: step4. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(jsBool);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "true")) {
        printf("JSIInterfaceTest026 pass\n");
    } else {
        printf("JSIInterfaceTest026 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "true"));
    JSI::ReleaseValueList(jsBool, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest027()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateObject
     */
    JSIValue object = JSI::CreateObject();
    /**
     * @tc.expected: step2. ValueIsObject return true
     */
    bool res1 = JSI::ValueIsObject(object);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsObject with Undefined object
     */
    bool res2 = JSI::ValueIsObject(undefined);

    /**
     * @tc.steps: step4. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(object);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "[object Object]")) {
        printf("JSIInterfaceTest027 pass\n");
    } else {
        printf("JSIInterfaceTest027 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "[object Object]"));
    JSI::ReleaseValueList(object, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest028()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateArray
     */
    uint32_t initLength = 10;
    JSIValue array = JSI::CreateArray(initLength);

    /**
     * @tc.steps: step2. call JSI ValueIsArray with Array object
     */
    bool res1 = JSI::ValueIsArray(array);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsArray with Undefined object
     */
    bool res2 = JSI::ValueIsArray(undefined);

    /**
     * @tc.steps: step4. call JSI GetArrayLength
     */
    uint32_t length = JSI::GetArrayLength(array);

    /**
     * @tc.steps: step5. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(array);
    /**
     * @tc.expected: step2-step5. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && (length == initLength) && !strcmp(strValue, ",,,,,,,,,")) {
        printf("JSIInterfaceTest028 pass\n");
    } else {
        printf("JSIInterfaceTest028 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && (length == initLength) && !strcmp(strValue, ",,,,,,,,,"));
    JSI::ReleaseValueList(array, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest029()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call JSI CreateFunction
     */
    JSIValue func = JSI::CreateFunction(Function);

    /**
     * @tc.steps: step2. call JSI ValueIsFunction with Function object
     */
    bool res1 = JSI::ValueIsFunction(func);
    JSIValue undefined = JSI::CreateUndefined();

    /**
     * @tc.steps: step3. call JSI ValueIsFunction with Undefined object
     */
    bool res2 = JSI::ValueIsFunction(undefined);

    /**
     * @tc.steps: step4. call JSI JSIValueToString
     */
    char *strValue = JSI::JSIValueToString(func);
    /**
     * @tc.expected: step2-step4. res1 = true, res2 = false and strValue = "test"
     */
    if ((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "function(){/* ecmascript */}")) {
        printf("JSIInterfaceTest029 pass\n");
    } else {
        printf("JSIInterfaceTest029 fail\n");
    }
    EXPECT_TRUE((strValue != nullptr) && res1 && !res2 && !strcmp(strValue, "function(){/* ecmascript */}"));
    JSI::ReleaseValueList(func, undefined);
    JSI::ReleaseString(strValue);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest030()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with null msg
     */
    JSIValue errRef = JSI::CreateErrorWithCode(JSI_ERR_CODE_PERMISSION_DENIED, nullptr);
    /**
     * @tc.expected: step1. errRef = undefined
     */
    JSIValue undefinedValue = JSI::CreateUndefined();
    if (errRef == undefinedValue) {
        printf("JSIInterfaceTest030 pass\n");
    } else {
        printf("JSIInterfaceTest030 fail\n");
    }
    EXPECT_TRUE(errRef == undefinedValue);
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest031()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with valid msg
     */
    JSIValue errRef = JSI::CreateErrorWithCode(0, "this is error message");
    /**
     * @tc.expected: step1. errRef is valid
     */
    if (JSI::ValueIsError(errRef)) {
        printf("JSIInterfaceTest031 pass\n");
    } else {
        printf("JSIInterfaceTest031 fail\n");
    }
    EXPECT_TRUE(JSI::ValueIsError(errRef));
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest032()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with very long msg which is over 256
     */
    const char *veryLongMessage =
        "this is one very long test message which is used to check if the length can be verified by the function, this "
        "is one very long test message which is used to check if the length can be verified by the function, this is "
        "one very long test message which is used to check if the length can be verified by the function.";
    JSIValue errRef = JSI::CreateErrorWithCode(JSI_ERR_CODE_NOT_SUPPORTED, veryLongMessage);
    /**
     * @tc.expected: step1. errRef = undefined
     */
    JSIValue undefValue = JSI::CreateUndefined();
    if (errRef == undefValue) {
        printf("JSIInterfaceTest032 pass\n");
    } else {
        printf("JSIInterfaceTest032 fail\n");
    }
    EXPECT_TRUE(errRef == undefValue);
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest033()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with empty string
     */
    JSIValue errResult = JSI::CreateErrorWithCode(JSI_ERR_CODE_PARAM_CHECK_FAILED, "");
    /**
     * @tc.expected: step1. errRef is valid
     */
    if (JSI::ValueIsError(errResult)) {
        printf("JSIInterfaceTest033 pass\n");
    } else {
        printf("JSIInterfaceTest033 fail\n");
    }
    EXPECT_TRUE(JSI::ValueIsError(errResult));
    JSI::ReleaseValue(errResult);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest034()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with extra data
     */
    JSIValue extraData = JSI::CreateObject();
    JSIValue errRef = JSI::CreateErrorWithCode(JSI_ERR_CODE_NOT_SUPPORTED, "error info", extraData);
    /**
     * @tc.expected: step1. errRef is valid
     */
    bool result = JSI::ValueIsError(errRef);
    if (result) {
        printf("JSIInterfaceTest034 pass\n");
    } else {
        printf("JSIInterfaceTest034 fail\n");
    }
    EXPECT_TRUE(result);
    JSI::ReleaseValue(extraData);
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest035()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with extra data
     */
    JSIValue extraData = JSI::CreateObject();
    JSI::SetStringProperty(extraData, "info", "message");
    const char *errMsg = "method is not support";
    JSIValue errRef = JSI::CreateErrorWithCode(JSI_ERR_CODE_NOT_SUPPORTED, errMsg, extraData);
    /**
     * @tc.expected: step1. errRef is valid
     */
    JSIValue resultExtracted = TO_JSI_VALUE(jerry_get_value_from_error(TO_JERRY_VALUE(errRef), false));
    bool result = JSI::ValueIsObject(resultExtracted);
    if (result) {
        result = (JSI::GetNumberProperty(resultExtracted, "code") == JSI_ERR_CODE_NOT_SUPPORTED);
    }
    if (result) {
        printf("JSIInterfaceTest035 pass\n");
    } else {
        printf("JSIInterfaceTest035 fail\n");
    }
    EXPECT_TRUE(result);
    JSI::ReleaseValue(resultExtracted);
    JSI::ReleaseValue(extraData);
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest036()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with extra data
     */
    JSIValue extraData = JSI::CreateObject();
    JSI::SetStringProperty(extraData, "test", "info");
    const char *errMsgContent = "check failed";
    JSIValue errRef = JSI::CreateErrorWithCode(JSI_ERR_CODE_PARAM_CHECK_FAILED, errMsgContent, extraData);

    /**
     * @tc.expected: step1. errRef is valid
     */
    JSIValue extractedValue = TO_JSI_VALUE(jerry_get_value_from_error(TO_JERRY_VALUE(errRef), false));
    bool msgCheckResult = JSI::ValueIsObject(extractedValue);
    if (msgCheckResult) {
        char *messageStr = JSI::GetStringProperty(extractedValue, "message");
        msgCheckResult = (messageStr != nullptr && strcmp(messageStr, errMsgContent) == 0);
    }
    if (msgCheckResult) {
        printf("JSIInterfaceTest036 pass\n");
    } else {
        printf("JSIInterfaceTest036 fail\n");
    }
    EXPECT_TRUE(msgCheckResult);
    // clean up
    JSI::ReleaseValue(extractedValue);
    JSI::ReleaseValue(extraData);
    JSI::ReleaseValue(errRef);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest037()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with extra data
     */
    JSIValue extraDataObject = JSI::CreateObject();
    const char *propertyKey = "dataKey";
    JSI::SetStringProperty(extraDataObject, propertyKey, "result");
    const char *notSupportedMsg = "not supported";
    JSIValue errRefToBeCheck = JSI::CreateErrorWithCode(JSI_ERR_CODE_NOT_SUPPORTED, notSupportedMsg, extraDataObject);

    /**
     * @tc.expected: step1. errRef is valid, check data property
     */
    JSIValue extractV = TO_JSI_VALUE(jerry_get_value_from_error(TO_JERRY_VALUE(errRefToBeCheck), false));
    bool dataCheckResult = JSI::ValueIsObject(extractV);
    if (dataCheckResult) {
        JSIValue dataPropertyValue = JSI::GetNamedProperty(extractV, "data");
        dataCheckResult = JSI::ValueIsObject(dataPropertyValue);
        char *dataValue = JSI::GetStringProperty(dataPropertyValue, propertyKey);
        if (dataValue != nullptr && strcmp(dataValue, propertyKey) == 0) {
            dataCheckResult = true;
        }
        JSI::ReleaseValue(dataPropertyValue);
    }
    if (dataCheckResult) {
        printf("JSIInterfaceTest037 pass\n");
    } else {
        printf("JSIInterfaceTest037 fail\n");
    }
    EXPECT_TRUE(dataCheckResult);
    // clean up
    JSI::ReleaseValue(extractV);
    JSI::ReleaseValue(extraDataObject);
    JSI::ReleaseValue(errRefToBeCheck);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::JSIInterfaceTest038()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. try to create one error with invalid extra data
     */
    JSIValue functionValue = JSI::CreateFunction(Function); // not obj type
    const char *errorInfo = "not supported";
    JSIValue errResult = JSI::CreateErrorWithCode(JSI_ERR_CODE_NOT_SUPPORTED, errorInfo, functionValue);

    /**
     * @tc.expected: step1. errRef is valid, check data property
     */
    JSIValue extractedVal = TO_JSI_VALUE(jerry_get_value_from_error(TO_JERRY_VALUE(errResult), false));
    bool dataCheckRes = JSI::ValueIsObject(extractedVal);
    if (dataCheckRes) {
        JSIValue dataPropertyValue = JSI::GetNamedProperty(extractedVal, "data");
        dataCheckRes = JSI::ValueIsUndefined(dataPropertyValue);
        JSI::ReleaseValue(dataPropertyValue);
    }
    if (!dataCheckRes) {
        printf("JSIInterfaceTest038 pass\n");
    } else {
        printf("JSIInterfaceTest038 fail\n");
    }
    EXPECT_FALSE(dataCheckRes);
    // clean up
    JSI::ReleaseValue(extractedVal);
    JSI::ReleaseValue(functionValue);
    JSI::ReleaseValue(errResult);
    TDD_CASE_END();
}

void JsiInterfaceTddTest::RunTests()
{
    JSIInterfaceTest001();
    JSIInterfaceTest002();
    JSIInterfaceTest003();
    JSIInterfaceTest004();
    JSIInterfaceTest005();
    JSIInterfaceTest006();
    JSIInterfaceTest007();
    JSIInterfaceTest008();
    JSIInterfaceTest009();
    JSIInterfaceTest010();
#if IS_ENABLED(JS_FWK_SYMBOL)
    JSIInterfaceTest011();
#endif // ENABLED(JS_FWK_SYMBOL)
    JSIInterfaceTest012();
    JSIInterfaceTest013();
    JSIInterfaceTest014();
    JSIInterfaceTest015();
    JSIInterfaceTest016();
    JSIInterfaceTest017();
#if IS_ENABLED(JS_FWK_TYPEDARRAY)
    JSIInterfaceTest018();
    JSIInterfaceTest019();
#endif // ENABLED(JS_FWK_TYPEDARRAY)
    JSIInterfaceTest020();
    JSIInterfaceTest021();
    JSIInterfaceTest022();
    JSIInterfaceTest023();
    JSIInterfaceTest024();
    JSIInterfaceTest025();
    JSIInterfaceTest026();
    JSIInterfaceTest027();
    JSIInterfaceTest028();
    JSIInterfaceTest029();
    JSIInterfaceTest030();
    JSIInterfaceTest031();
    JSIInterfaceTest032();
    JSIInterfaceTest033();
    JSIInterfaceTest034();
    JSIInterfaceTest035();
    JSIInterfaceTest036();
    JSIInterfaceTest037();
    JSIInterfaceTest038();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: JSIInterfaceTest001
 * @tc.desc: Verify JSI interface CreateObject.
 */
HWTEST_F(JsiInterfaceTddTest, test001, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest001();
}

/**
 * @tc.name: JSIInterfaceTest002
 * @tc.desc: Verify JSI interface GetGlobalObject.
 */
HWTEST_F(JsiInterfaceTddTest, test002, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest002();
}

/**
 * @tc.name: JSIInterfaceTest003
 * @tc.desc: Verify JSI interface GetJSHeapStatus.
 */
HWTEST_F(JsiInterfaceTddTest, test003, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest003();
}

/**
 * @tc.name: JSIInterfaceTest004
 * @tc.desc: Verify JSI interface related to Array object.
 */
HWTEST_F(JsiInterfaceTddTest, test004, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest004();
}

/**
 * @tc.name: JSIInterfaceTest005
 * @tc.desc: Verify JSI interface related to Function object.
 */
HWTEST_F(JsiInterfaceTddTest, test005, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest005();
}

/**
 * @tc.name: JSIInterfaceTest006
 * @tc.desc: Verify JSI interface related to Number object.
 */
HWTEST_F(JsiInterfaceTddTest, test006, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest006();
}

/**
 * @tc.name: JSIInterfaceTest007
 * @tc.desc: Verify JSI interface related to String object.
 */
HWTEST_F(JsiInterfaceTddTest, test007, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest007();
}

/**
 * @tc.name: JSIInterfaceTest008
 * @tc.desc: Verify JSI interface related to Boolean object.
 */
HWTEST_F(JsiInterfaceTddTest, test008, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest008();
}

/**
 * @tc.name: JSIInterfaceTest009
 * @tc.desc: Verify JSI interface related to Undefined object.
 */
HWTEST_F(JsiInterfaceTddTest, test009, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest009();
}

/**
 * @tc.name: JSIInterfaceTest010
 * @tc.desc: Verify JSI interface related to Null object.
 */
HWTEST_F(JsiInterfaceTddTest, test010, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest010();
}

#if IS_ENABLED(JS_FWK_SYMBOL)
/**
 * @tc.name: JSIInterfaceTest011
 * @tc.desc: Verify JSI interface related to Symbol object.
 */
HWTEST_F(JsiInterfaceTddTest, test011, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest011();
}
#endif // ENABLED(JS_FWK_SYMBOL)

/**
 * @tc.name: JSIInterfaceTest012
 * @tc.desc: Verify JSI interface related to JS Object.
 */
HWTEST_F(JsiInterfaceTddTest, test012, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest012();
}

/**
 * @tc.name: JSIInterfaceTest013
 * @tc.desc: Verify JSI interface related to JS JSON.
 */
HWTEST_F(JsiInterfaceTddTest, test013, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest013();
}

/**
 * @tc.name: JSIInterfaceTest014
 * @tc.desc: Verify JSI interface related to JS Number property.
 */
HWTEST_F(JsiInterfaceTddTest, test014, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest014();
}

/**
 * @tc.name: JSIInterfaceTest015
 * @tc.desc: Verify JSI interface related to JS Boolean property.
 */
HWTEST_F(JsiInterfaceTddTest, test015, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest015();
}

/**
 * @tc.name: JSIInterfaceTest016
 * @tc.desc: Verify JSI interface related to JS String property.
 */
HWTEST_F(JsiInterfaceTddTest, test016, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest016();
}

/**
 * @tc.name: JSIInterfaceTest017
 * @tc.desc: Verify JSI interface related to JS property with index.
 */
HWTEST_F(JsiInterfaceTddTest, test017, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest017();
}

#if IS_ENABLED(JS_FWK_TYPEDARRAY)
/**
 * @tc.name: JSIInterfaceTest018
 * @tc.desc: Verify JSI interface related to JS ArrayBuffer.
 */
HWTEST_F(JsiInterfaceTddTest, test018, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest018();
}

/**
 * @tc.name: JSIInterfaceTest019
 * @tc.desc: Verify JSI interface related to JS TypedArray.
 */
HWTEST_F(JsiInterfaceTddTest, test019, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest019();
}
#endif

/**
 * @tc.name: JSIInterfaceTest020
 * @tc.desc: Verify JSI interface related to JS descriptor.
 */
HWTEST_F(JsiInterfaceTddTest, test020, TestSize.Level1)
{
    JsiInterfaceTddTest::JSIInterfaceTest020();
}

/**
 * @tc.name: JSIInterfaceTest021
 * @tc.desc: Verify JSI interface GetObjectKeys.
 */
HWTEST_F(JsiInterfaceTddTest, test021, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest021();
}

/**
 * @tc.name: JSIInterfaceTest022
 * @tc.desc: Verify JSI interfaces related to OnDestroy callback.
 */
HWTEST_F(JsiInterfaceTddTest, test022, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest022();
}

/**
 * @tc.name: JSIInterfaceTest023
 * @tc.desc: Verify JSI interfaces related to OnTerminate callback.
 */
HWTEST_F(JsiInterfaceTddTest, test023, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest023();
}

/**
 * @tc.name: JSIInterfaceTest024
 * @tc.desc: Verify JSI interfaces related to JS String to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test024, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest024();
}

/**
 * @tc.name: JSIInterfaceTest025
 * @tc.desc: Verify JSI interfaces related to JS Number to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test025, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest025();
}

/**
 * @tc.name: JSIInterfaceTest026
 * @tc.desc: Verify JSI interfaces related to JS bool to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test026, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest026();
}

/**
 * @tc.name: JSIInterfaceTest027
 * @tc.desc: Verify JSI interfaces related to JS Object to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test027, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest027();
}

/**
 * @tc.name: JSIInterfaceTest028
 * @tc.desc: Verify JSI interfaces related to JS array to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test028, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest028();
}

/**
 * @tc.name: JSIInterfaceTest029
 * @tc.desc: Verify JSI interfaces related to JS func to String.
 * @tc.require: I5NCYY
 */
HWTEST_F(JsiInterfaceTddTest, test029, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest029();
}

/**
 * @tc.name: JSIInterfaceTest030
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test030, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest030();
}

/**
 * @tc.name: JSIInterfaceTest031
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test031, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest031();
}

/**
 * @tc.name: JSIInterfaceTest032
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test032, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest032();
}

/**
 * @tc.name: JSIInterfaceTest033
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test033, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest033();
}

/**
 * @tc.name: JSIInterfaceTest034
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test034, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest034();
}

/**
 * @tc.name: JSIInterfaceTest035
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test035, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest035();
}

/**
 * @tc.name: JSIInterfaceTest036
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test036, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest036();
}

/**
 * @tc.name: JSIInterfaceTest037
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test037, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest037();
}

/**
 * @tc.name: JSIInterfaceTest038
 * @tc.desc: Verify JSI create error function.
 * @tc.require: I5TIPU
 */
HWTEST_F(JsiInterfaceTddTest, test038, TestSize.Level0)
{
    JsiInterfaceTddTest::JSIInterfaceTest038();
}
#endif
} // namespace ACELite
} // namespace OHOS
