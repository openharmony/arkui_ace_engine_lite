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

#include "link_queue.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
void LinkQueue::InitQueue()
{
    front_ = new QueueNode(nullptr);
    if (front_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create front failed");
        return;
    }
    rear_ = front_;
    length_ = 0;
}

bool LinkQueue::IsEmpty() const
{
    if (front_ == rear_ && length_ == 0) {
        return true;
    }
    return false;
}

bool LinkQueue::IsFull() const
{
    if (maxSize_ > 0 && length_ >= maxSize_) {
        return true;
    }
    return false;
}

uint32_t LinkQueue::LengthQueue() const
{
    return length_;
}

bool LinkQueue::Enqueue(const char *value)
{
    if (value == nullptr || rear_ == nullptr) {
        return false;
    }
    if (IsFull()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "queue is full");
        return false;
    }
    QueueNode* node = new QueueNode(value);
    if (node == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create queue node failed");
        return false;
    }
    rear_->SetNodeNext(node);
    rear_ = node;
    length_++;
    return true;
}

bool LinkQueue::Dequeue(const char **value)
{
    if (front_ == nullptr || IsEmpty()) {
        return false;
    }
    QueueNode *tmp = front_->GetNodeNext();
    if (value != nullptr) {
        *value = tmp->GetNodeData();
    }
    delete front_;
    front_ = tmp;
    length_--;
    return true;
}

QueueNode *LinkQueue::GetNext() const
{
    return front_ ? front_->GetNodeNext() : nullptr;
}

void LinkQueue::FreeNode()
{
    QueueNode *tmp = front_;
    length_ = 0;
    while (tmp != nullptr) {
        front_ = front_->GetNodeNext();
        delete tmp;
        tmp = front_;
    }
    rear_ = nullptr;
}
} // namespace ACELite
} // namespace OHOS
