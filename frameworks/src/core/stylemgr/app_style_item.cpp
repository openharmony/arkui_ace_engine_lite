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

#include "stylemgr/app_style_item.h"
#include "ace_log.h"
#include "keys.h"
#include "number_parser.h"

namespace OHOS {
namespace ACELite {
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
bool AppStyleItem::TrySetAspectRatioValue(uint16_t keyId, const jerry_value_t stylePropValue)
{
    if (keyId != K_ASPECT_RATIO) {
        return false;
    }
    SetFloatingValue(jerry_get_number_value(stylePropValue));
    return true;
}

bool AppStyleItem::TrySetGapValue(const char *strValueBuffer, uint16_t keyId)
{
    if (keyId != K_GAP) {
        return false;
    }
    SetStringValue(strValueBuffer);
    return true;
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

uint8_t AppStyleItem::EstimatePseudoClassType(const char * const styleKey, uint16_t *keyLength)
{
    char *p = strchr(const_cast<char *>(styleKey), ':');
    if (p == nullptr) {
        return PSEUDO_CLASS_UNKNOWN;
    }

    uint8_t type = PSEUDO_CLASS_UNKNOWN;
    // :active meets
    if (!strcmp(p, PSEUDO_CLASS_TYPE_ACTIVE)) {
        type = PSEUDO_CLASS_ACTIVE;
    } else if (!strcmp(p, PSEUDO_CLASS_TYPE_CHECKED)) {
        type = PSEUDO_CLASS_CHECKED;
    }

    // drop :xxxx
    if (type != PSEUDO_CLASS_UNKNOWN) {
        *p = '\0';
        *keyLength = (uint16_t)(p - styleKey);
    }
    return type;
}

void AppStyleItem::SetStringValue(const char * const value)
{
    if (value == nullptr) {
        return;
    }
    size_t len = strlen(value);
    if (len >= UINT16_MAX) {
        return;
    }
    valueType_ = STYLE_PROP_VALUE_TYPE_STRING;
    styleValue_.string = static_cast<char *>(ace_malloc(sizeof(char) * (len + 1)));
    if (styleValue_.string == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create style item string failed.");
        return;
    }
    if (memcpy_s(styleValue_.string, len, value, len) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "app_style set string value error");
        ace_free(styleValue_.string);
        styleValue_.string = nullptr;
        return;
    }
    *(styleValue_.string + len) = '\0';
}

#if FEATURE_PATH_ANIMATOR
bool AppStyleItem::SetPathPolylineValue(const OHOS::PathPolyline &polyline)
{
    OHOS::PathPolyline *copy = new OHOS::PathPolyline(polyline);
    if (copy == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create path polyline value failed.");
        return false;
    }
    valueType_ = STYLE_PROP_VALUE_TYPE_PATH_POLYLINE;
    styleValue_.pathPolyline = copy;
    return true;
}

void AppStyleItem::SetNumberOffsetRotateValue(const jerry_value_t stylePropValue)
{
    /* number input means a fixed angle in degrees (W3C CSS Motion Path) */
    SetOffsetRotateValue(OFFSET_ROTATE_FIXED,
                         static_cast<int16_t>(jerry_get_number_value(stylePropValue)));
}

void AppStyleItem::ParseStringPathValue(uint16_t keyId, const char *strValueBuffer)
{
    if (keyId == K_OFFSET_ROTATE) {
        /* parse once at creation time: the item stores the structured value instead of the raw string */
        OffsetRotateMode mode;
        int16_t degree;
        if (AppStylePathParser::ParseOffsetRotate(strValueBuffer, mode, degree)) {
            SetOffsetRotateValue(mode, degree);
        }
    } else if (keyId == K_OFFSET_PATH) {
        /* parse once at creation time: on failure the item stays UNKNOWN and no path animation starts */
        OHOS::PathPolyline polyline;
        if (AppStylePathParser::ParseOffsetPath(strValueBuffer, polyline)) {
            SetPathPolylineValue(polyline);
        }
    }
}
#endif

bool AppStyleItem::UpdateNumValToStr()
{
    if (GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        int32_t numVal = GetNumValue();
        const uint8_t len = 11; // max size of int32_t
        char strVal[len] = { 0 };
        if (sprintf_s(strVal, len, "%d", numVal) < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "style item transform num to string fail");
            return false;
        }
        SetStringValue(static_cast<const char *>(strVal));
    }
    return true;
}

AppStyleItem *AppStyleItem::GenerateFromJSValue(jerry_value_t stylePropName, jerry_value_t stylePropValue)
{
    uint16_t strLen = 0;
    char *keyNameBuffer = MallocStringOf(stylePropName, &strLen);
    if (keyNameBuffer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "convert style prop name to char failed, will be dropped");
        return nullptr;
    }

    if (strLen == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "style prop name length is 0, will be dropped");
        ace_free(keyNameBuffer);
        keyNameBuffer = nullptr;
        return nullptr;
    }

    uint8_t pseudoType_ = EstimatePseudoClassType(static_cast<char *>(keyNameBuffer), &strLen);
    AppStyleItem *styleItem =
        CreateStyleItem(KeyParser::ParseKeyId(static_cast<const char *>(keyNameBuffer), strLen),
                        stylePropValue, pseudoType_);
    ace_free(keyNameBuffer);
    keyNameBuffer = nullptr;
    return styleItem;
}

