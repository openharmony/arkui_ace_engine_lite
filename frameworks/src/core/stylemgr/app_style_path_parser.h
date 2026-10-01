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

/**
 * @file app_style_path_parser.h
 * @brief Parser for the CSS Motion Path fields (offset-path / offset-rotate).
 *
 * Parsing happens once at AppStyleItem creation time: the raw CSS string is
 * turned into a structured value (a sampled PathPolyline for offset-path, an
 * OffsetRotateValue for offset-rotate) which is stored inside the style item,
 * so the component layer only consumes the parsed result and never touches
 * the raw string.
 */

#ifndef OHOS_ACELITE_APP_STYLE_PATH_PARSER_H
#define OHOS_ACELITE_APP_STYLE_PATH_PARSER_H

#include <cstdint>

#if FEATURE_PATH_ANIMATOR
#include "offset_path_parser.h"

namespace OHOS {
namespace ACELite {

/* offset-rotate modes (W3C CSS Motion Path):
   FIXED: constant angle; AUTO: follow the path tangent direction plus an offset angle */
enum OffsetRotateMode : uint8_t { OFFSET_ROTATE_FIXED = 0, OFFSET_ROTATE_AUTO = 1 };

/* parsed result of the offset-rotate CSS field: POD so it can live in the StyleValue union */
struct OffsetRotateValue {
    uint8_t mode;
    int16_t degree;
};

namespace AppStylePathParser {

/**
 * @brief Parse the offset-path value (path("...")) into a sampled polyline.
 *
 * @param pathStr the raw CSS value, must not be nullptr.
 * @param outPoly output polyline (defined in ui_lite offset_path_parser.h, namespace OHOS;
 *                fully qualified to avoid silent shadowing if a same-name type appears in ACELite);
 *                only written when true is returned.
 * @return true when the path is valid and outPoly.count >= 2, false otherwise.
 */
bool ParseOffsetPath(const char *pathStr, OHOS::PathPolyline &outPoly);

/**
 * @brief Parse the offset-rotate string value.
 *
 * Supported forms (W3C CSS Motion Path): global keywords (inherit / initial /
 * revert / revert-layer / unset), "reverse", "auto" with an optional additive
 * angle, and a fixed angle with unit suffix (deg/turn/rad/grad, unitless means
 * deg). Invalid values fall back to OFFSET_ROTATE_FIXED + 0deg.
 *
 * @param strValue the raw CSS value.
 * @param outMode output rotate mode, only written when true is returned.
 * @param outDeg output angle in degrees (normalized to (-180, 180]), only
 *               written when true is returned.
 * @return false only when strValue is nullptr.
 */
bool ParseOffsetRotate(const char *strValue, OffsetRotateMode &outMode, int16_t &outDeg);

} // namespace AppStylePathParser
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_PATH_ANIMATOR

#endif // OHOS_ACELITE_APP_STYLE_PATH_PARSER_H
