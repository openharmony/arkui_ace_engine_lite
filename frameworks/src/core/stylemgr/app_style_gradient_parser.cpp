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
 * @file app_style_gradient_parser.cpp
 * @brief Implementation of the CSS linear-gradient() parser.
 *
 * Adaptation notes:
 *   - Colors are emitted as the ColorType union rather than a bespoke class.
 *   - ColorType exposes .alpha, .red, .green, .blue and .full.
 *   - Color stops are GradientColorStop values (ColorType + offset).
 *
 * Design notes:
 *   - Every literal length, channel bound and unit conversion factor lives in
 *     the anonymous namespace below so that the parsing code reads as prose.
 *   - Angle units share one table driven code path (ANGLE_UNITS) instead of the
 *     three near identical blocks the previous revision carried.
 *   - Numbers are converted with strtof rather than std::stof: it does not
 *     throw, which suits the lite runtime, and it lets us reject partially
 *     consumed or non finite input in a single place.
 */

#include "app_style_gradient_parser.h"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
namespace CssGradientParser {

bool GradientStopList::PushBack(const GradientColorStop &stop)
{
    if (count >= maxStops) {
        return false;
    }
    stops[count++] = stop;
    return true;
}

bool GradientStopList::Empty() const
{
    return count == 0;
}

uint8_t GradientStopList::Size() const
{
    return count;
}

GradientColorStop &GradientStopList::Front()
{
    if (count == 0) {
        static GradientColorStop fallback;
        return fallback;
    }
    return stops[0];
}

const GradientColorStop &GradientStopList::Front() const
{
    if (count == 0) {
        static const GradientColorStop fallback;
        return fallback;
    }
    return stops[0];
}

GradientColorStop &GradientStopList::Back()
{
    if (count == 0) {
        static GradientColorStop fallback;
        return fallback;
    }
    return stops[count - 1];
}

const GradientColorStop &GradientStopList::Back() const
{
    if (count == 0) {
        static const GradientColorStop fallback;
        return fallback;
    }
    return stops[count - 1];
}

GradientColorStop &GradientStopList::operator[](uint8_t index)
{
    if (index >= count) {
        static GradientColorStop fallback;
        return fallback;
    }
    return stops[index];
}

const GradientColorStop &GradientStopList::operator[](uint8_t index) const
{
    if (index >= count) {
        static const GradientColorStop fallback;
        return fallback;
    }
    return stops[index];
}

void GradientStopList::Clear()
{
    count = 0;
}

namespace {
/** Function prefix. Matched case-insensitively so that every casing variant of
 *  the function name (linear-gradient / LINEAR-GRADIENT / Linear-Gradient / ...)
 *  is accepted the same way the canonical lowercase form is. */
const char* const LINEAR_GRADIENT_PREFIX = "linear-gradient(";
const char* const RGB_FUNCTION_PREFIX = "rgb(";
const char* const RGBA_FUNCTION_PREFIX = "rgba(";
const char HEX_COLOR_MARKER = '#';
const char PERCENT_MARKER = '%';
const char ARGUMENT_SEPARATOR = ',';
const char PAREN_OPEN = '(';
const char PAREN_CLOSE = ')';
const char* const WHITESPACE_CHARS = " \t\n\r";
/** Base used by strtol/strtof when parsing decimal integer tokens in rgb()/rgba(). */
constexpr int DECIMAL_BASE = 10;
/** Base used by strtoul when parsing hexadecimal color literals (#RGB / #RRGGBB / #RRGGBBAA). */
constexpr int HEX_BASE = 16;
/** Accepted hexadecimal payload lengths, excluding the leading '#'. */
const size_t HEX_SHORTHAND_LENGTH = 3; /* #RGB       */
const size_t HEX_RGB_LENGTH = 6;       /* #RRGGBB    */
const size_t HEX_RGBA_LENGTH = 8;      /* #RRGGBBAA  */
/** 0xF * 17 == 0xFF, which is how #RGB expands into #RRGGBB. */
const unsigned int HEX_SHORTHAND_EXPAND_FACTOR = 17;
/**
 * @brief Bit layout used when unpacking a #RGB shorthand color.
 *
 * The 12-bit payload packs three 4-bit channels: R at bits [11:8], G at [7:4]
 * and B at [3:0]. The mask and shift constants below replace the bare literals
 * that previously appeared at the unpack site, so the field geometry reads as
 * prose and the magic numbers can no longer drift out of sync.
 */
const uint32_t HEX_SHORTHAND_BIT_WIDTH = 12;     /* #RGB is 3 hex digits = 12 bits */
const uint32_t HEX_SHORTHAND_RED_SHIFT = 8;      /* R occupies bits [11:8]          */
const uint32_t HEX_SHORTHAND_GREEN_SHIFT = 4;    /* G occupies bits [7:4]           */
const uint32_t HEX_SHORTHAND_NIBBLE_MASK = 0xFu; /* low 4 bits of a single channel  */
/** Bit layout used when unpacking a packed hexadecimal color. */
const uint32_t BYTE_MASK = 0xFFu;
const uint32_t HEX_RGB_RED_SHIFT = 16;
const uint32_t HEX_RGB_GREEN_SHIFT = 8;
const uint32_t HEX_RGBA_RED_SHIFT = 24;
const uint32_t HEX_RGBA_GREEN_SHIFT = 16;
const uint32_t HEX_RGBA_BLUE_SHIFT = 8;
/** Numeric bounds of an 8 bit color channel expressed as int / float. */
const int CHANNEL_VALUE_MIN = 0;
const int CHANNEL_VALUE_MAX = 255;
const float CHANNEL_VALUE_MIN_F = 0.0f;
const float CHANNEL_VALUE_MAX_F = 255.0f;
/** Percentages are authored in [0, 100] but stored as a [0, 1] ratio. */
const float PERCENT_TO_RATIO_DIVISOR = 100.0f;
/** Added to a channel before truncation so that the cast rounds to nearest. */
const float CHANNEL_ROUNDING_BIAS = 0.5f;
/**
 * @brief Shift applied to an authored -100% position.
 *
 * -100% resolves to -1.0f, the very value used as the "position omitted"
 * sentinel. Two epsilons keep IsGradientOffsetUnspecified() false while staying
 * three orders of magnitude below one LUT step (1/512).
 */
const float SENTINEL_DISAMBIGUATION_STEP = 2.0f * GRADIENT_OFFSET_SENTINEL_EPSILON;
/**
 * @brief Smallest span between two stops that still counts as an interpolation.
 *
 * A shorter span is a CSS hard stop: the two stops share a position and the
 * color changes abruptly, so there is nothing to interpolate.
 */
const float GRADIENT_SPAN_EPSILON = 1e-6f;
/** Below this alpha a premultiplied color carries no usable RGB information. */
const float PREMULTIPLIED_ALPHA_EPSILON = 1e-6f;
/** Room for the longest generated angle token, "315deg", plus a terminator. */
const size_t ANGLE_TEXT_BUFFER_SIZE = 16;
/** Conversion factors from an authored angle unit to degrees. */
const float DEGREES_PER_RADIAN = 57.29577951308232f; /* 180 / pi */
const float DEGREES_PER_TURN = CSS_ANGLE_FULL_TURN;
const float DEGREES_PER_DEGREE = 1.0f;
const float DEGREES_PER_GRADIAN = CSS_ANGLE_FULL_TURN / 400.0f; /* 360 / 400 == 0.9 */
/**
 * @brief One supported angle unit.
 *
 * Replaces the three copy pasted deg / rad / turn blocks of the previous
 * revision. Adding a unit (for example "grad") is a single table entry.
 * Order matters only if one suffix is a suffix of another, which is not the
 * case for the units below.
 */
struct AngleUnitSpec {
    const char* suffix;
    float degreesPerUnit;
};
const AngleUnitSpec ANGLE_UNITS[] = {
    {"deg", DEGREES_PER_DEGREE},
    {"rad", DEGREES_PER_RADIAN},
    {"turn", DEGREES_PER_TURN},
    {"grad", DEGREES_PER_GRADIAN},
};
const size_t ANGLE_UNIT_COUNT = sizeof(ANGLE_UNITS) / sizeof(ANGLE_UNITS[0]);
/**
 * @brief Direction keyword table.
 *
 * Both CSS word orders are accepted ("to top right" and "to right top") so the
 * caller never has to reorder the tokens.
 */
struct DirectionEntry {
    const char* keyword;
    CssGradientDirection direction;
};

static const DirectionEntry DIRECTION_TABLE[] = {
    {"to bottom",       CssGradientDirection::TO_BOTTOM},
    {"to bottom left",  CssGradientDirection::TO_BOTTOM_LEFT},
    {"to bottom right", CssGradientDirection::TO_BOTTOM_RIGHT},
    {"to left",         CssGradientDirection::TO_LEFT},
    {"to left bottom",  CssGradientDirection::TO_BOTTOM_LEFT},
    {"to left top",     CssGradientDirection::TO_TOP_LEFT},
    {"to right",        CssGradientDirection::TO_RIGHT},
    {"to right bottom", CssGradientDirection::TO_BOTTOM_RIGHT},
    {"to right top",    CssGradientDirection::TO_TOP_RIGHT},
    {"to top",          CssGradientDirection::TO_TOP},
    {"to top left",     CssGradientDirection::TO_TOP_LEFT},
    {"to top right",    CssGradientDirection::TO_TOP_RIGHT},
};
static const size_t DIRECTION_COUNT = sizeof(DIRECTION_TABLE) / sizeof(DIRECTION_TABLE[0]);

static bool FindDirection(const std::string& keyword, CssGradientDirection& out)
{
    for (size_t i = 0; i < DIRECTION_COUNT; ++i) {
        if (strcmp(keyword.c_str(), DIRECTION_TABLE[i].keyword) == 0) {
            out = DIRECTION_TABLE[i].direction;
            return true;
        }
    }
    return false;
}

/**
 * @brief Named CSS colors supported by the lite runtime.
 *
 * Values are packed as 0xAARRGGBB so they can be assigned to ColorType::full
 * in one store. Every entry except "transparent" is fully opaque.
 */
struct NamedColorEntry {
    const char* name;
    uint32_t value;
};

static const NamedColorEntry NAMED_COLOR_TABLE[] = {
    {"aqua",        0xFF00FFFF},
    {"black",       0xFF000000},
    {"blue",        0xFF0000FF},
    {"cyan",        0xFF00FFFF},
    {"fuchsia",     0xFFFF00FF},
    {"gray",        0xFF808080},
    {"green",       0xFF008000},
    {"grey",        0xFF808080},
    {"lime",        0xFF00FF00},
    {"magenta",     0xFFFF00FF},
    {"maroon",      0xFF800000},
    {"navy",        0xFF000080},
    {"olive",       0xFF808000},
    {"orange",      0xFFFFA500},
    {"pink",        0xFFFFC0CB},
    {"purple",      0xFF800080},
    {"red",         0xFFFF0000},
    {"silver",      0xFFC0C0C0},
    {"teal",        0xFF008080},
    {"transparent", 0x00000000},
    {"white",       0xFFFFFFFF},
    {"yellow",      0xFFFFFF00},
};
static const size_t NAMED_COLOR_COUNT = sizeof(NAMED_COLOR_TABLE) / sizeof(NAMED_COLOR_TABLE[0]);

bool GetNamedColor(const std::string& name, ColorType& out)
{
    for (size_t i = 0; i < NAMED_COLOR_COUNT; ++i) {
        if (strcmp(name.c_str(), NAMED_COLOR_TABLE[i].name) == 0) {
            out.full = NAMED_COLOR_TABLE[i].value;
            return true;
        }
    }
    return false;
}

/**
 * @brief Strict float conversion.
 *
 * Requires the whole string to be consumed and the result to be finite, which
 * rejects inputs such as "12abc", "inf" or "nan" that strtof would otherwise
 * accept silently.
 *
 * @param text  candidate number, already trimmed.
 * @param out   parsed value, untouched on failure.
 * @return true when @p text is entirely a finite decimal number.
 */
bool TryParseFiniteFloat(const std::string& text, float& out)
{
    if (text.empty()) {
        return false;
    }
    /*
     * G.STD.04-CPP: the pointer returned by c_str() must not be stored, and
     * neither must a pointer derived from it such as the end pointer strtof
     * writes back. The conversion is therefore confined to the block below and
     * only the consumed length, a plain index, escapes it. c_str() is called
     * again inside the block instead of being cached; `text` is a const
     * reference that is never modified in between, so the standard guarantees
     * the same buffer address is returned.
     */
    float value = 0.0f;
    size_t consumedLength = 0;
    {
        char* parseEnd = nullptr;
        value = strtof(text.c_str(), &parseEnd);
        consumedLength = (parseEnd == nullptr) ? 0 : static_cast<size_t>(parseEnd - text.c_str());
    }
    /*
     * Reject partial consumption ("12abc") and empty conversions. Indexing at
     * consumedLength is the exact equivalent of the former "*parseEnd == '\0'"
     * test: std::string::operator[](size()) yields the terminating null.
     */
    if (consumedLength == 0 || consumedLength > text.length() || text[consumedLength] != '\0') {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] TryParseFiniteFloat: malformed number '%s', length=%u "
                    "consumed=%u (status=rejected, reason=not-a-complete-decimal-number)",
                    text.c_str(), static_cast<unsigned>(text.length()),
                    static_cast<unsigned>(consumedLength));
        return false;
    }
    /* Reject NaN (self comparison) and infinities. */
    if (value != value || value >= HUGE_VALF || value <= -HUGE_VALF) {
        return false;
    }
    out = value;
    return true;
}

