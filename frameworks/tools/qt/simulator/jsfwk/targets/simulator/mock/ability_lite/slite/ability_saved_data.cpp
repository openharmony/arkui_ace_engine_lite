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
#ifdef _MINI_MULTI_TASKS_
#include "ability_saved_data.h"

namespace OHOS {
namespace AbilitySlite {

AbilitySavedData::AbilitySavedData() = default;

AbilitySavedData::~AbilitySavedData()
{
}

SavedResultCode AbilitySavedData::SetSavedData(const void *buffer, uint16_t bufferSize)
{
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::SetUserSavedData(const void *buffer, uint16_t bufferSize)
{
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::GetSavedData(void *buffer, uint16_t bufferSize, uint16_t *getDataSize)
{
    return SavedResultCode::SAVED_RESULT_OK;
}

SavedResultCode AbilitySavedData::GetUserSavedData(void *buffer, uint16_t bufferSize, uint16_t *getDataSize)
{
    return SavedResultCode::SAVED_RESULT_OK;
}

void AbilitySavedData::SetSavedResultCode(SavedResultCode code)
{
    savedResult = code;
}

SavedResultCode AbilitySavedData::GetSavedResultCode() const
{
    return savedResult;
}

void AbilitySavedData::Reset()
{
    savedDataSize = 0;
    userSavedDataSize = 0;
    savedResult = SavedResultCode::SAVED_RESULT_NO_DATA;
}
} // namespace AbilitySlite
} // namespace OHOS
#endif