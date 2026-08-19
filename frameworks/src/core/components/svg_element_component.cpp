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

#include "svg_element_component.h"

#include <cstring>

#include "svg_component_utils.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "app_style_item.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"
#include "securec.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {

namespace {

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

void FreeSvgAttr(SvgElementComponent::SvgAttr *node)
{
    if (node == nullptr) {
        return;
    }
    ACE_FREE(node->name);
    ACE_FREE(node->value);
    ACE_FREE(node);
}

SvgElementComponent::SvgAttr *CreateSvgAttr(const char *name, const char *value)
{
    char *nameCopy = DupString(name);
    if (nameCopy == nullptr) {
        return nullptr;
    }
    char *valueCopy = DupString(value);
    if (valueCopy == nullptr) {
        ACE_FREE(nameCopy);
        return nullptr;
    }
    SvgElementComponent::SvgAttr *node = static_cast<SvgElementComponent::SvgAttr *>(
        ace_malloc(sizeof(SvgElementComponent::SvgAttr)));
    if (node == nullptr) {
        ACE_FREE(nameCopy);
        ACE_FREE(valueCopy);
        return nullptr;
    }
    node->name = nameCopy;
    node->value = valueCopy;
    node->next = nullptr;
    return node;
}

struct SvgStyleAttrMapping {
    uint16_t keyId;
    const char *name;
};

const SvgStyleAttrMapping SVG_STYLE_ATTRS[] = {
    { K_FILL, "fill" },
    { K_STROKE, "stroke" },
    { K_STROKE_WIDTH, "stroke-width" },
    { K_STROKE_LINECAP, "stroke-linecap" },
    { K_STROKE_LINEJOIN, "stroke-linejoin" },
    { K_STROKE_MITERLIMIT, "stroke-miterlimit" },
    { K_STROKE_DASHARRAY, "stroke-dasharray" },
    { K_STROKE_DASHOFFSET, "stroke-dashoffset" },
    { K_FILL_OPACITY, "fill-opacity" },
    { K_STROKE_OPACITY, "stroke-opacity" },
    { K_VISIBILITY, "visibility" },
    { K_TEXT_ANCHOR, "text-anchor" },
    { K_FONT_SIZE, "font-size" },
    { K_OPACITY, "opacity" },
};
constexpr uint32_t SVG_STYLE_ATTRS_SIZE = sizeof(SVG_STYLE_ATTRS) / sizeof(SVG_STYLE_ATTRS[0]);

struct AutoFreePtr {
    char *&ptr;
    explicit AutoFreePtr(char *&p) : ptr(p) {}
    ~AutoFreePtr() { ACE_FREE(ptr); }
    AutoFreePtr(const AutoFreePtr &) = delete;
    AutoFreePtr &operator=(const AutoFreePtr &) = delete;
};

bool IsColorSvgAttribute(uint16_t tagNameId, const char *name)
{
    if (SvgComponentUtils::IsColorAttr(name)) {
        return true;
    }
    return (tagNameId == K_ANIMATE_COLOR) &&
           (strcmp(name, "from") == 0 || strcmp(name, "to") == 0);
}

} // namespace

SvgElementType TagToSvgShapeType(uint16_t tag)
{
    switch (tag) {
        case K_RECT:
            return SVG_RECT;
        case K_CIRCLE:
            return SVG_CIRCLE;
        case K_ELLIPSE:
            return SVG_ELLIPSE;
        case K_LINE:
            return SVG_LINE;
        case K_POLYLINE:
            return SVG_POLYLINE;
        case K_POLYGON:
            return SVG_POLYGON;
        case K_PATH:
            return SVG_PATH;
        default:
            return SVG_UNKNOWN;
    }
}

SvgElementType TagToSvgStructType(uint16_t tag)
{
    switch (tag) {
        case K_SVG_G:
            return SVG_GROUP;
        case K_DEFS:
            return SVG_DEFS;
        case K_LINEAR_GRADIENT:
            return SVG_LINEAR_GRADIENT;
        case K_RADIAL_GRADIENT:
            return SVG_RADIAL_GRADIENT;
        case K_STOP:
            return SVG_STOP;
        case K_SOLID_COLOR:
            return SVG_SOLID_COLOR;
        case K_ANIMATE:
            return SVG_ANIMATE;
        case K_ANIMATE_COLOR:
            return SVG_ANIMATE_COLOR;
        case K_ANIMATE_TRANSFORM:
            return SVG_ANIMATE_TRANSFORM;
        case K_ANIMATE_MOTION:
            return SVG_ANIMATE_MOTION;
        case K_SET:
            return SVG_SET;
        case K_MPATH:
            return SVG_MPATH;
        case K_USE:
            return SVG_USE;
        case K_TEXT:
            return SVG_TEXT;
        case K_TEXT_AREA:
            return SVG_TEXT_AREA;
        case K_TSPAN:
            return SVG_TSPAN;
        case K_IMAGE:
            return SVG_IMAGE;
        default:
            return SVG_UNKNOWN;
    }
}