/** @brief Clamp an integral channel into [0, 255] and narrow it to uint8_t. */
uint8_t ClampToChannel(int value)
{
    return static_cast<uint8_t>(std::max(CHANNEL_VALUE_MIN, std::min(CHANNEL_VALUE_MAX, value)));
}

/** @brief Clamp a float channel into [0, 255] and narrow it to uint8_t. */
uint8_t ClampToChannel(float value)
{
    return static_cast<uint8_t>(std::max(CHANNEL_VALUE_MIN_F, std::min(CHANNEL_VALUE_MAX_F, value)));
}

/** @brief Clamp a color stop offset into [0, 1]. */
float ClampToOffsetRange(float value)
{
    return std::max(GRADIENT_OFFSET_MIN, std::min(GRADIENT_OFFSET_MAX, value));
}

/**
 * @brief Interpolate two color stops the way W3C mandates.
 *
 * CSS Images Module Level 3, 3.4.3 "Color Stop Interpolation": colors are
 * interpolated in the premultiplied sRGBA space, otherwise a transition towards
 * a transparent stop travels through the transparent color's (meaningless) RGB
 * channels and visibly darkens in the middle.
 *
 * @param from color of the stop on the left hand side.
 * @param to   color of the stop on the right hand side.
 * @param ratio position inside the span, expected in [0, 1].
 * @return the interpolated color, un-premultiplied back into ColorType.
 */
ColorType InterpolateStopColor(const ColorType& from, const ColorType& to, float ratio)
{
    ColorType result;
    result.full = COLOR_FULL_TRANSPARENT;
    const float fromAlpha = static_cast<float>(from.alpha) / CHANNEL_VALUE_MAX_F;
    const float toAlpha = static_cast<float>(to.alpha) / CHANNEL_VALUE_MAX_F;
    const float alpha = fromAlpha + (toAlpha - fromAlpha) * ratio;
    /* Fully transparent: the RGB channels are undefined, keep them at zero. */
    if (alpha <= PREMULTIPLIED_ALPHA_EPSILON) {
        return result;
    }
    /* Premultiply, interpolate, then divide the straight alpha back out. */
    const float red = (static_cast<float>(from.red) * fromAlpha) +
        ((static_cast<float>(to.red) * toAlpha) - (static_cast<float>(from.red) * fromAlpha)) * ratio;
    const float green = (static_cast<float>(from.green) * fromAlpha) +
        ((static_cast<float>(to.green) * toAlpha) - (static_cast<float>(from.green) * fromAlpha)) * ratio;
    const float blue = (static_cast<float>(from.blue) * fromAlpha) +
        ((static_cast<float>(to.blue) * toAlpha) - (static_cast<float>(from.blue) * fromAlpha)) * ratio;
    result.red = ClampToChannel(red / alpha + CHANNEL_ROUNDING_BIAS);
    result.green = ClampToChannel(green / alpha + CHANNEL_ROUNDING_BIAS);
    result.blue = ClampToChannel(blue / alpha + CHANNEL_ROUNDING_BIAS);
    result.alpha = ClampToChannel(alpha * CHANNEL_VALUE_MAX_F + CHANNEL_ROUNDING_BIAS);
    return result;
}

