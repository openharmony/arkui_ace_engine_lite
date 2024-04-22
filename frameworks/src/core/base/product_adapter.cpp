/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "product_adapter.h"

#include "acelite_config.h"
#include "graphic_config.h"
#include "js_async_work.h"
#include "message_queue_utils.h"
#include "module_manager.h"
#include "securec.h"
#include "js_app_context.h"
#include "global.h"
#include "string_util.h"

namespace OHOS {
namespace ACELite {
/**
 * Used for holding all the related dfx interfaces assigned from specific implementation.
 */
struct DFXWrapper {
    DFXWrapper()
        : eventTag(0),
          eventSubTag(0),
          errCodeTag(0),
          errCodeSubTag(0),
          eventPrintHandler(nullptr),
          errCodePrintHandler(nullptr),
          jsLogOutputHandler(nullptr),
          nativeMemInfoGetter(nullptr)
    {
    }
    uint8_t eventTag;
    uint8_t eventSubTag;
    uint8_t errCodeTag;
    uint8_t errCodeSubTag;
    EventPrintHandler eventPrintHandler;
    ErrCodePrintHandler errCodePrintHandler;
    JSLogOutputHandler jsLogOutputHandler;
    NativeMemInfoGetter nativeMemInfoGetter;
};

static DFXWrapper g_dfxWrapper;
static TEHandlingHooks g_teHandlingHooks = {nullptr, nullptr};
static TerminateAbilityHandler g_termiantingHandler = nullptr;
static SetScreenOnVisibleHandler g_setScreenOnHandler = nullptr;
static UpdateDefaultFontHandler g_updateDefaultFontHandler = nullptr;
static ExtraPresetModulesHook g_extraPresetModulesHooks = {nullptr, nullptr};
static RestoreSystemHandler g_restoreSystemHandler = nullptr;
static IsPNGSupportedHandler g_isPNGSupportedHandler = nullptr;
static SetViewsParaHandler g_setViewsParaHandler = nullptr;
// default font styles
static char *g_defaultFontFamilyName = nullptr;
static uint8_t g_defaultFontSize = 30;
static uint16_t g_screenWidth = 454;
static uint16_t g_screenHeight = 454;

// default app private data root path
const static char *DEFAULT_APP_DATA_PATH = "user/ace/data/";
static const char *DEFAULT_DATA_ROOT_PATH = DEFAULT_APP_DATA_PATH;

// default device info
const static uint8_t DEVICE_TYPE_STR_LEN = 24;
const static char *DEFAULT_DEVICE_TYPE_NAME = "smartVision";
// smartVision as default
static const char *DEVICE_TYPE = DEFAULT_DEVICE_TYPE_NAME;

// indicating if the ace application is on foreground
static bool g_isRenderTickAcceptable = false;

void ProductAdapter::InitAceTags(uint8_t *aceTags, uint8_t tagCount)
{
    const uint8_t minCount = 4;
    if (aceTags == nullptr || tagCount < minCount) {
        return;
    }
    uint8_t index = 0;
    g_dfxWrapper.eventTag = aceTags[index++];
    g_dfxWrapper.eventSubTag = aceTags[index++];
    g_dfxWrapper.errCodeTag = aceTags[index++];
    g_dfxWrapper.errCodeSubTag = aceTags[index++];
}

void ProductAdapter::InitTraceHandlers(EventPrintHandler eventHandler, ErrCodePrintHandler errCodeHandler)
{
    g_dfxWrapper.eventPrintHandler = eventHandler;
    g_dfxWrapper.errCodePrintHandler = errCodeHandler;
}

void ProductAdapter::InitConsoleNativeHandler(JSLogOutputHandler handler)
{
    g_dfxWrapper.jsLogOutputHandler = handler;
}

void ProductAdapter::InitNativeMemPoolHook(NativeMemInfoGetter getter)
{
    g_dfxWrapper.nativeMemInfoGetter = getter;
}

void ProductAdapter::InitExtraModulesGetter(ProductModulesGetter productModuleGetter,
                                            PrivateModulesGetter privateModuleGetter)
{
    ModuleManager* moduleManager = ModuleManager::GetInstance();
    if (moduleManager == nullptr) {
        return;
    }
    moduleManager->SetProductModulesGetter(productModuleGetter);
    moduleManager->SetPrivateModulesGetter(privateModuleGetter);
}

void ProductAdapter::PrintEventTrace(uint8_t info2, uint8_t info3, uint8_t info4)
{
    if (g_dfxWrapper.eventPrintHandler == nullptr || g_dfxWrapper.eventTag == 0 || g_dfxWrapper.eventSubTag == 0) {
        return;
    }

    uint8_t subTag = (info2 == 0) ? g_dfxWrapper.eventSubTag : info2;
    g_dfxWrapper.eventPrintHandler(g_dfxWrapper.eventTag, subTag, info3, info4);
}

void ProductAdapter::PrintErrCode(uint8_t info2, uint16_t rfu)
{
    if (g_dfxWrapper.errCodePrintHandler == nullptr || g_dfxWrapper.errCodeTag == 0 ||
        g_dfxWrapper.errCodeSubTag == 0) {
        return;
    }
    g_dfxWrapper.errCodePrintHandler(g_dfxWrapper.errCodeTag, g_dfxWrapper.errCodeSubTag, info2, rfu);
}

void ProductAdapter::OutputJSConsoleLog(uint8_t level, const char *content)
{
    if (g_dfxWrapper.jsLogOutputHandler == nullptr) {
        return;
    }
    g_dfxWrapper.jsLogOutputHandler(level, content);
}

void ProductAdapter::GetNativeMemInfo(NativeMemInfo *memInfo)
{
    if (g_dfxWrapper.nativeMemInfoGetter == nullptr) {
        return;
    }

    g_dfxWrapper.nativeMemInfoGetter(memInfo);
}

void ProductAdapter::RegTerminatingHandler(TerminateAbilityHandler handler)
{
    g_termiantingHandler = handler;
}

void ProductAdapter::RegTEHandlers(const TEHandlingHooks &teHandlingHooks)
{
    g_teHandlingHooks.renderTEHandler = teHandlingHooks.renderTEHandler;
    g_teHandlingHooks.renderEndHandler = teHandlingHooks.renderEndHandler;
}

void ProductAdapter::RegRestoreSystemHandler(RestoreSystemHandler handler)
{
    g_restoreSystemHandler = handler;
}

void ProductAdapter::RegIsPNGSupportedHandler(IsPNGSupportedHandler handler)
{
    g_isPNGSupportedHandler = handler;
}

void ProductAdapter::RegSetViewsParaHandler(SetViewsParaHandler handler)
{
    g_setViewsParaHandler = handler;
}
// NOTE: This TE function will be called in VSYNC interrupt, and
// as no any task can be switched to during an interrupt, so it's safe to
// read the global value directly here.
TEDispatchingResult ProductAdapter::DispatchTEMessage()
{
#if (OHOS_ACELITE_PRODUCT_WATCH == 1) // only some specific products support TE dispatching
    if (!g_isRenderTickAcceptable) {
        return TEDispatchingResult::REFUSED;
    }

    if (JsAsyncWork::DispatchToLoop(TE_EVENT, nullptr)) {
        return TEDispatchingResult::ACCEPTED;
    }
    // if the ACE application is on foreground and the dispatching failed, should retry sending to ACE again
    return TEDispatchingResult::ACCEPT_FAILED;
#else
    return TEDispatchingResult::REFUSED;
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

void ProductAdapter::SendTerminatingRequest(uint32_t token, bool forceStop)
{
    if (g_termiantingHandler != nullptr) {
        g_termiantingHandler(token, forceStop);
    }
}

bool ProductAdapter::IsTEHandlersRegisted()
{
    return (g_teHandlingHooks.renderTEHandler != nullptr);
}

void ProductAdapter::ProcessOneTE()
{
    if (g_teHandlingHooks.renderTEHandler != nullptr) {
        (void)(g_teHandlingHooks.renderTEHandler());
    }
}

void ProductAdapter::NotifyRenderEnd()
{
    if (g_teHandlingHooks.renderEndHandler != nullptr) {
        g_teHandlingHooks.renderEndHandler();
    }
}

void ProductAdapter::SetDefaultFontStyle(const char *defaultFontFamily, uint8_t defaultFontSize)
{
    g_defaultFontFamilyName = const_cast<char *>(defaultFontFamily);
    g_defaultFontSize = defaultFontSize;
}

const char *ProductAdapter::GetDefaultFontFamilyName()
{
    return (g_defaultFontFamilyName != nullptr) ? g_defaultFontFamilyName : DEFAULT_VECTOR_FONT_FILENAME;
}

uint8_t ProductAdapter::GetDefaultFontSize()
{
    return g_defaultFontSize;
}

void ProductAdapter::UpdateRenderTickAcceptable(bool acceptable)
{
    g_isRenderTickAcceptable = acceptable;
}

void ProductAdapter::SetScreenSize(uint16_t width, uint16_t height)
{
    g_screenWidth = width;
    g_screenHeight = height;
}

void ProductAdapter::GetScreenSize(uint16_t &width, uint16_t &height)
{
    width = g_screenWidth;
    height = g_screenHeight;
}

void ProductAdapter::RegSetScreenOnVisibleHandler(SetScreenOnVisibleHandler handler)
{
    g_setScreenOnHandler = handler;
}

void ProductAdapter::RegUpdateDefaultFontHandler(UpdateDefaultFontHandler handler)
{
    g_updateDefaultFontHandler = handler;
}

bool ProductAdapter::SetScreenOnVisible(bool visible)
{
    return (g_setScreenOnHandler != nullptr) ? g_setScreenOnHandler(visible) : false;
}

bool ProductAdapter::UpdateDefaultFont()
{
    if (g_updateDefaultFontHandler == nullptr) {
        return false;
    }
    char *currentLanguage = static_cast<char *>(ace_malloc(MAX_LANGUAGE_LENGTH));
    char *currentOrigion = static_cast<char *>(ace_malloc(MAX_REGION_LENGTH));
    if ((currentLanguage == nullptr) || (currentOrigion == nullptr)) {
        ACE_FREE(currentLanguage);
        ACE_FREE(currentOrigion);
        return false;
    }
    GLOBAL_GetLanguage(currentLanguage, MAX_LANGUAGE_LENGTH);
    GLOBAL_GetRegion(currentOrigion, MAX_REGION_LENGTH);

    uint8_t addedLen = 7; // the length of '-', ".json" and '\0'
    uint8_t langLen = strlen(currentLanguage);
    size_t fileLen = langLen + strlen(currentOrigion) + addedLen;
    char *languageFileName = StringUtil::Malloc(fileLen);
    if (languageFileName == nullptr) {
        ACE_FREE(languageFileName);
        ACE_FREE(currentLanguage);
        ACE_FREE(currentOrigion);
        return false;
    }
    
    errno_t error = strcpy_s(languageFileName, fileLen, currentLanguage);
    languageFileName[langLen] = '-';
    languageFileName[langLen + 1] = '\0';
    error += strcat_s(languageFileName, fileLen, currentOrigion);
    error += strcat_s(languageFileName, fileLen, ".json");
    if (error > 0) {
        ACE_FREE(languageFileName);
        ACE_FREE(currentLanguage);
        ACE_FREE(currentOrigion);
        return false;
    }
    const char * const filePath = JsAppContext::GetInstance()->GetCurrentAbilityPath();
    const char * const folderName = "i18n";
    char *curLanguageFilePath = RelocateResourceFilePath(filePath, folderName);
    curLanguageFilePath = RelocateResourceFilePath(curLanguageFilePath, languageFileName);
    ACE_FREE(languageFileName);

    bool hasJson = IsFileExisted(curLanguageFilePath);
    ACE_FREE(curLanguageFilePath);
    g_updateDefaultFontHandler(currentLanguage, currentOrigion, hasJson);
    ACE_FREE(currentLanguage);
    ACE_FREE(currentOrigion);
    return true;
}

void ProductAdapter::RegExtraPresetModulesHook(ExtraPresetModulesHook hook)
{
    g_extraPresetModulesHooks.loadingHandler = hook.loadingHandler;
    g_extraPresetModulesHooks.unloadingHandler = hook.unloadingHandler;
}

void ProductAdapter::LoadExtraPresetModules()
{
    if (g_extraPresetModulesHooks.loadingHandler != nullptr) {
        g_extraPresetModulesHooks.loadingHandler();
    }
}

void ProductAdapter::UnloadExtraPresetModules()
{
    if (g_extraPresetModulesHooks.unloadingHandler != nullptr) {
        g_extraPresetModulesHooks.unloadingHandler();
    }
}

void ProductAdapter::ConfigPrivateDataRootPath(const char *appDataRoot)
{
    if (appDataRoot == nullptr) {
        return;
    }
    size_t pathLen = strlen(appDataRoot);
    if (pathLen == 0 || pathLen >= UINT8_MAX) {
        return;
    }
    DEFAULT_DATA_ROOT_PATH = appDataRoot;
}

const char *ProductAdapter::GetPrivateDataRootPath()
{
    return DEFAULT_DATA_ROOT_PATH;
}

void ProductAdapter::InitDeviceInfo(const char *deviceType)
{
    if (deviceType == nullptr || (strlen(deviceType) == 0) || strlen(deviceType) >= DEVICE_TYPE_STR_LEN) {
        return;
    }
    DEVICE_TYPE = deviceType;
}

const char *ProductAdapter::GetDeviceType()
{
    return DEVICE_TYPE;
}

void ProductAdapter::RestoreSystemWrapper(const char *crashMessage)
{
    if (g_restoreSystemHandler != nullptr) {
        g_restoreSystemHandler(crashMessage);
    }
}

bool ProductAdapter::IsPNGSupportedWrapper(const char *imagePath, const char *bundleName)
{
    return (g_isPNGSupportedHandler != nullptr) ? g_isPNGSupportedHandler(imagePath, bundleName) : false;
}

void ProductAdapter::SetViewsParaWrapper(void *ComponentHandler)
{
    if (g_setViewsParaHandler != nullptr) {
        g_setViewsParaHandler(ComponentHandler);
    }
}
} // namespace ACELite
} // namespace OHOS
