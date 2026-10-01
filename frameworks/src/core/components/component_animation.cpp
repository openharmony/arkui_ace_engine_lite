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

#if FEATURE_TRANSITION_ANIMATOR

#include "component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "keyframes_transition_impl.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {

bool Component::TryStartKeyframesTransition()
{
    /* the keyframes path takes over when a plan can be built from the staged name */
    UIView *rootView = GetComponentRootView();
    if ((rootView != nullptr) && (trans_ != nullptr) && (trans_->keyframesName != nullptr)) {
        DropKeyframesTransitionImpl();
        BuildKeyframesTransition(*rootView);
        if (keyframesTransitionImpl_ != nullptr) {
            isAnimationKeyFramesSet_ = false; // the feature executor takes over
            return true;
        }
    }
    return false;
}

void Component::StopAndReleaseCurrentTransition()
{
    if (curTransitionImpl_ == nullptr) {
        return;
    }
    curTransitionImpl_->Stop();
    RemoveAnimationFromList(curTransitionImpl_);
    delete curTransitionImpl_;
    curTransitionImpl_ = nullptr;
}

void Component::DropKeyframesTransitionImpl()
{
    if (keyframesTransitionImpl_ == nullptr) {
        return;
    }
    keyframesTransitionImpl_->Stop();
    RemoveKeyframesTransitionFromList(keyframesTransitionImpl_);
    delete keyframesTransitionImpl_;
    keyframesTransitionImpl_ = nullptr;
}

void Component::ReleaseKeyframesTransitionImpl()
{
    DropKeyframesTransitionImpl();
    if (trans_ != nullptr) {
        ACE_FREE(trans_->keyframesName);
    }
}

void Component::RemoveAnimationFromList(const TransitionImpl *transitionImpl) const
{
    AnimationsNode *prev = nullptr;
    AnimationsNode *node = Component::AnimationListHeadRef();
    while (node != nullptr) {
        if (node->transitionImpl == transitionImpl) {
            if (prev == nullptr) {
                Component::AnimationListHeadRef() = node->next;
            } else {
                prev->next = node->next;
            }
            delete node;
            return;
        }
        prev = node;
        node = node->next;
    }
}

void Component::AddKeyframesTransitionToList(KeyframesTransitionImpl *transition) const
{
    AnimationsNode *animation = new AnimationsNode();
    if (animation == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create animation node error for keyframes transition");
        return;
    }
    animation->keyframesTransitionImpl = transition;
    animation->next = Component::AnimationListHeadRef();
    Component::AnimationListHeadRef() = animation;
}

void Component::RemoveKeyframesTransitionFromList(const KeyframesTransitionImpl *transition) const
{
    AnimationsNode *prev = nullptr;
    AnimationsNode *node = Component::AnimationListHeadRef();
    while (node != nullptr) {
        if (node->keyframesTransitionImpl == transition) {
            if (prev == nullptr) {
                Component::AnimationListHeadRef() = node->next;
            } else {
                prev->next = node->next;
            }
            delete node;
            return;
        }
        prev = node;
        node = node->next;
    }
}

void Component::StageKeyframesName(const char *name)
{
    if ((name == nullptr) || (trans_ == nullptr)) {
        return;
    }
    if ((trans_->keyframesName != nullptr) && (strcmp(trans_->keyframesName, name) == 0)) {
        return; // unchanged
    }
    ACE_FREE(trans_->keyframesName);
    size_t nameLen = strlen(name);
    trans_->keyframesName = reinterpret_cast<char *>(ace_malloc(sizeof(char) * (nameLen + 1)));
    if (trans_->keyframesName != nullptr) {
        if (strcpy_s(trans_->keyframesName, nameLen + 1, name) != 0) {
            ACE_FREE(trans_->keyframesName);
        }
    }
    /* name changed: drop the stale executor */
    DropKeyframesTransitionImpl();
}

void Component::BuildKeyframesTransition(UIView &uiView)
{
    const AppStyleSheet *styleSheet = GetStyleManager()->GetStyleSheet();
    if ((styleSheet == nullptr) || (trans_ == nullptr) || (trans_->keyframesName == nullptr)) {
        return;
    }
    /* the factory assembles the plan and creates+initializes the executor in one go;
       nullptr means "no valid feature plan" and the classic path takes over */
    keyframesTransitionImpl_ =
        KeyframesTransitionImpl::Build(*styleSheet, trans_->keyframesName, *trans_, &uiView);
    if (keyframesTransitionImpl_ == nullptr) {
        return;
    }
    AddKeyframesTransitionToList(keyframesTransitionImpl_);
    /* same timing as the classic path: start immediately only when the page has started */
    if (Component::IsAnimatorStarted()) {
        keyframesTransitionImpl_->Start();
    }
}

} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_TRANSITION_ANIMATOR

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

#if FEATURE_PATH_ANIMATOR

#include "component.h"
#include "ace_log.h"
#include "js_fwk_common.h"
#include "stylemgr/app_style_item.h"

namespace OHOS {
namespace ACELite {

static TransitionParams *EnsureTransitionParams(TransitionParams *&trans)
{
    if (trans == nullptr) {
        trans = new TransitionParams();
        if (trans == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "create TransitionParams object error");
        }
    }
    return trans;
}

void Component::SetAnimationStyle(const AppStyleItem *styleItem, const int16_t keyId)
{
    if (styleItem == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetAnimationStyle fail: style item is null");
        return;
    }

    switch (keyId) {
        case K_OFFSET_PATH: {
            /* the polyline was parsed once when the style item was created */
            const OHOS::PathPolyline *polyline = styleItem->GetPathPolyline();
            if (polyline == nullptr) {
                return;
            }
            if (EnsureTransitionParams(trans_) == nullptr) {
                return;
            }
            trans_->pathPoly = *polyline;
            trans_->transformType = const_cast<char *>(TRANSITION_OFFSET_PATH);
            break;
        }
        case K_OFFSET_ROTATE: {
            /* the (mode, degree) value was parsed once when the style item was created */
            OffsetRotateMode mode;
            int16_t degree;
            if (!styleItem->GetOffsetRotate(mode, degree)) {
                return;
            }
            if (EnsureTransitionParams(trans_) == nullptr) {
                return;
            }
            trans_->offsetRotateMode = mode;
            trans_->offsetRotate = degree;
            break;
        }
        default:
            break;
    }
}

void Component::SetOffsetDistanceKeyFrames(int32_t valueFrom, int32_t valueTo)
{
    trans_->offsetDistanceFrom = static_cast<int16_t>(valueFrom);
    trans_->offsetDistanceTo = static_cast<int16_t>(valueTo);
    isAnimationKeyFramesSet_ = true;
}

} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_PATH_ANIMATOR
