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

#include "svg_component_factory.h"

#include "fatal_handler.h"
#include "js_app_context.h"
#include "app_style_manager.h"

namespace OHOS {
namespace ACELite {

namespace {

// Keep only the tags that LVGL's _svg_tag_map also supports.
bool IsSupportedSvgElementTag(uint16_t componentNameId)
{
    switch (componentNameId) {
        case K_RECT:
        case K_CIRCLE:
        case K_ELLIPSE:
        case K_LINE:
        case K_POLYLINE:
        case K_POLYGON:
        case K_PATH:
        case K_SVG_G:
        case K_DEFS:
        case K_USE:
        case K_LINEAR_GRADIENT:
        case K_RADIAL_GRADIENT:
        case K_STOP:
        case K_SOLID_COLOR:
        case K_ANIMATE:
        case K_ANIMATE_COLOR:
        case K_ANIMATE_TRANSFORM:
        case K_ANIMATE_MOTION:
        case K_SET:
        case K_MPATH:
        case K_TEXT_AREA:
        case K_TSPAN:
            return true;
        default:
            return false;
    }
}

Component* CreateSvgElementIfSupported(uint16_t componentNameId,
                                       jerry_value_t options,
                                       jerry_value_t children,
                                       AppStyleManager* styleManager)
{
    bool isSvgElement = IsSupportedSvgElementTag(componentNameId);
    if (!isSvgElement && componentNameId == K_IMAGE) {
        isSvgElement = SvgComponentUtils::IsSvgImageElement(componentNameId, options);
    }
    if (!isSvgElement && componentNameId == K_TEXT) {
        isSvgElement = SvgComponentUtils::IsSvgTextElement(componentNameId, options);
    }
    if (!isSvgElement) {
        return nullptr;
    }
    return new SvgElementComponent(componentNameId, options, children, styleManager);
}

} // namespace

Component* SvgComponentFactory::CreateComponent(uint16_t componentNameId,
                                                jerry_value_t options,
                                                jerry_value_t children)
{
    if (!KeyParser::IsKeyValid(componentNameId)) {
        return nullptr;
    }

    JsAppContext* context = JsAppContext::GetInstance();
    if (context == nullptr) {
        return nullptr;
    }
    AppStyleManager* styleManager = const_cast<AppStyleManager *>(context->GetStyleManager());

    Component* component = nullptr;
    if (componentNameId == K_SVG) {
        component = new SvgComponent(options, children, styleManager);
    } else {
        component = CreateSvgElementIfSupported(componentNameId, options, children, styleManager);
    }
    if (component == nullptr) {
        return nullptr;
    }

    FatalHandler::GetInstance().AttachComponentNode(component);
    return component;
}

} // namespace ACELite
} // namespace OHOS
