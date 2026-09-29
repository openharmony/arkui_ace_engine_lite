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

/**
 * @file gradient_info_tdd_test.h
 * @brief Unit test declarations for the GradientInfo data contract.
 *
 * Covers every public behaviour of the cross repository contract defined in
 * gradient_info.h:
 *   - SetColorStops: nominal (two color / multi color) and boundary cases
 *     (nullptr, count < 2, truncation above the hard limit, re-entry).
 *   - Direction and angle survive a SetColorStops call (regression test for the
 *     angle being silently dropped by Clear()).
 *   - DeepCopy independence, move semantics (construction and assignment) and
 *     Clear() reset.
 *   - GetBeginColor / GetEndColor / GetDirectionValue legacy adapters.
 *   - Shared angle helpers extracted during the refactoring:
 *     CssGradientDirectionToAngle, NormalizeCssAngleDegrees, ResolveCssAngle.
 */

#ifndef OHOS_ACELITE_GRADIENT_INFO_TDD_TEST_H
#define OHOS_ACELITE_GRADIENT_INFO_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class GradientInfoTddTest : public testing::Test {
#else
class GradientInfoTddTest {
#endif
public:
    void SetUp() {}
    void TearDown() {}

    void DefaultState001();
    void SetTwoColorStops002();
    void SetMultiColorStops003();
    void SetColorStopsNull004();
    void SetColorStopsLessThanTwo005();
    void SetColorStopsMaxLimit006();
    void DirectionAnglePreserved007();
    void GetBeginEndColor008();
    void GetBeginEndColorInvalid009();
    void GetDirectionValue010();
    void DeepCopyIndependence011();
    void DeepCopyInvalidSource012();
    void MoveConstructor013();
    void ClearResets014();
    void SharedDirectionAngleTable015();
    void NormalizeCssAngleDegrees016();
    void ResolveCssAngleKeyword017();
    void ResolveCssAngleCustom018();
    void MoveAssignment019();
    void SetColorStopsReentry020();

#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_GRADIENT_INFO_TDD_TEST_H
