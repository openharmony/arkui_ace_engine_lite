/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "acelite_config.h"

#if (FEATURE_COMPONENT_CANVAS == 1)
#include "canvas_component.h"
#include "js_fwk_common.h"
#include "product_adapter.h"
#include "securec.h"
#include "jerryscript.h"
#include "modules/presets/image_module.h"
#include "image_component.h"

namespace OHOS {
namespace ACELite {
// default fill style color=black
const char * const CanvasComponent::DEFAULT_FILLSTYLE = "#000000";
// default stroke style color=black
const char * const CanvasComponent::DEFAULT_STROKESTYLE = "#000000";
// default text align=left
const char * const CanvasComponent::DEFAULT_TEXTALIGN = "left";
// default line lineCap
#if GRAPHIC_ENABLE_LINECAP_FLAG
const char * const CanvasComponent::DEFAULT_LINECAP = "butt";
#endif
// default line lineJoin
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
const char * const CanvasComponent::DEFAULT_LINEJOIN = "miter";
// default line lineJoin
const int16_t CanvasComponent::DEFAULT_MITERLIMIT = 10;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
// default lineDashOffset
const int16_t CanvasComponent::DEFAULT_LINEDASHOFFSET = 0;
#endif
// API-attribute
const char * const CanvasComponent::ATTR_FILLSTYLE = "fillStyle";
const char * const CanvasComponent::ATTR_STROKESTYLE = "strokeStyle";
const char * const CanvasComponent::ATTR_LINEWIDTH = "lineWidth";
const char * const CanvasComponent::ATTR_FONT = "font";
const char * const CanvasComponent::ATTR_TEXTALIGN = "textAlign";
#if GRAPHIC_ENABLE_LINECAP_FLAG
const char * const CanvasComponent::ATTR_LINECAP = "lineCap";
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
const char * const CanvasComponent::ATTR_LINEJOIN = "lineJoin";
const char * const CanvasComponent::ATTR_MITERLIMIT = "miterLimit";
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
const char * const CanvasComponent::ATTR_LINEDASHOFFSET = "lineDashOffset";
#endif
const char * const CanvasComponent::ATTR_GLOBALALPHA = "globalAlpha";
const char * const CanvasComponent::ATTR_GLOBALCOMPOSITEOPERATION = "globalCompositeOperation";
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
const char * const CanvasComponent::ATTR_SHADOWOFFSETX = "shadowOffsetX";
const char * const CanvasComponent::ATTR_SHADOWOFFSETY = "shadowOffsetY";
const char * const CanvasComponent::ATTR_SHADOWCOLOR = "shadowColor";
const char * const CanvasComponent::ATTR_SHADOWBLUR = "shadowBlur";
#endif
// API-method
const char * const CanvasComponent::FUNC_GETCONTEXT = "getContext";
const char * const CanvasComponent::FUNC_FILLRECT = "fillRect";
const char * const CanvasComponent::FUNC_STROKERECT = "strokeRect";
const char * const CanvasComponent::FUNC_FILLTEXT = "fillText";
const char * const CanvasComponent::FUNC_BEGINPATH = "beginPath";
const char * const CanvasComponent::FUNC_MOVETO = "moveTo";
const char * const CanvasComponent::FUNC_LINETO = "lineTo";
const char * const CanvasComponent::FUNC_RECT = "rect";
const char * const CanvasComponent::FUNC_CLEANRECT = "cleanRect";
const char * const CanvasComponent::FUNC_ARC = "arc";
const char * const CanvasComponent::FUNC_CLOSEPATH = "closePath";
const char * const CanvasComponent::FUNC_STROKE = "stroke";
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
const char * const CanvasComponent::FUNC_CREATELINEARGRADIENT = "createLinearGradient";
const char * const CanvasComponent::FUNC_CREATERADIALGRADIENT = "createRadialGradient";
const char * const CanvasComponent::FUNC_ADDCOLORSTOP = "addColorStop";
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
const char * const CanvasComponent::FUNC_CREATEPATTERN = "createPattern";
#endif
const char * const CanvasComponent::FUNC_FILL = "fill";
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
const char * const CanvasComponent::FUNC_DRAWIMAGE = "drawImage";
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
const char * const CanvasComponent::FUNC_SETLINEDASH = "setLineDash";
const char * const CanvasComponent::FUNC_GETLINEDASH = "getLineDash";
#endif
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
const char * const CanvasComponent::FUNC_STROKETEXT = "strokeText";
#endif
const char * const CanvasComponent::FUNC_ROTATE = "rotate";
const char * const CanvasComponent::FUNC_SCALE = "scale";
const char * const CanvasComponent::FUNC_MEASURETEXT = "measureText";
const char * const CanvasComponent::FUNC_TRANSLATE = "translate";
const char * const CanvasComponent::FUNC_TRANFORM = "transform";
const char * const CanvasComponent::FUNC_SETTRANFORM = "setTransform";
const char * const CanvasComponent::FUNC_SAVE = "save";
const char * const CanvasComponent::FUNC_RESTORE = "restore";
const char * const CanvasComponent::FUNC_DRAWCIRCLE = "drawCircle";
// create canvas draw attribute-callback mapping
const AttrMap CanvasComponent::attrMap_[] = {
    {ATTR_FILLSTYLE, FillStyleSetter, FillStyleGetter},
    {ATTR_STROKESTYLE, StrokeStyleSetter, StrokeStyleGetter},
    {ATTR_LINEWIDTH, LineWidthSetter, LineWidthGetter},
    {ATTR_FONT, FontSetter, FontGetter},
    {ATTR_TEXTALIGN, TextAlignSetter, TextAlignGetter},
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    {ATTR_SHADOWOFFSETX, ShadowOffsetXSetter, ShadowOffsetXGetter},
    {ATTR_SHADOWOFFSETY, ShadowOffsetYSetter, ShadowOffsetYGetter},
    {ATTR_SHADOWCOLOR, ShadowColorSetter, ShadowColorGetter},
    {ATTR_SHADOWBLUR, ShadowBlurSetter, ShadowBlurGetter},
#endif
#if GRAPHIC_ENABLE_LINECAP_FLAG
    {ATTR_LINECAP, LineCapSetter, LineCapGetter},
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    {ATTR_LINEJOIN, LineJoinSetter, LineJoinGetter},
    {ATTR_MITERLIMIT, MiterLimitSetter, MiterLimitGetter},
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    {ATTR_LINEDASHOFFSET, LineDashOffsetSetter, LineDashOffsetGetter},
#endif
    {ATTR_GLOBALALPHA, GlobalAlphaSetter, GlobalAlphaGetter},
    {ATTR_GLOBALCOMPOSITEOPERATION, GlobalCompositeOperationSetter, GlobalCompositeOperationGetter}
};

// create canvas draw method-callback mapping
const MethodMap CanvasComponent::methodMap_[] = {
    {FUNC_GETCONTEXT, GetContext},
    {FUNC_FILLRECT, FillRect},
    {FUNC_STROKERECT, StrokeRect},
    {FUNC_FILLTEXT, FillText},
    {FUNC_BEGINPATH, BeginPath},
    {FUNC_MOVETO, MoveTo},
    {FUNC_LINETO, LineTo},
    {FUNC_RECT, Rect},
    {FUNC_CLEANRECT, CleanRect},
    {FUNC_ARC, Arc},
    {FUNC_CLOSEPATH, ClosePath},
    {FUNC_STROKE, Stroke},
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    {FUNC_CREATELINEARGRADIENT, CreateLInearGradient},
    {FUNC_CREATERADIALGRADIENT, CreateRadialGradient},
    {FUNC_ADDCOLORSTOP, AddColorStop},
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    {FUNC_CREATEPATTERN, CreatePattern},
#endif
    {FUNC_FILL, Fill},
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
    {FUNC_DRAWIMAGE, DrawImage},
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    {FUNC_SETLINEDASH, SetLineDash},
    {FUNC_GETLINEDASH, GetLineDash},
#endif
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    {FUNC_STROKETEXT, StrokeText},
#endif
    {FUNC_ROTATE, Rotate},
    {FUNC_SCALE, Scale},
    {FUNC_MEASURETEXT, MeasureText},
    {FUNC_TRANSLATE, Translate},
    {FUNC_TRANFORM, Transform},
    {FUNC_SETTRANFORM, SetTransform},
    {FUNC_SAVE, Save},
    {FUNC_RESTORE, Restore},
    {FUNC_DRAWCIRCLE, DrawCircle}
};

CanvasComponent::CanvasComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      dom_(UNDEFINED),
      dashArray_(UNDEFINED),
      measureTextObject_(UNDEFINED),
      measureTextWidthString_(UNDEFINED),
      fillStyleValue_(nullptr),
      strokeStyleValue_(nullptr),
      fontValue_(nullptr),
      textAlignValue_(nullptr),
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
      shadowOffsetXValue_(0),
      shadowOffsetYValue_(0),
      shadowBlurValue_(0),
      shadowColorValue_(nullptr),
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
      miterLimitValue_(DEFAULT_MITERLIMIT),
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
      lineDashOffsetValue_(0),
#endif
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
      colorStopValue_(nullptr),
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
      patternPathValue_(nullptr),
      patternRepeatTypeValue_(nullptr),
#endif
      lineWidthValue_(1),
      lineCapValue_(nullptr),
      lineJoinValue_(nullptr)
{
    SetComponentName(K_CANVAS);
    // set default paint pattern
    paint_.SetFillColor(Color::Black());
    paint_.SetOpacity(OPA_OPAQUE);
    paint_.SetStrokeColor(Color::Black());
    paint_.SetStrokeWidth(1);

    // set default text font pattern
    fontStyle_.align = TEXT_ALIGNMENT_LEFT;
    fontStyle_.direct = TEXT_DIRECT_LTR;
    fontStyle_.fontSize = ProductAdapter::GetDefaultFontSize();
    char *defaultFontName = nullptr;
    CopyFontFamily(defaultFontName, ProductAdapter::GetDefaultFontFamilyName());
    fontStyle_.fontName = defaultFontName;
    fontStyle_.letterSpace = DEFAULT_FONT_LETTERSPACE;
    RegisterNamedFunction(methodMap_[0].methodName, methodMap_[0].callbackName);
}

// Create UICanvas
bool CanvasComponent::CreateNativeViews()
{
    return true;
}

void CanvasComponent::ReleaseNativeViews()
{
    ACE_FREE(fillStyleValue_);
    ACE_FREE(strokeStyleValue_);
    ACE_FREE(fontValue_);
    ACE_FREE(textAlignValue_);
    ACE_FREE(lineCapValue_);
    ACE_FREE(lineJoinValue_);
    // free fontStyle_.fontName memory which malloc in FontSetter method.
    if (fontStyle_.fontName != nullptr) {
        ace_free(const_cast<char *>(fontStyle_.fontName));
        fontStyle_.fontName = nullptr;
    }
    if (!IS_UNDEFINED(dom_)) {
        bool deleted = jerry_delete_object_native_pointer(dom_, nullptr);
        if (!deleted) {
            HILOG_WARN(HILOG_MODULE_ACE, "canvas_component: delete object native pointer dom_ failed!");
        }
        jerry_release_value(dom_);
    }
    if (!IS_UNDEFINED(dashArray_)) {
        uint32_t length = jerry_get_array_length(dashArray_);
        for (uint32_t i = 0; i < length; i++) {
            jerry_value_t val = jerry_get_property_by_index(dashArray_, i);
            jerry_release_value(val);
        }
        jerry_release_value(dashArray_);
    }
    if (!IS_UNDEFINED(jerry_get_property(measureTextObject_, measureTextWidthString_))) {
        jerry_release_value(jerry_get_property(measureTextObject_, measureTextWidthString_));
    }
    if (!IS_UNDEFINED(measureTextWidthString_)) {
        jerry_release_value(measureTextWidthString_);
    }
    if (!IS_UNDEFINED(measureTextObject_)) {
        jerry_release_value(measureTextObject_);
    }
}

UIView *CanvasComponent::GetComponentRootView() const
{
    return (const_cast<UICanvas *>(&canvas_));
}

jerry_value_t CanvasComponent::GetContext(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of getContext method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("getdom method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (IS_UNDEFINED(component->dom_)) {
        component->dom_ = jerry_create_object();
        jerry_set_object_native_pointer(component->dom_, component, nullptr);

        // register fillStyle, strokeStyle, lineWidth, font, textAlign attribute
        uint16_t attrMapLength = sizeof(attrMap_) / sizeof(attrMap_[0]);
        for (uint16_t index = 0; index < attrMapLength; index++) {
            RegisterAttributeFunc(component->dom_, attrMap_[index].attrName, attrMap_[index].setterName,
                                  attrMap_[index].getterName);
        }

        // register fillRect, strokeRect, fillText, beginPath, moveTo, lineTo, rect, arc, closePath, stroke method
        uint16_t methodMapLength = sizeof(methodMap_) / sizeof(methodMap_[0]);
        for (uint16_t index = 1; index < methodMapLength; index++) {
            RegisterDrawMethodFunc(component->dom_, methodMap_[index].methodName, methodMap_[index].callbackName);
        }
    }

    // BeginPath function need to be called after getting canvas API object
    component->canvas_.BeginPath();

    jerry_acquire_value(component->dom_);
    return component->dom_;
}

jerry_value_t CanvasComponent::FillStyleSetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: fillStyle value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("fillStyle value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->fillStyleValue_);
    uint16_t fillStyleLength = 0;
    component->fillStyleValue_ = MallocStringOf(args[ArgsIndex::IDX_0], &fillStyleLength);
    if (component->fillStyleValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: fillStyle value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("fillStyle value erro!"));
    }

    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    uint16_t fillStyleId = KeyParser::ParseKeyId(component->fillStyleValue_, fillStyleLength);
    if (fillStyleId == K_CANVASPATTERN) {
        component->paint_.SetStyle(Paint::PaintStyle::PATTERN);
        return UNDEFINED;
    }
    if (fillStyleId == K_CANVASGRADIENT) {
        component->paint_.SetStyle(Paint::PaintStyle::GRADIENT);
    }
    if (ParseColor(component->fillStyleValue_, color, alpha)) {
        component->paint_.SetFillColor(component->GetRGBColor(color));
        component->paint_.SetOpacity(alpha);
    }
    return UNDEFINED;
}

jerry_value_t CanvasComponent::FillStyleGetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (component->fillStyleValue_ == nullptr) {
        CopyFontFamily(component->fillStyleValue_, DEFAULT_FILLSTYLE);
    }

