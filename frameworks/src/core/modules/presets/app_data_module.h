/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_APP_DATA_MODULE_H
#define OHOS_ACELITE_APP_DATA_MODULE_H

#include "acelite_config.h"
#include "js_config.h"
#include "non_copyable.h"
#include "presets/preset_module.h"

namespace OHOS {
namespace ACELite {
class AppDataModule final : public PresetModule {
public:
    ACE_DISALLOW_COPY_AND_MOVE(AppDataModule);

    /**
     * @fn AppDataModule::AppDataModule()
     *
     * @brief Constructor.
     */
    AppDataModule() : PresetModule(nullptr) {}

    /**
     * @fn AppDataModule::~AppDataModule()
     *
     * @brief Constructor.
     */
    ~AppDataModule() = default;

    static void Load()
    {
        AppDataModule appDataModule;
        appDataModule.Init();
    }

    void Init() override;

private:
    static jerry_value_t GetApp(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);
    static const char * const GETAPP_METHOD_NAME;
    static const char * const GETAPP_DATA_STR;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_DATA_MODULE_H