SvgElementType TagToSvgType(uint16_t tag)
{
    SvgElementType t = TagToSvgShapeType(tag);
    if (t != SVG_UNKNOWN) {
        return t;
    }
    return TagToSvgStructType(tag);
}

// Convert a raw bundle-relative URI (e.g. "/common/icon.png") to the absolute
// filesystem path used by the installed application. This delegates to the same
// runtime resource resolver used by UI <image>, so the standard "/common"
// URIs map to the installed rawfile directory. Fragment references such as
// <use href="#shape1"> are left untouched. This helper only resolves tags that
// actually carry external resources.
static char *ResolveSvgResourceHref(uint16_t tagNameId, const char *value)
{
    if (value == nullptr || value[0] == '\0' || value[0] == '#') {
        return nullptr;
    }
    if (tagNameId != K_IMAGE) {
        return nullptr;
    }
    return JsAppContext::GetInstance()->GetResourcePath(value);
}

// Resolves a raw bundle-relative href/src attribute to an absolute path when the
// value is an external (non-fragment) reference. Returns a newly allocated string
// the caller must free, or nullptr if no replacement is needed. The input valStr
// is never freed here; the caller decides whether to replace it.
// "src" is accepted because the component map declares it for tags such as <image>.
static char *ResolveHrefAttr(uint16_t tagNameId, const char *canon, const char *valStr)
{
    if (valStr == nullptr || valStr[0] == '#') {
        return nullptr;
    }
    if (strcmp(canon, "href") != 0 && strcmp(canon, "xlink:href") != 0 && strcmp(canon, "src") != 0) {
        return nullptr;
    }
    return ResolveSvgResourceHref(tagNameId, valStr);
}

SvgElementComponent::SvgElementComponent(uint16_t tagNameId, jerry_value_t options,
                                         jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager), tagNameId_(tagNameId)
{
    SetComponentName(tagNameId_);
}

SvgElementComponent::~SvgElementComponent()
{
    ReleaseNativeViews();
}

bool SvgElementComponent::CreateNativeViews()
{
    CaptureAttrs();
    return true;
}

void SvgElementComponent::CaptureAttrs()
{
    jerry_value_t options = GetOptions();
    if (JSUndefined::Is(options) || !JSObject::Is(options)) {
        return;
    }
    jerry_value_t attrs = JSObject::Get(options, ATTR_ATTRS);
    if (jerry_value_is_undefined(attrs) || jerry_value_is_null(attrs)) {
        JSRelease(attrs);
        return;
    }
    jerry_value_t keys = jerry_get_object_keys(attrs);
    if (jerry_value_is_error(keys) || jerry_value_is_undefined(keys)) {
        jerry_release_value(keys);
        jerry_release_value(attrs);
        return;
    }
    jerry_value_t viewModel = GetViewModel();
    uint32_t n = jerry_get_array_length(keys);
    for (uint32_t i = 0; i < n; ++i) {
        CaptureOneAttr(keys, attrs, viewModel, i);
    }
    jerry_release_value(keys);
    jerry_release_value(attrs);
}

void SvgElementComponent::CaptureOneAttr(jerry_value_t keys, jerry_value_t attrs, jerry_value_t viewModel,
    uint32_t index)
{
    jerry_value_t key = jerry_get_property_by_index(keys, index);
    char *keyStr = MallocStringOf(key);
    jerry_release_value(key);
    if (keyStr == nullptr) {
        return;
    }
    AutoFreePtr keyStrGuard(keyStr);

    jerry_value_t val = jerry_get_property(attrs, key);
    char *canon = SvgComponentUtils::CanonicalName(keyStr);
    if (canon == nullptr) {
        jerry_release_value(val);
        return;
    }
    AutoFreePtr canonGuard(canon);

    bool isColor = IsColorSvgAttribute(tagNameId_, canon);
    char *valStr = SvgComponentUtils::ResolveSvgValue(val, isColor, viewModel);
    jerry_release_value(val);
    char *resolved = ResolveHrefAttr(tagNameId_, canon, valStr);
    if (resolved != nullptr) {
        ACE_FREE(valStr);
        valStr = resolved;
    }
    AutoFreePtr valStrGuard(valStr);
    if (valStr != nullptr) {
        SetAttr(canon, valStr);
    }
}