    if (component->fillStyleValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->fillStyleValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("fillStyle value error"));
}

jerry_value_t CanvasComponent::StrokeStyleSetter(const jerry_value_t func,
                                                 const jerry_value_t dom,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: strokeStyle value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("the value of strokeStyle is null"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->strokeStyleValue_);
    uint16_t strokeStyleLength = 0;
    component->strokeStyleValue_ = MallocStringOf(args[ArgsIndex::IDX_0], &strokeStyleLength);
    if (component->strokeStyleValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: strokeStyle value error");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("strokeStyle value error"));
    }

    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    uint16_t strokeStyleId = KeyParser::ParseKeyId(component->strokeStyleValue_, strokeStyleLength);
    if (strokeStyleId == K_CANVASPATTERN) {
        component->paint_.SetStyle(Paint::PaintStyle::PATTERN);
        return UNDEFINED;
    }
    if (strokeStyleId == K_CANVASGRADIENT) {
        component->paint_.SetStyle(Paint::PaintStyle::GRADIENT);
        return UNDEFINED;
    }
    if (ParseColor(component->strokeStyleValue_, color, alpha)) {
        component->paint_.SetStrokeColor(component->GetRGBColor(color));
        component->paint_.SetOpacity(alpha);
    }
    return UNDEFINED;
}

