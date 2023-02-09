/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <cstring>

#include "ace_log.h"
#include "locale_util.h"
#include "key_parser.h"
#include "keys.h"
#include "image.h"
#include "image_module.h"
namespace OHOS {
namespace ACELite {
constexpr jerry_object_native_info_t ImageModule::gcCallback;
const char * const ImageModule::attrOnload = "onload";
const char * const ImageModule::attrOnError = "onerror";
const char * const ImageModule::attrSrc = "src";
const char * const ImageModule::attrWidth = "width";
const char * const ImageModule::attrHeight = "height";
const char * const ImageModule::className = "Image";

void ImageModule::Init(jerry_value_t intlHandle)
{
    (void)intlHandle;
    jerry_value_t gObj = jerry_get_global_object();
    JerrySetFuncProperty(gObj, className, CreateImage);
}
void ImageModule::RegisterAttributeFunc(jerry_value_t canvas2dContext,
                                        const char *attributeName,
                                        jerry_external_handler_t setterHandler,
                                        jerry_external_handler_t getterHandler)
{
    // register canvas js attribute setter property via the jerry_define_own_property method
    jerry_value_t propName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attributeName));
    jerry_property_descriptor_t desc;
    jerry_init_property_descriptor_fields(&desc);
    desc.is_set_defined = true;
    desc.setter = jerry_create_external_function(setterHandler);
    desc.is_get_defined = true;
    desc.getter = jerry_create_external_function(getterHandler);
    jerry_value_t returnValue = jerry_define_own_property(canvas2dContext, propName, &desc);
    jerry_free_property_descriptor_fields(&desc);
    ReleaseJerryValue(propName, returnValue, VA_ARG_END_FLAG);
}
jerry_value_t ImageModule::CreateImage(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum)
{
    if (!jerry_value_is_constructor(func)) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("use new to create Image"));
    }
    ImageModule *imageModule = new ImageModule();
    if (imageModule == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("memory is not enough"));
    }

    imageModule->width_ = -1;
    imageModule->height_ = -1;
    if (argsNum >= 1) {
        if (jerry_value_is_number(args[0])) {
            imageModule->width_ = jerry_value_to_number(args[0]);
        } else if (jerry_value_is_string(args[0])) {
            char* val = MallocStringOf(args[0]);
            if (val == nullptr) {
                return jerry_create_error(JERRY_ERROR_EVAL,
                                          reinterpret_cast<const jerry_char_t *>("val is nullptr"));
            }
            imageModule->width_ = atoi(val);
            ACE_FREE(val);
        }
    }
    if (argsNum >= static_cast<uint32_t>(ArgsCount::NUM_2)) {
        if (jerry_value_is_number(args[1])) {
            imageModule->height_ = jerry_value_to_number(args[1]);
        } else if (jerry_value_is_string(args[1])) {
            char* val = MallocStringOf(args[1]);
            if (val == nullptr) {
                return jerry_create_error(JERRY_ERROR_EVAL,
                                          reinterpret_cast<const jerry_char_t *>("val is nullptr"));
            }
            imageModule->height_ = atoi(val);
            ACE_FREE(val);
        }
    }

    jerry_set_object_native_pointer(context, imageModule, &gcCallback);
    RegisterAttributeFunc(context, attrWidth, OnLoadSetter, OnLoadGetter);
    RegisterAttributeFunc(context, attrOnError, OnErrorSetter, OnErrorGetter);
    RegisterAttributeFunc(context, attrSrc, OnSrcSetter, OnSrcGetter);
    RegisterAttributeFunc(context, attrWidth, OnWidthSetter, OnWidthGetter);
    RegisterAttributeFunc(context, attrHeight, OnHeightSetter, OnHeightGetter);
    return UNDEFINED;
}

jerry_value_t ImageModule::OnLoadSetter(const jerry_value_t func,
                                        const jerry_value_t context,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum)
{
    (void)func;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (argsNum == 1 && jerry_value_is_function(args[0])) {
        jerry_release_value(imageModel->onLoadFunc_);
        imageModel->onLoadFunc_ = args[0];
    } else {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("set imageModel onload arg fail"));
    }
    return UNDEFINED;
}

jerry_value_t ImageModule::OnLoadGetter(const jerry_value_t func,
                                        const jerry_value_t context,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    return imageModel->onLoadFunc_;
}

jerry_value_t ImageModule::OnErrorSetter(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (argsNum >= 1 && jerry_value_is_function(args[0])) {
        jerry_release_value(imageModel->onErrorFunc_);
        imageModel->onErrorFunc_ = args[0];
    } else {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("set imageModel onload arg fail"));
    }
    return UNDEFINED;
}

jerry_value_t ImageModule::OnErrorGetter(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    return imageModel->onErrorFunc_;
}

jerry_value_t ImageModule::OnSrcSetter(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum)
{
    (void)func;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (argsNum >= 1) {
        ACE_FREE(imageModel->src_);
        imageModel->src_ = MallocStringOf(args[0]);
    }
    if (imageModel->src_ == nullptr || strlen(imageModel->src_) == 0) {
        OnCallBack(context, imageModel, false, "src is null");
        return UNDEFINED;
    }
    jerry_release_value(imageModel->jerrySrc_);
    imageModel->jerrySrc_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>(imageModel->src_));
    Image image;
    bool isLoad = image.PreParse(imageModel->src_);
    if (!isLoad) {
        OnCallBack(context, imageModel, false, "unknown image");
        return UNDEFINED;
    }
    OnCallBack(context, imageModel, true, "load sucess");
    return UNDEFINED;
}

jerry_value_t ImageModule::OnSrcGetter(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (jerry_value_is_string(imageModel->jerrySrc_)) {
        return imageModel->jerrySrc_;
    }
    return UNDEFINED;
}

jerry_value_t ImageModule::OnHeightSetter(const jerry_value_t func,
                                          const jerry_value_t context,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (argsNum < 1) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("set height fail"));
    }
    imageModel->height_ = IntegerOf(args[0]);
    return UNDEFINED;
}

jerry_value_t ImageModule::OnHeightGetter(const jerry_value_t func,
                                          const jerry_value_t context,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    return imageModel->height_;
}

jerry_value_t ImageModule::OnWidthSetter(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    if (argsNum < 1) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("set width fail"));
    }
    imageModel->width_ = IntegerOf(args[0]);
    return UNDEFINED;
}

jerry_value_t ImageModule::OnWidthGetter(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    ImageModule* imageModel = nullptr;
    jerry_get_object_native_pointer(context, (void**)&imageModel, &gcCallback);
    if (imageModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("get imageModel fail"));
    }
    return imageModel->width_;
}

void ImageModule::OnCallBack(const jerry_value_t context,
                             const ImageModule *imageModule,
                             bool isSucess,
                             const char* msg)
{
    jerry_value_t  targetFunction;
    if (imageModule == nullptr) {
        return;
    }
    if (isSucess) {
        targetFunction = imageModule->onLoadFunc_;
    } else {
        targetFunction = imageModule->onErrorFunc_;
    }
    if (!jerry_value_is_function(targetFunction)) {
        return;
    }
    const int16_t argcount = 1;
    jerry_value_t arg = jerry_create_string(reinterpret_cast<const jerry_char_t *>(msg));
    jerry_value_t args[argcount] = {arg};
    jerry_value_t ret_val = jerry_call_function (targetFunction, context, args, argcount);
    if (jerry_value_is_error (ret_val)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "call back failed!");
    }
    jerry_release_value(arg);
    jerry_release_value (ret_val);
}
}
}

