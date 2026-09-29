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
 * @file app_style_gradient_parser.h
 * @brief Full featured parser for the CSS linear-gradient() function.
 *
 * Adaptation notes:
 *   - Parsed colors are emitted as the ColorType union declared in style.h.
 *   - Color stops are emitted as GradientColorStop.
 *   - The final product is a GradientInfo payload, see gradient_info.h.
 *
 * Supported syntax:
 *   linear-gradient(<direction> | <angle>, <color-stop>, <color-stop>, ...)
 *
 *   <direction>  : to top | to bottom | to left | to right |
 *                  to top left | to top right | to bottom left |
 *                  to bottom right (both word orders accepted)
 *   <angle>      : <number>deg | <number>rad | <number>turn | <number>grad
 *   <color-stop> : <color> [<percentage>]
 *   <color>      : named color | #RGB | #RRGGBB | #RRGGBBAA |
 *                  rgb(r, g, b) | rgba(r, g, b, a)
 *
 * Angles are stored exactly as authored (negative and larger than 360 values
 * are preserved). Wrapping into [0, 360) is the responsibility of the drawing
 * adapter through GradientInfo::ResolveCssAngle().
 */

#ifndef ACE_CSS_GRADIENT_PARSER_H
#define ACE_CSS_GRADIENT_PARSER_H

#include <string>
#include "gfx_utils/gradient_info.h"

namespace OHOS {
namespace ACELite {

namespace CssGradientParser {

/**
 * @brief Lightweight fixed-size list of gradient color stops.
 *
 * Replaces std::vector<GradientColorStop> for mini/embedded targets that want
 * to avoid the STL vector header. Capacity equals LINEAR_GRADIENT_MAX_COLORS.
 */
struct GradientStopList {
    static constexpr uint8_t maxStops = LINEAR_GRADIENT_MAX_COLORS;
    GradientColorStop stops[maxStops];
    uint8_t count = 0;

    bool PushBack(const GradientColorStop& stop);
    bool Empty() const;
    uint8_t Size() const;
    GradientColorStop& Front();
    const GradientColorStop& Front() const;
    GradientColorStop& Back();
    const GradientColorStop& Back() const;
    GradientColorStop& operator[](uint8_t index);
    const GradientColorStop& operator[](uint8_t index) const;
    void Clear();
};

/**
 * @brief Lightweight fixed-size list of gradient argument strings.
 *
 * Replaces std::vector<std::string> for mini/embedded targets. 32 entries are
 * enough for one direction/angle plus up to LINEAR_GRADIENT_MAX_COLORS stops.
 */
struct GradientArgList {
    static constexpr uint8_t maxArguments = 32;
    std::string args[maxArguments];
    uint8_t count = 0;

