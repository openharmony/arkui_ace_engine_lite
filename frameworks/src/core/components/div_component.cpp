/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "div_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
#include "flex_layout_utils.h"
#include "number_parser.h"
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

namespace OHOS {
namespace ACELite {
DivComponent::DivComponent(jerry_value_t options,
                           jerry_value_t children,
                           AppStyleManager* styleManager)
    : Component(options, children, styleManager),
      isSecondaryAxisAlignSet_(false), isVerticalLayout_(false)
{
    SetComponentName(K_DIV);
    nativeView_.SetStyle(STYLE_BACKGROUND_OPA, 0);
}


inline UIView *DivComponent::GetComponentRootView() const
{
    return const_cast<FlexLayout *>(&nativeView_);
}

bool DivComponent::ApplyPrivateStyle(const AppStyleItem* style)
{
    // Set default value
    if (!isSecondaryAxisAlignSet_) {
        nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_START);
    }
    uint16_t stylePropNameId = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(stylePropNameId)) {
        return false;
    }
    const char * const strValue = GetStyleStrValue(style);
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    if (ApplyEnhancedPrivateStyle(style, stylePropNameId, strValue)) {
        return true;
    }
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

    if (strValue == nullptr) {
        return false;
    }

    bool applyResult = true;
    NativeViewSetDirection(style, stylePropNameId, strValue, applyResult);
    return applyResult;
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
bool DivComponent::ApplyEnhancedPrivateStyle(const AppStyleItem* style, uint16_t stylePropNameId,
                                             const char * const strValue)
{
    switch (stylePropNameId) {
        case K_GAP:
        case K_COLUMN_GAP:
        case K_ROW_GAP:
            return NativeViewSetGapStyle(style, stylePropNameId, strValue);
        case K_ALIGN_ITEMS: {
            if (strValue == nullptr) {
                return false;
            }
            uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
            if (valueId != K_STRETCH) {
                return false;
            }
            nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_STRETCH);
            isSecondaryAxisAlignSet_ = true;
            return true;
        }
        case K_ALIGN_CONTENT:
        case K_OVERFLOW: {
            if (strValue == nullptr) {
                return false;
            }
            bool applyResult = true;
            NativeViewSetEnhancedDirection(style, stylePropNameId, strValue, applyResult);
            return applyResult;
        }
        default:
            return false;
    }
}

void DivComponent::NativeViewSetEnhancedDirection(const AppStyleItem* style, uint16_t stylePropNameId,
                                                  const char * const strValue, bool& applyResult)
{
    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
    switch (stylePropNameId) {
        case K_ALIGN_CONTENT:
            NativeViewSetAlignContent(valueId, applyResult);
            break;
        case K_OVERFLOW:
            NativeViewSetOverflow(valueId);
            break;
        default:
            applyResult = false;
            break;
    }
}

void DivComponent::ApplyGapToken(const char *token, uint16_t tokenLen, bool isColumn)
{
    if ((token == nullptr) || (tokenLen == 0)) {
        return;
    }
    float percentValue = 0;
    if (NumberParser::ParsePercentValue(token, tokenLen, percentValue)) {
        // percent gaps are resolved by FlexLayout on the next layout pass
        if (isColumn) {
            nativeView_.SetColumnGapPercent(percentValue);
        } else {
            nativeView_.SetRowGapPercent(percentValue);
        }
        return;
    }
    int16_t pixelValue = FlexLayoutUtils::ParseGapPixelLength(token, tokenLen);
    if (isColumn) {
        nativeView_.SetColumnGap(pixelValue);
    } else {
        nativeView_.SetRowGap(pixelValue);
    }
}