/**
 * @brief Sample the color the ramp shows at @p position on the gradient line.
 *
 * Implements the W3C sampling rules: before the first stop and after the last
 * stop the ramp is a flat extension of the nearest stop color (the gradient is
 * padded, never repeated); in between it is the premultiplied interpolation of
 * the two surrounding stops; on a hard stop the right hand color wins.
 *
 * @param stops    monotonically non decreasing, sentinel free stop list.
 * @param position point on the gradient line, may be outside the stop range.
 * @return the color rendered at @p position.
 */
ColorType SampleGradientColorAt(const GradientStopList& stops, float position)
{
    if (position <= stops.Front().offset) {
        return stops.Front().color;
    }
    if (position >= stops.Back().offset) {
        return stops.Back().color;
    }
    for (uint8_t i = 1; i < stops.Size(); i++) {
        if (position > stops[i].offset) {
            continue;
        }
        const float span = stops[i].offset - stops[i - 1].offset;
        if (span <= GRADIENT_SPAN_EPSILON) {
            /* Hard stop: the color on the right hand side takes over. */
            return stops[i].color;
        }
        return InterpolateStopColor(stops[i - 1].color, stops[i].color,
                                    (position - stops[i - 1].offset) / span);
    }
    /* Unreachable while the list is sorted; kept as a defensive fallback. */
    return stops.Back().color;
}

/** @brief Whether every character is a hexadecimal digit. */
bool IsHexDigitSequence(const std::string& text)
{
    if (text.empty()) {
        return false;
    }
    for (size_t i = 0; i < text.length(); i++) {
        const char c = text[i];
        const bool isHexDigit = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!isHexDigit) {
            return false;
        }
    }
    return true;
}

/** @brief Whether @p text ends with @p suffix. */
bool EndsWith(const std::string& text, const char* suffix, size_t suffixLength)
{
    return text.length() > suffixLength &&
           text.compare(text.length() - suffixLength, suffixLength, suffix) == 0;
}

/** @brief Whether @p text starts with @p prefix. */
bool StartsWith(const std::string& text, const char* prefix, size_t prefixLength)
{
    return text.length() >= prefixLength && text.compare(0, prefixLength, prefix) == 0;
}

/**
 * @brief Whether @p text starts with @p prefix, ignoring ASCII case.
 *
 * Accepts every casing variant of the gradient function name
 * (linear-gradient / LINEAR-GRADIENT / Linear-Gradient / ...). Each character is
 * folded to lower case during the compare so the test stays locale independent
 * and no extra allocation is needed.
 */
bool StartsWithCaseInsensitive(const std::string& text, const char* prefix, size_t prefixLength)
{
    if (text.length() < prefixLength) {
        return false;
    }
    for (size_t i = 0; i < prefixLength; i++) {
        char tc = text[i];
        char pc = prefix[i];
        if (tc >= 'A' && tc <= 'Z') {
            tc = static_cast<char>(tc + ('a' - 'A'));
        }
        if (pc >= 'A' && pc <= 'Z') {
            pc = static_cast<char>(pc + ('a' - 'A'));
        }
        if (tc != pc) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Try to read "<number><unit>" and convert it to degrees.
 *
 * @param text       trimmed, lowercased candidate such as "45deg" or "0.125turn".
 * @param outDegrees resulting angle in degrees, untouched on failure.
 * @return true when a supported unit matched and its number is well formed.
 */
bool TryParseAngleWithUnit(const std::string& text, float& outDegrees)
{
    for (size_t i = 0; i < ANGLE_UNIT_COUNT; i++) {
        const AngleUnitSpec& unit = ANGLE_UNITS[i];
        const size_t suffixLength = strlen(unit.suffix);
        if (!EndsWith(text, unit.suffix, suffixLength)) {
            continue;
        }
        const std::string numberPart = Trim(text.substr(0, text.length() - suffixLength));
        float value = 0.0f;
        if (!TryParseFiniteFloat(numberPart, value)) {
            /* The suffix matched but the number did not; try the next unit. */
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "[GRADIENT] TryParseAngleWithUnit: angle unit '%s' matched token '%s' "
                        "but numeric part '%s' is not a finite decimal (status=failed, "
                        "reason=invalid-angle-number, trigger=angle-table-lookup)",
                        unit.suffix, text.c_str(), numberPart.c_str());
            continue;
        }
        outDegrees = value * unit.degreesPerUnit;
        return true;
    }
    return false;
}

/** @brief Whether @p text ends with one of the supported angle unit suffixes. */
bool EndsWithAngleUnit(const std::string& text)
{
    for (size_t i = 0; i < ANGLE_UNIT_COUNT; i++) {
        const char* suffix = ANGLE_UNITS[i].suffix;
        const size_t suffixLength = strlen(suffix);
        if (EndsWith(text, suffix, suffixLength)) {
            return true;
        }
    }
    return false;
}

/* --- ParseHexColor helpers: one per hex literal width ---------------------- */
/**
 * @brief Parse an unsigned long from a hexadecimal string.
 *
 * Wraps strtoul with explicit end-pointer and errno checks so the conversion
 * result is fully validated, satisfying G.FUU.01 without introducing exceptions.
 *
 * @param hex  candidate hexadecimal text.
 * @param out  receives the converted value, untouched on failure.
 * @return true when the entire string was consumed and no overflow occurred.
 */
bool TryParseHexUint(const std::string& hex, unsigned long& out)
{
    if (hex.empty()) {
        return false;
    }
    errno = 0;
    char* parseEnd = nullptr;
    out = strtoul(hex.c_str(), &parseEnd, HEX_BASE);
    if (parseEnd == nullptr || parseEnd == hex.c_str() || *parseEnd != '\0') {
        return false;
    }
    if (errno == ERANGE) {
        return false;
    }
    return true;
}

/** @brief Parse a #RGB shorthand literal into @p out. */
bool ParseHexShorthandColor(const std::string& hex, ColorType& out)
{
    unsigned long value = 0;
    if (!TryParseHexUint(hex, value)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseHexShorthandColor: malformed shorthand hex '%s' "
                    "(status=failed, reason=invalid-hex-number)",
                    hex.c_str());
        return false;
    }
    /* A #RGB payload is exactly 12 bits; a wider value means the length gate
     * in ParseHexColor() was bypassed and the unpack below would silently
     * drop the high bits, producing the wrong color. */
    if (value >> HEX_SHORTHAND_BIT_WIDTH) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseHexShorthandColor: shorthand hex '%s' exceeds the 12-bit "
                    "#RGB payload width (status=failed, reason=payload-overflow, value=0x%lx, "
                    "trigger=length-gate-bypassed, hexLength=%u)",
                    hex.c_str(), value, static_cast<unsigned>(hex.length()));
        return false;
    }
    const unsigned int red = (value >> HEX_SHORTHAND_RED_SHIFT) & HEX_SHORTHAND_NIBBLE_MASK;
    const unsigned int green = (value >> HEX_SHORTHAND_GREEN_SHIFT) & HEX_SHORTHAND_NIBBLE_MASK;
    const unsigned int blue = value & HEX_SHORTHAND_NIBBLE_MASK;
    out.red = static_cast<uint8_t>(red * HEX_SHORTHAND_EXPAND_FACTOR);
    out.green = static_cast<uint8_t>(green * HEX_SHORTHAND_EXPAND_FACTOR);
    out.blue = static_cast<uint8_t>(blue * HEX_SHORTHAND_EXPAND_FACTOR);
    out.alpha = COLOR_CHANNEL_MAX;
    return true;
}

/** @brief Parse a #RRGGBB literal into @p out. */
bool ParseHexRgbColor(const std::string& hex, ColorType& out)
{
    unsigned long packed = 0;
    if (!TryParseHexUint(hex, packed)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseHexRgbColor: malformed hex '%s' "
                    "(status=failed, reason=invalid-hex-number)",
                    hex.c_str());
        return false;
    }
    out.red = static_cast<uint8_t>((packed >> HEX_RGB_RED_SHIFT) & BYTE_MASK);
    out.green = static_cast<uint8_t>((packed >> HEX_RGB_GREEN_SHIFT) & BYTE_MASK);
    out.blue = static_cast<uint8_t>(packed & BYTE_MASK);
    out.alpha = COLOR_CHANNEL_MAX;
    return true;
}

