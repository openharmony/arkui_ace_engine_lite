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

#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_profiler.h"
#include "jsi.h"
#include "jsi/internal/jsi_internal.h"
#include "jsi_types.h"

#include "event_listener.h"
#include "digital_crown_module.h"
#include "rotate_manager.h"

namespace OHOS {
namespace ACELite {
void DigitalCrownModule::Init()
{
    globalRotateEventListener_.Reset();
    const char *const setMonitorForCrownEvents = "setMonitorForCrownEvents";
    const char *const clearMonitorForCrownEvents = "clearMonitorForCrownEvents";
    CreateNamedFunction(setMonitorForCrownEvents, SetMonitorForCrownEvents);
    CreateNamedFunction(clearMonitorForCrownEvents, ClearMonitorForCrownEvents);
}

DigitalCrownModule &DigitalCrownModule::GetInstance()
{
    static DigitalCrownModule instance;
    return instance;
}

jerry_value_t DigitalCrownModule::SetMonitorForCrownEvents(const jerry_value_t func,
                                                           const jerry_value_t context,
                                                           const jerry_value_t *args,
                                                           const jerry_length_t argsNum)
{
    if (argsNum != 1 || args == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetMonitorForCrownEvents args invalid.");
        return UNDEFINED;
    }
    DigitalCrownModule &digitalCrownModule = DigitalCrownModule::GetInstance();
    jerry_value_t function = args[0];
    if (!jerry_value_is_function(function)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetMonitorForCrownEvents args should be function.");
        return UNDEFINED;
    }
    digitalCrownModule.globalRotateEventListener_.SetGlobalRotateEventFunc(function);
    RotateManager::GetInstance().Add(&(digitalCrownModule.globalRotateEventListener_));
    return UNDEFINED;
}

jerry_value_t DigitalCrownModule::ClearMonitorForCrownEvents(const jerry_value_t func,
                                                             const jerry_value_t context,
                                                             const jerry_value_t *args,
                                                             const jerry_length_t argsNum)
{
    if (argsNum > 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClearMonitorForCrownEvents does not accept any arguments.");
        return UNDEFINED;
    }
    DigitalCrownModule &digitalCrownModule = DigitalCrownModule::GetInstance();
    digitalCrownModule.globalRotateEventListener_.Reset();
    RotateManager::GetInstance().Remove(&(digitalCrownModule.globalRotateEventListener_));
    return UNDEFINED;
}

void DigitalCrownModule::Clear()
{
    globalRotateEventListener_.Reset();
    RotateManager::GetInstance().Remove(&globalRotateEventListener_);
}
} // namespace ACELite
} // namespace OHOS