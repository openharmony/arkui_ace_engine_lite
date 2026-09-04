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

#include "app_data_module.h"

#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_fwk_common.h"

namespace OHOS {
namespace ACELite {
const char * const AppDataModule::getAppMethodName = "getApp";
const char * const AppDataModule::getAppDataStr = "data";
void AppDataModule::Init()
{
    CreateNamedFunction(getAppMethodName, GetApp);
}

jerry_value_t AppDataModule::GetApp(const jerry_value_t func,
                                    const jerry_value_t context,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum)
{
    (void)func;
    (void)context;
    (void)args;

    if (argsNum != 0) {
        return UNDEFINED;
    }

    const JSAbilityImpl *abilityImpl = JsAppContext::GetInstance()->GetTopJSAbilityImpl();
    if (!abilityImpl) {
        return UNDEFINED;
    }

    jerry_value_t abilityVM = abilityImpl->GetAppVM(); // do not release here
    if (IS_UNDEFINED(abilityVM)) {
        return UNDEFINED;
    }

    jerry_value_t dataObj = jerryx_get_property_str(abilityVM, getAppDataStr);
    if (IS_UNDEFINED(dataObj)) {
        return UNDEFINED;
    }
    jerry_release_value(dataObj);

#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    // getApp() returns a reference borrowed from JSAbilityImpl, but a native handler's return
    // value is owned by the VM and freed when the frame is torn down. Without this acquire each
    // getApp() call drops one $app reference that was never granted: the count walks past zero,
    // wraps, and the next ecma_ref_object() kills the process with ERR_REF_COUNT_LIMIT
    // ("JS REF LIMIT").
    jerry_acquire_value(abilityVM);
#endif // ENABLE_PAGE_TRANSITION_EFFECT
    return abilityVM;
}
} // namespace ACELite
} // namespace OHOS