AppStyleItem *AppStyleItem::CreateStyleItem(uint16_t keyId, const jerry_value_t stylePropValue, uint8_t pseudoClassType)
{
    AppStyleItem *newStyleItem = new AppStyleItem();
    if (newStyleItem == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "new styleItem error");
        return nullptr;
    }

    char *strValueBuffer = nullptr;

    newStyleItem->propNameId_ = keyId;
    newStyleItem->pseudoClassType_ = pseudoClassType;

    if (jerry_value_is_number(stylePropValue)) {
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
        if (newStyleItem->TrySetAspectRatioValue(keyId, stylePropValue)) {
            return newStyleItem;
        }
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#if FEATURE_COMPONENT_TEXT_SPANNABLE
        if (keyId == K_OPACITY || keyId == K_RELATIVESIZESPANSIZE) {
#else
        if (keyId == K_OPACITY) {
#endif
            newStyleItem->SetFloatingValue(jerry_get_number_value(stylePropValue));
#if FEATURE_PATH_ANIMATOR
        } else if (keyId == K_OFFSET_ROTATE) {
            newStyleItem->SetNumberOffsetRotateValue(stylePropValue);
#endif
        } else {
            newStyleItem->SetNumValue((int32_t)(jerry_get_number_value(stylePropValue)));
        }
    } else if (jerry_value_is_boolean(stylePropValue)) {
        newStyleItem->SetBoolValue(jerry_get_boolean_value(stylePropValue));
    } else {
        uint16_t strLength = 0;
        strValueBuffer = MallocStringOf(stylePropValue, &strLength);
        if (strValueBuffer == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "convert style value to char failed, will be dropped");
            delete newStyleItem;
            newStyleItem = nullptr;
            return nullptr;
        }
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
        if (newStyleItem->TrySetGapValue(strValueBuffer, keyId)) {
            ace_free(strValueBuffer);
            strValueBuffer = nullptr;
            return newStyleItem;
        }
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
        float percentValue = 0;
        if (NumberParser::ParsePercentValue(strValueBuffer, strLength, percentValue)) {
            newStyleItem->SetPercentValue(percentValue);
#if FEATURE_PATH_ANIMATOR
        } else if (keyId == K_OFFSET_ROTATE || keyId == K_OFFSET_PATH) {
            newStyleItem->ParseStringPathValue(keyId, strValueBuffer);
#endif
        } else {
            newStyleItem->SetStringValue(static_cast<const char *>(strValueBuffer));
        }
        ace_free(strValueBuffer);
        strValueBuffer = nullptr;
    }

    return newStyleItem;
}

AppStyleItem *AppStyleItem::CopyFrom(const AppStyleItem *from)
{
    if (from == nullptr) {
        return nullptr;
    }

    AppStyleItem *styleItem = new AppStyleItem();
    if (styleItem == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "new styleItem error");
        return nullptr;
    }

    styleItem->UpdateValueFrom(*from);

    return styleItem;
}

void AppStyleItem::UpdateValueFrom(const AppStyleItem &from)
{
    if (valueType_ == STYLE_PROP_VALUE_TYPE_STRING) {
        ACE_FREE(styleValue_.string);
    }
#if FEATURE_PATH_ANIMATOR
    else if (valueType_ == STYLE_PROP_VALUE_TYPE_PATH_POLYLINE) {
        delete styleValue_.pathPolyline;
        styleValue_.pathPolyline = nullptr;
    }
#endif
    propNameId_ = from.propNameId_;
    pseudoClassType_ = from.pseudoClassType_;
    switch (from.GetValueType()) {
        case STYLE_PROP_VALUE_TYPE_STRING: {
            const char *strValue = from.GetStrValue();
            if (strValue != nullptr) {
                SetStringValue(strValue);
            }
            break;
        }
        case STYLE_PROP_VALUE_TYPE_BOOL:
            SetBoolValue(from.GetBoolValue());
            break;
        case STYLE_PROP_VALUE_TYPE_NUMBER:
            SetNumValue(from.GetNumValue());
            break;
        case STYLE_PROP_VALUE_TYPE_FLOATING:
            SetFloatingValue(from.GetFloatingValue());
            break;
        case STYLE_PROP_VALUE_TYPE_PERCENT:
            SetPercentValue(from.GetPercentValue());
            break;
#if FEATURE_PATH_ANIMATOR
        case STYLE_PROP_VALUE_TYPE_OFFSET_ROTATE:
            SetOffsetRotateValue(static_cast<OffsetRotateMode>(from.styleValue_.offsetRotate.mode),
                                 from.styleValue_.offsetRotate.degree);
            break;
        case STYLE_PROP_VALUE_TYPE_PATH_POLYLINE: {
            const OHOS::PathPolyline *polyline = from.GetPathPolyline();
            if (polyline != nullptr) {
                SetPathPolylineValue(*polyline);
            }
            break;
        }
#endif
        default:
            break;
    }
}
} // namespace ACELite
} // namespace OHOS
