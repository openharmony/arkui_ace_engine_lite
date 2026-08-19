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

#include "svg_component.h"
#include "svg_component_utils.h"
#include <cmath>
#include <cstring>
#include <climits>
#include "ace_log.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"
#include "svg_element_component.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {

namespace {
// Local replacement for ui_lite's SvgAttributeParser::ParseLength. ACE must not depend on
// ui_lite innerkits, so the length parsing is replicated here with the same clamp semantics
// (NaN -> 0, clamped to int16). A unit suffix such as "px" is ignored by strtof, matching the
// previous behavior.
int16_t ParseSvgLength(const char* str)
{
    if (str == nullptr) {
        return 0;
    }
    float value = strtof(str, nullptr);
    if (std::isnan(value)) {
        return 0;
    }
    if (!std::isfinite(value)) {
        return 0;
    }
    if (value > static_cast<float>(INT16_MAX)) {
        return INT16_MAX;
    }
    if (value < static_cast<float>(INT16_MIN)) {
        return INT16_MIN;
    }
    return static_cast<int16_t>(value);
}

template<void (*SvgEngineFn)(SvgDocumentHandle)>
jerry_value_t JsAnimationImpl(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum)
{
    (void)func;
    (void)args;
    (void)argsNum;
    Component *comp = nullptr;
    if (!JSObject::GetNativePointer(dom, reinterpret_cast<void **>(&comp)) || comp == nullptr) {
        return UNDEFINED;
    }
    SvgComponent *svgComp = static_cast<SvgComponent *>(comp);
    SvgDocumentHandle doc = svgComp->GetSvgDocument();
    if (doc != nullptr) {
        SvgEngineFn(doc);
    }
    return UNDEFINED;
}

struct AutoFreePtr {
    char *&ptr;
    explicit AutoFreePtr(char *&p) : ptr(p) {}
    ~AutoFreePtr() { ACE_FREE(ptr); }
    AutoFreePtr(const AutoFreePtr &) = delete;
    AutoFreePtr &operator=(const AutoFreePtr &) = delete;
};
}

SvgComponent::SvgComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager)
{
    SetComponentName(K_SVG);
    // The SVG host view must be transparent by default: SVG has no intrinsic
    // background color, and any area not covered by shapes should show through
    // to the parent. The default UIView style is opaque black, which would
    // otherwise make the SVG appear to have a black background.
    hostView_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    RegisterNamedFunction("startAnimation", JsStartAnimation);
    RegisterNamedFunction("stopAnimation", JsStopAnimation);
    RegisterNamedFunction("pauseAnimations", JsPauseAnimations);
    RegisterNamedFunction("unpauseAnimations", JsUnpauseAnimations);
}

SvgComponent::~SvgComponent()
{
    ReleaseNativeViews();
}

bool SvgComponent::CreateNativeViews()
{
    if (doc_ == nullptr) {
        doc_ = SvgEngine::CreateDocument();
        if (doc_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "SvgComponent: failed to create document.");
            return false;
        }
    }
    if (root_ == nullptr) {
        root_ = SvgEngine::CreateElement(doc_, SVG_ROOT);
        if (root_ == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "SvgComponent: failed to create root element.");
            ReleaseNativeViews();
            return false;
        }
    }
    jerry_value_t attrs = JSObject::Get(GetOptions(), ATTR_ATTRS);
    if (jerry_value_is_object(attrs)) {
        jerry_value_t keys = jerry_get_object_keys(attrs);
        uint32_t n = jerry_get_array_length(keys);
        for (uint32_t i = 0; i < n; ++i) {
            jerry_value_t key = jerry_get_property_by_index(keys, i);
            char *keyStr = MallocStringOf(key);
            if (keyStr != nullptr) {
                jerry_value_t val = jerry_get_property(attrs, key);
                ApplyRootAttribute(keyStr, val);
                ACE_FREE(keyStr);
                jerry_release_value(val);
            }
            jerry_release_value(key);
        }
        jerry_release_value(keys);
    }
    jerry_release_value(attrs);
    return true;
}

