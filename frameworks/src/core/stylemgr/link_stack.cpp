/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "link_stack.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
void LinkStack::InitStack()
{
    head_ = new StackNode(nullptr);
    if (head_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create head failed");
        return;
    }
    top_ = head_;
    count_ = 0;
}

void LinkStack::FreeNode()
{
    StackNode *tmp = head_;
    count_ = 0;
    while (tmp != nullptr) {
        head_ = head_->GetNodeNext();
        delete tmp;
        tmp = head_;
    }
    top_ = nullptr;
}

bool LinkStack::IsEmpty() const
{
    return head_ == top_;
}

bool LinkStack::IsFull() const
{
    return maxSize_ > 0 && count_ >= maxSize_;
}

bool LinkStack::Push(const char *value)
{
    if (value == nullptr || top_ == nullptr) {
        return false;
    }
    if (IsFull()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "stack is full");
        return false;
    }

    StackNode *node = new StackNode(value);
    if (node == nullptr) {
        return false;
    }

    top_->SetNodeNext(node);
    node->SetNodePrev(top_);
    count_++;
    top_ = top_->GetNodeNext();
    return true;
}

bool LinkStack::Pop(const char **value)
{
    if (top_ == nullptr || IsEmpty()) {
        return false;
    }
    StackNode *tmp = top_;
    if (value != nullptr) {
        *value = top_->GetNodeData();
    }
    top_->SetNodeData(nullptr);
    top_ = top_->GetNodePrev();
    delete tmp;
    tmp = nullptr;
    top_->SetNodeNext(nullptr);
    count_--;
    return true;
}
} // namespace ACELite
} // namespace OHOS
