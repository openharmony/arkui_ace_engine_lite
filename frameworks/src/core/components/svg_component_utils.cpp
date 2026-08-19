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

#include "svg_component_utils.h"
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include "ace_mem_base.h"
#include "app_style_item.h"
#include "js_fwk_common.h"
#include "keys.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
namespace SvgComponentUtils {

const char *CanonicalSvgTagName(const char *name)
{
    if (name == nullptr) {
        return nullptr;
    }
    if (strcmp(name, "lineargradient") == 0) {
        return "linearGradient";
    }
    if (strcmp(name, "radialgradient") == 0) {
        return "radialGradient";
    }
    if (strcmp(name, "solidcolor") == 0) {
        return "solidColor";
    }
    if (strcmp(name, "animatecolor") == 0) {
        return "animateColor";
    }
    if (strcmp(name, "animatetransform") == 0) {
        return "animateTransform";
    }
    if (strcmp(name, "animatemotion") == 0) {
        return "animateMotion";
    }
    if (strcmp(name, "textarea") == 0) {
        return "textArea";
    }
    return nullptr;
}

namespace {

bool HasSvgAttrs(jerry_value_t options, const char *attrsList[], uint32_t count)
{
    if (jerry_value_is_undefined(options) || jerry_value_is_null(options)) {
        return false;
    }
    jerry_value_t attrs = JSObject::Get(options, ATTR_ATTRS);
    if (!jerry_value_is_object(attrs)) {
        JSRelease(attrs);
        return false;
    }
    bool matched = false;
    for (uint32_t i = 0; i < count; ++i) {
        if (JSObject::Has(attrs, attrsList[i])) {
            matched = true;
            break;
        }
    }
    JSRelease(attrs);
    return matched;
}

constexpr uint32_t COLOR_BUF_SIZE = 8;
constexpr uint32_t NUMBER_BUF_SIZE = 32;
constexpr uint32_t RGB_MASK = 0x00FFFFFF;
constexpr uint32_t MAX_SVG_STRING_LEN = 4096;

char *DupString(const char *s)
{
    if (s == nullptr) {
        return nullptr;
    }
    size_t n = strlen(s);
    if (n >= MAX_SVG_STRING_LEN) {
        return nullptr;
    }
    char *d = static_cast<char *>(ace_malloc(n + 1));
    if (d == nullptr) {
        return nullptr;
    }
    if (memcpy_s(d, n + 1, s, n + 1) != EOK) {
        ACE_FREE(d);
        return nullptr;
    }
    return d;
}

char *BuildValue(bool isColor, bool isNumber, double numVal, const char *strVal)
{
    if (isNumber) {
        if (isColor) {
            uint32_t c = static_cast<uint32_t>(numVal) & RGB_MASK;
            char *b = static_cast<char *>(ace_malloc(COLOR_BUF_SIZE));
            if (b != nullptr) {
                snprintf_s(b, COLOR_BUF_SIZE, COLOR_BUF_SIZE, "#%06X", c);
            }
            return b;
        }
        char *b = static_cast<char *>(ace_malloc(NUMBER_BUF_SIZE));
        if (b != nullptr) {
            if (numVal == static_cast<double>(static_cast<int64_t>(numVal))) {
                int64_t intVal = static_cast<int64_t>(numVal);
                snprintf_s(b, NUMBER_BUF_SIZE, NUMBER_BUF_SIZE, "%" PRId64, intVal);
            } else {
                snprintf_s(b, NUMBER_BUF_SIZE, NUMBER_BUF_SIZE, "%g", numVal);
            }
        }
        return b;
    }
    if (strVal != nullptr) {
        return DupString(strVal);
    }
    return nullptr;
}

} // namespace

bool IsSvgTextElement(uint16_t componentNameId, jerry_value_t options)
{
    if (componentNameId != K_TEXT) {
        return false;
    }
    const char *svgTextAttrs[] = {
        "x", "y", "dx", "dy", "fill", "stroke",
        "text-anchor", "fill-opacity", "stroke-opacity", "stroke-width"
    };
    uint32_t count = sizeof(svgTextAttrs) / sizeof(svgTextAttrs[0]);
    return HasSvgAttrs(options, svgTextAttrs, count);
}

bool IsSvgImageElement(uint16_t componentNameId, jerry_value_t options)
{
    if (componentNameId != K_IMAGE) {
        return false;
    }
    const char *svgImageAttrs[] = {
        "href", "xlink:href", "x", "y",
        "preserveAspectRatio", "transform"
    };
    uint32_t count = sizeof(svgImageAttrs) / sizeof(svgImageAttrs[0]);
    return HasSvgAttrs(options, svgImageAttrs, count);
}

bool IsColorAttr(const char *name)
{
    if (name == nullptr) {
        return false;
    }
    return (strcmp(name, "fill") == 0) || (strcmp(name, "stroke") == 0) ||
           (strcmp(name, "stop-color") == 0) || (strcmp(name, "solid-color") == 0) ||
           (strcmp(name, "color") == 0) || (strcmp(name, "flood-color") == 0);
}

namespace {

struct NameMapping {
    const char *lower;
    const char *canonical;
};

const NameMapping SVG_NAME_MAP[] = {
    { "attributename", "attributeName" },
    { "basefrequency", "baseFrequency" },
    { "calcmode", "calcMode" },
    { "clippath", "clipPath" },
    { "clippathunits", "clipPathUnits" },
    { "filterunits", "filterUnits" },
    { "glyphref", "glyphRef" },
    { "gradienttransform", "gradientTransform" },
    { "gradientunits", "gradientUnits" },
    { "keypoints", "keyPoints" },
    { "keysplines", "keySplines" },
    { "keytimes", "keyTimes" },
    { "lengthadjust", "lengthAdjust" },
    { "maskcontentunits", "maskContentUnits" },
    { "maskunits", "maskUnits" },
    { "patterntransform", "patternTransform" },
    { "patternunits", "patternUnits" },
    { "preserveaspectratio", "preserveAspectRatio" },
    { "primitiveunits", "primitiveUnits" },
    { "repeatcount", "repeatCount" },
    { "spreadmethod", "spreadMethod" },
    { "startoffset", "startOffset" },
    { "stddeviation", "stdDeviation" },
    { "textlength", "textLength" },
    { "viewbox", "viewBox" },
};
constexpr uint32_t SVG_NAME_MAP_SIZE = sizeof(SVG_NAME_MAP) / sizeof(SVG_NAME_MAP[0]);

const char *MatchSpecialName(const char *jsName)
{
    if (jsName == nullptr) {
        return nullptr;
    }
    for (uint32_t i = 0; i < SVG_NAME_MAP_SIZE; ++i) {
        if (strcmp(jsName, SVG_NAME_MAP[i].lower) == 0 ||
            strcmp(jsName, SVG_NAME_MAP[i].canonical) == 0) {
            return SVG_NAME_MAP[i].canonical;
        }
    }
    return nullptr;
}

} // namespace

char *CanonicalName(const char *jsName)
{
    if (jsName == nullptr) {
        return nullptr;
    }
    const char *special = MatchSpecialName(jsName);
    if (special != nullptr) {
        return DupString(special);
    }
    size_t len = strlen(jsName);
    if (len >= MAX_SVG_STRING_LEN) {
        return nullptr;
    }
    uint32_t n = static_cast<uint32_t>(len);
    char *out = static_cast<char *>(ace_malloc(2 * n + 1));
    if (out == nullptr) {
        return nullptr;
    }
    uint32_t j = 0;
    for (uint32_t i = 0; i < n; ++i) {
        char c = jsName[i];
        if (c >= 'A' && c <= 'Z') {
            out[j++] = '-';
            out[j++] = static_cast<char>(c - 'A' + 'a');
        } else {
            out[j++] = c;
        }
    }
    out[j] = '\0';
    return out;
}

char *ValueFromJerry(jerry_value_t v, bool isColor)
{
    if (jerry_value_is_number(v)) {
        return BuildValue(isColor, true, jerry_get_number_value(v), nullptr);
    }
    if (jerry_value_is_string(v)) {
        // NOTE: jerry_get_utf8_string_length returns the CHARACTER count, but
        // jerry_string_to_utf8_char_buffer requires the buffer sized by the UTF-8 BYTE count
        // (jerry_get_utf8_string_size). Sizing by character count under-allocates multi-byte
        // (e.g. CJK) strings, so the copy returns 0 and the value is silently dropped (SVG
        // <text> with Chinese content previously showed empty). Use byte count for sizing.
        jerry_size_t size = jerry_get_utf8_string_size(v);
        char *b = static_cast<char *>(ace_malloc(size + 1));
        if (b == nullptr) {
            return nullptr;
        }
        jerry_size_t written = jerry_string_to_utf8_char_buffer(v, reinterpret_cast<jerry_char_t *>(b), size);
        if (written == 0 || written > size) {
            ACE_FREE(b);
            return nullptr;
        }
        b[written] = '\0';
        return b;
    }
    if (jerry_value_is_boolean(v)) {
        return DupString(jerry_get_boolean_value(v) ? "true" : "false");
    }
    return nullptr;
}

char *ResolveSvgValue(jerry_value_t v, bool isColor, jerry_value_t viewModel)
{
    if (!jerry_value_is_function(v)) {
        return ValueFromJerry(v, isColor);
    }
    jerry_value_t result = JSFunction::Call(v, viewModel, nullptr, 0);
    if (jerry_value_is_error(result)) {
        jerry_release_value(result);
        return nullptr;
    }
    char *str = ValueFromJerry(result, isColor);
    jerry_release_value(result);
    return str;
}

char *ValueFromStyle(const AppStyleItem *s, bool isColor)
{
    if (s == nullptr) {
        return nullptr;
    }
    if (s->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER) {
        return BuildValue(isColor, true, s->GetNumValue(), nullptr);
    }
    if (s->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING) {
        return BuildValue(isColor, false, 0, s->GetStrValue());
    }
    return nullptr;
}

const char *KeyIdToSvgName(uint16_t keyId)
{
    switch (keyId) {
        case K_FILL:
            return "fill";
        case K_STROKE:
            return "stroke";
        case K_STROKE_WIDTH:
            return "stroke-width";
        case K_STROKE_LINECAP:
            return "stroke-linecap";
        case K_STROKE_LINEJOIN:
            return "stroke-linejoin";
        case K_STROKE_MITERLIMIT:
            return "stroke-miterlimit";
        case K_STROKE_DASHARRAY:
            return "stroke-dasharray";
        case K_STROKE_DASHOFFSET:
            return "stroke-dashoffset";
        case K_FILL_OPACITY:
            return "fill-opacity";
        case K_STROKE_OPACITY:
            return "stroke-opacity";
        case K_VISIBILITY:
            return "visibility";
        case K_TEXT_ANCHOR:
            return "text-anchor";
        case K_FONT_SIZE:
            return "font-size";
        case K_OPACITY:
            return "opacity";
        case K_WIDTH:
            return "width";
        case K_HEIGHT:
            return "height";
        case K_ID:
            return "id";
        default:
            return nullptr;
    }
}

} // namespace SvgComponentUtils
} // namespace ACELite
} // namespace OHOS
