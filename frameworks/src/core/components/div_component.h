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

#ifndef OHOS_ACELITE_DIV_COMPONENT_H
#define OHOS_ACELITE_DIV_COMPONENT_H
#include "component.h"
#include "flex_layout.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class DivComponent final : public Component {
public:

    ACE_DISALLOW_COPY_AND_MOVE(DivComponent);
    DivComponent() = delete;
    DivComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~DivComponent() override {}

protected:
    UIView *GetComponentRootView() const override;
    bool ApplyPrivateStyle(const AppStyleItem* style) override;
    bool ProcessChildren() override;
    void PostUpdate(uint16_t attrKeyId) override;
    void AttachView(const Component *child) override;
    void LayoutChildren() override;

private:
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    bool NativeViewSetGapStyle(const AppStyleItem* style, uint16_t stylePropNameId,
                               const char * const strValue);
    bool ApplyEnhancedPrivateStyle(const AppStyleItem* style, uint16_t stylePropNameId,
                                   const char * const strValue);
    void NativeViewSetEnhancedDirection(const AppStyleItem* style, uint16_t stylePropNameId,
                                        const char * const strValue, bool& applyResult);
    /**
     * @brief Applies one gap value token, which is either a non-negative pixel length
     *        (number or string, with or without the px suffix) or a percentage resolved
     *        later by FlexLayout against the container content-box.
     * @param token the token characters; may point into a larger string, so only the
     *        first tokenLen characters are considered (percent detection requires '%'
     *        to be the last character of the token).
     * @param tokenLen length of the token, excluding any trailing separator.
     * @param isColumn true for the column-gap, false for the row-gap.
     */
    void ApplyGapToken(const char *token, uint16_t tokenLen, bool isColumn);
    /**
     * @brief Applies the gap shorthand string of the form "<row-gap> [<column-gap>]?";
     *        a single value applies to both gaps with the same type.
     * @param strValue the raw shorthand string, must not be nullptr.
     */
    void ApplyGapShorthand(const char * const strValue);
    /**
     * @brief Applies one single-dimension gap style (column-gap or row-gap), which is
     *        either a non-negative pixel length or a percentage resolved later by FlexLayout.
     * @param isColumn true for the column-gap, false for the row-gap.
     */
    void ApplyGapValue(const AppStyleItem* style, bool isColumn);
    void NativeViewSetOverflow(uint16_t valueId);
    void NativeViewSetAlignContent(uint16_t valueId, bool& applyResult);
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
    void NativeViewSetDirection(const AppStyleItem* style, uint16_t stylePropNameId,
                                const char * const strValue, bool& applyResult);
    void NativeViewSetLayoutDirection(uint16_t valueId, bool& applyResult);
    void NativeViewSetMajorAxisAlign(uint16_t valueId, bool& applyResult);
    bool isSecondaryAxisAlignSet_;
    bool isVerticalLayout_;
    FlexLayout nativeView_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_DIV_COMPONENT_H