/** @brief Parse a #RRGGBBAA literal into @p out. */
bool ParseHexRgbaColor(const std::string& hex, ColorType& out)
{
    unsigned long packed = 0;
    if (!TryParseHexUint(hex, packed)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseHexRgbaColor: malformed hex '%s' "
                    "(status=failed, reason=invalid-hex-number)",
                    hex.c_str());
        return false;
    }
    out.red = static_cast<uint8_t>((packed >> HEX_RGBA_RED_SHIFT) & BYTE_MASK);
    out.green = static_cast<uint8_t>((packed >> HEX_RGBA_GREEN_SHIFT) & BYTE_MASK);
    out.blue = static_cast<uint8_t>((packed >> HEX_RGBA_BLUE_SHIFT) & BYTE_MASK);
    out.alpha = static_cast<uint8_t>(packed & BYTE_MASK);
    return true;
}

bool ParseHexColor(const std::string& lowercaseLiteral, ColorType& out)
{
    const std::string hex = lowercaseLiteral.substr(1);
    if (!IsHexDigitSequence(hex)) {
        /* Guards against "#gggggg" and rejects trailing junk such as "#12345z"
         * before the width-specific parsers attempt to decode the payload. */
        return false;
    }
    if (hex.length() == HEX_SHORTHAND_LENGTH) {
        return ParseHexShorthandColor(hex, out);
    }
    if (hex.length() == HEX_RGB_LENGTH) {
        return ParseHexRgbColor(hex, out);
    }
    if (hex.length() == HEX_RGBA_LENGTH) {
        return ParseHexRgbaColor(hex, out);
    }
    return false; /* Unsupported digit count such as "#ff00". */
}

/**
 * @brief Extract the argument list of "name(...)" into @p outArguments.
 *
 * @param literal      the whole function call, lowercased.
 * @param prefixLength length of the "name(" prefix.
 * @param outArguments text located between the parentheses.
 * @return true when a closing parenthesis follows a non empty argument list.
 */
bool ExtractFunctionArguments(const std::string& literal, size_t prefixLength, std::string& outArguments)
{
    const size_t closing = literal.rfind(PAREN_CLOSE);
    if (closing == std::string::npos || closing <= prefixLength) {
        return false;
    }
    outArguments = literal.substr(prefixLength, closing - prefixLength);
    return true;
}

/**
 * @brief Validate the prefix of a linear-gradient() declaration and extract its
 *        comma separated arguments.
 *
 * Performs the case insensitive prefix match, records non canonical casing
 * variants, extracts the text between the outer parentheses, and splits it on
 * top level commas.
 *
 * @param value    the full CSS declaration.
 * @param outParts receives the trimmed, non empty argument list.
 * @return true when the prefix is valid, the parentheses are balanced, and at
 *         least GRADIENT_MIN_COLOR_STOP_COUNT arguments were found.
 */
bool ValidateAndExtractGradientArgs(const std::string& value, GradientArgList& outParts)
{
    const size_t prefixLength = strlen(LINEAR_GRADIENT_PREFIX);
    if (!StartsWithCaseInsensitive(value, LINEAR_GRADIENT_PREFIX, prefixLength)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ValidateAndExtractGradientArgs: unrecognized function name in "
                    "'%s' (status=rejected, reason=missing-linear-gradient-prefix, "
                    "expectedPrefix='%s')",
                    value.c_str(), LINEAR_GRADIENT_PREFIX);
        return false;
    }
    if (!StartsWith(value, LINEAR_GRADIENT_PREFIX, prefixLength)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ValidateAndExtractGradientArgs: non-canonical function name "
                    "casing detected in '%s' (status=normalized, action=case-insensitive-match, "
                    "expectedPrefix='%s')",
                    value.c_str(), LINEAR_GRADIENT_PREFIX);
    }
    std::string inner;
    if (!ExtractFunctionArguments(value, prefixLength, inner)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[GRADIENT] ValidateAndExtractGradientArgs: missing closing ')' in '%s'",
                    value.c_str());
        return false;
    }
    outParts = SplitGradientArgs(inner);
    if (outParts.Size() < GRADIENT_MIN_COLOR_STOP_COUNT) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ValidateAndExtractGradientArgs: expected >=%u arguments, got %u in '%s'",
                    static_cast<unsigned>(GRADIENT_MIN_COLOR_STOP_COUNT), static_cast<unsigned>(outParts.Size()),
                    value.c_str());
        return false;
    }
    return true;
}

/**
 * @brief Resolve the first argument of a linear-gradient() as either a
 *        direction/angle or the first color stop.
 *
 * When the first argument is a direction keyword or angle, the corresponding
 * fields in @p outInfo are set and the color stop index starts at 1.
 * Otherwise the argument is probed as a color stop; if it is a valid color
 * stop or looks like a malformed angle, the default direction "to bottom" is
 * applied and the index starts at 0. An unrecognized token (e.g. "to nowhere")
 * causes the gradient to be rejected.
 *
 * @param parts             the split arguments of the gradient function.
 * @param outInfo           destination payload for direction/angle.
 * @param value             the original CSS value (for error logging).
 * @param outFirstColorIndex receives the index of the first color stop.
 * @return true when the first argument was successfully resolved.
 */
bool ResolveFirstGradientArgument(const GradientArgList &parts,
                                  GradientInfo *outInfo,
                                  const std::string &value,
                                  size_t &outFirstColorIndex)
{
    const std::string firstArg = Trim(parts[0]);
    const bool firstIsDirection = ParseDirectionOrAngle(firstArg, outInfo);
    if (!firstIsDirection) {
        GradientColorStop probeStop;
        const bool isColorStop = ParseColorStop(firstArg, probeStop);
        const bool looksLikeAngle = EndsWithAngleUnit(firstArg);
        if (isColorStop) {
            /* A leading color stop means the gradient has no explicit direction,
             * so the default (to bottom) applies. */
            outInfo->direction = CssGradientDirection::TO_BOTTOM;
            outInfo->angle = CSS_ANGLE_DEFAULT;
        } else if (looksLikeAngle) {
            /* The first argument is shaped like an angle (ends with deg/rad/turn)
             * but ParseDirectionOrAngle could not turn it into a finite number,
             * so it is NOT a valid <angle>. Per W3C CSS Images a gradient that
             * contains an invalid <angle> makes the whole declaration invalid;
             * ignore it instead of silently degrading to a default direction. */
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "[GRADIENT] ResolveFirstGradientArgument: illegal <angle> '%s' "
                        "in '%s', gradient disabled (status=failed, reason=invalid-angle)",
                        firstArg.c_str(), value.c_str());
            return false;
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "[GRADIENT] ResolveFirstGradientArgument: illegal direction value '%s' "
                        "in '%s', gradient disabled (status=failed, reason=invalid-direction)",
                        firstArg.c_str(), value.c_str());
            return false;
        }
    }
    outFirstColorIndex = firstIsDirection ? 1 : 0;
    return true;
}

/**
 * @brief Normalize, clip, and store the parsed color stops into the gradient
 *        info payload.
 *
 * Applies the W3C normalization and projection steps, validates the minimum
 * stop count, and publishes the result via GradientInfo::SetColorStops().
 *
 * @param colorStops the parsed color stops (may be modified in place).
 * @param outInfo    destination payload.
 * @param value      the original CSS value (for error logging).
 * @return true when the stops were successfully stored.
 */
bool ProcessAndStoreColorStops(GradientStopList& colorStops,
                               GradientInfo* outInfo,
                               const std::string& value)
{
    if (colorStops.Size() < GRADIENT_MIN_COLOR_STOP_COUNT) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ProcessAndStoreColorStops: only %u valid color stops in '%s'",
                    static_cast<unsigned>(colorStops.Size()), value.c_str());
        return false;
    }
    NormalizeColorStops(colorStops);
    ClipColorStopsToGradientLine(colorStops);
    if (colorStops.Size() > LINEAR_GRADIENT_MAX_COLORS) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ProcessAndStoreColorStops: %u stops after clipping exceed the "
                    "limit of %u in '%s' (status=truncated)",
                    static_cast<unsigned>(colorStops.Size()),
                    static_cast<unsigned>(LINEAR_GRADIENT_MAX_COLORS), value.c_str());
    }
    outInfo->type = GradientType::LINEAR;
    outInfo->SetColorStops(colorStops.stops, colorStops.Size());
    if (!outInfo->isValid) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ProcessAndStoreColorStops: failed to store %u color stops "
                    "(out of memory?)",
                    static_cast<unsigned>(colorStops.Size()));
        return false;
    }
    return true;
}

/**
 * @brief Split a color stop string into its color literal and optional position.
 *
 * A trailing "<number>%" token (separated by whitespace) is the stop position;
 * everything before it is the color literal. When no such token exists the whole
 * string is the color and @p outOffsetPart stays empty, telling the caller to
 * leave the offset unspecified.
 *
 * @param stopStr      the trimmed color stop text.
 * @param outColorPart receives the color literal (always assigned).
 * @param outOffsetPart receives the trailing "<number>%" token, empty when absent.
 */
