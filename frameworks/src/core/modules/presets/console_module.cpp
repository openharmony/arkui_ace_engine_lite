/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "presets/console_module.h"
#if IS_ENABLED(CONSOLE_LOG_OUTPUT)
#include "presets/console_log_impl.h"
#endif // ENABLED(CONSOLE_LOG_OUTPUT)

namespace OHOS {
namespace ACELite {
void ConsoleModule::Init()
{
    const char * const debug = "debug";
    const char * const info = "info";
    const char * const warn = "warn";
    const char * const log = "log";
    const char * const error = "error";
    CreateNamedFunction(debug, LogDebug);
    CreateNamedFunction(info, LogInfo);
    CreateNamedFunction(warn, LogWarn);
    CreateNamedFunction(log, Log);
    CreateNamedFunction(error, LogError);
}

jerry_value_t ConsoleModule::LogDebug(const jerry_value_t func,
                                      const jerry_value_t context,
                                      const jerry_value_t *args,
                                      const jerry_length_t length)
{
#if IS_DISABLED(CONSOLE_LOG_OUTPUT)
    return UNDEFINED;
#else
    return LogNative(LOG_LEVEL_DEBUG, args, length);
#endif // DISABLED(CONSOLE_LOG_OUTPUT)
}

jerry_value_t ConsoleModule::LogInfo(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t *args,
                                     const jerry_length_t length)
{
#if IS_DISABLED(CONSOLE_LOG_OUTPUT)
    return UNDEFINED;
#else
    return LogNative(LOG_LEVEL_INFO, args, length);
#endif // DISABLED(CONSOLE_LOG_OUTPUT)
}

jerry_value_t ConsoleModule::LogWarn(const jerry_value_t func,
                                     const jerry_value_t context,
                                     const jerry_value_t *args,
                                     const jerry_length_t length)
{
#if IS_DISABLED(CONSOLE_LOG_OUTPUT)
    return UNDEFINED;
#else
    return LogNative(LOG_LEVEL_WARN, args, length);
#endif // DISABLED(CONSOLE_LOG_OUTPUT)
}

jerry_value_t ConsoleModule::Log(const jerry_value_t func,
                                 const jerry_value_t context,
                                 const jerry_value_t *args,
                                 const jerry_length_t length)
{
#if IS_DISABLED(CONSOLE_LOG_OUTPUT)
    return UNDEFINED;
#else
    return LogNative(LOG_LEVEL_NONE, args, length);
#endif // DISABLED(CONSOLE_LOG_OUTPUT)
}

jerry_value_t ConsoleModule::LogError(const jerry_value_t func,
                                      const jerry_value_t context,
                                      const jerry_value_t *args,
                                      const jerry_length_t length)
{
#if IS_DISABLED(CONSOLE_LOG_OUTPUT)
    return UNDEFINED;
#else
    return LogNative(LOG_LEVEL_ERR, args, length);
#endif // DISABLED(CONSOLE_LOG_OUTPUT)
}
} // namespace ACELite
} // namespace OHOS