void SvgComponent::ApplyRootAttribute(const char *keyStr, jerry_value_t val)
{
    char *canon = SvgComponentUtils::CanonicalName(keyStr);
    if (canon == nullptr) {
        return;
    }
    AutoFreePtr canonGuard(canon);
    bool isColor = SvgComponentUtils::IsColorAttr(canon);
    char *valStr = SvgComponentUtils::ResolveSvgValue(val, isColor, GetViewModel());
    AutoFreePtr valStrGuard(valStr);
    if (valStr == nullptr) {
        return;
    }
    SvgEngine::SetAttribute(root_, canon, valStr);
    if (strchr(valStr, '%') != nullptr) {
        return;
    }
    if (strcmp(canon, "width") == 0) {
        int16_t w = ParseSvgLength(valStr);
        if (w > 0) {
            hostView_.SetWidth(w);
        }
    } else if (strcmp(canon, "height") == 0) {
        int16_t h = ParseSvgLength(valStr);
        if (h > 0) {
            hostView_.SetHeight(h);
        }
    }
}

UIView *SvgComponent::GetComponentRootView() const
{
    return const_cast<UIViewGroup *>(&hostView_);
}

bool SvgComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    if (root_ == nullptr) {
        return false;
    }
    const char *name = SvgComponentUtils::KeyIdToSvgName(attrKeyId);
    if (name == nullptr) {
        return false;
    }
    char *val = SvgComponentUtils::ValueFromJerry(attrValue, SvgComponentUtils::IsColorAttr(name));
    if (val != nullptr) {
        SvgEngine::SetAttribute(root_, name, val);
        ACE_FREE(val);
    }
    return true;
}

void SvgComponent::AttachView(const Component *child)
{
    if (root_ == nullptr || child == nullptr || !child->IsSvgElementComponent()) {
        return;
    }
    // Nested <svg> root components are not supported as children.
    const SvgElementComponent *se = static_cast<const SvgElementComponent *>(child);
    SvgElementHandle childElem = se->GetSvgElement();
    if (childElem != nullptr) {
        SvgEngine::AppendChild(root_, childElem);
    }
}

void SvgComponent::PostRender()
{
    if (doc_ == nullptr || root_ == nullptr) {
        return;
    }
    SvgEngine::SetRoot(doc_, root_);
    SvgEngine::AttachToView(doc_, &hostView_);
}

void SvgComponent::LayoutChildren()
{
    Component::LayoutChildren();
    if (doc_ == nullptr) {
        return;
    }
    int16_t width = hostView_.GetWidth();
    int16_t height = hostView_.GetHeight();
    if (width <= 0 || height <= 0) {
        const Component *parent = GetParent();
        if (parent != nullptr) {
            UIView *parentView = parent->GetComponentRootView();
            if (parentView != nullptr) {
                width = (width <= 0) ? parentView->GetWidth() : width;
                height = (height <= 0) ? parentView->GetHeight() : height;
            }
        }
    }
    if (width > 0 && height > 0) {
        SvgEngine::UpdateRootViewport(doc_, width, height);
    }
    if (!animationsStarted_) {
        animationsStarted_ = true;
        SvgEngine::StartAnimation(doc_);
    }
    SvgEngine::Render(doc_);
}

void SvgComponent::ReleaseNativeViews()
{
    if (doc_ != nullptr) {
        SvgEngine::DestroyDocument(doc_);
        doc_ = nullptr;
        root_ = nullptr;
    }
}

void SvgComponent::PostUpdate(uint16_t attrKeyId)
{
    (void)attrKeyId;
    if (doc_ != nullptr) {
        SvgEngine::Invalidate(doc_);
    }
}

jerry_value_t SvgComponent::JsStartAnimation(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum)
{
    return JsAnimationImpl<SvgEngine::StartAnimation>(func, dom, args, argsNum);
}

jerry_value_t SvgComponent::JsStopAnimation(const jerry_value_t func,
                                            const jerry_value_t dom,
                                            const jerry_value_t args[],
                                            const jerry_length_t argsNum)
{
    return JsAnimationImpl<SvgEngine::StopAnimation>(func, dom, args, argsNum);
}

jerry_value_t SvgComponent::JsPauseAnimations(const jerry_value_t func,
                                              const jerry_value_t dom,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum)
{
    return JsAnimationImpl<SvgEngine::PauseAnimations>(func, dom, args, argsNum);
}

jerry_value_t SvgComponent::JsUnpauseAnimations(const jerry_value_t func,
                                                const jerry_value_t dom,
                                                const jerry_value_t args[],
                                                const jerry_length_t argsNum)
{
    return JsAnimationImpl<SvgEngine::UnpauseAnimations>(func, dom, args, argsNum);
}

} // namespace ACELite
} // namespace OHOS