void SplitColorStopParts(const std::string& stopStr, std::string& outColorPart, std::string& outOffsetPart)
{
    outColorPart = stopStr;
    outOffsetPart.clear();
    /* A trailing "<number>%" token, if present, is the stop position. */
    const size_t lastSpace = stopStr.rfind(' ');
    if (lastSpace != std::string::npos) {
        const std::string candidate = stopStr.substr(lastSpace + 1);
        if (!candidate.empty() && candidate.back() == PERCENT_MARKER) {
            outColorPart = stopStr.substr(0, lastSpace);
            outOffsetPart = candidate;
        }
    }
}

/**
 * @brief Resolve the stop position from its trailing "%" token (W3C rules).
 *
 * A malformed percentage falls back to an unspecified offset so the stop stays
 * usable; a well formed percentage is converted to a ratio and handed to
 * ApplyW3cOffsetRules() for the extent clamp, sentinel fix up and out of range
 * preservation.
 *
 * @param stopStr    original stop text (for error logging).
 * @param offsetPart trailing "<number>%" token (must be non empty).
 * @param color      the already parsed stop color (for error logging).
 * @param outOffset  receives the resolved offset ratio.
 */
void ApplyW3cOffsetRules(const std::string& stopStr,
                         float percent,
                         const ColorType& color,
                         float& offset)
{
    /* Clamp absurd magnitudes so the interpolation stays well conditioned. */
    if (offset < -GRADIENT_OFFSET_EXTENT_LIMIT || offset > GRADIENT_OFFSET_EXTENT_LIMIT) {
        const float limited = std::max(-GRADIENT_OFFSET_EXTENT_LIMIT,
                                       std::min(GRADIENT_OFFSET_EXTENT_LIMIT, offset));
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: extreme position in '%s' percent=%.2f offset=%.4f "
                    "limit=%.1f color=0x%08x (status=clamped, reason=out-of-extent)",
                    stopStr.c_str(), percent, offset,
                    static_cast<double>(GRADIENT_OFFSET_EXTENT_LIMIT),
                    static_cast<unsigned>(color.full));
        offset = limited;
    }
    /* -100% == the unspecified sentinel; nudge it aside to stay distinguishable. */
    if (IsGradientOffsetUnspecified(offset)) {
        const float disambiguated = GRADIENT_OFFSET_UNSPECIFIED + SENTINEL_DISAMBIGUATION_STEP;
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: position collides with the unspecified sentinel "
                    "in '%s' offset=%.6f->%.6f color=0x%08x (status=disambiguated)",
                    stopStr.c_str(), offset, disambiguated, static_cast<unsigned>(color.full));
        offset = disambiguated;
    }
    /* Legal out of range positions are preserved, not clamped (CSS-Images-3.4.1). */
    if (offset < GRADIENT_OFFSET_MIN || offset > GRADIENT_OFFSET_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: out-of-range position kept for W3C interpolation "
                    "stop='%s' percent=%.2f offset=%.4f color=0x%08x (status=preserved)",
                    stopStr.c_str(), percent, offset, static_cast<unsigned>(color.full));
    }
}

/**
 * @brief Resolve the stop position from its trailing "%" token (W3C rules).
 *
 * A malformed percentage falls back to an unspecified offset so the stop stays
 * usable; a well formed percentage is converted to a ratio and handed to
 * ApplyW3cOffsetRules() for the extent clamp, sentinel fix up and out of range
 * preservation.
 *
 * @param stopStr    original stop text (for error logging).
 * @param offsetPart trailing "<number>%" token (must be non empty).
 * @param color      the already parsed stop color (for error logging).
 * @param outOffset  receives the resolved offset ratio.
 */
void ResolveColorStopOffset(const std::string& stopStr,
                            const std::string& offsetPart,
                            const ColorType& color,
                            float& outOffset)
{
    float percent = 0.0f;
    const std::string numberPart = Trim(offsetPart.substr(0, offsetPart.size() - 1));
    if (!TryParseFiniteFloat(numberPart, percent)) {
        /* Malformed percentage: keep the stop usable with an unspecified offset. */
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: malformed percentage in '%s' number='%s' "
                    "color=0x%08x (status=fallback, reason=invalid-percentage-number)",
                    stopStr.c_str(), numberPart.c_str(), static_cast<unsigned>(color.full));
        outOffset = GRADIENT_OFFSET_UNSPECIFIED;
        return;
    }
    /* W3C 3.4.1: keep the authored ratio verbatim; project onto [0,1] later. */
    float offset = percent / PERCENT_TO_RATIO_DIVISOR;
    ApplyW3cOffsetRules(stopStr, percent, color, offset);
    outOffset = offset;
}

/**
 * @brief Parse three comma-separated integers from a string such as "0, 0, 255".
 *
 * Lightweight replacement for std::stringstream used by rgb(). Rejects trailing
 * garbage and missing components.
 */
static bool ParseThreeIntegers(const char* str, int& red, int& green, int& blue)
{
    if (str == nullptr) {
        return false;
    }
    char* end = nullptr;
    red = static_cast<int>(strtol(str, &end, DECIMAL_BASE));
    if (end == nullptr || *end != ',') {
        return false;
    }
    green = static_cast<int>(strtol(end + 1, &end, DECIMAL_BASE));
    if (end == nullptr || *end != ',') {
        return false;
    }
    blue = static_cast<int>(strtol(end + 1, &end, DECIMAL_BASE));
    if (end == nullptr) {
        return false;
    }
    while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r') {
        ++end;
    }
    return *end == '\0';
}

/**
 * @brief Parse three comma-separated integers plus one float from a string such
 * as "255, 0, 0, 0.5".
 *
 * Lightweight replacement for std::stringstream used by rgba(). Rejects NaN,
 * infinities, trailing garbage and missing components.
 */
static bool ParseThreeIntegersAndFloat(const char* str, int& red, int& green, int& blue, float& alpha)
{
    if (str == nullptr) {
        return false;
    }
    char* end = nullptr;
    red = static_cast<int>(strtol(str, &end, DECIMAL_BASE));
    if (end == nullptr || *end != ',') {
        return false;
    }
    green = static_cast<int>(strtol(end + 1, &end, DECIMAL_BASE));
    if (end == nullptr || *end != ',') {
        return false;
    }
    blue = static_cast<int>(strtol(end + 1, &end, DECIMAL_BASE));
    if (end == nullptr || *end != ',') {
        return false;
    }
    alpha = strtof(end + 1, &end);
    if (end == nullptr || alpha != alpha || alpha >= HUGE_VALF || alpha <= -HUGE_VALF) {
        return false;
    }
    while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r') {
        ++end;
    }
    return *end == '\0';
}

/** @brief Parse an "rgba(r,g,b,a)" color literal into @p out. */
bool ParseRgbaFunction(const std::string& literal, ColorType& out)
{
    std::string arguments;
    if (!ExtractFunctionArguments(literal, strlen(RGBA_FUNCTION_PREFIX), arguments)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseRgbaFunction: missing ')' in rgba arguments '%s' "
                    "(status=failed, reason=invalid-rgba-args, trigger=extract-function-arguments, "
                    "argLength=%u)",
                    literal.c_str(), static_cast<unsigned>(literal.length()));
        return false;
    }
    int red = 0;
    int green = 0;
    int blue = 0;
    float alpha = 0.0f;
    if (!ParseThreeIntegersAndFloat(arguments.c_str(), red, green, blue, alpha)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseRgbaFunction: failed to parse rgba components from '%s' "
                    "(status=failed, reason=invalid-rgba-token, trigger=parse-three-integers-and-float, "
                    "argLength=%u)",
                    arguments.c_str(), static_cast<unsigned>(arguments.length()));
        return false;
    }
    out.red = ClampToChannel(red);
    out.green = ClampToChannel(green);
    out.blue = ClampToChannel(blue);
    out.alpha = ClampToChannel(alpha * CHANNEL_VALUE_MAX_F);
    return true;
}

