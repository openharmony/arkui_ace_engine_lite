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
 * @file gradient_parser_tdd_test.h
 * @brief Unit test declarations for the CSS linear-gradient() parser.
 *
 * Covers every public interface exposed by app_style_gradient_parser.h:
 *   - ParseLinearGradient        Main entry point (standard 0/45/90/135 degree angles,
 *                                two-color and multi-color stops, horizontal and vertical directions).
 *   - ParseDirectionOrAngle      Direction keywords plus deg/rad/turn angle units.
 *   - DirectionToCssAngle        Mapping from the eight direction keywords to CSS angles.
 *   - NormalizeDirectionKeywordToAngle  Rewrites a direction keyword into explicit "<deg>deg" syntax.
 *   - ParseColorStop             Color stop value and percentage offset parsing.
 *   - NormalizeColorStops        Offset normalization (interpolating omitted offsets,
 *                                enforcing a monotonically increasing sequence).
 *   - ClipColorStopsToGradientLine  W3C projection of out of range color stops
 *                                (negative and beyond 100% positions) onto the painted segment.
 *   - SplitGradientArgs          Bracket-depth aware argument splitting (commas inside rgba()).
 *   - ParseColorToColorType      Color parsing for named/hex/rgb/rgba notations.
 *   - Trim/ToLower               String helper utilities.
 */

#ifndef OHOS_ACELITE_GRADIENT_PARSER_TDD_TEST_H
#define OHOS_ACELITE_GRADIENT_PARSER_TDD_TEST_H

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
class GradientParserTddTest : public testing::Test {
#else
class GradientParserTddTest {
#endif
public:
    void SetUp() {}
    void TearDown() {}

    /* Standard angle gradients, including the diagonals. */
    void ParseAngle0Deg001();
    void ParseAngle45Deg002();
    void ParseAngle90Deg003();
    void ParseAngle135Deg004();
    void ParseKeywordDiagonal005();
    void ParseRadTurnAngle006();
    void ParseNegativeAndLargeAngle007();
    void DirectionToCssAngle008();

    /* Multi color gradients and direction keywords. */
    void ParseTwoColor009();
    void ParseThreeColor010();
    void ParseFiveColorWithOffsets011();
    void ParseHorizontalDirection012();
    void ParseVerticalDirection013();
    void ParseDefaultDirection014();
    void ParseMaxColorsTruncated015();

    /* Boundary conditions and error handling. */
    void ParseNullOutput016();
    void ParseWrongPrefix017();
    void ParseMissingCloseParen018();
    void ParseTooFewColors019();
    void ParseInvalidColorSkipped020();
    void ParseInvalidAngleRejected021();

    /* Helper functions. */
    void ParseColorFormats022();
    void ParseColorInvalid023();
    void ParseColorStopOffset024();
    void NormalizeColorStops025();
    void NormalizeMonotonic026();
    void SplitGradientArgsRgba027();
    void TrimToLower028();

    /* Shared direction/angle helpers extracted during the gradient refactor. */
    void ParseDirectionOrAngleKeywords029();
    void ParseDirectionOrAngleRejectsColor030();
    void NormalizeDirectionKeywordToAngle031();

    /* Illegal direction value handling: silent disable of the gradient. */
    void ParseIllegalDirectionToNowhere032();
    void ParseIllegalDirectionMeaningless033();
    void ParseIllegalDirectionUppercaseSpaces034();

    /* W3C out of range color stops (CSS Images Module Level 3, 3.4.1). */
    void ClipOutOfRangeStops035();
    void ClipKeepsInnerStops036();
    void ClipPreservesInRangeRamp037();

    /* W3C <angle> validity: illegal angles and missing params reject the whole
     * declaration; valid boundary angles (0/360/negative/over-360) are accepted. */
    void ParseInvalidAngleVariants039();
    void ParseInvalidAngleMissingParam040();
    void ParseValidAngleBoundary041();

    /* W3C: a gradient function token split by whitespace ("linear-  gradient(...)")
     * breaks tokenization and must be rejected as an invalid declaration. */
    void ParseInvalidGradientBrokenToken042();

#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_GRADIENT_PARSER_TDD_TEST_H
