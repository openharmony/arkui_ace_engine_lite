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

#ifndef OHOS_ACELITE_OHOS_MODULE_CONFIG_H
#define OHOS_ACELITE_OHOS_MODULE_CONFIG_H

#include "acelite_config.h"
#include "jsi_types.h"

namespace OHOS {
namespace ACELite {
#if (ENABLE_MODULE_REQUIRE_TEST == 1)
extern void InitSampleModule(JSIValue exports);
#endif
extern void InitRouterModule(JSIValue exports);
extern void InitAppModule(JSIValue exports);
#if (FEATURE_SUPPORT_HTTP == 1)
extern void InitFetchModule(JSIValue exports);
#endif // FEATURE_SUPPORT_HTTP
#if (FEATURE_MODULE_AUDIO == 1)
extern void InitAudioModule(JSIValue exports);
#endif // FEATURE_MODULE_AUDIO
#if (FEATURE_ACELITE_DFX_MODULE == 1)
extern void InitDfxModule(JSIValue exports);
#endif // FEATURE_ACELITE_DFX_MODULE
#if (ENABLE_MODULE_CIPHER == 1)
extern void InitCipherModule(JSIValue exports);
#endif
#if (FEATURE_MODULE_DIALOG == 1)
extern void InitDialogModule(JSIValue exports);
#endif // FEATURE_MODULE_DIALOG

#if (FEATURE_MODULE_STORAGE == 1)
extern void InitNativeApiFs(JSIValue exports);
extern void InitNativeApiKv(JSIValue exports);
#endif

#if (FEATURE_MODULE_DEVICE == 1)
extern void InitDeviceModule(JSIValue exports);
extern void InitDeviceInfoModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_GEO == 1)
extern void InitLocationModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_SENSOR == 1)
extern void InitVibratorModule(JSIValue exports);
extern void InitSensorModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_BRIGHTNESS == 1)
extern void InitBrightnessModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_BATTERY == 1)
extern void InitBatteryModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_CONFIGURATION == 1)
extern void InitLocaleModule(JSIValue exports);
#endif

#if (FEATURE_ACELITE_SYSTEM_CAPABILITY == 1)
extern void InitCapabilityModule(JSIValue exports);
#endif

#if (ENABLE_MODULE_DM_LITE == 1)
extern void InitDeviceManagerModule(JSIValue exports);
#endif

#if (FEATURE_MODULE_OHOS_HILOG == 1)
extern void InitHilogModule(JSIValue exports);
#endif

#if (FEATURE_COMPONENT_DEVICE_ATTEST == 1)
extern void InitDeviceAttestModule(JSIValue exports);
#endif

#if (FEATURE_ACELITE_HUKS == 1)
extern void InitHuksModule(JSIValue exports);
#endif

// Config information for built-in JS modules of OHOS platform
const Module OHOS_MODULES[] = {
#if (ENABLE_MODULE_REQUIRE_TEST == 1)
    {"sample", InitSampleModule},
#endif
    {"app", InitAppModule},
#if (FEATURE_SUPPORT_HTTP == 1)
    {"fetch", InitFetchModule},
#endif // FEATURE_SUPPORT_HTTP
#if (FEATURE_MODULE_AUDIO == 1)
    {"audio", InitAudioModule},
#endif // FEATURE_MODULE_AUDIO
#if (FEATURE_ACELITE_DFX_MODULE == 1)
    {"dfx", InitDfxModule},
#endif // FEATURE_ACELITE_DFX_MODULE
    {"router", InitRouterModule},
#if (ENABLE_MODULE_CIPHER == 1)
    {"cipher", InitCipherModule},
#endif
#if (FEATURE_MODULE_DIALOG == 1)
    {"prompt", InitDialogModule},
#endif // FEATURE_MODULE_DIALOG

#if (FEATURE_MODULE_STORAGE == 1)
    {"file", InitNativeApiFs},
    {"storage", InitNativeApiKv},
#endif
#if (FEATURE_MODULE_DEVICE == 1)
    {"device", InitDeviceModule},
    {"deviceInfo", InitDeviceInfoModule},
#endif
#if (FEATURE_MODULE_GEO == 1)
    {"geolocation", InitLocationModule},
#endif
#if (FEATURE_MODULE_SENSOR == 1)
    {"vibrator", InitVibratorModule},
    {"sensor", InitSensorModule},
#endif
#if (FEATURE_MODULE_BRIGHTNESS == 1)
    {"brightness", InitBrightnessModule},
#endif
#if (FEATURE_MODULE_BATTERY == 1)
    {"battery", InitBatteryModule},
#endif
#if (FEATURE_MODULE_CONFIGURATION == 1)
    {"configuration", InitLocaleModule},
#endif
#if (FEATURE_ACELITE_SYSTEM_CAPABILITY == 1)
    {"capability", InitCapabilityModule},
#endif
#if (ENABLE_MODULE_DM_LITE == 1)
    {"devicemanager", InitDeviceManagerModule},
#endif
#if (FEATURE_MODULE_OHOS_HILOG == 1)
    {"hilog", InitHilogModule},
#endif
#if (FEATURE_COMPONENT_DEVICE_ATTEST == 1)
    {"deviceAttest", InitDeviceAttestModule},
#endif
#if (FEATURE_ACELITE_HUKS == 1)
    {"security.huks", InitHuksModule},
#endif
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_OHOS_MODULE_CONFIG_H
