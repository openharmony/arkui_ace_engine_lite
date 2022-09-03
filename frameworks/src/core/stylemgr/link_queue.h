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

#ifndef OHOS_ACELITE_LINK_QUEUE_H
#define OHOS_ACELITE_LINK_QUEUE_H

#include "memory_heap.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class QueueNode final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(QueueNode);
    explicit QueueNode(const char* data) : data_(data), next_(nullptr) {}
    ~QueueNode() = default;
    void SetNodeData(const char *data)
    {
        data_ = data;
    }

    void SetNodeNext(QueueNode *next)
    {
        next_ = next;
    }

    const char *GetNodeData() const
    {
        return data_;
    }

    QueueNode *GetNodeNext() const
    {
        return next_;
    }

private:
    const char *data_;
    QueueNode *next_;
};

class LinkQueue final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LinkQueue);
    LinkQueue() : maxSize_(0)
    {
        InitQueue();
    }

    explicit LinkQueue(uint32_t maxSize) : maxSize_(maxSize)
    {
        InitQueue();
    }

    ~LinkQueue()
    {
        FreeNode();
    }

    const QueueNode *GetFront() const
    {
        return front_;
    }

    const QueueNode *GetRear() const
    {
        return rear_;
    }

    void InitQueue();
    bool IsEmpty() const;
    bool IsFull() const;

    /**
     * @brief Enqueue the address of a character array
     * @param value Enqueue the first address of a character array
     * @return  if the Enqueue is successful return true
     *
     * Note: The value field of the queue stores the address information of the character array, and does not manage
     * the life cycle of the character array. Please ensure that the data is legal when the queue is dequeued.
     */
    bool Enqueue(const char *value);

    /**
     * @brief Dequeue the address of a character array
     * @param value Dequeue the first address of the character array, if you don't care about
     * the value of the dequeue, you can pass this parameter empty
     * @return  if the Dequeue is successful return true
     */
    bool Dequeue(const char **value);
    uint32_t LengthQueue() const;
    QueueNode *GetNext() const;
    void FreeNode();
private:
    QueueNode *front_;
    QueueNode *rear_;
    uint32_t maxSize_;
    uint32_t length_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_LINK_QUEUE_H