/** @brief Parse an "rgb(r,g,b)" color literal into @p out. */
bool ParseRgbFunction(const std::string& literal, ColorType& out)
{
    std::string arguments;
    if (!ExtractFunctionArguments(literal, strlen(RGB_FUNCTION_PREFIX), arguments)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseRgbFunction: missing ')' in rgb arguments '%s' "
                    "(status=failed, reason=invalid-rgb-args, trigger=extract-function-arguments, "
                    "argLength=%u)",
                    literal.c_str(), static_cast<unsigned>(literal.length()));
        return false;
    }
    int red = 0;
    int green = 0;
    int blue = 0;
    if (!ParseThreeIntegers(arguments.c_str(), red, green, blue)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseRgbFunction: failed to parse rgb components from '%s' "
                    "(status=failed, reason=invalid-rgb-token, trigger=parse-three-integers, "
                    "argLength=%u)",
                    arguments.c_str(), static_cast<unsigned>(arguments.length()));
        return false;
    }
    out.red = ClampToChannel(red);
    out.green = ClampToChannel(green);
    out.blue = ClampToChannel(blue);
    out.alpha = COLOR_CHANNEL_MAX;
    return true;
}

/** @brief Locate "linear-gradient(" (case-insensitively).
 *  Logs a missing-prefix rejection and a non-canonical-casing note. */
bool FindLinearGradientPrefix(const std::string& cssValue, size_t& prefixPos)
{
    const std::string loweredValue = ToLower(cssValue);
    const size_t pos = loweredValue.find(LINEAR_GRADIENT_PREFIX);
    if (pos == std::string::npos) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] NormalizeDirectionKeywordToAngle: no 'linear-gradient(' in '%s' "
                    "(status=rejected, reason=missing-prefix)",
                    cssValue.c_str());
        return false;
    }
    /* Non-canonical function-name casing was detected; record it for visibility. */
    if (cssValue.find(LINEAR_GRADIENT_PREFIX) == std::string::npos) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] NormalizeDirectionKeywordToAngle: non-canonical function name casing "
                    "detected in '%s' (status=normalized, action=case-insensitive-match, "
                    "expectedPrefix='%s')",
                    cssValue.c_str(), LINEAR_GRADIENT_PREFIX);
    }
    prefixPos = pos;
    return true;
}

/*
 * Compile-time guarantee that the angle buffer can hold ANY int angle followed by
 * "deg" and the terminating '\0'. The decimal representation of an int is at most
 * std::numeric_limits<int>::digits10 + 2 characters (sign + digits); plus 3 for
 * "deg" and 1 for the '\0'. Fails the build immediately if ANGLE_TEXT_BUFFER_SIZE
 * is ever shrunk below this bound — no runtime overflow is possible by construction.
 */
static_assert(ANGLE_TEXT_BUFFER_SIZE >= (std::numeric_limits<int>::digits10 + 2 + 3 + 1),
              "ANGLE_TEXT_BUFFER_SIZE is too small to hold \"<int>deg\"");

/** @brief Format a whole-degree angle as "<n>deg", validating the buffer. */
bool FormatAngleToken(int angleDeg, char (&angleText)[ANGLE_TEXT_BUFFER_SIZE])
{
    /*
     * C++11 type-safe formatting: build the token with std::to_string() + "deg".
     * There is no printf format string and no variadic argument list, so the
     * entire class of format-string bugs (wrong specifier, missing argument,
     * mismatch) is eliminated by construction. Every target ships a conformant
     * std::to_string, so no platform branch is needed.
     */
    const std::string token = std::to_string(angleDeg) + "deg";
    /*
     * Boundary discipline:
     *   - The static_assert above already proves at COMPILE time that the buffer
     *     fits any int, so this runtime test is a defensive backstop (e.g. if the
     *     constant is edited without rebuilding the assertion, or a non-int path
     *     is added later).
     *   - If it does not fit, the buffer would be left unterminated, so we must
     *     reject and let NormalizeDirectionKeywordToAngle() bail out rather than
     *     feed a corrupted angle to the caller.
     */
    if (token.size() >= sizeof(angleText)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] FormatAngleToken: angle token buffer too small "
                    "(status=failed, reason=buffer-too-small, angle=%d, len=%u, capacity=%u)",
                    angleDeg, static_cast<unsigned>(token.size()),
                    static_cast<unsigned>(sizeof(angleText)));
        return false;
    }
    std::copy(token.begin(), token.end(), angleText); /* short tokens stay in the SSO buffer: no heap allocation */
    angleText[token.size()] = '\0';
    return true;
}

/** @brief Anchor the two endpoints when their offset was left unspecified. */
void AnchorEndpointOffsets(GradientStopList& stops)
{
    /*
     * IsGradientOffsetUnspecified() is used (not a sign test): CSS-legal negative
     * positions such as -20% survive parsing, so a plain "offset < 0" check would
     * wrongly reset them to 0.
     */
    if (IsGradientOffsetUnspecified(stops.Front().offset)) {
        stops.Front().offset = GRADIENT_OFFSET_MIN;
    }
    if (IsGradientOffsetUnspecified(stops.Back().offset)) {
        stops.Back().offset = GRADIENT_OFFSET_MAX;
    }
}

/** @brief Spread each run of unspecified offsets evenly across its gap. */
void DistributeUnspecifiedOffsets(GradientStopList& stops)
{
    const size_t lastIndex = stops.Size() - 1;
    size_t i = 1;
    while (i < lastIndex) {
        if (!IsGradientOffsetUnspecified(stops[i].offset)) {
            i++;
            continue;
        }
        /* [i, runEnd) is the run of unspecified stops; runEnd is anchored. */
        size_t runEnd = i + 1;
        while (runEnd < stops.Size() && IsGradientOffsetUnspecified(stops[runEnd].offset)) {
            runEnd++;
        }
        const float gapStart = stops[i - 1].offset;
        const float gapEnd = (runEnd < stops.Size()) ? stops[runEnd].offset : GRADIENT_OFFSET_MAX;
        const size_t segmentCount = runEnd - i + 1;
        const float step = (gapEnd - gapStart) / static_cast<float>(segmentCount);
        for (size_t k = i; k < runEnd; k++) {
            stops[k].offset = gapStart + step * static_cast<float>(k - i + 1);
        }
        i = runEnd;
    }
}

/** @brief W3C 3.4.1 fix-up: clamp an out-of-order stop onto its predecessor. */
void ClampOutOfOrderStops(GradientStopList& stops)
{
    /*
     * W3C CSS Images Module Level 3, 3.4.1 fix-up rule 2: a stop whose position
     * is less than the largest specified position of any stop before it is
     * raised IN PLACE to that largest position (clamp-in-place, never sorted).
     *
     * This runs BEFORE DistributeUnspecifiedOffsets(): unspecified stops still
     * carry the GRADIENT_OFFSET_UNSPECIFIED sentinel here and are skipped - they
     * only receive a position in fix-up rule 3 - so the running maximum tracks
     * stops with an authored position only. The first stop is always specified
     * after AnchorEndpointOffsets(), hence it seeds the running maximum.
     */
    float maxSpecifiedOffset = stops.Front().offset;
    for (uint8_t k = 1; k < stops.Size(); k++) {
        if (IsGradientOffsetUnspecified(stops[k].offset)) {
            continue; /* fix-up rule 3 assigns the position. */
        }
        if (stops[k].offset < maxSpecifiedOffset) {
            const float authored = stops[k].offset;
            stops[k].offset = maxSpecifiedOffset;
        } else {
            maxSpecifiedOffset = stops[k].offset;
        }
    }
}

/** @brief Decide whether @p stops must be projected onto the painted segment.
 *  Returns false (and logs) when there are too few stops, or when the ramp is
 *  already fully inside [0,1] and can be left untouched. */
bool GradientNeedsClipping(const GradientStopList& stops, float& firstOffset, float& lastOffset)
{
    if (stops.Size() < GRADIENT_MIN_COLOR_STOP_COUNT) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ClipColorStopsToGradientLine: expected >=%u stops, got %u "
                    "(status=skipped, reason=too-few-stops)",
                    static_cast<unsigned>(GRADIENT_MIN_COLOR_STOP_COUNT),
                    static_cast<unsigned>(stops.Size()));
        return false;
    }
    firstOffset = stops.Front().offset;
    lastOffset = stops.Back().offset;
    if (firstOffset >= GRADIENT_OFFSET_MIN && lastOffset <= GRADIENT_OFFSET_MAX) {
        return false; /* fast path: already within the painted segment */
    }
    return true;
}

