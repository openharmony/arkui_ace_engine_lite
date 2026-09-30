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

#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)

#include <cstdint>
#include "component.h"
#include "key_parser.h"
#include "flex_layout_utils.h"
#include "layout/layout.h"

namespace OHOS {
namespace ACELite {

bool Component::IsFlexLayoutAttr(uint16_t attrKeyId) const
{
    return (attrKeyId == K_ALIGN_SELF || attrKeyId == K_FLEX_GROW || attrKeyId == K_FLEX_SHRINK ||
            attrKeyId == K_FLEX_BASIS || attrKeyId == K_MIN_WIDTH || attrKeyId == K_MAX_WIDTH ||
            attrKeyId == K_MIN_HEIGHT || attrKeyId == K_MAX_HEIGHT || attrKeyId == K_ASPECT_RATIO ||
            attrKeyId == K_RIGHT || attrKeyId == K_BOTTOM || attrKeyId == K_POSITION);
}

bool Component::ApplyFlexPositionStyle(UIView &view, const AppStyleItem *style, uint16_t styleNameId)
{
    switch (styleNameId) {
        case K_RIGHT: {
            GetDimensionFromStyle(right_, *style);
            if (right_.type == DimensionType::TYPE_PIXEL) {
                view.SetFlexRight(right_.value.pixel);
            } else if (right_.type == DimensionType::TYPE_PERCENT) {
                view.SetFlexRightPercent(right_.value.percentage);
            } else {
                view.ClearFlexRight();
            }
            return true;
        }
        case K_BOTTOM: {
            GetDimensionFromStyle(bottom_, *style);
            if (bottom_.type == DimensionType::TYPE_PIXEL) {
                view.SetFlexBottom(bottom_.value.pixel);
            } else if (bottom_.type == DimensionType::TYPE_PERCENT) {
                view.SetFlexBottomPercent(bottom_.value.percentage);
            } else {
                view.ClearFlexBottom();
            }
            return true;
        }
        case K_POSITION: {
            const char *strValue = GetStyleStrValue(style);
            if (strValue == nullptr) {
                return false;
            }
            uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
            view.SetPositionType((valueId == K_ABSOLUTE) ? POSITION_ABSOLUTE : POSITION_STATIC);
            return true;
        }
        default:
            return false;
    }
}

bool Component::SetAlignSelfStyle(UIView &view, const AppStyleItem *style) const
{
    const char *strValue = GetStyleStrValue(style);
    if (strValue == nullptr) {
        return false;
    }
    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
    switch (valueId) {
        case K_FLEX_START:
            view.SetAlignSelf(OHOS::ALIGN_START);
            break;
        case K_FLEX_END:
            view.SetAlignSelf(OHOS::ALIGN_END);
            break;
        case K_CENTER:
            view.SetAlignSelf(OHOS::ALIGN_CENTER);
            break;
        case K_STRETCH:
            // ALIGN_SELF_STRETCH is a UIView-specific marker (0xFE) for explicit align-self:stretch,
            // distinct from OHOS::ALIGN_STRETCH (6) used for container align-items:stretch.
            view.SetAlignSelf(UIView::ALIGN_SELF_STRETCH);
            break;
        default:
            return false;
    }
    return true;
}

bool Component::SetFlexFactorStyle(UIView &view, const AppStyleItem *style, uint16_t styleNameId) const
{
    int32_t factor = (style->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) ? style->GetNumValue() : -1;
    if (factor < 0 || factor > UINT16_MAX) {
        return false;
    }
    if (styleNameId == K_FLEX_GROW) {
        view.SetFlexGrow(static_cast<uint16_t>(factor));
    } else {
        view.SetFlexShrink(static_cast<uint16_t>(factor));
    }
    return true;
}

bool Component::SetFlexBasisStyle(UIView &view, const AppStyleItem *style) const
{
    int32_t basis = GetStylePixelValue(style, -1);
    if (basis < 0 || basis > INT16_MAX) {
        return false;
    }
    view.SetFlexBasis(static_cast<int16_t>(basis));
    return true;
}

bool Component::SetMinMaxDimensionStyle(UIView &view, const AppStyleItem *style, uint16_t styleNameId) const
{
    int32_t constraint = GetStylePixelValue(style, -1);
    if (constraint < 0 || constraint > INT16_MAX) {
        return false;
    }
    if (styleNameId == K_MIN_WIDTH) {
        view.SetMinWidth(static_cast<int16_t>(constraint));
    } else if (styleNameId == K_MAX_WIDTH) {
        view.SetMaxWidth(static_cast<int16_t>(constraint));
    } else if (styleNameId == K_MIN_HEIGHT) {
        view.SetMinHeight(static_cast<int16_t>(constraint));
    } else {
        view.SetMaxHeight(static_cast<int16_t>(constraint));
    }
    return true;
}

bool Component::SetAspectRatioStyle(UIView &view, const AppStyleItem *style) const
{
    uint16_t ratio = 0;
    switch (style->GetValueType()) {
        case STYLE_PROP_VALUE_TYPE_NUMBER:
            if (!FlexLayoutUtils::ConvertAspectRatioValue(style->GetNumValue(), ratio)) {
                return false;
            }
            break;
        case STYLE_PROP_VALUE_TYPE_FLOATING:
            if (!FlexLayoutUtils::ConvertAspectRatioValue(style->GetFloatingValue(), ratio)) {
                return false;
            }
            break;
        case STYLE_PROP_VALUE_TYPE_PERCENT:
            if (!FlexLayoutUtils::ConvertAspectRatioValue(style->GetPercentValue() /
                FlexLayoutUtils::ASPECT_RATIO_PERCENT_BASE, ratio)) {
                return false;
            }
            break;
        case STYLE_PROP_VALUE_TYPE_STRING:
            if (!FlexLayoutUtils::ParseAspectRatioString(style->GetStrValue(), ratio)) {
                return false;
            }
            break;
        default:
            return false;
    }
    if (ratio == 0) {
        return false;
    }
    view.SetAspectRatio(ratio);
    return true;
}

} // namespace ACELite
} // namespace OHOS

#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
