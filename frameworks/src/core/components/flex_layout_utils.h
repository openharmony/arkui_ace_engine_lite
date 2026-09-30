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

#ifndef OHOS_ACELITE_FLEX_LAYOUT_UTILS_H
#define OHOS_ACELITE_FLEX_LAYOUT_UTILS_H

#include <cstdint>

#include "components/ui_view.h"
#include "component.h"

namespace OHOS {
namespace ACELite {
namespace FlexLayoutUtils {

constexpr double ASPECT_RATIO_PERCENT_BASE = 100.0;

bool ConvertAspectRatioValue(double ratio, uint16_t &scaledRatio);
bool ParseAspectRatioString(const char *value, uint16_t &scaledRatio);
bool HasAspectRatioMainSize(const Component& child, bool isVerticalLayout);
int16_t ParseGapPixelLength(const char *token, uint16_t tokenLen);
int16_t ClampGapValue(int32_t value);

} // namespace FlexLayoutUtils
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_FLEX_LAYOUT_UTILS_H
