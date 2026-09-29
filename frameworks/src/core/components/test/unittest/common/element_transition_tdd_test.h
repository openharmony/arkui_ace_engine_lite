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

#ifndef OHOS_ACELITE_TEST_ELEMENT_TRANSITION_H
#define OHOS_ACELITE_TEST_ELEMENT_TRANSITION_H

#if FEATURE_ELEMENT_TRANSITION

#include "basic_tdd_test.h"

namespace OHOS {
namespace ACELite {
class ElementTransitionTddTest : public BasicTddTest {
public:
    ElementTransitionTddTest() {}
    ~ElementTransitionTddTest() override {}

    // TransitionParser pure parsers
    void ParseTransitionEffectTest001();
    void ParseEasingFuncTest001();
    void ParseTransitionArgsFullTest001();
    void ParseTransitionArgsPartialTest001();
    void ParseTransitionArgsSharedNoRectsTest001();
    // style parsing (SetTransitionConfig)
    void TransitionEffectRegisterTest001();
    void TransitionDurationSyncTest001();
    void TransitionDurationAsyncTest001();
    void TransitionDurationNegativeTest001();
    // trigger flow
    void StartTransitionDirectElementTest001();
    void StartTransitionOptionsSharedTest001();
    void StartTransitionWithoutEffectTest001();
    void StaleParamsResetTest001();
    void StartTransitionTwiceTest001();
    void StartTransitionSameTargetSkipTest001();
    // design behavior guards
    void StartAnimationNoAutoTriggerTest001();
    void SharedElementNotOutgoingTest001();
    void RunTests();

private:
    // helpers around the transition test page (BUNDLE)
    JSValue GetRefDom(JSValue page, const char *ref) const;
    Component *GetRefComponent(JSValue page, const char *ref) const;
    void SetCharStyle(Component *component, const char *name, const char *value) const;
    void SetNumStyle(Component *component, const char *name, int32_t value) const;
    void TriggerWithArg(JSValue stageDom, jerry_value_t arg) const;
};
} // namespace ACELite
} // namespace OHOS

#endif // FEATURE_ELEMENT_TRANSITION
#endif // OHOS_ACELITE_TEST_ELEMENT_TRANSITION_H
