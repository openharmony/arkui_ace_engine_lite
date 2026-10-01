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
 * @file app_style_path_parser.cpp
 * @brief Implementation of the CSS Motion Path field parser.
 */

#include "app_style_path_parser.h"

#if FEATURE_PATH_ANIMATOR
#include <cerrno>
#include <cmath>
#include <cstring>

#include "ace_log.h"

namespace OHOS {
namespace ACELite {
namespace AppStylePathParser {
namespace {
/* W3C <angle> unit conversion to degrees */
constexpr float OFFSET_RAD_TO_DEG = 57.2957795f; // 180/pi
constexpr float OFFSET_GRAD_TO_DEG = 0.9f;       // 400grad = 360deg
constexpr float OFFSET_TURN_TO_DEG = 360.0f;     // 1turn = 360deg
constexpr int16_t OFFSET_REVERSE_DEG = 180;      // reverse === auto 180deg (W3C)

/* the unit suffix must end exactly here (NUL or space) to avoid prefix
   false-matches like "45degx"→deg or "45turnaround"→turn */
bool UnitEndsExactly(const char* after)
{
    return (after != nullptr) && ((*after == '\0') || (*after == ' '));
}

/* normalize angle to (-180, 180] range and round to int16 */
int16_t NormalizeAngleToInt16(float deg)
{
    deg = std::fmod(deg, 360.0f);
    if (deg > 180.0f) {
        deg -= 360.0f;
    } else if (deg <= -180.0f) {
        deg += 360.0f;
    }
    return static_cast<int16_t>(deg + ((deg >= 0) ? 0.5f : -0.5f));
}

/* match angle unit suffix and convert to degrees, false on unknown unit */
bool MatchAngleUnit(const char* end, float value, float& outDeg)
{
    // skip trailing spaces
    while (*end == ' ') {
        end++;
    }
    if (*end == '\0') {
        outDeg = value; // unitless means deg
        return true;
    }
    if ((strncmp(end, "turn", strlen("turn")) == 0) && UnitEndsExactly(end + strlen("turn"))) {
        outDeg = value * OFFSET_TURN_TO_DEG;
        return true;
    }
    if ((strncmp(end, "grad", strlen("grad")) == 0) && UnitEndsExactly(end + strlen("grad"))) {
        outDeg = value * OFFSET_GRAD_TO_DEG;
        return true;
    }
    if ((strncmp(end, "rad", strlen("rad")) == 0) && UnitEndsExactly(end + strlen("rad"))) {
        outDeg = value * OFFSET_RAD_TO_DEG;
        return true;
    }
    if ((strncmp(end, "deg", strlen("deg")) == 0) && UnitEndsExactly(end + strlen("deg"))) {
        outDeg = value;
        return true;
    }
    return false; // unknown or malformed unit suffix
}

/* parse a numeric angle with unit suffix (deg/turn/rad/grad, unitless means deg),
   returns degrees rounded to int16; false on invalid input (unknown unit/no number) */
bool ParseOffsetAngleDeg(const char* str, int16_t& outDeg)
{
    // early return for null or empty input
    if ((str == nullptr) || (*str == '\0')) {
        return false;
    }

    char* end = nullptr;
    errno = 0;
    float value = strtof(str, &end);

    // early return for invalid number or out of range
    bool invalidNumber = (errno == ERANGE) || (value == HUGE_VALF) || (value == -HUGE_VALF) || (end == str);
    if (invalidNumber) {
        return false;
    }

    float deg = 0.0f;
    if (!MatchAngleUnit(end, value, deg)) {
        return false;
    }

    outDeg = NormalizeAngleToInt16(deg);
    return true;
}
} // namespace

bool ParseOffsetPath(const char *pathStr, OHOS::PathPolyline &outPoly)
{
    if (pathStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "offset-path value is not string");
        return false;
    }
    bool parseOk = OHOS::OffsetPathParser::ParseToPolyline(pathStr, outPoly);
    if (!parseOk) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid offset-path value");
        return false;
    }
    return true;
}

bool ParseOffsetRotate(const char *strValue, OffsetRotateMode &outMode, int16_t &outDeg)
{
    if (strValue == nullptr) {
        return false;
    }
    /* W3C global keywords: map to the initial value (auto) */
    if (!strcmp(strValue, "inherit") || !strcmp(strValue, "initial") || !strcmp(strValue, "revert") ||
        !strcmp(strValue, "revert-layer") || !strcmp(strValue, "unset")) {
        outMode = OFFSET_ROTATE_AUTO;
        outDeg = 0;
        return true;
    }
    /* reverse === auto 180deg (W3C) */
    if (!strcmp(strValue, "reverse")) {
        outMode = OFFSET_ROTATE_AUTO;
        outDeg = OFFSET_REVERSE_DEG;
        return true;
    }
    /* auto [angle]: follow the path tangent, plus an optional additive offset angle */
    if (!strncmp(strValue, "auto", strlen("auto"))) {
        const char* rest = strValue + strlen("auto");
        while (*rest == ' ') {
            rest++;
        }
        int16_t offsetDeg = 0;
        if ((*rest != '\0') && !ParseOffsetAngleDeg(rest, offsetDeg)) {
            HILOG_WARN(HILOG_MODULE_ACE, "invalid offset-rotate angle after auto, fallback to auto");
            offsetDeg = 0;
        }
        outMode = OFFSET_ROTATE_AUTO;
        outDeg = offsetDeg;
        return true;
    }
    /* fixed angle: number with unit suffix (deg/turn/rad/grad) */
    int16_t fixedDeg = 0;
    if (ParseOffsetAngleDeg(strValue, fixedDeg)) {
        outMode = OFFSET_ROTATE_FIXED;
        outDeg = fixedDeg;
        return true;
    }
    HILOG_WARN(HILOG_MODULE_ACE, "invalid offset-rotate value, fallback to 0deg");
    outMode = OFFSET_ROTATE_FIXED;
    outDeg = 0;
    return true;
}

} // namespace AppStylePathParser
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_PATH_ANIMATOR