void DivComponent::ApplyGapShorthand(const char * const strValue)
{
    // gap: <row-gap> [<column-gap>]?; tokens are split on spaces first so each
    // value is parsed independently and a percent first token keeps its '%'
    const char *rowToken = strValue;
    while (*rowToken == ' ') {
        rowToken++;
    }
    const char *ptr = rowToken;
    while ((*ptr != '\0') && (*ptr != ' ')) {
        ptr++;
    }
    uint16_t rowTokenLen = ptr - rowToken;
    ApplyGapToken(rowToken, rowTokenLen, false);
    while (*ptr == ' ') {
        ptr++;
    }
    if (*ptr == '\0') {
        // a single value applies to both gaps with the same type
        ApplyGapToken(rowToken, rowTokenLen, true);
        return;
    }
    const char *columnToken = ptr;
    while ((*ptr != '\0') && (*ptr != ' ')) {
        ptr++;
    }
    ApplyGapToken(columnToken, ptr - columnToken, true);
}

void DivComponent::ApplyGapValue(const AppStyleItem* style, bool isColumn)
{
    if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_PERCENT) {
        if (isColumn) {
            nativeView_.SetColumnGapPercent(style->GetPercentValue());
        } else {
            nativeView_.SetRowGapPercent(style->GetPercentValue());
        }
        return;
    }
    int32_t gapValue = GetStylePixelValue(style, 0);
    int16_t clampedGap = FlexLayoutUtils::ClampGapValue(gapValue);
    if (isColumn) {
        nativeView_.SetColumnGap(clampedGap);
    } else {
        nativeView_.SetRowGap(clampedGap);
    }
}

bool DivComponent::NativeViewSetGapStyle(const AppStyleItem* style, uint16_t stylePropNameId,
                                         const char * const strValue)
{
    // gap styles support number value as well, so handle them before checking string value
    switch (stylePropNameId) {
        case K_GAP: {
            if (style->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
                int32_t gapValue = style->GetNumValue();
                int16_t clampedGap = FlexLayoutUtils::ClampGapValue(gapValue);
                nativeView_.SetRowGap(clampedGap);
                nativeView_.SetColumnGap(clampedGap);
                return true;
            }
            if (strValue != nullptr) {
                ApplyGapShorthand(strValue);
            }
            return true;
        }
        case K_COLUMN_GAP:
            ApplyGapValue(style, true);
            return true;
        case K_ROW_GAP:
            ApplyGapValue(style, false);
            return true;
        default:
            return false;
    }
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

void DivComponent::NativeViewSetDirection(const AppStyleItem* style, uint16_t stylePropNameId,
                                          const char * const strValue, bool& applyResult)
{
    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
    switch (stylePropNameId) {
        case K_FLEX_DIRECTION: {
            NativeViewSetLayoutDirection(valueId, applyResult);
            break;
        }
        case K_JUSTIFY_CONTENT: {
            NativeViewSetMajorAxisAlign(valueId, applyResult);
            break;
        }
        case K_ALIGN_ITEMS: {
            switch (valueId) {
                case K_FLEX_START:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_START);
                    break;
                case K_FLEX_END:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_END);
                    break;
                case K_CENTER:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_CENTER);
                    break;
                default:
                    applyResult = false;
                    break;
            }
            if (applyResult) {
                isSecondaryAxisAlignSet_ = true;
            }
            break;
        }
        case K_FLEX_WRAP: {
            if (valueId == K_WRAP) {
                nativeView_.SetFlexWrap(FlexLayout::WRAP);
            } else {
                nativeView_.SetFlexWrap(FlexLayout::NOWRAP);
            }
            break;
        }
        default:
            applyResult = false;
            break;
    }
}

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
void DivComponent::NativeViewSetOverflow(uint16_t valueId)
{
    if (valueId == K_HIDDEN) {
        nativeView_.SetOverflow(OHOS::OVERFLOW_HIDDEN);
    } else {
        nativeView_.SetOverflow(OHOS::OVERFLOW_VISIBLE);
    }
}

