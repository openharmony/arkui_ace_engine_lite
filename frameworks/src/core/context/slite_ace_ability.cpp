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

#include "slite_ace_ability.h"
#include "js_async_work.h"

namespace OHOS {
namespace ACELite {
SliteAceAbility::SliteAceAbility(const char *bundleName) : SliteAbility(bundleName)
{
}

void SliteAceAbility::OnCreate(const Want &want)
{
    jsAbility_.Launch(want.appPath, want.element->bundleName, token_);
    SliteAbility::OnCreate(want);
}

#ifdef _MINI_MULTI_TASKS_
void SliteAceAbility::OnRestoreData(AbilitySlite::AbilitySavedData *data)
{
    jsAbility_.OnRestoreData(data);
    SliteAbility::OnRestoreData(data);
}
#endif

void SliteAceAbility::OnForeground(const Want &want)
{
    jsAbility_.Show();
    isBackground_ = false;
    SliteAbility::OnForeground(want);
}

void SliteAceAbility::OnBackground()
{
    isBackground_ = true;
    jsAbility_.Hide();
    SliteAbility::OnBackground();
}

#ifdef _MINI_MULTI_TASKS_
void SliteAceAbility::OnSaveData(AbilitySlite::AbilitySavedData *data)
{
    jsAbility_.OnSaveData(data);
    SliteAbility::OnSaveData(data);
}
#endif

void SliteAceAbility::OnDestroy()
{
    JsAsyncWork::SetAppQueueHandler(nullptr);
    // the TE owner will be JS application after JS application start up except for it's lying in background,
    // call render once to make sure the last TE message is processed properly
    if (!isBackground_) {
        jsAbility_.HandleRenderTick();
    }
    jsAbility_.TransferToDestroy();
    SliteAbility::OnDestroy();
}

void SliteAceAbility::HandleExtraMessage(const AbilitySlite::SliteAbilityInnerMsg &innerMsg)
{
    switch (innerMsg.msgId) {
        case AbilitySlite::SliteAbilityMsgId::BACKPRESSED:
            jsAbility_.BackPressed();
            break;
        case AbilitySlite::SliteAbilityMsgId::ASYNCWORK: {
            auto* work = reinterpret_cast<AsyncWork *>(innerMsg.data);
            JsAsyncWork::ExecuteAsyncWork(work);
            break;
        }
        case AbilitySlite::SliteAbilityMsgId::TE_EVENT:
            jsAbility_.HandleRenderTick();
            break;
        default:
            break;
    }
}
} // namespace ACELite
} // namespace OHOS