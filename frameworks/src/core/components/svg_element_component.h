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

#ifndef OHOS_ACELITE_SVG_ELEMENT_COMPONENT_H
#define OHOS_ACELITE_SVG_ELEMENT_COMPONENT_H

#include "acelite_config.h"

#if (FEATURE_COMPONENT_SVG == 1)

#include "component.h"
#include "non_copyable.h"
#include "svg_engine.h"

namespace OHOS {
namespace ACELite {

class SvgElementComponent final : public Component {
public:
    struct SvgAttr {
        char *name;
        char *value;
        SvgAttr *next;
    };

    ACE_DISALLOW_COPY_AND_MOVE(SvgElementComponent);
    SvgElementComponent() = delete;
    SvgElementComponent(uint16_t tagNameId, jerry_value_t options, jerry_value_t children,
                        AppStyleManager *styleManager);
    ~SvgElementComponent() override;

    SvgElementHandle GetSvgElement() const;
    bool IsSvgComponent() const override { return true; }
    bool IsSvgElementComponent() const override { return true; }

    void SetViewExtraMsg() override {}

protected:
    bool CreateNativeViews() override;
    UIView *GetComponentRootView() const override { return nullptr; }
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool ApplyStyle(const AppStyleItem *style) override;
    bool ProcessChildren() override
    {
        AppendChildren(this);
        return true;
    }
    void AttachView(const Component *child) override;
    void PostUpdate(uint16_t attrKeyId) override;
    void ReleaseNativeViews() override;

private:
    bool EnsureElement() const;
    void CaptureAttrs();
    void CaptureOneAttr(jerry_value_t keys, jerry_value_t attrs, jerry_value_t viewModel, uint32_t index);
    void SetAttr(const char *name, const char *value);
    void CaptureStyle(const AppStyleItem *style, const char *svgName);

    uint16_t tagNameId_ = K_UNKNOWN;
    mutable SvgDocumentHandle doc_ = nullptr;
    mutable SvgElementHandle element_ = nullptr;
    SvgAttr *attrHead_ = nullptr;
};

} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_COMPONENT_SVG

#endif // OHOS_ACELITE_SVG_ELEMENT_COMPONENT_H
