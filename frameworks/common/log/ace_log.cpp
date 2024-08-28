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

#include "ace_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>
#if defined(TARGET_SIMULATOR) && (TARGET_SIMULATOR == 1)
#include <string>

namespace OHOS {
namespace ACELite {
using namespace std;
void ReplaceHiLogPrivacyKeyWords(string &original, const string &toBeReplaced, const string &targetStr)
{
    string::size_type toBeReplacedLen = toBeReplaced.length();
    do {
        string::size_type foundPos = original.find(toBeReplaced);
        if (foundPos == string::npos) {
            // replace all end
            break;
        }
        // replace to target
        original.replace(foundPos, toBeReplacedLen, targetStr);
    } while (1);
}

void PrintInfo(const char *format, va_list args)
{
    string originalStr(format);
    string toBeReplacedStr("{public}");
    string emptyStr("");
    ReplaceHiLogPrivacyKeyWords(originalStr, toBeReplacedStr, emptyStr);
    vprintf(originalStr.c_str(), args);
    printf("\n");
}

void HILOG_FATAL(HiLogModuleType mod, const char *msg, ...)
{
    (void)(mod);
    printf("[ACELite][FATAL]:");
    va_list args;
    va_start(args, msg);
    PrintInfo(msg, args);
    va_end(args);
}

void HILOG_ERROR(HiLogModuleType mod, const char *msg, ...)
{
    (void)(mod);
    printf("[ACELite][ERROR]:");
    va_list args;
    va_start(args, msg);
    PrintInfo(msg, args);
    va_end(args);
}

void HILOG_INFO(HiLogModuleType mod, const char *msg, ...)
{
    (void)(mod);
    printf("[ACELite][INFO]:");
    va_list args;
    va_start(args, msg);
    PrintInfo(msg, args);
    va_end(args);
}

void HILOG_WARN(HiLogModuleType mod, const char *msg, ...)
{
    (void)(mod);
    printf("[ACELite][WARN]:");
    va_list args;
    va_start(args, msg);
    PrintInfo(msg, args);
    va_end(args);
}

void HILOG_DEBUG(HiLogModuleType mod, const char *msg, ...)
{
    (void)(mod);
    printf("[ACELite][DEBUG]:");
    va_list args;
    va_start(args, msg);
    PrintInfo(msg, args);
    va_end(args);
}
} // namespace ACELite
} // namespace OHOS
#endif // TARGET_SIMULATOR

#if defined(FEATURE_ACELITE_MC_LOG_PRINTF) && (FEATURE_ACELITE_MC_LOG_PRINTF == 1)
void HILOG_CHARACTERS(const size_t size, const char* buffer)
{
    if (size == 0 || buffer == nullptr) {
        return;
    }

    const size_t len1 = 1;
    const size_t len2 = 2;
    const size_t len3 = 3;
    const size_t len4 = 4;
    const size_t len5 = 5;
    const size_t chunkSize = 6;

    for (size_t index = 0; index < size; index += chunkSize) {
        size_t len = std::min(size - index, chunkSize);
        if (len == chunkSize) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c%{public}c%{public}c%{public}c%{public}c%{public}c",
                buffer[index], buffer[index + len1], buffer[index + len2],
                buffer[index + len3], buffer[index + len4], buffer[index + len5]);
        } else if (len == len5) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c%{public}c%{public}c%{public}c%{public}c",
                buffer[index], buffer[index + len1], buffer[index + len2],
                buffer[index + len3], buffer[index + len4]);
        } else if (len == len4) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c%{public}c%{public}c%{public}c",
                buffer[index], buffer[index + len1], buffer[index + len2], buffer[index + len3]);
        } else if (len == len3) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c%{public}c%{public}c",
                buffer[index], buffer[index + len1], buffer[index + len2]);
        } else if (len == len2) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c%{public}c", buffer[index], buffer[index + len1]);
        } else if (len == len1) {
            HILOG_ERROR(HILOG_MODULE_ACE, "%{public}c", buffer[index]);
        }
    }
}
#endif
