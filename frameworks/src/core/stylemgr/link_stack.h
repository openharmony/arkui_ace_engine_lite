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

#ifndef OHOS_ACELITE_LINK_STACK_H
#define OHOS_ACELITE_LINK_STACK_H

#include "memory_heap.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class StackNode final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(StackNode);
    explicit StackNode(const char *data) : data_(data), prev_(nullptr), next_(nullptr) {}
    ~StackNode() = default;

    void SetNodeData(const char *data)
    {
        data_ = data;
    }

    void SetNodePrev(StackNode *prev)
    {
        prev_ = prev;
    }

    void SetNodeNext(StackNode *next)
    {
        next_ = next;
    }

    const char *GetNodeData() const
    {
        return data_;
    }

    StackNode *GetNodePrev() const
    {
        return prev_;
    }

    StackNode *GetNodeNext() const
    {
        return next_;
    }
private:
    const char *data_;
    StackNode *prev_;
    StackNode *next_;
};

class LinkStack final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LinkStack);
    LinkStack() : maxSize_(0)
    {
        InitStack();
    }

    explicit LinkStack(uint32_t maxSize) : maxSize_(maxSize)
    {
        InitStack();
    }

    ~LinkStack()
    {
        FreeNode();
    }

    const StackNode *GetTop() const
    {
        return top_;
    }

    uint32_t StackSize() const
    {
        return count_;
    }

    const char *Peak() const
    {
        return top_->GetNodeData();
    }

    void InitStack();
    void FreeNode();
    bool IsEmpty() const;
    bool IsFull() const;

    /**
     * @brief push the address of a character array the stack
     * @param value Push the first address of the character array onto the stack
     * @return return true if the push is successful
     *
     * Note: The value field that is pushed into the stack stores the address information of the
     * character array and does not manage the life cycle of the character array. Please ensure
     * that the data is legal when popping the stack.
     */
    bool Push(const char *value);

    /**
     * @brief pop the address of a character array the stack
     * @param value Pop the first address of the character array from the stack. If you don't care
     * about the popped elements, please pass the parameter NULL.
     * @return return true if the push is successful
     */
    bool Pop(const char **value);
private:
    StackNode *head_;
    StackNode *top_;
    uint32_t maxSize_;
    uint32_t count_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_LINK_STACK_H