    bool PushBack(const std::string& arg)
    {
        if (count >= maxArguments) {
            return false;
        }
        args[count++] = arg;
        return true;
    }
    uint8_t Size() const { return count; }
    std::string& operator[](uint8_t index) { return args[index]; }
    const std::string& operator[](uint8_t index) const { return args[index]; }
    void Clear() { count = 0; }
};

/**
 * @brief Parse a complete linear-gradient() declaration.
 *
 * @param value   full CSS value; must start with any casing variant of the
 *                "linear-gradient(" prefix (linear-gradient / LINEAR-GRADIENT /
 *                Linear-Gradient / ...) and end with a closing parenthesis.
 * @param outInfo destination payload, must not be nullptr. On success it holds
 *                the direction/angle plus at least two normalized color stops.
 * @return true when at least two valid color stops were produced, false when
 *         the input is malformed or the payload could not be allocated.
 *
 * Error handling: a single unparsable color stop is skipped and reported at
 * warning level, the remaining stops are still used. The call only fails when
 * fewer than GRADIENT_MIN_COLOR_STOP_COUNT stops survive, or when the first
 * argument is neither a valid direction/angle nor a real color stop (for
 * example an unknown keyword such as "to nowhere" or a meaningless token such
 * as "abcdef") in which case it is treated as an illegal direction value and
 * the gradient is silently disabled.
 */
bool ParseLinearGradient(const std::string& value, GradientInfo* outInfo);

/**
 * @brief Detect a CSS gradient function token whose name is split by whitespace,
 *        e.g. "linear-  gradient(45deg, red, blue)".
 *
 * Per the CSS tokenizer a function-token name must be a single contiguous
 * identifier; an embedded space ("linear-" <ws> "gradient") breaks tokenization,
 * so the whole declaration is invalid and must be dropped (no gradient, no
 * accidental background-image fallback). This is a lexer-stage check performed
 * before any argument parsing.
 *
 * @param value the raw CSS property value (case insensitive).
 * @return true when @p value contains a whitespace-split "linear-gradient("
 *         function token; false otherwise.
 */
bool IsBrokenGradientFunctionToken(const std::string& value);

/**
 * @brief Parse the first argument of linear-gradient() as a direction or angle.
 *
 * @param param the raw first argument, case insensitive.
 * @param info  destination payload, must not be nullptr; only `direction` and
 *              `angle` are written.
 * @return true when @p param is a direction keyword or a valid angle, false
 *         when it should instead be treated as the first color stop.
 */
bool ParseDirectionOrAngle(const std::string& param, GradientInfo* info);

/**
 * @brief Convert a direction keyword into its CSS angle.
 *
 * Thin forwarder to OHOS::CssGradientDirectionToAngle(), which is the single
 * shared implementation used by every repository. Kept as part of the parser
 * API for source compatibility.
 *
 * @param dir direction keyword.
 * @return CSS angle in degrees (0 points up, growing clockwise).
 */
float DirectionToCssAngle(CssGradientDirection dir);

/**
 * @brief Rewrite the leading direction keyword of a gradient into an angle.
 *
 * Turns "linear-gradient(to right, red, blue)" into
 * "linear-gradient(90deg, red, blue)". Kept as a compatibility shim for parser
 * builds that only understand the angle syntax, and reused by the component
 * layer so that the keyword table lives in exactly one place.
 *
 * @param cssValue   original declaration.
 * @param normalized rewritten declaration, only written when true is returned.
 * @return true when a direction keyword was found and replaced, false when the
 *         declaration has no keyword to rewrite (the caller should keep using
 *         @p cssValue unchanged).
 */
bool NormalizeDirectionKeywordToAngle(const std::string& cssValue, std::string& normalized);

/**
 * @brief Extract the first complete linear-gradient() function of a declaration.
 *
 * ParseLinearGradient() requires its input to start with the function prefix,
 * which holds for "background-image" but not for the other gradient capable
 * properties: "border-image" appends the slice/width/outset components
 * ("linear-gradient(red, blue) 30 / 10px"), "cursor" appends the hotspot and a
 * mandatory fallback keyword ("linear-gradient(red, blue), auto"), and any of
 * them may be authored with leading spaces or an uppercase function name.
 *
 * The scan is parenthesis aware, so the nested commas and parentheses of
 * rgba() color stops never terminate the extraction early. The lookup itself is
 * case insensitive while the returned text keeps the original casing.
 *
 * @param declaration full CSS declaration value.
 * @param outGradient receives the "linear-gradient(...)" substring, closing
 *                    parenthesis included; only written when true is returned.
 * @return true when a balanced linear-gradient() function was found, false when
 *         the declaration carries no gradient or its parentheses are unbalanced.
 */
bool ExtractLinearGradient(const std::string& declaration, std::string& outGradient);

/**
 * @brief Parse one color stop such as "red" or "#ff0000 50%".
 *
 * @param stopStr the trimmed color stop text.
 * @param out     destination stop. `offset` is set to
 *                GRADIENT_OFFSET_UNSPECIFIED when no percentage was authored,
 *                otherwise it holds the authored ratio verbatim, including the
 *                out of range values CSS allows ("-20%" -> -0.2, "150%" -> 1.5).
 *                Only absurd magnitudes beyond GRADIENT_OFFSET_EXTENT_LIMIT are
 *                clamped. Projecting the ramp back onto [0, 1] is the job of
 *                ClipColorStopsToGradientLine().
 * @return true when the color part could be parsed.
 */
bool ParseColorStop(const std::string& stopStr, GradientColorStop& out);

/**
 * @brief Resolve every unspecified offset and enforce a monotonic sequence.
 *
 * The first stop defaults to 0.0 and the last one to 1.0; runs of unspecified
 * offsets in between are distributed evenly across the enclosing gap. Any stop
 * that would move backwards is raised to its predecessor's offset.
 *
 * Out of range positions are preserved: an omitted position is recognized with
 * IsGradientOffsetUnspecified(), never with a sign test, so a legal "-20%" stop
 * survives normalization untouched.
 *
 * @param stops stops to normalize in place.
 */
void NormalizeColorStops(GradientStopList& stops);

/**
 * @brief Project a ramp whose stops exceed [0, 1] back onto the painted segment.
 *
 * W3C CSS Images Module Level 3, 3.4.1: color stop positions may be negative or
 * larger than 100%. Those stops live on the gradient line outside the box, and
 * the box shows only the [0%, 100%] segment of the resulting ramp. Since the
 * rasterizer (FillGradientLut) hard clamps every offset into [0, 1], feeding it
 * an out of range ramp would collapse the outer stops onto the endpoints and
 * paint the wrong colors. This function therefore samples the colors the
 * standard requires at 0 and at 1, keeps the stops strictly inside the segment
 * and drops the rest, which yields a ramp that is pixel identical to the
 * browser output while remaining representable by the drawing layer.
 *
 * Sampling follows the standard: padding outside the authored range,
 * premultiplied sRGBA interpolation inside it, right hand color on a hard stop.
 *
 * Expects a list already processed by NormalizeColorStops() (no sentinel, non
 * decreasing offsets). A list that is already inside [0, 1] is left untouched.
 *
 * @param stops stops to project in place.
 */
void ClipColorStopsToGradientLine(GradientStopList& stops);

/**
 * @brief Split the arguments of a gradient function on top level commas.
 *
 * Commas nested inside a function such as rgba(255, 0, 0, 0.5) are preserved
 * because the splitter tracks the parenthesis depth.
 *
 * @param inner text located between the outer parentheses.
 * @return the trimmed, non empty arguments in source order.
 */
GradientArgList SplitGradientArgs(const std::string& inner);

/** @brief Remove leading and trailing spaces, tabs and line breaks. */
std::string Trim(const std::string& str);

/** @brief ASCII lowercase conversion (locale independent). */
std::string ToLower(const std::string& str);

/** @brief Collapse any run of ASCII whitespace to a single space (locale independent). */
std::string CollapseWhitespace(const std::string& str);

/**
 * @brief Parse a CSS color literal into a ColorType union.
 *
 * @param colorStr color text, for example "#ff0000", "red" or "rgba(255,0,0,1)".
 * @param out      destination union; set to fully transparent on failure.
 * @return true when the literal is a supported and well formed color.
 */
bool ParseColorToColorType(const std::string& colorStr, ColorType& out);

} // namespace CssGradientParser
} // namespace ACELite
} // namespace OHOS

#endif // ACE_CSS_GRADIENT_PARSER_H
