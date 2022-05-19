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

#ifndef OHOS_ACELITE_CANVAS_COMPONENT_H
#define OHOS_ACELITE_CANVAS_COMPONENT_H

#include "acelite_config.h"

#if (FEATURE_COMPONENT_CANVAS == 1)
#include "component.h"
#include "non_copyable.h"
#include "ui_canvas.h"
#include <string>
namespace OHOS {
namespace ACELite {
enum ArgsCount : uint8_t { NUM_1 = 1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6 };
enum ArgsIndex : uint8_t { IDX_0, IDX_1, IDX_2, IDX_3, IDX_4, IDX_5 };
struct AttrMap {
    ACE_DISALLOW_COPY_AND_MOVE(AttrMap);
    const char *attrName;
    jerry_external_handler_t setterName;
    jerry_external_handler_t getterName;
};
struct MethodMap {
    ACE_DISALLOW_COPY_AND_MOVE(MethodMap);
    const char *methodName;
    jerry_external_handler_t callbackName;
};

class CanvasComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(CanvasComponent);
    CanvasComponent() = delete;
    CanvasComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);
    ~CanvasComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;

private:
    void GetSubFont(const char *font, const uint8_t index, char *&subFont) const;
    bool FormatArcAngle(double sAngle,
                        double eAngle,
                        int16_t &startAngle,
                        int16_t &endAngle,
                        bool counterClockwise) const;
						
    static void RegisterAttributeFunc(jerry_value_t canvas2ddom,
                                      const char *attributeName,
                                      jerry_external_handler_t setterHandler,
                                      jerry_external_handler_t getterHandler);

    static void RegisterDrawMethodFunc(jerry_value_t canvas2ddom,
                                       const char *drawMethodName,
                                       jerry_external_handler_t handler);