jerry_value_t CanvasComponent::StrokeStyleGetter(const jerry_value_t func,
                                                 const jerry_value_t dom,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (component->strokeStyleValue_ == nullptr) {
        CopyFontFamily(component->strokeStyleValue_, DEFAULT_STROKESTYLE);
    }

    if (component->strokeStyleValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->strokeStyleValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("strokeStyle value error"));
}

jerry_value_t CanvasComponent::LineWidthSetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineWidth value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineWidth value error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    component->lineWidthValue_ = IntegerOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetStrokeWidth(component->lineWidthValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::LineWidthGetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->lineWidthValue_);
}

jerry_value_t CanvasComponent::FontSetter(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: font value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("font value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->fontValue_);

    component->fontValue_ = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (component->fontValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: text font value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("text font value error"));
    }

    const int8_t maxParamCount = 2;
    for (int8_t index = 0; index < maxParamCount; index++) {
        // get font size or font family from fontValue.
        char *mallocSubFont = nullptr;
        component->GetSubFont(component->fontValue_, index, mallocSubFont);
        if (mallocSubFont == nullptr) {
            HILOG_WARN(HILOG_MODULE_ACE, "canvas_component: get text font size or font family failed");
            ace_free(component->fontValue_);
            component->fontValue_ = nullptr;
            return UNDEFINED;
        }

        // if it is a font size
        if (*mallocSubFont >= '0' && *mallocSubFont <= '9') {
            component->fontStyle_.fontSize = strtol(mallocSubFont, nullptr, DEC);
            ace_free(mallocSubFont);
            mallocSubFont = nullptr;
        } else { // if it is a font family
            // free default component->fontStyle_.fontName memory.
            if (component->fontStyle_.fontName != nullptr) {
                ace_free(const_cast<char *>(component->fontStyle_.fontName));
                component->fontStyle_.fontName = nullptr;
            }
            // set customized font name, the memory of mallocFontFamily need be free in CanvasFillText method.
            component->fontStyle_.fontName = mallocSubFont;
        }
    }
    return UNDEFINED;
}

jerry_value_t CanvasComponent::FontGetter(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (component->fontValue_ == nullptr) {
        uint8_t defaultFontSize = ProductAdapter::GetDefaultFontSize();
        const char *defaultFontName = ProductAdapter::GetDefaultFontFamilyName();
        size_t nameLength = (defaultFontName != nullptr) ? strlen(defaultFontName) : 0;
        const uint8_t maxNameLen = 32;
        const uint8_t maxFontSizeLen = 16;
        const uint8_t maxBufferLen = maxNameLen + maxFontSizeLen;
        if (nameLength > 0 && (nameLength < maxNameLen) && (defaultFontSize < UINT8_MAX)) {
            char defaultFont[maxBufferLen] = {0};
            int length = sprintf_s(defaultFont, (maxBufferLen - 1), "%dpx %s", defaultFontSize, defaultFontName);
            if (length > 0 && length < maxBufferLen) {
                CopyFontFamily(component->fontValue_, defaultFont, length);
            }
        }
    }

    if (component->fontValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->fontValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("text font value error"));
}

jerry_value_t CanvasComponent::TextAlignSetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: textAlign value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("textAlign value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->textAlignValue_);

    uint16_t textAlignLength = 0;
    component->textAlignValue_ = MallocStringOf(args[ArgsIndex::IDX_0], &textAlignLength);
    if (component->textAlignValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: text align value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("text align value error"));
    }

    uint16_t textAlignId = KeyParser::ParseKeyId(component->textAlignValue_, textAlignLength);
    if (textAlignId == K_LEFT) {
        component->fontStyle_.align = TEXT_ALIGNMENT_LEFT;
    } else if (textAlignId == K_RIGHT) {
        component->fontStyle_.align = TEXT_ALIGNMENT_RIGHT;
    } else if (textAlignId == K_CENTER) {
        component->fontStyle_.align = TEXT_ALIGNMENT_CENTER;
    } else {
        // do nothing(use default align=TEXT_ALIGNMENT_LEFT)
    }
    return UNDEFINED;
}

