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

#ifndef OHOS_ACELITE_DIGITAL_CROWN_MODULE_H
#define OHOS_ACELITE_DIGITAL_CROWN_MODULE_H

#include "jsi.h"
#include "non_copyable.h"
#include "event_listener.h"
#include "presets/preset_module.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief: The module of digitalCrown
 */
class DigitalCrownModule final : public PresetModule {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DigitalCrownModule);
    static DigitalCrownModule &GetInstance();

    void Init() override;

    void Clear();

    static jerry_value_t SetMonitorForCrownEvents(const jerry_value_t func,
                                                  const jerry_value_t context,
                                                  const jerry_value_t *args,
                                                  const jerry_length_t argsNum);

    static jerry_value_t ClearMonitorForCrownEvents(const jerry_value_t func,
                                                    const jerry_value_t context,
                                                    const jerry_value_t *args,
                                                    const jerry_length_t argsNum);

    ~DigitalCrownModule() = default;

private:
    DigitalCrownModule() : PresetModule(nullptr) {}

    GlobalRotateEventListener globalRotateEventListener_;
};

class DigitalCrownEventsModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DigitalCrownEventsModule);
    ~DigitalCrownEventsModule() = default;
    static void Load()
    {
        DigitalCrownModule &digitalCrownModule = DigitalCrownModule::GetInstance();
        digitalCrownModule.Init();
    }

    static void Clear()
    {
        DigitalCrownModule &digitalCrownModule = DigitalCrownModule::GetInstance();
        digitalCrownModule.Clear();
    }
private:
    DigitalCrownEventsModule() = default;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_DIGITAL_CROWN_MODULE_H
