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

#ifndef OHOS_ACELITE_SLITE_ACE_ABILITY_H
#define OHOS_ACELITE_SLITE_ACE_ABILITY_H

#include "js_ability.h"
#include "slite_ability.h"

namespace OHOS {
namespace ACELite {
class SliteAceAbility final : public SliteAbility {
public:
    explicit SliteAceAbility(const char *bundleName);
    virtual ~SliteAceAbility() = default;

    void OnCreate(const Want &want) override;
#ifdef _MINI_MULTI_TASKS_
    void OnRestoreData(AbilitySlite::AbilitySavedData *data) override;
    void OnSaveData(AbilitySlite::AbilitySavedData *data) override;
#endif
    void OnForeground(const Want &want) override;
    void OnBackground() override;
    void OnDestroy() override;
    void HandleExtraMessage(const AbilitySlite::SliteAbilityInnerMsg &innerMsg) override;

private:
    JSAbility jsAbility_;
    bool isBackground_ = false;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_SLITE_ACE_ABILITY_H
