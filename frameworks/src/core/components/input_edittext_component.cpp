/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "acelite_config.h"
#if (FEATURE_COMPONENT_EDITTEXT == 1)
#include "input_edittext_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#include "product_adapter.h"

namespace OHOS {
namespace ACELite {
bool InputEditTextComponent::CreateNativeViews()
{
    fontSize_ = ProductAdapter::GetDefaultFontSize();
    // set font family
    if (!CopyFontFamily(fontFamily_, ProductAdapter::GetDefaultFontFamilyName())) {
        return false;
    }
    return true;
}

void InputEditTextComponent::ReleaseNativeViews()
{
    if (onValueChangeListener_ != nullptr) {
        delete onValueChangeListener_;
        onValueChangeListener_ = nullptr;
    }
    ACE_FREE(fontFamily_);
}

bool InputEditTextComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    switch (attrKeyId) {
        case K_TYPE:
            if (jerry_value_is_string(attrValue)) {
                char *type = MallocStringOf(attrValue);
                if (strcmp(type, "password")) {
                    edittext_.SetInputType(InputType::TEXT_TYPE);
                } else {
                    edittext_.SetInputType(InputType::PASSWORD_TYPE);
                }
                ACE_FREE(type);
                return true;
            }
            HILOG_ERROR(HILOG_MODULE_ACE, "the edittext type is error value");
            break;
        case K_VALUE:
            if (jerry_value_is_string(attrValue)) {
                char *value = MallocStringOf(attrValue);
                edittext_.SetText(value);
                ACE_FREE(value);
                return true;
            }
            HILOG_ERROR(HILOG_MODULE_ACE, "the edittext value is error value");
            break;
        case K_PLACEHOLDER:
            if (jerry_value_is_string(attrValue)) {
                char *placeholder = MallocStringOf(attrValue);
                edittext_.SetPlaceholder(placeholder);
                ACE_FREE(placeholder);
                return true;
            }
            HILOG_ERROR(HILOG_MODULE_ACE, "the edittext placeholder is error value");
            break;
        case K_MAX_LENGTH:
            if (jerry_value_is_number(attrValue)) {
                uint16_t maxLenth = IntegerOf(attrValue);
                edittext_.SetMaxLength(maxLenth);
                return true;
            }
            HILOG_ERROR(HILOG_MODULE_ACE, "the edittext maxLenth is error value");
            break;
        default:
            return false;
    }
    return false;
}

bool InputEditTextComponent::RegisterPrivateEventListener(uint16_t eventTypeId,
                                                          jerry_value_t funcValue,
                                                          bool isStopPropagation)
{
    if (eventTypeId == K_CHANGE) {
        if (onValueChangeListener_) {
            HILOG_ERROR(HILOG_MODULE_ACE, "valueChange listener existed");
            return false;
        }
        onValueChangeListener_ = new ValueChangeListener(funcValue);
        if (onValueChangeListener_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create valueChange listener failed");
            return false;
        }
        edittext_.SetOnChangeListener(onValueChangeListener_);
        return true;
    }
    return false;
}

bool InputEditTextComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    bool result = true;
    uint16_t styleKey = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(styleKey)) {
        return false;
    }
    switch (styleKey) {
        case K_FONT_SIZE:
            result = SetFontSize(style);
            break;
        case K_COLOR:
            result = SetColor(*style);
            break;
        case K_PLACEHOLDER_COLOR:
            result = SetPlaceholderColor(*style);
            break;
        default:
            result = false;
            break;
    }
    return result;
}

void InputEditTextComponent::PostUpdate(uint16_t attrKeyId)
{
    if (!KeyParser::IsKeyValid(attrKeyId)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input component post update check args failed");
        return;
    }
    switch (attrKeyId) {
        case K_VALUE:
            break;
        case K_FONT_SIZE:
        case K_FONT_FAMILY:
            if (fontFamily_ != nullptr) {
                edittext_.SetFont(fontFamily_, fontSize_);
            }
            break;
        default:
            break;
    }
}

uint8_t InputEditTextComponent::GetFontSize()
{
    return fontSize_;
}


bool InputEditTextComponent::SetFontSize(const AppStyleItem *style)
{
    fontSize_ = GetStylePixelValue(style);
    return true;
}

bool InputEditTextComponent::SetColor(const AppStyleItem &style)
{
    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    if (GetStyleColorValue(&style, color, alpha)) {
        edittext_.SetStyle(STYLE_TEXT_COLOR, GetRGBColor(color).full);
        edittext_.SetStyle(STYLE_TEXT_OPA, alpha);
        return true;
    }
    return false;
}

bool InputEditTextComponent::SetPlaceholderColor(const AppStyleItem &style)
{
    uint32_t color = 0;
    OHOS::ColorType color32;
    uint8_t alpha = OPA_OPAQUE;
    if (GetStyleColorValue(&style, color, alpha)) {
        color32.full = color;
        edittext_.SetPlaceholderColor(color32);
        return true;
    }
    return false;
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_VIDEO