void SvgElementComponent::SetAttr(const char *name, const char *value)
{
    if (name == nullptr || value == nullptr) {
        return;
    }
    for (SvgAttr *cur = attrHead_; cur != nullptr; cur = cur->next) {
        if (strcmp(cur->name, name) == 0) {
            char *newVal = DupString(value);
            if (newVal != nullptr) {
                ACE_FREE(cur->value);
                cur->value = newVal;
            }
            return;
        }
    }
    SvgAttr *node = CreateSvgAttr(name, value);
    if (node == nullptr) {
        return;
    }
    node->next = attrHead_;
    attrHead_ = node;
}

void SvgElementComponent::CaptureStyle(const AppStyleItem *style, const char *svgName)
{
    if (style == nullptr || svgName == nullptr) {
        return;
    }
    bool isColor = SvgComponentUtils::IsColorAttr(svgName);
    char *valStr = SvgComponentUtils::ValueFromStyle(style, isColor);
    if (valStr != nullptr) {
        SetAttr(svgName, valStr);
        if (element_ != nullptr) {
            SvgEngine::SetAttribute(element_, svgName, valStr);
        }
        ACE_FREE(valStr);
    }
}

bool SvgElementComponent::ApplyStyle(const AppStyleItem *style)
{
    if (style == nullptr) {
        return false;
    }
    uint16_t id = style->GetPropNameId();
    for (uint32_t i = 0; i < SVG_STYLE_ATTRS_SIZE; ++i) {
        if (SVG_STYLE_ATTRS[i].keyId == id) {
            CaptureStyle(style, SVG_STYLE_ATTRS[i].name);
            return true;
        }
    }
    return false;
}

bool SvgElementComponent::EnsureElement() const
{
    if (element_ != nullptr) {
        return true;
    }
    for (Component *p = GetParent(); p != nullptr; p = p->GetParent()) {
        SvgDocumentHandle doc = p->GetSvgDocument();
        if (doc != nullptr) {
            doc_ = doc;
            break;
        }
    }
    if (doc_ == nullptr) {
        return false;
    }
    SvgElementType type = TagToSvgType(tagNameId_);
    if (type == SVG_UNKNOWN) {
        return false;
    }
    element_ = SvgEngine::CreateElement(doc_, type);
    if (element_ == nullptr) {
        return false;
    }
    for (SvgAttr *a = attrHead_; a != nullptr; a = a->next) {
        SvgEngine::SetAttribute(element_, a->name, a->value);
    }
    return true;
}

SvgElementHandle SvgElementComponent::GetSvgElement() const
{
    if (element_ != nullptr) {
        return element_;
    }
    return EnsureElement() ? element_ : nullptr;
}

bool SvgElementComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    const char *name = SvgComponentUtils::KeyIdToSvgName(attrKeyId);
    if (name == nullptr) {
        return false;
    }
    bool isColor = SvgComponentUtils::IsColorAttr(name);
    char *val = SvgComponentUtils::ValueFromJerry(attrValue, isColor);
    if (val == nullptr) {
        return false;
    }
    SetAttr(name, val);
    if (element_ != nullptr) {
        SvgEngine::SetAttribute(element_, name, val);
    }
    ACE_FREE(val);
    return true;
}

void SvgElementComponent::AttachView(const Component *child)
{
    if (!EnsureElement() || child == nullptr || !child->IsSvgElementComponent()) {
        return;
    }
    // Nested <svg> root components are not supported as children.
    const SvgElementComponent *se = static_cast<const SvgElementComponent *>(child);
    SvgElementHandle childElem = se->GetSvgElement();
    if (childElem != nullptr) {
        SvgEngine::AppendChild(element_, childElem);
    }
}

void SvgElementComponent::PostUpdate(uint16_t attrKeyId)
{
    (void)attrKeyId;
    if (element_ != nullptr && doc_ != nullptr) {
        SvgEngine::Invalidate(doc_);
    }
}

void SvgElementComponent::ReleaseNativeViews()
{
    SvgAttr *cur = attrHead_;
    while (cur != nullptr) {
        SvgAttr *next = cur->next;
        FreeSvgAttr(cur);
        cur = next;
    }
    attrHead_ = nullptr;
    element_ = nullptr;
    doc_ = nullptr;
}

} // namespace ACELite
} // namespace OHOS
