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

#ifndef OHOS_ACELITE_SVG_COMPONENT_H
#define OHOS_ACELITE_SVG_COMPONENT_H

#include "acelite_config.h"

#if (FEATURE_COMPONENT_SVG == 1)

#include "component.h"
#include "non_copyable.h"
#include "svg_engine.h"
#include "ui_view_group.h"

namespace OHOS {
namespace ACELite {

class SvgComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SvgComponent);
    SvgComponent() = delete;
    SvgComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);

    ~SvgComponent() override;

    bool IsSvgComponent() const override { return true; }
    SvgDocumentHandle GetSvgDocument() const override { return doc_; }

protected:
    bool CreateNativeViews() override;
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool ProcessChildren() override
    {
        AppendChildren(this);
        return true;
    }
    void AttachView(const Component *child) override;
    void PostRender() override;
    void ReleaseNativeViews() override;
    void PostUpdate(uint16_t attrKeyId) override;
    void LayoutChildren() override;

private:
    void ApplyRootAttribute(const char *keyStr, jerry_value_t val);
    static jerry_value_t JsStartAnimation(const jerry_value_t func,
                                          const jerry_value_t dom,
                                          const jerry_value_t args[],
                                          const jerry_length_t argsNum);
    static jerry_value_t JsStopAnimation(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);
    static jerry_value_t JsPauseAnimations(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);
    static jerry_value_t JsUnpauseAnimations(const jerry_value_t func,
                                             const jerry_value_t dom,
                                             const jerry_value_t args[],
                                             const jerry_length_t argsNum);

    UIViewGroup hostView_;
    SvgDocumentHandle doc_ = nullptr;
    SvgElementHandle root_ = nullptr;
    bool animationsStarted_ = false;
};

} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_COMPONENT_SVG

#endif // OHOS_ACELITE_SVG_COMPONENT_H
