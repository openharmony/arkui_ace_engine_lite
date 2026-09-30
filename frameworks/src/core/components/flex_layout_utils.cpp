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

#include "flex_layout_utils.h"

#include <cstdlib>

#include "js_fwk_common.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
namespace FlexLayoutUtils {

namespace {
constexpr double ASPECT_RATIO_ROUNDING_OFFSET = 0.5;
} // namespace

bool ConvertAspectRatioValue(double ratio, uint16_t &scaledRatio)
{
    if (!(ratio > 0)) {
        return false;
    }
    double scaled = ratio * ASPECT_RATIO_PERCENT_BASE;
    if (!(scaled <= UINT16_MAX)) {
        return false;
    }
    scaledRatio = static_cast<uint16_t>(scaled + ASPECT_RATIO_ROUNDING_OFFSET);
    return scaledRatio != 0;
}

bool ParseAspectRatioString(const char *value, uint16_t &scaledRatio)
{
    if (value == nullptr) {
        return false;
    }
    char *end = nullptr;
    double width = strtod(value, &end);
    if (end == value || width <= 0) {
        return false;
    }
    if (*end == '/') {
        const char *heightValue = end + 1;
        char *heightEnd = nullptr;
        double height = strtod(heightValue, &heightEnd);
        if (heightEnd == heightValue) {
            return false;
        }
        if (height == 0.0 || height < 0.0) {
            return false;
        }
        return ConvertAspectRatioValue(width / height, scaledRatio);
    }
    return ConvertAspectRatioValue(width, scaledRatio);
}

bool HasAspectRatioMainSize(const Component& child, bool isVerticalLayout)
{
    const UIView* view = child.GetComponentRootView();
    if (view == nullptr || view->GetAspectRatio() == 0) {
        return false;
    }
    return child.GetDimension(isVerticalLayout ? K_HEIGHT : K_WIDTH).type != DimensionType::TYPE_UNKNOWN;
}

int16_t ClampGapValue(int32_t value)
{
    if (value < 0) {
        return 0;
    }
    if (value > INT16_MAX) {
        return INT16_MAX;
    }
    return static_cast<int16_t>(value);
}

int16_t ParseGapPixelLength(const char *token, uint16_t tokenLen)
{
    if ((token == nullptr) || (tokenLen == 0)) {
        return 0;
    }
    uint16_t index = 0;
    bool isNegative = false;
    if ((token[index] == '-') || (token[index] == '+')) {
        isNegative = (token[index] == '-');
        index++;
    }
    int32_t pixelValue = 0;
    for (; index < tokenLen; index++) {
        if ((token[index] < '0') || (token[index] > '9')) {
            break;
        }
        pixelValue = pixelValue * DEC + (token[index] - '0');
        if (pixelValue > INT16_MAX) {
            pixelValue = INT16_MAX;
            break;
        }
    }
    if (isNegative) {
        pixelValue = -pixelValue;
    }
    return ClampGapValue(pixelValue);
}

} // namespace FlexLayoutUtils
} // namespace ACELite
} // namespace OHOS
