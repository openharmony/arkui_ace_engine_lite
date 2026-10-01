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

#if (FEATURE_COMPONENT_SVG == 1)

#include "component.h"

namespace OHOS {
namespace ACELite {

void Component::RemoveSvgChild(Component *childNode)
{
    // SVG element components own no native UIView (GetComponentRootView()
    // returns nullptr). We must not return early on null view, otherwise
    // RemoveAllChildren() would hang (it loops until childHead_ becomes
    // nullptr) and Release() would leave a dangling child pointer.
    UIView *childNativeView = childNode->GetComponentRootView();
    UIViewGroup *parentView = reinterpret_cast<UIViewGroup *>(GetComponentRootView());
    if ((childNativeView != nullptr) && (parentView != nullptr)) {
        parentView->Remove(childNativeView);
    }

    if (childNode == childHead_) {
        Component *next = childHead_->GetNextSibling();
        childNode->SetNextSibling(nullptr);
        childNode->SetParent(nullptr);
        childHead_ = next;
        return;
    }

    Component *temp = childHead_;
    while (temp != nullptr) {
        if (temp->GetNextSibling() == childNode) {
            break;
        }
        temp = temp->GetNextSibling();
    }
    if (temp == nullptr) {
        return;
    }

    temp->SetNextSibling(childNode->GetNextSibling());
    childNode->SetNextSibling(nullptr);
    childNode->SetParent(nullptr);
}

} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_COMPONENT_SVG == 1
