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

#ifndef OHOS_ACELITE_SVG_COMPONENT_UTILS_H
#define OHOS_ACELITE_SVG_COMPONENT_UTILS_H

#include <cstdint>
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {

class AppStyleItem;

namespace SvgComponentUtils {

/**
 * @brief Convert lowercase SVG multi-word tag names (e.g. "lineargradient") to
 *        the canonical camelCase form used by the key parser (e.g. "linearGradient").
 *
 * @param name The raw tag name from JS.
 * @return The canonical tag name, or nullptr if no mapping is known.
 */
const char *CanonicalSvgTagName(const char *name);

/**
 * @brief Distinguish SVG <text> from UI <text>.
 *
 * SVG text carries geometry/paint attributes such as x/y/dx/dy/fill/stroke.
 * UI <text> only carries value/type.
 *
 * @param componentNameId The parsed component name key id.
 * @param options The JS options object.
 * @return true if this is an SVG text element.
 */
bool IsSvgTextElement(uint16_t componentNameId, jerry_value_t options);

/**
 * @brief Distinguish SVG <image> from UI <image>.
 *
 * SVG image carries href/xlink:href and SVG geometry attributes (x/y/transform).
 *
 * @param componentNameId The parsed component name key id.
 * @param options The JS options object.
 * @return true if this is an SVG image element.
 */
bool IsSvgImageElement(uint16_t componentNameId, jerry_value_t options);

/**
 * @brief Check whether the given SVG attribute name represents a color value.
 *
 * @param name The canonical attribute name.
 * @return true if the attribute value should be treated as a color.
 */
bool IsColorAttr(const char *name);

/**
 * @brief Convert a camelCase JS attribute name to canonical SVG kebab-case.
 *
 * Special names such as "viewBox" and "gradientTransform" preserve their
 * SVG-required spelling.
 *
 * @param jsName The raw JS attribute name.
 * @return The canonical SVG attribute name, or nullptr on error.
 */
char *CanonicalName(const char *jsName);

/**
 * @brief Format a jerry_value_t as a string value for SvgEngine::SetAttribute.
 *
 * @param v The jerry value.
 * @param isColor Whether the value should be formatted as a color.
 * @return The formatted string, or nullptr if the value type is unsupported.
 */
char *ValueFromJerry(jerry_value_t v, bool isColor);

/**
 * @brief Evaluate a jerry_value_t if it is a function, then format the result as a string
 *        value for SvgEngine::SetAttribute. Non-function values are formatted directly.
 *
 * @param v The jerry value or function.
 * @param isColor Whether the value should be formatted as a color.
 * @param viewModel The view model used as the function's this/scope.
 * @return The formatted string, or nullptr on error.
 */
char *ResolveSvgValue(jerry_value_t v, bool isColor, jerry_value_t viewModel);

/**
 * @brief Format an AppStyleItem as a string value for SvgEngine::SetAttribute.
 *
 * @param s The style item.
 * @param isColor Whether the value should be formatted as a color.
 * @return The formatted string, or nullptr if the item type is unsupported.
 */
char *ValueFromStyle(const AppStyleItem *s, bool isColor);

/**
 * @brief Map a parsed component attribute key id to its canonical SVG attribute name.
 *
 * @param keyId The parsed key id.
 * @return The SVG attribute name, or nullptr if the key is not an SVG attribute.
 */
const char *KeyIdToSvgName(uint16_t keyId);

} // namespace SvgComponentUtils
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_SVG_COMPONENT_UTILS_H