jerry_value_t CanvasComponent::TextAlignGetter(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (component->textAlignValue_ == nullptr) {
        CopyFontFamily(component->textAlignValue_, DEFAULT_TEXTALIGN);
    }

    if (component->textAlignValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->textAlignValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("textAlign value error"));
}

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
jerry_value_t CanvasComponent::ShadowOffsetXSetter(const jerry_value_t func,
                                                   const jerry_value_t context,
                                                   const jerry_value_t *args,
                                                   const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: shadowOffsetX value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("shadowOffsetX value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->shadowOffsetXValue_ = IntegerOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetShadowOffsetX(component->shadowOffsetXValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ShadowOffsetXGetter(const jerry_value_t func,
                                                   const jerry_value_t dom,
                                                   const jerry_value_t args[],
                                                   const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->shadowOffsetXValue_);
}

jerry_value_t CanvasComponent::ShadowOffsetYSetter(const jerry_value_t func,
                                                   const jerry_value_t context,
                                                   const jerry_value_t *args,
                                                   const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: shadowOffsetY value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("shadowOffsetY value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->shadowOffsetYValue_ = IntegerOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetShadowOffsetY(component->shadowOffsetYValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ShadowOffsetYGetter(const jerry_value_t func,
                                                   const jerry_value_t dom,
                                                   const jerry_value_t args[],
                                                   const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->shadowOffsetYValue_);
}

jerry_value_t CanvasComponent::ShadowBlurSetter(const jerry_value_t func,
                                                const jerry_value_t context,
                                                const jerry_value_t *args,
                                                const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: shadowBlur value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("shadowBlur value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->shadowBlurValue_ = IntegerOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetShadowBlur(component->shadowBlurValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ShadowBlurGetter(const jerry_value_t func,
                                                const jerry_value_t dom,
                                                const jerry_value_t args[],
                                                const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->shadowBlurValue_);
}

jerry_value_t CanvasComponent::ShadowColorSetter(const jerry_value_t func,
                                                 const jerry_value_t context,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: shadowColor value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("shadowColor value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->shadowColorValue_);

    component->shadowColorValue_ = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (component->shadowColorValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: shadowColor value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("shadowColor value erro!"));
    }

    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    ParseColor(component->shadowColorValue_, color, alpha);
    ColorType shadowColor = component->GetRGBColor(color);
    shadowColor.alpha = alpha;
    component->paint_.SetShadowColor(shadowColor);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ShadowColorGetter(const jerry_value_t func,
                                                 const jerry_value_t dom,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    if (component->shadowColorValue_ == nullptr) {
        CopyFontFamily(component->shadowColorValue_, DEFAULT_FILLSTYLE);
    }

    if (component->shadowColorValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->shadowColorValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("shadowColor value error"));
}
#endif

#if GRAPHIC_ENABLE_LINECAP_FLAG
jerry_value_t CanvasComponent::LineCapSetter(const jerry_value_t func,
                                             const jerry_value_t contex,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineCap value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("lineCap value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->lineCapValue_);

    uint16_t length = 0;
    component->lineCapValue_ = MallocStringOf(args[ArgsIndex::IDX_0], &length);
    if (component->lineCapValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineCap value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineCap value error"));
    }

    int8_t lineCap = ParseLineCap(component->lineCapValue_);
    component->paint_.SetLineCap((LineCap)lineCap);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::LineCapGetter(const jerry_value_t func,
                                             const jerry_value_t contex,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    if (component->lineCapValue_ == nullptr) {
        CopyFontFamily(component->lineCapValue_, DEFAULT_LINECAP);
    }
    if (component->lineCapValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->lineCapValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineCap value error"));
}
#endif

#if GRAPHIC_ENABLE_LINEJOIN_FLAG
jerry_value_t CanvasComponent::LineJoinSetter(const jerry_value_t func,
                                              const jerry_value_t contex,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineCap value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("lineCap value error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->lineJoinValue_);

    uint16_t length = 0;
    component->lineJoinValue_ = MallocStringOf(args[ArgsIndex::IDX_0], &length);
    if (component->lineJoinValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineJoin value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineJoin value error"));
    }

    uint8_t lineJoin = ParseLineJoin(component->lineJoinValue_);
    component->paint_.SetLineJoin((LineJoin)lineJoin);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::LineJoinGetter(const jerry_value_t func,
                                              const jerry_value_t contex,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    if (component->lineJoinValue_ == nullptr) {
        CopyFontFamily(component->lineJoinValue_, DEFAULT_LINEJOIN);
    }
    if (component->lineJoinValue_ != nullptr) {
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(component->lineJoinValue_));
    }
    return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineJoin value error"));
}

jerry_value_t CanvasComponent::MiterLimitSetter(const jerry_value_t func,
                                                const jerry_value_t contex,
                                                const jerry_value_t args[],
                                                const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: miterLimit value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineWidth value error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->miterLimitValue_ = FloatOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetMiterLimit(component->miterLimitValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::MiterLimitGetter(const jerry_value_t func,
                                                const jerry_value_t contex,
                                                const jerry_value_t args[],
                                                const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->miterLimitValue_);
}
#endif

#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
jerry_value_t CanvasComponent::LineDashOffsetSetter(const jerry_value_t func,
                                                    const jerry_value_t contex,
                                                    const jerry_value_t args[],
                                                    const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineWidth value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineWidth value error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->lineDashOffsetValue_ = FloatOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetLineDashOffset(component->lineDashOffsetValue_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::LineDashOffsetGetter(const jerry_value_t func,
                                                    const jerry_value_t contex,
                                                    const jerry_value_t args[],
                                                    const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(contex));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->lineDashOffsetValue_);
}
#endif

jerry_value_t CanvasComponent::GlobalAlphaSetter(const jerry_value_t func,
                                                 const jerry_value_t context,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineWidth value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineWidth value error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float globalAlpha = FloatOf(args[ArgsIndex::IDX_0]);
    component->paint_.SetGlobalAlpha(globalAlpha);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::GlobalAlphaGetter(const jerry_value_t func,
                                                 const jerry_value_t dom,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    return jerry_create_number(component->paint_.GetGlobalAlpha());
}

jerry_value_t CanvasComponent::GlobalCompositeOperationSetter(const jerry_value_t func,
                                                              const jerry_value_t context,
                                                              const jerry_value_t args[],
                                                              const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: lineWidth value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("lineWidth value error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    char *globalCompositeOperation = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (globalCompositeOperation == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get text value failed");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("get text value failed"));
    }

    if (strcmp(globalCompositeOperation, "source-over") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::SOURCE_OVER);
    } else if (strcmp(globalCompositeOperation, "source-atop") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::SOURCE_ATOP);
    } else if (strcmp(globalCompositeOperation, "source-in") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::SOURCE_IN);
    } else if (strcmp(globalCompositeOperation, "source-out") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::SOURCE_OUT);
    } else if (strcmp(globalCompositeOperation, "destination-over") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::DESTINATION_OVER);
    } else if (strcmp(globalCompositeOperation, "destination-atop") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::DESTINATION_ATOP);
    } else if (strcmp(globalCompositeOperation, "destination-in") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::DESTINATION_IN);
    } else if (strcmp(globalCompositeOperation, "destination-out") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::DESTINATION_OUT);
    } else if (strcmp(globalCompositeOperation, "lighter") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::LIGHTER);
    } else if (strcmp(globalCompositeOperation, "copy") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::COPY);
    } else if (strcmp(globalCompositeOperation, "xor") == 0) {
        component->paint_.SetGlobalCompositeOperation(OHOS::XOR);
    }

    ACE_FREE(globalCompositeOperation);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::GlobalCompositeOperationGetter(const jerry_value_t func,
                                                              const jerry_value_t dom,
                                                              const jerry_value_t args[],
                                                              const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int8_t globalCompositeOperation = component->paint_.GetGlobalCompositeOperation();
    return jerry_create_number(globalCompositeOperation);
}