void DivComponent::NativeViewSetAlignContent(uint16_t valueId, bool& applyResult)
{
    switch (valueId) {
        case K_FLEX_START:
            nativeView_.SetAlignContent(OHOS::ALIGN_CONTENT_START);
            break;
        case K_CENTER:
            nativeView_.SetAlignContent(OHOS::ALIGN_CONTENT_CENTER);
            break;
        case K_FLEX_END:
            nativeView_.SetAlignContent(OHOS::ALIGN_CONTENT_END);
            break;
        case K_SPACE_BETWEEN:
            nativeView_.SetAlignContent(OHOS::ALIGN_CONTENT_BETWEEN);
            break;
        case K_STRETCH:
            nativeView_.SetAlignContent(OHOS::ALIGN_CONTENT_STRETCH);
            break;
        default:
            applyResult = false;
            break;
    }
}
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

void DivComponent::NativeViewSetLayoutDirection(uint16_t valueId, bool& applyResult)
{
    switch (valueId) {
        case K_COLUMN:
            nativeView_.SetLayoutDirection(LAYOUT_VER);
            isVerticalLayout_ = true;
            break;
        case K_ROW:
            nativeView_.SetLayoutDirection(LAYOUT_HOR);
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
            isVerticalLayout_ = false;
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
            break;
        case K_ROW_REVERSE:
            nativeView_.SetLayoutDirection(LAYOUT_HOR_R);
            break;
        case K_COLUMN_REVERSE:
            nativeView_.SetLayoutDirection(LAYOUT_VER_R);
            break;
        default:
            applyResult = false;
            break;
    }
}

void DivComponent::NativeViewSetMajorAxisAlign(uint16_t valueId, bool& applyResult)
{
    switch (valueId) {
        case K_FLEX_START:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_START);
            break;
        case K_FLEX_END:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_END);
            break;
        case K_CENTER:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_CENTER);
            break;
        case K_SPACE_BETWEEN:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_BETWEEN);
            break;
        case K_SPACE_AROUND:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_AROUND);
            break;
        case K_SPACE_EVENLY:
            nativeView_.SetMajorAxisAlign(OHOS::ALIGN_EVENLY);
            break;
        default:
            applyResult = false;
            break;
    }
}

bool DivComponent::ProcessChildren()
{
    // add all children to this container
    AppendChildren(this);

    return true;
}

void DivComponent::PostUpdate(uint16_t attrKeyId)
{
    nativeView_.LayoutChildren();
}

void DivComponent::AttachView(const Component *child)
{
    if (child == nullptr) {
        return;
    }
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    if (isSecondaryAxisAlignSet_) {
        nativeView_.Add(child->GetComponentRootView());
        return;
    }
    if (FlexLayoutUtils::HasAspectRatioMainSize(*child, isVerticalLayout_)) {
        nativeView_.Add(child->GetComponentRootView());
        return;
    }
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
    if (!isSecondaryAxisAlignSet_) {
        ConstrainedParameter param;
        child->GetConstrainedParam(param);
        DimensionType type;
        Component *component = const_cast<Component *>(child);
        if (isVerticalLayout_) {
            type = child->GetDimension(K_WIDTH).type;
            if (type == DimensionType::TYPE_UNKNOWN) {
                const int16_t width = this->GetWidth();
                component->SetWidth(width);
                param.maxWidth = width;
                component->AlignDimensions(param);
                child->AdaptBoxSizing();
            }
        } else {
            type = child->GetDimension(K_HEIGHT).type;
            if (type == DimensionType::TYPE_UNKNOWN) {
                const int16_t height = this->GetHeight();
                component->SetHeight(height);
                param.maxHeight = height;
                component->AlignDimensions(param);
                child->AdaptBoxSizing();
            }
        }
    }
    nativeView_.Add(child->GetComponentRootView());
}

void DivComponent::LayoutChildren()
{
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    // gap percents resolve against the container content-box; a dimension that is not
    // set (auto, content-driven) makes the percent gap on that axis degrade to 0px
    bool widthAuto = (GetDimension(K_WIDTH).type != DimensionType::TYPE_PIXEL);
    bool heightAuto = (GetDimension(K_HEIGHT).type != DimensionType::TYPE_PIXEL);
    nativeView_.SetGapBaseAuto(widthAuto, heightAuto);
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
    nativeView_.LayoutChildren();
}
} // namespace ACELite
} // namespace OHOS
