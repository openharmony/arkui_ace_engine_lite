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
#include "router_module.h"
#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include "js_fwk_common.h"
#endif
#include "js_profiler.h"
#include "jsi/internal/jsi_internal.h"
#include "jsi.h"
#include "jsi_types.h"
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include "page_transition.h"
#endif

namespace OHOS {
namespace ACELite {
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
// ---- page-transition animation feature (compiled only when the gate is ON) ----
namespace {
constexpr uint16_t TRANSITION_DURATION_DEFAULT = 300;
constexpr uint16_t TRANSITION_DURATION_MAX = 5000;

PageTransitionType ParseTransitionType(const char * const typeStr)
{
    if (typeStr == nullptr) {
        return PageTransitionType::UNKNOWN;
    }
    if (strcmp(typeStr, "fade") == 0) {
        return PageTransitionType::FADE;
    }
    if (strcmp(typeStr, "scale") == 0) {
        return PageTransitionType::SCALE;
    }
    if (strcmp(typeStr, "slide_left") == 0) {
        return PageTransitionType::SLIDE_LEFT;
    }
    if (strcmp(typeStr, "slide_right") == 0) {
        return PageTransitionType::SLIDE_RIGHT;
    }
    if (strcmp(typeStr, "slide_up") == 0) {
        return PageTransitionType::SLIDE_UP;
    }
    if (strcmp(typeStr, "slide_down") == 0) {
        return PageTransitionType::SLIDE_DOWN;
    }
    if (strcmp(typeStr, "slide_over_left") == 0) {
        return PageTransitionType::SLIDE_OVER_LEFT;
    }
    if (strcmp(typeStr, "slide_over_right") == 0) {
        return PageTransitionType::SLIDE_OVER_RIGHT;
    }
    if (strcmp(typeStr, "slide_over_up") == 0) {
        return PageTransitionType::SLIDE_OVER_UP;
    }
    if (strcmp(typeStr, "slide_over_down") == 0) {
        return PageTransitionType::SLIDE_OVER_DOWN;
    }
    return PageTransitionType::UNKNOWN;
}
} // namespace

PageTransitionConfig RouterModule::ParseAnimationConfig(JSIValue object)
{
    PageTransitionConfig config;
    jerry_value_t jObject = AS_JERRY_VALUE(object);
    jerry_value_t animation = jerryx_get_property_str(jObject, ROUTER_PAGE_ANIMATION);
    if (!jerry_value_is_object(animation)) {
        jerry_release_value(animation);
        return config;
    }
    jerry_value_t typeValue = jerryx_get_property_str(animation, ROUTER_PAGE_ANIMATION_TYPE);
    char *typeStr = MallocStringOf(typeValue);
    config.type = ParseTransitionType(typeStr);
    if (typeStr != nullptr) {
        ace_free(typeStr);
        typeStr = nullptr;
    }
    jerry_release_value(typeValue);
    if (config.type == PageTransitionType::UNKNOWN) {
        HILOG_WARN(HILOG_MODULE_ACE, "router.replace: invalid animation type, no transition");
    }
    jerry_value_t durationValue = jerryx_get_property_str(animation, ROUTER_PAGE_ANIMATION_DURATION);
    if (jerry_value_is_number(durationValue)) {
        double duration = jerry_get_number_value(durationValue);
        if (duration != duration) {
            // NaN matches no clamp branch and truncating it to an integer is undefined behaviour,
            // so degrade it to "no animation". ±Infinity is still handled by the clamps below
            // (-Inf -> 0, +Inf -> 5000).
            duration = 0;
        }
        if (duration < 0) {
            duration = 0;
        }
        if (duration > TRANSITION_DURATION_MAX) {
            duration = TRANSITION_DURATION_MAX;
        }
        // sub-millisecond values in (0, 1) truncate to 0, same semantics as duration=0 (no animation)
        config.duration = static_cast<uint16_t>(duration);
    } else {
        config.duration = TRANSITION_DURATION_DEFAULT;
    }
    jerry_release_value(durationValue);
    jerry_release_value(animation);
    return config;
}
#endif // ENABLE_PAGE_TRANSITION_EFFECT

void InitRouterModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "replace", RouterModule::Replace);
    JSI::SetModuleAPI(exports, "replaceUrl", RouterModule::Replace);
}

JSIValue RouterModule::Replace(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    if (argsNum != 1 || args == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Replace args invalid, args num(%{public}d).", argsNum);
        return JSI::CreateErrorWithCode(JSI_ERR_CODE_PARAM_CHECK_FAILED, "params should only be one object.");
    }
    jerry_value_t object = AS_JERRY_VALUE(args[0]);
    // router.replace({uri: 'About', params: {id:'1'}}
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    // an optional "animation" member triggers the page-transition effect, see ParseAnimationConfig
    PageTransitionConfig config = ParseAnimationConfig(args[0]);
#endif
    JsAppContext* appContext = JsAppContext::GetInstance();
    const JSAbilityImpl* topJsAbilityImpl = appContext->GetTopJSAbilityImpl();
    if (topJsAbilityImpl == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "topJsAbilityImpl is null.");
        return AS_JSI_VALUE(UNDEFINED);
    }
    // get top ability's router
    Router* router = const_cast<Router *>(topJsAbilityImpl->GetRouter());
    if (router == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "router is null.");
        return AS_JSI_VALUE(UNDEFINED);
    }
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
    // only a valid config (valid type and positive duration) enables the transition;
    // an invalid config keeps the original no-animation behavior.
    if (config.IsValid()) {
        router->SetTransitionConfig(config);
    } else {
        // no-animation request (e.g. a back navigation): clear any leftover config so this replace
        // is not accidentally animated by a stale transition.
        router->SetTransitionConfig(PageTransitionConfig());
    }
#endif
    jerry_value_t replaceResult = router->Replace(object);
    return AS_JSI_VALUE(replaceResult);
}
} // namespace ACELite
} // namespace OHOS