jerry_value_t CanvasComponent::FillRect(const jerry_value_t func,
                                        const jerry_value_t dom,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_4) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of fillRect method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("fillRect method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t startX = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t startY = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t width = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t height = IntegerOf(args[ArgsIndex::IDX_3]);

    // FILL_STYLE: fill rect support fill shapses only
    component->paint_.SetStyle(component->paint_.FILL_STYLE);

    Point startPoint;
    startPoint.x = startX;
    startPoint.y = startY;
    component->canvas_.DrawRect(startPoint, height, width, component->paint_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::StrokeRect(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_4) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of strokeRect method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("strokeRect method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

#if defined(ENABLE_CANVAS_EXTEND) && ENABLE_CANVAS_EXTEND
    int16_t startX = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t startY = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t width = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t height = IntegerOf(args[ArgsIndex::IDX_3]);

    // STROKE_STYLE: stroke rect support draw borders only
    component->paint_.SetStyle(component->paint_.STROKE_STYLE);

    Point startPoint;
    startPoint.x = startX;
    startPoint.y = startY;
    component->canvas_.StrokeRect(startPoint, height, width, component->paint_);
#endif
    return UNDEFINED;
}

jerry_value_t CanvasComponent::FillText(const jerry_value_t func,
                                        const jerry_value_t dom,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_3) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of fillText method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("fillText method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    char *textValue = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (textValue == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get text value failed");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("get text value failed"));
    }

    int16_t startX = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t startY = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t maxWidth = component->canvas_.GetWidth();

    // FILL_STYLE: fillText support fill shapses only
    component->paint_.SetStyle(component->paint_.FILL_STYLE);

    Point startPoint;
    const int8_t half = 2;
    startPoint.x = startX;
    startPoint.y = startY;

    // if textAlign=left, not need transfer startX, if textAlign=right or center, need transfer startX
    if (component->fontStyle_.align == TEXT_ALIGNMENT_RIGHT) {
        startPoint.x = startX - maxWidth;
    } else if (component->fontStyle_.align == TEXT_ALIGNMENT_CENTER) {
        startPoint.x = (startX + startX - maxWidth) / half;
    } else {
        startPoint.x = startX;
    }

    component->canvas_.DrawLabel(startPoint, textValue, maxWidth, component->fontStyle_, component->paint_);

    ACE_FREE(textValue);
    return UNDEFINED;
}

#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
jerry_value_t CanvasComponent::StrokeText(const jerry_value_t func,
                                          const jerry_value_t context,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_5) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of strokeText method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("strokeText method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    char *textValue = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (textValue == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get text value failed");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("get text value failed"));
    }

    int16_t startX = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t startY = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t maxWidth = component->canvas_.GetWidth();

    // FILL_STYLE: strokeText support fill shapses only
    component->paint_.SetStyle(component->paint_.FILL_STYLE);

    Point startPoint;
    const int8_t half = 2;
    startPoint.x = startX;
    startPoint.y = startY;

    // if textAlign=left, not need transfer startX, if textAlign=right or center, need transfer startX
    if (component->fontStyle_.align == TEXT_ALIGNMENT_RIGHT) {
        startPoint.x = startX - maxWidth;
    } else if (component->fontStyle_.align == TEXT_ALIGNMENT_CENTER) {
        startPoint.x = (startX + startX - maxWidth) / half;
    } else {
        startPoint.x = startX;
    }

    component->canvas_.StrokeText(textValue, startPoint, component->fontStyle_, component->paint_);
    return UNDEFINED;
}
#endif

