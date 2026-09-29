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

#include "component_gradient.h"

#include "component.h"

#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#include "securec.h"
#include "stylemgr/app_style.h"
#if (FEATURE_COMPONENT_GRADIENT == 1)
#include "stylemgr/app_style_gradient_parser.h"
#endif // FEATURE_COMPONENT_GRADIENT
#include "time_util.h"

namespace OHOS {
namespace ACELite {

#if (FEATURE_COMPONENT_GRADIENT == 1)
namespace {
/** Marker used to detect a gradient declaration before running the parser. */
const char* const LINEAR_GRADIENT_MARKER = "linear-gradient";
/** CSS value that removes the background image. */
const char* const BACKGROUND_IMAGE_NONE = "none";

using namespace OHOS::ACELite::CssGradientParser;

/**
 * @brief Case-insensitive substring test.
 *
 * Used to detect the gradient function name ("linear-gradient", the marker is
 * authored in lower case) regardless of how the style sheet spelled it, so every
 * casing variant (LINEAR-GRADIENT / Linear-Gradient / ...) is routed to the
 * gradient path. Each character is folded during the compare, which keeps the
 * test locale independent and avoids any heap allocation.
 */
bool ContainsCaseInsensitive(const std::string& text, const char* sub)
{
    if (sub == nullptr) {
        return false;
    }
    const size_t subLen = strlen(sub);
    if (text.length() < subLen) {
        return false;
    }
    const size_t last = text.length() - subLen;
    for (size_t start = 0; start <= last; ++start) {
        size_t i = 0;
        for (; i < subLen; ++i) {
            char tc = text[start + i];
            char sc = sub[i];
            if (tc >= 'A' && tc <= 'Z') {
                tc = static_cast<char>(tc + ('a' - 'A'));
            }
            if (sc >= 'A' && sc <= 'Z') {
                sc = static_cast<char>(sc + ('a' - 'A'));
            }
            if (tc != sc) {
                break;
            }
        }
        if (i == subLen) {
            return true;
        }
    }
    return false;
}
} // namespace

ComponentGradient::~ComponentGradient()
{
    Clear();
}

void ComponentGradient::Clear()
{
    if (info_ != nullptr) {
        delete info_;
        info_ = nullptr;
    }
}

void ComponentGradient::SyncToView(UIView& view) const
{
    /* No gradient, or a gradient that cannot be drawn: clear the view. */
    if (info_ == nullptr || !info_->isValid) {
        view.SetGradientInfo(nullptr);
        return;
    }

    GradientInfo* viewInfo = info_->DeepCopy();
    if (viewInfo == nullptr) {
        /* Allocation failed: keep the view as it is instead of blanking it,
         * a stale gradient is less disruptive than a sudden style loss. */
        HILOG_ERROR(HILOG_MODULE_ACE, "[GRADIENT] SyncToView: DeepCopy failed");
        return;
    }

    view.SetGradientInfo(viewInfo);
}

bool ComponentGradient::ApplyStyle(const std::string& cssValue, UIView& view)
{
    GradientInfo* parsed = new (std::nothrow) GradientInfo();
    if (parsed == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[GRADIENT] ApplyStyle: out of memory");
        return false;
    }

    bool parseOk = ParseLinearGradient(cssValue, parsed);
    if (!parseOk) {
        /*
         * Compatibility retry: rewrite a leading direction keyword into the
         * equivalent angle ("to right" -> "90deg") and parse once more. The
         * keyword table itself lives in the parser, this layer only asks for
         * the rewrite.
         */
        std::string normalizedCss;
        if (NormalizeDirectionKeywordToAngle(cssValue, normalizedCss) && normalizedCss != cssValue) {
            parseOk = ParseLinearGradient(normalizedCss, parsed);
        }
    }

    if (!parseOk) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[GRADIENT] ApplyStyle: cannot parse '%s'", cssValue.c_str());
        delete parsed;
        return false;
    }

    /* Single ownership swap for every success path. */
    Clear();
    info_ = parsed;

    SyncToView(view);
    return true;
}

bool ComponentGradient::TryHandleBackground(const char *value, UIView *view)
{
    const std::string cssValue(value);
    if (view == nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE,
                   "[GRADIENT] HandleBackgroundImg: no root view for '%s'", value);
        return true;
    }
    /* "background-image: none" clears any previously applied gradient. */
    if (cssValue.empty() || cssValue == BACKGROUND_IMAGE_NONE) {
        Clear();
        SyncToView(*view);
        return true;
    }
    /* Broken-token probe runs before the marker probe: a whitespace-split name */
    /* is never mistaken for a valid gradient. */
    if (IsBrokenGradientFunctionToken(cssValue)) {
        return true;
    }
    /* Gradient decl never resolves to an image path; return reports success. */
    if (ContainsCaseInsensitive(cssValue, LINEAR_GRADIENT_MARKER)) {
        (void)ApplyStyle(cssValue, *view);
        return true;
    }
    return false;
}

bool Component::HandleBackground(const AppStyleItem &styleItem)
{
    if (styleItem.GetValueType() != STYLE_PROP_VALUE_TYPE_STRING) {
        return false;
    }
    const char *value = styleItem.GetStrValue();
    if (value == nullptr) {
        return false;
    }
    bool hadGradient = gradient_.HasInfo();
    bool consumed = gradient_.TryHandleBackground(value, GetComponentRootView());
    /* tree-flag maintenance stays at the component layer: only ownership
       transitions (gained / cleared) require a subtree-flag recompute */
    if (consumed && (hadGradient != gradient_.HasInfo())) {
        RecomputeGradientSubtreeFlag();
    }
    return consumed;
}

void Component::RecomputeGradientSubtreeFlag()
{
    bool oldFlag = hasGradientInSubtree_;
    bool newFlag = gradient_.HasInfo();
    Component *child = const_cast<Component *>(GetChildHead());
    while (child != nullptr && !newFlag) {
        if (child->HasGradientInSubtree()) {
            newFlag = true;
            break;
        }
        child = const_cast<Component *>(child->GetNextSibling());
    }
    hasGradientInSubtree_ = newFlag;
    if (oldFlag != newFlag && parent_ != nullptr) {
        parent_->RecomputeGradientSubtreeFlag();
    }
}
#endif // FEATURE_COMPONENT_GRADIENT

} // namespace ACELite
} // namespace OHOS