/** @brief Build the [0,1] projection: sample endpoints, keep interior stops. */
GradientStopList BuildClippedStops(
    const GradientStopList& stops, const ColorType& startColor, const ColorType& endColor)
{
    GradientStopList clipped;
    clipped.PushBack(GradientColorStop(startColor, GRADIENT_OFFSET_MIN));
    for (uint8_t k = 0; k < stops.Size(); k++) {
        const float offset = stops[k].offset;
        if (offset > GRADIENT_OFFSET_MIN && offset < GRADIENT_OFFSET_MAX) {
            if (!clipped.PushBack(GradientColorStop(stops[k].color, ClampToOffsetRange(offset)))) {
                break;
            }
        }
    }
    clipped.PushBack(GradientColorStop(endColor, GRADIENT_OFFSET_MAX));
    return clipped;
}
} // namespace

float DirectionToCssAngle(CssGradientDirection dir)
{
    /* Forward to the shared table in gradient_info.h; keeping a second switch
     * here is exactly the duplication this refactoring removes. */
    return CssGradientDirectionToAngle(dir);
}

bool ParseColorToColorType(const std::string& colorStr, ColorType& out)
{
    out.full = COLOR_FULL_TRANSPARENT;
    const std::string literal = ToLower(Trim(colorStr));
    if (literal.empty()) {
        return false;
    }
    if (GetNamedColor(literal, out)) {
        return true; /* attempt 1: named */
    }
    if (literal[0] == HEX_COLOR_MARKER) {
        return ParseHexColor(literal, out); /* attempt 2: hex */
    }
    /* Attempt 3 (rgba) is checked before rgb: "rgba(" also starts with "rgb". */
    if (StartsWith(literal, RGBA_FUNCTION_PREFIX, strlen(RGBA_FUNCTION_PREFIX))) {
        return ParseRgbaFunction(literal, out);
    }
    if (StartsWith(literal, RGB_FUNCTION_PREFIX, strlen(RGB_FUNCTION_PREFIX))) {
        return ParseRgbFunction(literal, out);                          /* attempt 4: rgb */
    }
    return false; /* Unrecognized color syntax. */
}

namespace {
/* Case-insensitive match of `literal` (len chars) against value[offset, offset+len).
 * Returns false when the window would run past the end of `value`. */
bool IsLinearPrefixLiteralAt(const std::string& value, size_t offset, const char* literal, size_t len)
{
    if (offset + len > value.length()) {
        return false;
    }
    for (size_t k = 0; k < len; ++k) {
        char tc = value[offset + k];
        if (tc >= 'A' && tc <= 'Z') {
            tc = static_cast<char>(tc + ('a' - 'A'));
        }
        if (tc != literal[k]) {
            return false;
        }
    }
    return true;
}

/* Advance past one or more ASCII whitespace chars starting at `from`.
 * Returns the index of the first non-whitespace char, or std::string::npos when
 * there is no whitespace gap (so the caller can skip the suffix check). */
size_t SkipMandatoryGradientWhitespace(const std::string& value, size_t from)
{
    size_t j = from;
    bool hasWs = false;
    while (j < value.length()) {
        const char c = value[j];
        const bool ws = (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');
        if (!ws) {
            break;
        }
        hasWs = true;
        ++j;
    }
    return hasWs ? j : std::string::npos;
}
} // namespace

bool IsBrokenGradientFunctionToken(const std::string& value)
{
    /* Locate "linear-" (case-insensitive), then require one or more ASCII
     * whitespace chars, then the literal "gradient(" — the '(' makes this a
     * function token and avoids false positives on image URLs such as
     * "url(linear- gradient.png)" which legitimately contain the substring. */
    const char* const prefix = "linear-";
    const char* const suffix = "gradient(";
    const size_t prefixLen = strlen(prefix);
    const size_t suffixLen = strlen(suffix);
    /* A valid broken-token candidate needs at least: prefix + 1 ws + suffix. */
    if (value.length() < prefixLen + 1 + suffixLen) {
        return false;
    }
    for (size_t i = 0; i + prefixLen <= value.length(); ++i) {
        if (!IsLinearPrefixLiteralAt(value, i, prefix, prefixLen)) {
            continue;
        }
        const size_t j = SkipMandatoryGradientWhitespace(value, i + prefixLen);
        if (j == std::string::npos) {
            continue; /* No whitespace gap between "linear-" and the next token. */
        }
        if (IsLinearPrefixLiteralAt(value, j, suffix, suffixLen) &&
            (j + suffixLen) <= value.length()) {
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "[GRADIENT] IsBrokenGradientFunctionToken: broken gradient function "
                        "token detected (status=broken, gradType=linear, token='%s', "
                        "prefixPos=%zu, op=IsBrokenGradientFunctionToken, "
                        "reason=linear-prefix followed by whitespace then gradient( without a "
                        "valid function form)",
                        value.c_str(), i);
            return true;
        }
    }
    return false;
}

bool ParseLinearGradient(const std::string& value, GradientInfo* outInfo)
{
    /* Step 1: validate the destination pointer. */
    if (outInfo == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[GRADIENT] ParseLinearGradient: outInfo is null");
        return false;
    }
    /* Step 2: validate the function prefix and extract the comma separated arguments. */
    GradientArgList parts;
    if (!ValidateAndExtractGradientArgs(value, parts)) {
        return false;
    }
    /* Step 3: resolve the first argument as a direction/angle or a color stop. */
    size_t firstColorIndex = 0;
    if (!ResolveFirstGradientArgument(parts, outInfo, value, firstColorIndex)) {
        return false;
    }
    /* Step 4: parse the color stops, tolerating individual failures. */
    GradientStopList colorStops;
    for (size_t i = firstColorIndex; i < parts.Size(); i++) {
        GradientColorStop stop;
        if (ParseColorStop(Trim(parts[i]), stop)) {
            if (!colorStops.PushBack(stop)) {
                break;
            }
        } else {
            HILOG_WARN(HILOG_MODULE_ACE, "[GRADIENT] ParseLinearGradient: skipping invalid color stop '%s'",
                       parts[i].c_str());
        }
    }
    /* Step 5: normalize, clip, and store the color stops. */
    return ProcessAndStoreColorStops(colorStops, outInfo, value);
}

bool ParseDirectionOrAngle(const std::string& param, GradientInfo* info)
{
    if (info == nullptr) {
        return false;
    }
    const std::string trimmed = Trim(ToLower(param));
    if (trimmed.empty()) {
        return false;
    }
    /* CSS treats any run of whitespace between tokens as a single separator, so
     * "to    right" and "to right" are equivalent. The direction map is keyed on
     * single-space tokens, therefore collapse runs of spaces before the lookup;
     * otherwise a valid but loosely-spaced keyword silently fails to match and
     * the entire gradient is dropped. This also normalises loosely-spaced angle
     * literals such as "45   deg" without changing the resolved value. */
    const std::string normalized = CollapseWhitespace(trimmed);
    /* Attempt 1: a direction keyword such as "to bottom right". */
    CssGradientDirection direction = CssGradientDirection::TO_BOTTOM;
    if (FindDirection(normalized, direction)) {
        info->direction = direction;
        info->angle = CssGradientDirectionToAngle(direction);
        return true;
    }
    /* Attempt 2: an explicit angle in deg, rad, turn or grad.
     * The authored value is stored verbatim: negative and beyond 360 angles are
     * legal CSS and are wrapped later by GradientInfo::ResolveCssAngle(). */
    float degrees = 0.0f;
    if (TryParseAngleWithUnit(normalized, degrees)) {
        info->direction = CssGradientDirection::CUSTOM_ANGLE;
        info->angle = degrees;
        return true;
    }
    /* Not a direction: the caller will treat this argument as a color stop. */
    return false;
}

bool NormalizeDirectionKeywordToAngle(const std::string& cssValue, std::string& normalized)
{
    const size_t prefixLength = strlen(LINEAR_GRADIENT_PREFIX);
    size_t prefixPos = 0;
    if (!FindLinearGradientPrefix(cssValue, prefixPos)) {
        return false;
    }
    /* The first argument ends at the first top level comma. Matching the whole
     * token instead of a bare prefix avoids the classic failure where
     * "to right bottom" was clipped to "to right". */
    const size_t argumentStart = prefixPos + prefixLength;
    const size_t argumentEnd = cssValue.find(ARGUMENT_SEPARATOR, argumentStart);
    if (argumentEnd == std::string::npos) {
        return false;
    }
    const std::string firstArgument = cssValue.substr(argumentStart, argumentEnd - argumentStart);
    /* Collapse runs of whitespace so loosely-spaced keywords ("to    right")
     * still resolve to their single-space map keys before the lookup below. */
    const std::string keyword = CollapseWhitespace(Trim(ToLower(firstArgument)));
    CssGradientDirection direction = CssGradientDirection::TO_BOTTOM;
    if (!FindDirection(keyword, direction)) {
        return false;
    }
    /* The eight keyword angles are whole degrees, so "%d" is exact. */
    char angleText[ANGLE_TEXT_BUFFER_SIZE] = {0};
    if (!FormatAngleToken(static_cast<int>(CssGradientDirectionToAngle(direction)), angleText)) {
        return false;
    }
    normalized = cssValue;
    normalized.replace(argumentStart, argumentEnd - argumentStart, angleText);
    return true;
}