    static jerry_value_t GetContext(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t FillStyleSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t FillStyleGetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t StrokeStyleSetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t StrokeStyleGetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t LineWidthSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t LineWidthGetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t FontSetter(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t FontGetter(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t TextAlignSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t TextAlignGetter(const jerry_value_t func,
                                         const jerry_value_t contex,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);
#if GRAPHIC_ENABLE_LINECAP_FLAG
    static jerry_value_t LineCapSetter(const jerry_value_t func,
                                       const jerry_value_t contex,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum);

    static jerry_value_t LineCapGetter(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum);
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    static jerry_value_t LineJoinSetter(const jerry_value_t func,
                                        const jerry_value_t contex,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum);

    static jerry_value_t LineJoinGetter(const jerry_value_t func,
                                        const jerry_value_t contex,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum);

    static jerry_value_t MiterLimitSetter(const jerry_value_t func,
                                          const jerry_value_t contex,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum);

    static jerry_value_t MiterLimitGetter(const jerry_value_t func,
                                          const jerry_value_t contex,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum);
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    static jerry_value_t LineDashOffsetSetter(const jerry_value_t func,
                                              const jerry_value_t contex,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t LineDashOffsetGetter(const jerry_value_t func,
                                              const jerry_value_t contex,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);
#endif
    static jerry_value_t GlobalAlphaSetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t GlobalAlphaGetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t GlobalCompositeOperationSetter(const jerry_value_t func,
                                                        const jerry_value_t dom,
                                                        const jerry_value_t args[],
                                                        const jerry_length_t argsNum);

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    static jerry_value_t ShadowOffsetXSetter(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum);

    static jerry_value_t ShadowOffsetXGetter(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum);
    static jerry_value_t ShadowOffsetYSetter(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum);

    static jerry_value_t ShadowOffsetYGetter(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum);
    static jerry_value_t ShadowColorSetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t ShadowColorGetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);
    static jerry_value_t ShadowBlurSetter(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum);

    static jerry_value_t ShadowBlurGetter(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum);
#endif

    static jerry_value_t GlobalCompositeOperationGetter(const jerry_value_t func,
                                                        const jerry_value_t dom,
                                                        const jerry_value_t args[],
                                                        const jerry_length_t argsNum);
														
    static jerry_value_t FillRect(const jerry_value_t func,
                                  const jerry_value_t dom,
                                  const jerry_value_t args[],
                                  const jerry_length_t argsNum);

    static jerry_value_t StrokeRect(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t FillText(const jerry_value_t func,
                                  const jerry_value_t dom,
                                  const jerry_value_t args[],
                                  const jerry_length_t argsNum);

    static jerry_value_t BeginPath(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t MoveTo(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t LineTo(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Rect(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);

    static jerry_value_t CleanRect(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Arc(const jerry_value_t func,
                             const jerry_value_t dom,
                             const jerry_value_t args[],
                             const jerry_length_t argsNum);

    static jerry_value_t ClosePath(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Stroke(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Fill(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
    static jerry_value_t DrawImage(const jerry_value_t func,
                                   const jerry_value_t context,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    static jerry_value_t SetLineDash(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t args[],
                                     const jerry_length_t argsNum);
    static jerry_value_t GetLineDash(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t args[],
                                     const jerry_length_t argsNum);
#endif
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    static jerry_value_t StrokeText(const jerry_value_t func,
                                    const jerry_value_t context,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);
#endif
    static jerry_value_t Rotate(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Scale(const jerry_value_t func,
                               const jerry_value_t dom,
                               const jerry_value_t args[],
                               const jerry_length_t argsNum);

    static jerry_value_t MeasureText(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t args[],
                                     const jerry_length_t argsNum);

    static jerry_value_t Translate(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Transform(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t SetTransform(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum);
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    static jerry_value_t CreateLInearGradient(const jerry_value_t func,
                                              const jerry_value_t dom,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t CreateRadialGradient(const jerry_value_t func,
                                              const jerry_value_t dom,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t AddColorStop(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum);
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    static jerry_value_t CreatePattern(const jerry_value_t func,
                                       const jerry_value_t context,
                                       const jerry_value_t args[],
                                       const jerry_length_t argsNum);
#endif
    static jerry_value_t Save(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);

    static jerry_value_t Restore(const jerry_value_t func,
                                 const jerry_value_t dom,
                                 const jerry_value_t args[],
                                 const jerry_length_t argsNum);

    static jerry_value_t DrawCircle(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);
    static jerry_value_t ParseImageName(const jerry_value_t args[],
                                        char*& imageName,
                                        int16_t &width,
                                        int16_t &height);
    static char* GetImageObjectParam(const jerry_value_t args[], int16_t &width, int16_t &height);
    UICanvas canvas_;
    jerry_value_t dom_;
    jerry_value_t dashArray_;
    jerry_value_t measureTextObject_;
    jerry_value_t measureTextWidthString_;
    Paint paint_;
    UICanvas::FontStyle fontStyle_;
    char *fillStyleValue_;
    char *strokeStyleValue_;
    char *fontValue_;
    char *textAlignValue_;
    int16_t shadowOffsetXValue_;
    int16_t shadowOffsetYValue_;
    int16_t shadowBlurValue_;
    char *shadowColorValue_;
    int16_t lineWidthValue_;

    char *lineCapValue_;
    char *lineJoinValue_;
    float miterLimitValue_;
    float lineDashOffsetValue_;

    char *colorStopValue_;
    char *patternPathValue_;
    char *patternRepeatTypeValue_;

    static const char * const DEFAULT_FILLSTYLE;
    static const char * const DEFAULT_STROKESTYLE;
    static const char * const DEFAULT_TEXTALIGN;
#if GRAPHIC_ENABLE_LINECAP_FLAG
    static const char * const DEFAULT_LINECAP;
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    static const char * const DEFAULT_LINEJOIN;
    static const int16_t DEFAULT_MITERLIMIT;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    static const int16_t DEFAULT_LINEDASHOFFSET;
#endif
    static const AttrMap attrMap_[];
    static const MethodMap methodMap_[];
    static const char * const ATTR_FILLSTYLE;
    static const char * const ATTR_STROKESTYLE;
    static const char * const ATTR_LINEWIDTH;
    static const char * const ATTR_FONT;
    static const char * const ATTR_TEXTALIGN;
#if GRAPHIC_ENABLE_LINECAP_FLAG
    static const char * const ATTR_LINECAP;
#endif
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    static const char * const ATTR_LINEJOIN;
    static const char * const ATTR_MITERLIMIT;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    static const char * const ATTR_LINEDASHOFFSET;
#endif
    static const char * const ATTR_GLOBALALPHA;
    static const char * const ATTR_GLOBALCOMPOSITEOPERATION;
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    static const char * const ATTR_SHADOWOFFSETX;
    static const char * const ATTR_SHADOWOFFSETY;
    static const char * const ATTR_SHADOWBLUR;
    static const char * const ATTR_SHADOWCOLOR;
#endif
    static const char * const FUNC_GETCONTEXT;
    static const char * const FUNC_FILLRECT;
    static const char * const FUNC_STROKERECT;
    static const char * const FUNC_FILLTEXT;
    static const char * const FUNC_BEGINPATH;
    static const char * const FUNC_MOVETO;
    static const char * const FUNC_LINETO;
    static const char * const FUNC_RECT;
    static const char * const FUNC_CLEANRECT;
    static const char * const FUNC_ARC;
    static const char * const FUNC_CLOSEPATH;
    static const char * const FUNC_STROKE;
    static const char * const FUNC_FILL;
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
    static const char * const FUNC_DRAWIMAGE;
#endif
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    static const char * const FUNC_SETLINEDASH;
    static const char * const FUNC_GETLINEDASH;
#endif
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    static const char * const FUNC_STROKETEXT;
#endif
    static const char * const FUNC_ROTATE;
    static const char * const FUNC_SCALE;
    static const char * const FUNC_MEASURETEXT;
    static const char * const FUNC_TRANSLATE;
    static const char * const FUNC_TRANFORM;
    static const char * const FUNC_SETTRANFORM;
    static const char * const FUNC_SAVE;
    static const char * const FUNC_RESTORE;
    static const char * const FUNC_DRAWCIRCLE;
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    static const char * const FUNC_CREATELINEARGRADIENT;
    static const char * const FUNC_CREATERADIALGRADIENT;
    static const char * const FUNC_ADDCOLORSTOP;
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    static const char * const FUNC_CREATEPATTERN;
#endif
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_CANVAS
#endif // OHOS_ACELITE_CANVAS_COMPONENT_H
