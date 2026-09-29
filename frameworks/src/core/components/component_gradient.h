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

#ifndef OHOS_ACELITE_COMPONENT_GRADIENT_H
#define OHOS_ACELITE_COMPONENT_GRADIENT_H

#if (FEATURE_COMPONENT_GRADIENT == 1)

#include <string>

#include "gfx_utils/gradient_info.h"
#include "non_copyable.h"

namespace OHOS {
class UIView;

namespace ACELite {
/**
 * @brief Owner of a component's parsed background linear gradient.
 *
 * Responsibilities: parse a linear-gradient() declaration (with the
 * direction-keyword compatibility retry), hold the resulting GradientInfo,
 * and sync an independent copy to the target view. The component keeps the
 * tree-level concerns (the has-gradient-in-subtree flag propagation) to
 * itself; this class deliberately knows nothing about the component tree.
 */
class ComponentGradient final {
public:
    ComponentGradient() = default;
    /* releases the owned GradientInfo */
    ~ComponentGradient();
    ACE_DISALLOW_COPY_AND_MOVE(ComponentGradient);

    /**
     * @brief Parse a linear-gradient() declaration and publish it to @p view.
     *
     * Encapsulates the whole "parse, take ownership, push to the view" sequence.
     *
     * @param cssValue the background-image declaration.
     * @param view     the component root view receiving the gradient.
     * @return true when the declaration was parsed and applied; false when it is
     *         malformed, in which case the previous gradient is left untouched
     *         and the temporary payload is released.
     */
    bool ApplyStyle(const std::string& cssValue, UIView& view);

    /**
     * @brief Route a background-image declaration: clear on "none"/empty, drop
     *        broken tokens silently, apply gradient declarations.
     * @param value the raw CSS value (non-null).
     * @param view  the component root view; may be null (declaration still consumed).
     * @return true when the declaration was consumed by the gradient feature;
     *         false when it is a plain image URL the caller should handle.
     */
    bool TryHandleBackground(const char *value, UIView *view);

    /**
     * @brief Hand an independent copy of the gradient over to the view.
     *
     * The component and the view must not share the color stop array: they have
     * unrelated lifetimes, so DeepCopy() is mandatory here. Passing nullptr clears
     * whatever gradient the view currently holds.
     */
    void SyncToView(UIView& view) const;

    /**
     * @brief Whether a parsed gradient is currently owned.
     */
    bool HasInfo() const
    {
        return info_ != nullptr;
    }

    /**
     * @brief Drop the owned gradient, if any.
     */
    void Clear();

private:
    GradientInfo *info_ = nullptr;
};
} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_COMPONENT_GRADIENT
#endif // OHOS_ACELITE_COMPONENT_GRADIENT_H