bool ExtractLinearGradient(const std::string& declaration, std::string& outGradient)
{
    /* Locate the function on a lowercased copy: ToLower() maps one character
     * onto exactly one character, so the indexes stay valid on the original
     * text and the extracted substring keeps its authored casing. */
    const std::string lowered = ToLower(declaration);
    const size_t start = lowered.find(LINEAR_GRADIENT_PREFIX);
    if (start == std::string::npos) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ExtractLinearGradient: no 'linear-gradient(' in '%s' "
                    "(status=rejected, length=%u)",
                    declaration.c_str(), static_cast<unsigned>(declaration.length()));
        return false;
    }
    /* Walk from the opening parenthesis and stop on the matching closing one so
     * that rgba(...) stops, which embed both commas and parentheses, are kept. */
    const size_t openParen = start + strlen(LINEAR_GRADIENT_PREFIX) - 1;
    int parenDepth = 0;
    for (size_t idx = openParen; idx < declaration.length(); idx++) {
        const char c = declaration[idx];
        if (c == PAREN_OPEN) {
            parenDepth++;
        } else if (c == PAREN_CLOSE) {
            parenDepth--;
            if (parenDepth == 0) {
                outGradient = declaration.substr(start, idx - start + 1);
                HILOG_DEBUG(HILOG_MODULE_ACE, "[GRADIENT] ExtractLinearGradient: ok, offset=%u length=%u",
                            static_cast<unsigned>(start), static_cast<unsigned>(outGradient.length()));
                return true;
            }
        }
    }
    HILOG_ERROR(HILOG_MODULE_ACE,
                "[GRADIENT] ExtractLinearGradient: unbalanced parentheses in '%s' "
                "(status=rejected, offset=%u, depth=%d)",
                declaration.c_str(), static_cast<unsigned>(start), parenDepth);
    return false;
}

bool ParseColorStop(const std::string& stopStr, GradientColorStop& out)
{
    if (stopStr.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: empty color stop string "
                    "(status=rejected, reason=empty-input)");
        return false;
    }
    std::string colorPart;
    std::string offsetPart;
    SplitColorStopParts(stopStr, colorPart, offsetPart);
    /* Parse the color literal first; an unrecognized color disables the stop. */
    if (!ParseColorToColorType(colorPart, out.color)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ParseColorStop: unrecognized color '%s' in stop '%s' "
                    "(status=rejected, reason=invalid-color)",
                    colorPart.c_str(), stopStr.c_str());
        return false;
    }
    /* No percentage authored: defer the position to NormalizeColorStops(). */
    if (offsetPart.empty()) {
        out.offset = GRADIENT_OFFSET_UNSPECIFIED;
        return true;
    }
    /*
     * W3C CSS Images Module Level 3, 3.4.1: a color stop position is allowed to
     * be negative or larger than 100%; keep the authored value verbatim here and
     * let ResolveColorStopOffset() apply the extent clamp, sentinel fix up and
     * out of range preservation.
     */
    ResolveColorStopOffset(stopStr, offsetPart, out.color, out.offset);
    return true;
}

void NormalizeColorStops(GradientStopList& stops)
{
    if (stops.Empty()) {
        return;
    }
    AnchorEndpointOffsets(stops);
    /*
     * W3C CSS Images Module Level 3, 3.4.1 fix-up order: rule 2 (clamp an
     * out-of-order stop up to the largest specified position before it) MUST run
     * before rule 3 (evenly distribute unspecified offsets). Running the
     * distribution first bakes non-monotonic gaps into the unspecified stops,
     * e.g. [a 80%, b(unspec), c 20%, d(unspec), e 90%] then yields d=80%
     * instead of the W3C d=85%.
     */
    ClampOutOfOrderStops(stops);
    DistributeUnspecifiedOffsets(stops);
}

void ClipColorStopsToGradientLine(GradientStopList& stops)
{
    float firstOffset = 0.0f;
    float lastOffset = 0.0f;
    if (!GradientNeedsClipping(stops, firstOffset, lastOffset)) {
        return;
    }

    /*
     * The box only shows the [0, 1] segment of the gradient line, so the ramp
     * is re-anchored on that segment: the colors the standard requires at 0 and
     * at 1 are sampled from the authored ramp, and every stop that falls
     * outside is dropped because its contribution is already carried by those
     * two sampled endpoints.
     *
     * Example - linear-gradient(to right, #ff0000 -20%, #0000ff 150%):
     *   ratio at 0 = (0 + 0.2) / 1.7 = 0.1176 -> #e1001e
     *   ratio at 1 = (1 + 0.2) / 1.7 = 0.7059 -> #4b00b4
     * which is what every browser paints, instead of the plain red-to-blue ramp
     * the previous clamping produced.
     */
    const ColorType startColor = SampleGradientColorAt(stops, GRADIENT_OFFSET_MIN);
    const ColorType endColor = SampleGradientColorAt(stops, GRADIENT_OFFSET_MAX);
    GradientStopList clipped = BuildClippedStops(stops, startColor, endColor);
    /*
     * Every authored stop landed outside the box: the standard result is a flat
     * fill with the nearest stop color. Worth an error trace because such a
     * declaration is almost always a style sheet mistake.
     */
    if (clipped.Size() == GRADIENT_MIN_COLOR_STOP_COUNT && startColor.full == endColor.full) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ClipColorStopsToGradientLine: whole ramp outside the box, "
                    "first=%.4f last=%.4f color=0x%08x (status=degraded-to-solid-color)",
                    firstOffset, lastOffset, static_cast<unsigned>(startColor.full));
    }
    stops = clipped;
}

static bool PushTrimmedArg(GradientArgList& list, const std::string& raw)
{
    const std::string trimmed = Trim(raw);
    if (trimmed.empty()) {
        return true;
    }
    return list.PushBack(trimmed);
}

GradientArgList SplitGradientArgs(const std::string& inner)
{
    GradientArgList result;
    std::string current;
    int parenDepth = 0;
    for (size_t idx = 0; idx < inner.length(); idx++) {
        const char c = inner[idx];
        if (c == PAREN_OPEN) {
            parenDepth++;
            current += c;
            continue;
        }
        if (c == PAREN_CLOSE) {
            /* Never go negative on malformed input such as "red), blue". */
            parenDepth = (parenDepth > 0) ? (parenDepth - 1) : 0;
            current += c;
            continue;
        }
        if (c == ARGUMENT_SEPARATOR && parenDepth == 0) {
            if (!PushTrimmedArg(result, current)) {
                break;
            }
            current.clear();
            continue;
        }
        current += c;
    }
    const std::string finalTrimmed = Trim(current);
    if (!finalTrimmed.empty()) {
        result.PushBack(finalTrimmed);
    }
    return result;
}

std::string Trim(const std::string& str)
{
    const size_t start = str.find_first_not_of(WHITESPACE_CHARS);
    if (start == std::string::npos) {
        return "";
    }
    const size_t end = str.find_last_not_of(WHITESPACE_CHARS);
    return str.substr(start, end - start + 1);
}

/**
 * @brief Collapse every run of one or more whitespace characters into a single
 *        space; leading/trailing whitespace is left for the caller to Trim().
 *
 * CSS treats any amount of whitespace between tokens as one separator, so
 * "to    right" and "to right" are equivalent. The direction map is keyed on
 * single-space literals, therefore this normalisation must run before the
 * lookup, or a valid but loosely-spaced keyword silently fails to match and the
 * whole gradient is dropped. Only the WHITESPACE_CHARS set is treated as space.
 */
std::string CollapseWhitespace(const std::string& str)
{
    std::string result;
    result.reserve(str.size());
    bool inSpace = false;
    for (char c : str) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            result += c;
            inSpace = false;
        }
    }
    return result;
}

std::string ToLower(const std::string& str)
{
    std::string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = static_cast<char>(result[i] + ('a' - 'A'));
        }
    }
    return result;
}
} // namespace CssGradientParser
} // namespace ACELite
} // namespace OHOS
