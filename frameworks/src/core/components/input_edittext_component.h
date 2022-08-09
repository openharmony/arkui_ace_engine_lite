/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_INPUT_EDIT_TEXT_COMPONENT_H
#define OHOS_ACELITE_INPUT_EDIT_TEXT_COMPONENT_H
#include "acelite_config.h"
#if (FEATURE_COMPONENT_EDITTEXT == 1)
#include "component.h"
#include "flex_layout.h"
#include "non_copyable.h"
#include "key_parser.h"
#include "ui_edit_text.h"

namespace OHOS {
namespace ACELite {
class InputEditTextComponent final : public Component {
public:

    ACE_DISALLOW_COPY_AND_MOVE(InputEditTextComponent);
    InputEditTextComponent() = delete;
    InputEditTextComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *manager)
        : Component(options, children, manager),
          onValueChangeListener_(nullptr),
          fontFamily_(nullptr),
          fontSize_(DEFAULT_FONT_SIZE)
    {
        SetComponentName(K_INPUT);
    }
    ~InputEditTextComponent() override {};
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    bool ApplyPrivateStyle(const AppStyleItem *style) override;
    void PostUpdate(uint16_t attrKeyId) override;
    uint8_t GetFontSize();
protected:
    UIView *GetComponentRootView() const override
    {
        return const_cast<UIEditText *>(&edittext_);
    }

private:
    bool SetFontSize(const AppStyleItem *style);
    bool SetColor(const AppStyleItem &style);
    bool SetPlaceholderColor(const AppStyleItem &style);
    UIEditText edittext_;
    ValueChangeListener* onValueChangeListener_;
    char *fontFamily_;
    uint8_t fontSize_;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_EDITTEXT
#endif // OHOS_ACELITE_INPUT_EDIT_TEXT_COMPONENT_H
