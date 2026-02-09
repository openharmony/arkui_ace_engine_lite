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

#ifndef OHOS_ACELITE_ACE_MEM_BASE_H
#define OHOS_ACELITE_ACE_MEM_BASE_H

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace ACELite {

inline uint32_t GetLR()
{
#if __riscv
    uint32_t lr;
    __asm volatile ("mv %0, ra" : "=r" (lr));
    return lr;
#elif (defined(__arm__) || defined(__aarch64__)) && !defined(__APPLE__)
    uint32_t lr;
    __asm volatile ("mov %0, LR" : "=r" (lr));
    return lr;
#else
    return static_cast<uint32_t>(0);
#endif
}

/**
 * @brief The ACEMemHooks struct saving the memory allocating hooks used for ACE
 */
struct ACEMemHooks {
    void *(*malloc_func)(size_t sz, uint32_t lr);
    void (*free_func)(void *ptr);
    void *(*calloc_func)(size_t num, size_t size);
};

/**
 * @brief InitMemHooks Set the memory hooks used in whole JS framework, malloc and free
 * must be given and must not be null. the initialization should be called at the very begin
 * of system start-up, and only can be called once.
 *
 * @param [in] hooks: malloc, free and calloc function pointer
 */
void InitMemHooks(const ACEMemHooks &hooks);

/**
 * @brief InitCacheBuf config the cache buffer used for ACE, for example: PSRAM buffer
 * @param bufAddress the start address of the buffer
 * @param bufSize the whole length of the buffer, count in bytes
 */
void InitCacheBuf(uintptr_t bufAddress, size_t bufSize);

/**
 * @brief Allocate the required memory space and return a pointer to it
 *
 * @param [in] size: size of memory block,in bytes
 *
 * @return a pointer to the allocated memory block.if the request fails,return NULL
 */
void *ace_malloc(size_t size);

/**
 * @brief Allocate the required memory space with LR tracking
 *
 * @param [in] size: size of memory block,in bytes
 * @param [in] lr: Link Register value for tracking, 0 means not available
 *
 * @return a pointer to the allocated memory block.if the request fails,return NULL
 */
void *ace_malloc(size_t size, uint32_t lr);

/**
 * @brief Allocate the required memory space and return a pointer to it
 *
 * @param [in] num: number of elements to be allocated
 * @param [in] size: size of memory block,in bytes
 *
 * @return a pointer to the allocated memory block.if the request fails,return NULL
 */
void *ace_calloc(size_t num, size_t size);

/**
 * @brief Free the memory space allocated by malloc(),calloc() or realloc()
 *
 * @param [in] ptr: pointer points to a memory block to be freed
 */
void ace_free(void *ptr);
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_ACE_MEM_BASE_H