jerry_value_t CanvasComponent::Rotate(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of rotate method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("rotate method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float angle = FloatOf(args[ArgsIndex::IDX_0]);
    component->paint_.Rotate(angle);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::Scale(const jerry_value_t func,
                                     const jerry_value_t dom,
                                     const jerry_value_t args[],
                                     const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of scale method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("scale method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float scaleX  = FloatOf(args[ArgsIndex::IDX_0]);
    float scaleY  = FloatOf(args[ArgsIndex::IDX_1]);
    component->paint_.Scale(scaleX, scaleY);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::MeasureText(const jerry_value_t func,
                                           const jerry_value_t context,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of measureText method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("measureText method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    char *textValue = MallocStringOf(args[ArgsIndex::IDX_0]);
    if (textValue == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get text value failed");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("get text value failed"));
    }

    if (IS_UNDEFINED(component->measureTextObject_)) {
        component->measureTextObject_ = jerry_create_object();
    }
    if (IS_UNDEFINED(component->measureTextWidthString_)) {
        component->measureTextWidthString_ = jerry_create_string(reinterpret_cast<const jerry_char_t *>("width"));
    }
    Point ponit = component->canvas_.MeasureText(textValue, component->fontStyle_);
    jerry_set_property(component->measureTextObject_, component->measureTextWidthString_, ponit.x);

    ACE_FREE(textValue);
    return component->measureTextObject_;
}

jerry_value_t CanvasComponent::Translate(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of translate method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("translate method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t positionX = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t positionY = IntegerOf(args[ArgsIndex::IDX_1]);

    component->paint_.Translate(positionX, positionY);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::Transform(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_6) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of rotate method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("rotate method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float scaleX  = FloatOf(args[ArgsIndex::IDX_0]);
    float shearX  = FloatOf(args[ArgsIndex::IDX_1]);
    float shearY  = FloatOf(args[ArgsIndex::IDX_2]);
    float scaleY  = FloatOf(args[ArgsIndex::IDX_3]);
    int16_t translateX = IntegerOf(args[ArgsIndex::IDX_4]);
    int16_t translateY = IntegerOf(args[ArgsIndex::IDX_5]);
    component->paint_.Transform(scaleX, shearX, shearY, scaleY, translateX, translateY);
    return UNDEFINED;
}


jerry_value_t CanvasComponent::SetTransform(const jerry_value_t func,
                                            const jerry_value_t dom,
                                            const jerry_value_t args[],
                                            const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_6) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of rotate method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("rotate method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float scaleX  = FloatOf(args[ArgsIndex::IDX_0]);
    float shearX  = FloatOf(args[ArgsIndex::IDX_1]);
    float shearY  = FloatOf(args[ArgsIndex::IDX_2]);
    float scaleY  = FloatOf(args[ArgsIndex::IDX_3]);
    int16_t translateX = IntegerOf(args[ArgsIndex::IDX_4]);
    int16_t translateY = IntegerOf(args[ArgsIndex::IDX_5]);
    component->paint_.SetTransform(scaleX, shearX, shearY, scaleY, translateX, translateY);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::Save(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum)
{
    (void)func;

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    component->canvas_.Save(component->paint_);

    return UNDEFINED;
}

jerry_value_t CanvasComponent::Restore(const jerry_value_t func,
                                       const jerry_value_t dom,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum)
{
    (void)func;

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    component->paint_ = component->canvas_.Restore();
    return UNDEFINED;
}


jerry_value_t CanvasComponent::DrawCircle(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_3) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of arc method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("arc method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t centerX = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t centerY = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t radius = IntegerOf(args[ArgsIndex::IDX_2]);

    Point centerPoint;
    centerPoint.x = centerX;
    centerPoint.y = centerY;

    component->canvas_.DrawCircle(centerPoint, radius, component->paint_);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::BeginPath(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    component->canvas_.BeginPath();
    return UNDEFINED;
}

jerry_value_t CanvasComponent::MoveTo(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of moveTo method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("moveTo method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t x = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t y = IntegerOf(args[ArgsIndex::IDX_1]);

    Point point;
    point.x = x;
    point.y = y;
    component->canvas_.MoveTo(point);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::LineTo(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of lineTo method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("lineTo method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t x = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t y = IntegerOf(args[ArgsIndex::IDX_1]);

    Point point;
    point.x = x;
    point.y = y;
    component->canvas_.LineTo(point);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::Rect(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_4) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of rect method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("rect method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t x = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t y = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t width = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t height = IntegerOf(args[ArgsIndex::IDX_3]);

    Point point;
    point.x = x;
    point.y = y;
    component->canvas_.AddRect(point, height, width);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::CleanRect(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_4) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of rect method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("rect method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

#if defined(ENABLE_CANVAS_EXTEND) && ENABLE_CANVAS_EXTEND
    int16_t x = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t y = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t width = IntegerOf(args[ArgsIndex::IDX_2]);
    int16_t height = IntegerOf(args[ArgsIndex::IDX_3]);

    Point point;
    point.x = x;
    point.y = y;
    component->canvas_.ClearRect(point, height, width);
#endif
    return UNDEFINED;
}
jerry_value_t CanvasComponent::Arc(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum != ArgsCount::NUM_5 && argsNum != ArgsCount::NUM_6) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of arc method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("arc method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    int16_t centerX = IntegerOf(args[ArgsIndex::IDX_0]);
    int16_t centerY = IntegerOf(args[ArgsIndex::IDX_1]);
    int16_t radius = IntegerOf(args[ArgsIndex::IDX_2]);
    double sAngle = jerry_get_number_value(args[ArgsIndex::IDX_3]);
    double eAngle = jerry_get_number_value(args[ArgsIndex::IDX_4]);

    bool counterClockwise = false;
    if (argsNum == ArgsCount::NUM_6) {
        if (jerry_value_is_boolean(args[ArgsIndex::IDX_5])) {
            counterClockwise = BoolOf(args[ArgsIndex::IDX_5]);
        }
    }

    Point centerPoint;
    centerPoint.x = centerX;
    centerPoint.y = centerY;
    int16_t startAngle = 0;
    int16_t endAngle = 0;

    bool result = component->FormatArcAngle(sAngle, eAngle, startAngle, endAngle, counterClockwise);
    if (!result) {
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("arc angle error"));
    }

    // 0 degree is at the 3 o'clock position of the arc's circle at JS layer,
    // but which is at the 12 o'clock position at Native layer.
    constexpr int16_t DEGREES_OFFSET = 90;
    constexpr int16_t DEGREES_360 = 360;
    if (((startAngle + DEGREES_OFFSET) <= INT16_MAX) && ((endAngle + DEGREES_OFFSET) <= INT16_MAX)) {
        startAngle = startAngle + DEGREES_OFFSET;
        endAngle = endAngle + DEGREES_OFFSET;
    } else {
        startAngle = startAngle + DEGREES_OFFSET - DEGREES_360;
        endAngle = endAngle + DEGREES_OFFSET - DEGREES_360;
    }

    component->canvas_.ArcTo(centerPoint, radius, startAngle, endAngle);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ClosePath(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    component->canvas_.ClosePath();
    return UNDEFINED;
}

jerry_value_t CanvasComponent::Stroke(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    component->canvas_.DrawPath(component->paint_);
    return UNDEFINED;
}

#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
jerry_value_t CanvasComponent::CreateLInearGradient(const jerry_value_t func,
                                                    const jerry_value_t context,
                                                    const jerry_value_t args[],
                                                    const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_4) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of createLinearGradient method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("createLinearGradient method param error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float startX = FloatOf(args[ArgsIndex::IDX_0]);
    float startY = FloatOf(args[ArgsIndex::IDX_1]);
    float endX = FloatOf(args[ArgsIndex::IDX_2]);
    float endY = FloatOf(args[ArgsIndex::IDX_3]);
    component->paint_.createLinearGradient(startX, startY, endX, endY);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::CreateRadialGradient(const jerry_value_t func,
                                                    const jerry_value_t context,
                                                    const jerry_value_t args[],
                                                    const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_6) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of createRadialGradient method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("createRadialGradient method param error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::
                                                                GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    float startX = FloatOf(args[ArgsIndex::IDX_0]);
    float startY = FloatOf(args[ArgsIndex::IDX_1]);
    float startR = FloatOf(args[ArgsIndex::IDX_2]);
    float endX = FloatOf(args[ArgsIndex::IDX_3]);
    float endY = FloatOf(args[ArgsIndex::IDX_4]);
    float endR = FloatOf(args[ArgsIndex::IDX_5]);
    component->paint_.createRadialGradient(startX, startY, startR, endX, endY, endR);
    return UNDEFINED;
}

jerry_value_t CanvasComponent::AddColorStop(const jerry_value_t func,
                                            const jerry_value_t context,
                                            const jerry_value_t args[],
                                            const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of addColorStop method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("addColorStop method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->colorStopValue_);
    float stop = FloatOf(args[ArgsIndex::IDX_0]);
    component->colorStopValue_ = MallocStringOf(args[ArgsIndex::IDX_1]);
    if (component->colorStopValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: colorStop value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("colorStop value erro!"));
    }

    uint32_t color = 0;
    uint8_t alpha = OPA_OPAQUE;
    ParseColor(component->colorStopValue_, color, alpha);
    ColorType colorStop = component->GetRGBColor(color);
    colorStop.alpha = alpha;
    component->paint_.addColorStop(stop, colorStop);
    return UNDEFINED;
}
#endif

#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
jerry_value_t CanvasComponent::CreatePattern(const jerry_value_t func,
                                             const jerry_value_t context,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum)
{
    (void)func;
    if (argsNum < ArgsCount::NUM_2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: the number of createPattern method parameter error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("createPattern method parameter error"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }

    ACE_FREE(component->patternPathValue_);
    ACE_FREE(component->patternRepeatTypeValue_);
    uint16_t patternRepeatTypeLength = 0;
    component->patternPathValue_ = MallocStringOf(args[ArgsIndex::IDX_0]);
    component->patternRepeatTypeValue_ = MallocStringOf(args[ArgsIndex::IDX_1], &patternRepeatTypeLength);
    if (component->patternPathValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: patternPath value error!");
        return jerry_create_error(JERRY_ERROR_TYPE, reinterpret_cast<const jerry_char_t *>("patternPath value erro!"));
    }
    if (component->patternRepeatTypeValue_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: patternRepeatType value error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("patternRepeatType value erro!"));
    }
    uint16_t patternRepeatTypeId = KeyParser::ParseKeyId(component->patternRepeatTypeValue_, patternRepeatTypeLength);
    if (IsFileExisted(component->patternPathValue_)) {
        switch (patternRepeatTypeId) {
            case K_NO_REPEAT:
                component->paint_.CreatePattern(component->patternPathValue_, NO_REPEAT);
                break;
            case K_REPEAT:
                component->paint_.CreatePattern(component->patternPathValue_, REPEAT);
                break;
            case K_REPEAT_X:
                component->paint_.CreatePattern(component->patternPathValue_, REPEAT_X);
                break;
            case K_REPEAT_Y:
                component->paint_.CreatePattern(component->patternPathValue_, REPEAT_Y);
                break;
        }
    }
    return UNDEFINED;
}
#endif

jerry_value_t CanvasComponent::Fill(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(dom));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
#if defined(ENABLE_CANVAS_EXTEND) && ENABLE_CANVAS_EXTEND
    component->canvas_.FillPath(component->paint_);
#endif
    return UNDEFINED;
}

jerry_value_t CanvasComponent::ParseImageName(const jerry_value_t args[],
                                              char*& imageName,
                                              int16_t &width,
                                              int16_t &height)
{
    ImageComponent* imageCom = nullptr;
    const char *src = nullptr;
    char* imageNameBuf = nullptr;
    // The parameter is the file name
    if (jerry_value_is_string(args[ArgsIndex::IDX_0])) {
        imageNameBuf = MallocStringOf(args[ArgsIndex::IDX_0]);
        if (imageNameBuf == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get imageName value failed");
            return jerry_create_error(JERRY_ERROR_TYPE,
                                      reinterpret_cast<const jerry_char_t *>("get imageName value failed"));
        }
        imageName = imageNameBuf;
        return UNDEFINED;
    // Parameters are objects retrieved from HML
    } else if (jerry_value_is_object(args[ArgsIndex::IDX_0])) {
        void *nativePtr = nullptr;
        jerry_get_object_native_pointer(args[ArgsIndex::IDX_0], &nativePtr, nullptr);
        imageCom = reinterpret_cast<ImageComponent*>(nativePtr);
        if (imageCom != nullptr) {
            src = imageCom->GetSrc();
            if (src == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get imagePath value failed");
                return jerry_create_error(JERRY_ERROR_TYPE,
                                          reinterpret_cast<const jerry_char_t *>("get imagePath value failed"));
            }
            size_t strLen = strlen(src) + 1;
            imageName = static_cast<char*>(ace_malloc(strLen));
            if (memcpy_s(imageName, strLen, src, strLen) != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get imagePath value failed");
                return UNDEFINED;
            }
            return UNDEFINED;
        } else {
            // The argument is the object passed through the new Image()
            imageName = GetImageObjectParam(args, width, height);
            if (strlen(imageName) < 1) {
                return jerry_create_error(JERRY_ERROR_TYPE,
                                          reinterpret_cast<const jerry_char_t *>("get imageName value failed"));
            }
            return UNDEFINED;
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get imageName value failed");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get imageName value failed"));
    }
    return UNDEFINED;
}

char* CanvasComponent::GetImageObjectParam(const jerry_value_t args[], int16_t &width, int16_t &height)
{
    jerry_value_t src = jerryx_get_property_str(args[ArgsIndex::IDX_0], ImageModule::attrSrc);
    char* srcVal = MallocStringOf(src);
    jerry_value_t jWidth = jerryx_get_property_str(args[ArgsIndex::IDX_0], ImageModule::attrWidth);
    jerry_value_t jHeight = jerryx_get_property_str(args[ArgsIndex::IDX_0], ImageModule::attrHeight);
    width = IntegerOf(jWidth);
    height = IntegerOf(jHeight);
    return srcVal;
}

#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
jerry_value_t CanvasComponent::DrawImage(const jerry_value_t func, const jerry_value_t context,
                                         const jerry_value_t args[], const jerry_length_t argsNum)
{
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: DrawImage arg error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("canvas_component: DrawImage arg error!"));
    }
    char* imageName = nullptr;
    int16_t startX = 0;
    int16_t startY = 0;
    int16_t width = -1;
    int16_t height = -1;
    if (ParseImageName(args, imageName, width, height) != UNDEFINED) {
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("parse Imagename function error"));
    }
    if (argsNum >= ArgsCount::NUM_2) {
        startX = IntegerOf(args[ArgsIndex::IDX_1]);
    }
    if (argsNum >= ArgsCount::NUM_3) {
        startY = IntegerOf(args[ArgsIndex::IDX_2]);
    }
    if (argsNum >= ArgsCount::NUM_4) {
        width = IntegerOf(args[ArgsIndex::IDX_3]);
    }
    if (argsNum >= ArgsCount::NUM_5) {
        height = IntegerOf(args[ArgsIndex::IDX_4]);
    }
    Point startLocat = {startX, startY};
    if (width > 0 || height > 0) {
        component->canvas_.DrawImage(startLocat, imageName, component->paint_, width, height);
    } else {
        component->canvas_.DrawImage(startLocat, imageName, component->paint_);
    }
    ACE_FREE(imageName);
    return UNDEFINED;
}
#endif

#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
jerry_value_t CanvasComponent::SetLineDash(const jerry_value_t func,
                                           const jerry_value_t context,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum)
{
    if (argsNum < ArgsCount::NUM_1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: SetLineDash arg error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("canvas_component: DrawImage arg error!"));
    }

    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    jerry_value_t array = args[ArgsIndex::IDX_0];
    if (!jerry_value_is_array(array)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: SetLineDash arg error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("canvas_component: DrawImage arg error!"));
    }
    uint32_t len = jerry_get_array_length(array);
    const uint32_t maxSize = 1024;
    if (len == 0 || len > maxSize * maxSize) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: len = 0 error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("canvas_component: len = 0 error!"));
    }
    float *buff = new float[len];
    if (buff == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: new buff error!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("canvas_component: new buff error!"));
    }
    for (uint32_t i = 0; i < len; i++) {
        jerry_value_t val = jerry_get_property_by_index(array, i);
        float num = 0;
        if (jerry_value_is_number(val)) {
            num = jerry_get_number_value(val);
        } else if (jerry_value_is_string(val)) {
            char *value = MallocStringOf(val);
            if (value == nullptr) {
                delete[] buff;
                HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: value is nullptr error!");
                return jerry_create_error(JERRY_ERROR_TYPE,
                                         reinterpret_cast<const jerry_char_t *>
                                         ("canvas_component: value is nullptr error!"));
            }
            num = atof(value);
            ACE_FREE(value);
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: SetLineDash arg error!");
        }

        buff[i] = num;
        jerry_release_value(val);
    }

    component->paint_.SetLineDash(buff, len);
    delete[] buff;
    return UNDEFINED;
}

jerry_value_t CanvasComponent::GetLineDash(const jerry_value_t func,
                                           const jerry_value_t context,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum)
{
    CanvasComponent *component = static_cast<CanvasComponent *>(ComponentUtils::GetComponentFromBindingObject(context));
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get canvas component from js object failed!");
        return jerry_create_error(JERRY_ERROR_TYPE,
                                  reinterpret_cast<const jerry_char_t *>("get canvas component from js object failed"));
    }
    float* pArr = component->paint_.GetLineDash();
    uint32_t length = component->paint_.GetLineDashCount();
    jerry_release_value(component->dashArray_);
    component->dashArray_ = jerry_create_array(length);
    for (uint32_t i = 0; i < length; i++) {
        jerry_set_property_by_index(component->dashArray_, i, jerry_create_number(pArr[i]));
    }
    return component->dashArray_;
}
#endif

void CanvasComponent::GetSubFont(const char *font, const uint8_t index, char *&subFont) const
{
    if (font == nullptr || *font == '\0') {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: get font attribute failed");
        return;
    }

    if (strlen(font) >= UINT16_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: font size overflow.");
        return;
    }

    // skip space character at the head of string.
    while ((*font != '\0') && (*font == ' ')) {
        font++;
    }

    uint8_t currentIndex = 0;
    const char *currentPos = font;
    const char *endPos = font;
    while (*currentPos != '\0') {
        // find the sub font string
        if (currentIndex == index) {
            if (*endPos != ' ' && *endPos != '\0') {
                endPos++;
                continue;
            }
            uint16_t len = endPos - currentPos;
            uint16_t startIndex = currentPos - font;
            subFont = static_cast<char *>(ace_malloc(len + 1));
            if (subFont == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: malloc buffer failed for subFont");
                return;
            }
            errno_t error = strncpy_s(subFont, len + 1, font + startIndex, len);
            if (error != 0) {
                HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: copy string failed");
                ace_free(subFont);
                subFont = nullptr;
                return;
            }
            subFont[len] = '\0';
            break;
        }
        // go forward to next sub font string.
        if (*(currentPos) == ' ' && *(currentPos + 1) != ' ') {
            currentIndex = currentIndex + 1;
            endPos = currentPos + 1;
        }
        currentPos++;
    }
}

bool CanvasComponent::FormatArcAngle(double sAngle,
                                     double eAngle,
                                     int16_t &startAngle,
                                     int16_t &endAngle,
                                     bool counterClockwise) const
{
    constexpr int16_t DEGREES_180 = 180;
    constexpr int16_t DEGREES_360 = 360;

    // calculate start-angle and end-angle degrees
    if (((DEGREES_180 * sAngle / UI_PI) > INT16_MAX) || ((DEGREES_180 * sAngle / UI_PI) < INT16_MIN) ||
        ((DEGREES_180 * eAngle / UI_PI) > INT16_MAX) || ((DEGREES_180 * eAngle / UI_PI) < INT16_MIN)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "canvas_component: startAngle or endAngle overflow.");
        return false;
    }

    startAngle = static_cast<uint32_t>(DEGREES_180 * sAngle / UI_PI);
    endAngle = static_cast<uint32_t>(DEGREES_180 * eAngle / UI_PI);

    if (!counterClockwise) {
        // reset end-angle degrees when (endAngle - startAngle) > 360
        if ((endAngle - startAngle) > DEGREES_360) {
            endAngle = startAngle + DEGREES_360;
        }
        // reset start-angle or end-angle degrees when startAngle > endAngle
        if (startAngle <= endAngle) {
            return true;
        }
        while (startAngle >= endAngle) {
            if ((endAngle + DEGREES_360) > INT16_MAX) {
                startAngle = startAngle - DEGREES_360;
                break;
            }
            endAngle = endAngle + DEGREES_360;
        }
    } else {
        // reset end-angle degrees when (endAngle - startAngle) > 360
        if ((startAngle - endAngle) > DEGREES_360) {
            endAngle = startAngle - DEGREES_360;
        }
        // reset start-angle or end-angle degrees when startAngle < endAngle
        if (startAngle >= endAngle) {
            return true;
        }
        while (startAngle <= endAngle) {
            if ((endAngle - DEGREES_360) < INT16_MIN) {
                startAngle = startAngle + DEGREES_360;
                break;
            }
            endAngle = endAngle - DEGREES_360;
        }
    }

    return true;
}

void CanvasComponent::RegisterAttributeFunc(jerry_value_t canvas2ddom,
                                            const char *attributeName,
                                            jerry_external_handler_t setterHandler,
                                            jerry_external_handler_t getterHandler)
{
    // register canvas js attribute setter property via the jerry_define_own_property method
    jerry_value_t propName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attributeName));
    jerry_property_descriptor_t desc;
    jerry_init_property_descriptor_fields(&desc);
    desc.is_set_defined = true;
    desc.setter = jerry_create_external_function(setterHandler);
    desc.is_get_defined = true;
    desc.getter = jerry_create_external_function(getterHandler);
    jerry_value_t returnValue = jerry_define_own_property(canvas2ddom, propName, &desc);
    jerry_free_property_descriptor_fields(&desc);
    ReleaseJerryValue(propName, returnValue, VA_ARG_END_FLAG);
}

void CanvasComponent::RegisterDrawMethodFunc(jerry_value_t canvas2ddom,
                                             const char *drawMethodName,
                                             jerry_external_handler_t handler)
{
    JerrySetFuncProperty(canvas2ddom, drawMethodName, handler);
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_CANVAS
