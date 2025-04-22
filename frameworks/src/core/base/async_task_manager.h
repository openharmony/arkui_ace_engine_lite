/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_ASYNC_TASK_MANAGER_H
#define OHOS_ACELITE_ASYNC_TASK_MANAGER_H

#if (defined(__LINUX__) || defined(__LITEOS_A__))
#include <pthread.h>
#elif defined(__LITEOS_M__)
#include "los_task.h"
#endif

#include "common/task_manager.h"
#include "memory_heap.h"
#include "non_copyable.h"

#if defined(__LITEOS_M__)
extern void LOS_TaskLock(void);
extern void LOS_TaskUnlock(void);
#endif

namespace OHOS {
namespace ACELite {
class TaskLockGuard {
public:
    explicit TaskLockGuard(void* lock): lock_(lock)
    {
#if (defined(__LINUX__) || defined(__LITEOS_A__))
        pthread_mutex_lock((pthread_mutex_t*)lock_);
#elif defined(__LITEOS_M__)
        LOS_TaskLock();
#endif
    }
    ~TaskLockGuard()
    {
#if (defined(__LINUX__) || defined(__LITEOS_A__))
        pthread_mutex_unlock((pthread_mutex_t*)lock_);
#elif defined(__LITEOS_M__)
        LOS_TaskUnlock();
#endif
        lock_ = nullptr;
    }
    TaskLockGuard(const TaskLockGuard&) = delete;
    TaskLockGuard& operator=(const TaskLockGuard&) = delete;
private:
    void* lock_ = nullptr;
};


using AsyncTaskHandler = void (*)(void *);

struct AsyncTask final : public MemoryHeap {
    uint16_t id;
    const void *context;
    AsyncTaskHandler handler;
    void *data;
    AsyncTask *next;
    bool isRunning;
};

constexpr uint16_t DISPATCH_FAILURE = 0;

class AsyncTaskManager final : public Task {
public:
    ACE_DISALLOW_COPY_AND_MOVE(AsyncTaskManager);

    static AsyncTaskManager &GetInstance();

    void Init() override;

    void Callback() override;

    uint16_t Dispatch(AsyncTaskHandler handler, void *data, const void *context = nullptr);

    void Cancel(uint16_t taskID);

    void CancelWithContext(const void *context);

    void SetFront(bool front);

    bool IsFront() const
    {
        return front_;
    }

private:
    AsyncTaskManager();

    ~AsyncTaskManager() = default;

    void Reset();

    AsyncTask *head_;
    AsyncTask *tail_;
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    pthread_mutex_t lock_;
#endif
    uint16_t uniqueTaskID_;
    bool front_;
    bool initialized_;
};
} // namespace ACELite
} // namespace OHOS
#endif
