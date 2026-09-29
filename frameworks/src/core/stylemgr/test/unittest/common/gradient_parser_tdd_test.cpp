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

#include "gradient_parser_tdd_test.h"
#include <cmath>
#include "app_style_gradient_parser.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
namespace {
const float ANGLE_0 = 0.0f;
const float ANGLE_45 = 45.0f;
const float ANGLE_90 = 90.0f;
const float ANGLE_135 = 135.0f;
const float ANGLE_180 = 180.0f;
const float ANGLE_225 = 225.0f;
const float ANGLE_270 = 270.0f;
const float ANGLE_315 = 315.0f;
const float ANGLE_TOLERANCE = 0.01f;
const float OFFSET_START = 0.0f;
const float OFFSET_QUARTER = 0.25f;
const float OFFSET_HALF = 0.5f;
const float OFFSET_THREE_QUARTER = 0.75f;
const float OFFSET_END = 1.0f;
/* Out of range positions used by the W3C color stop clipping tests. */
const float OFFSET_BEFORE_START = -0.2f;
const float OFFSET_BEYOND_END = 1.5f;
const float OFFSET_TOLERANCE = 0.001f;
/* Colors W3C requires at the box edges for "#ff0000 -20%, #0000ff 150%". */
const uint8_t CLIPPED_START_RED = 0xE1;
const uint8_t CLIPPED_START_BLUE = 0x1E;
const uint8_t CLIPPED_END_RED = 0x4B;
const uint8_t CLIPPED_END_BLUE = 0xB4;
const uint8_t CHANNEL_TOLERANCE = 1;
const uint8_t FULL = 0xFF;
const uint8_t ZERO = 0x00;
const uint8_t ALPHA_HALF = 0x80;
const uint8_t TWO_COLORS = 2;
const uint8_t THREE_COLORS = 3;
const uint8_t FIVE_COLORS = 5;
} // namespace

using namespace CssGradientParser;

/* ==========================================================================
 * Standard angle linear gradients: 0 / 45 / 90 / 135 degrees, including diagonals.
 * ========================================================================== */

/**
 * @tc.name: ParseAngle0Deg001
 * @tc.desc: Parse a 0deg custom angle gradient (pointing straight up); the resolved
 *           angle is exact and the color stops are normalized.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseAngle0Deg001, TestSize.Level1)
#else
void GradientParserTddTest::ParseAngle0Deg001()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(0deg, red, blue)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.type, GradientType::LINEAR);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_0, ANGLE_TOLERANCE);
    ASSERT_EQ(info.colorCount, TWO_COLORS);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_END);
    EXPECT_EQ(info.colorStops[0].color.red, FULL);
    EXPECT_EQ(info.colorStops[1].color.blue, FULL);
}

/**
 * @tc.name: ParseAngle45Deg002
 * @tc.desc: Parse a 45deg diagonal gradient (bottom left to top right).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseAngle45Deg002, TestSize.Level1)
#else
void GradientParserTddTest::ParseAngle45Deg002()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(45deg, #ff0000, #0000ff)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_45, ANGLE_TOLERANCE);
    ASSERT_EQ(info.colorCount, TWO_COLORS);
    EXPECT_EQ(info.colorStops[0].color.full, 0xFFFF0000u);
    EXPECT_EQ(info.colorStops[1].color.full, 0xFF0000FFu);
}

/**
 * @tc.name: ParseAngle90Deg003
 * @tc.desc: Parse a 90deg gradient (horizontal, left to right).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseAngle90Deg003, TestSize.Level1)
#else
void GradientParserTddTest::ParseAngle90Deg003()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(90deg, red, blue)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_90, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseAngle135Deg004
 * @tc.desc: Parse a 135deg diagonal gradient (top left to bottom right).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseAngle135Deg004, TestSize.Level1)
#else
void GradientParserTddTest::ParseAngle135Deg004()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(135deg, red, blue)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_135, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseKeywordDiagonal005
 * @tc.desc: Diagonal direction keyword mapping: "to top right" is 45 degrees and
 *           "to bottom right" is 135 degrees, including the reversed word order
 *           synonyms "to right top" and "to right bottom".
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseKeywordDiagonal005, TestSize.Level1)
#else
void GradientParserTddTest::ParseKeywordDiagonal005()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act: "to top right" = 45 deg
    result = ParseLinearGradient("linear-gradient(to top right, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_TOP_RIGHT);
    EXPECT_NEAR(info.angle, ANGLE_45, ANGLE_TOLERANCE);

    // Act: "to bottom right" = 135 deg
    result = ParseLinearGradient("linear-gradient(to bottom right, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM_RIGHT);
    EXPECT_NEAR(info.angle, ANGLE_135, ANGLE_TOLERANCE);

    /* Reversed word order synonym. */
    // Act
    result = ParseLinearGradient("linear-gradient(to right bottom, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM_RIGHT);
}

/**
 * @tc.name: ParseRadTurnAngle006
 * @tc.desc: Angle unit parsing for rad and turn: 0.125turn is 45 degrees and
 *           pi/4 rad is approximately 45 degrees.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseRadTurnAngle006, TestSize.Level1)
#else
void GradientParserTddTest::ParseRadTurnAngle006()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act: 0.125turn = 45 deg
    result = ParseLinearGradient("linear-gradient(0.125turn, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_45, ANGLE_TOLERANCE);

    // Act: π/4 rad ≈ 45 deg
    result = ParseLinearGradient("linear-gradient(0.7853982rad, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_45, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseNegativeAndLargeAngle007
 * @tc.desc: Boundary case: negative angles and angles above 360 degrees are stored
 *           verbatim; wrapping is the drawing adapter's responsibility.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseNegativeAndLargeAngle007, TestSize.Level1)
#else
void GradientParserTddTest::ParseNegativeAndLargeAngle007()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act: negative angle stored verbatim
    result = ParseLinearGradient("linear-gradient(-45deg, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, -ANGLE_45, ANGLE_TOLERANCE);

    // Act: >360 angle stored verbatim
    result = ParseLinearGradient("linear-gradient(390deg, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_NEAR(info.angle, 390.0f, ANGLE_TOLERANCE);
}

/**
 * @tc.name: DirectionToCssAngle008
 * @tc.desc: Complete mapping from the eight direction keywords to CSS angles, plus
 *           the 180 degree fallback used for NONE and CUSTOM_ANGLE.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, DirectionToCssAngle008, TestSize.Level1)
#else
void GradientParserTddTest::DirectionToCssAngle008()
#endif
{
    // Arrange
    float angle;

    // Act: complete keyword-to-angle mapping + fallback
    angle = DirectionToCssAngle(CssGradientDirection::TO_TOP);
    // Assert
    EXPECT_FLOAT_EQ(angle, ANGLE_0);

    angle = DirectionToCssAngle(CssGradientDirection::TO_TOP_RIGHT);
    EXPECT_FLOAT_EQ(angle, ANGLE_45);

    angle = DirectionToCssAngle(CssGradientDirection::TO_RIGHT);
    EXPECT_FLOAT_EQ(angle, ANGLE_90);

    angle = DirectionToCssAngle(CssGradientDirection::TO_BOTTOM_RIGHT);
    EXPECT_FLOAT_EQ(angle, ANGLE_135);

    angle = DirectionToCssAngle(CssGradientDirection::TO_BOTTOM);
    EXPECT_FLOAT_EQ(angle, ANGLE_180);

    angle = DirectionToCssAngle(CssGradientDirection::TO_BOTTOM_LEFT);
    EXPECT_FLOAT_EQ(angle, ANGLE_225);

    angle = DirectionToCssAngle(CssGradientDirection::TO_LEFT);
    EXPECT_FLOAT_EQ(angle, ANGLE_270);

    angle = DirectionToCssAngle(CssGradientDirection::TO_TOP_LEFT);
    EXPECT_FLOAT_EQ(angle, ANGLE_315);

    /* Non directional enum values (NONE/CUSTOM_ANGLE) fall back to 180 degrees. */
    angle = DirectionToCssAngle(CssGradientDirection::NONE);
    EXPECT_FLOAT_EQ(angle, ANGLE_180);

    angle = DirectionToCssAngle(CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_FLOAT_EQ(angle, ANGLE_180);
}

/* ==========================================================================
 * Multi color gradients and horizontal / vertical direction keywords.
 * ========================================================================== */

/**
 * @tc.name: ParseTwoColor009
 * @tc.desc: Two color gradient parsing; the first and last stops carry the expected
 *           offsets and color values.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseTwoColor009, TestSize.Level1)
#else
void GradientParserTddTest::ParseTwoColor009()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(red, blue)", &info);

    // Assert
    ASSERT_TRUE(result);
    ASSERT_EQ(info.colorCount, TWO_COLORS);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_END);
    EXPECT_EQ(info.colorStops[0].color.full, 0xFFFF0000u); /* red */
    EXPECT_EQ(info.colorStops[1].color.full, 0xFF0000FFu); /* blue */
}

/**
 * @tc.name: ParseThreeColor010
 * @tc.desc: Three color gradient parsing; omitted offsets are interpolated to
 *           0 / 0.5 / 1.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseThreeColor010, TestSize.Level1)
#else
void GradientParserTddTest::ParseThreeColor010()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(red, lime, blue)", &info);

    // Assert
    ASSERT_TRUE(result);
    ASSERT_EQ(info.colorCount, THREE_COLORS);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_HALF); /* Middle stop is interpolated automatically. */
    EXPECT_FLOAT_EQ(info.colorStops[2].offset, OFFSET_END);
    EXPECT_EQ(info.colorStops[1].color.green, FULL);
}

/**
 * @tc.name: ParseFiveColorWithOffsets011
 * @tc.desc: Five color gradient with explicit percentage offsets; verifies both the
 *           color count adaptability and the offset parsing.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseFiveColorWithOffsets011, TestSize.Level1)
#else
void GradientParserTddTest::ParseFiveColorWithOffsets011()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient(
        "linear-gradient(to right, red 0%, yellow 25%, green 50%, cyan 75%, blue 100%)", &info);

    // Assert
    ASSERT_TRUE(result);
    ASSERT_EQ(info.colorCount, FIVE_COLORS);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_RIGHT);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_QUARTER);
    EXPECT_FLOAT_EQ(info.colorStops[2].offset, OFFSET_HALF);
    EXPECT_FLOAT_EQ(info.colorStops[3].offset, OFFSET_THREE_QUARTER);
    EXPECT_FLOAT_EQ(info.colorStops[4].offset, OFFSET_END);
    EXPECT_EQ(info.colorStops[1].color.full, 0xFFFFFF00u); /* yellow */
    EXPECT_EQ(info.colorStops[3].color.full, 0xFF00FFFFu); /* cyan */
}

/**
 * @tc.name: ParseHorizontalDirection012
 * @tc.desc: Horizontal gradients: "to right" is 90 degrees and "to left" is 270.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseHorizontalDirection012, TestSize.Level1)
#else
void GradientParserTddTest::ParseHorizontalDirection012()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act: "to right" = 90 deg
    result = ParseLinearGradient("linear-gradient(to right, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_RIGHT);
    EXPECT_NEAR(info.angle, ANGLE_90, ANGLE_TOLERANCE);

    // Act: "to left" = 270 deg
    result = ParseLinearGradient("linear-gradient(to left, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_LEFT);
    EXPECT_NEAR(info.angle, ANGLE_270, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseVerticalDirection013
 * @tc.desc: Vertical gradients: "to top" is 0 degrees and "to bottom" is 180.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseVerticalDirection013, TestSize.Level1)
#else
void GradientParserTddTest::ParseVerticalDirection013()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act: "to top" = 0 deg
    result = ParseLinearGradient("linear-gradient(to top, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_TOP);
    EXPECT_NEAR(info.angle, ANGLE_0, ANGLE_TOLERANCE);

    // Act: "to bottom" = 180 deg
    result = ParseLinearGradient("linear-gradient(to bottom, red, blue)", &info);
    // Assert
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM);
    EXPECT_NEAR(info.angle, ANGLE_180, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseDefaultDirection014
 * @tc.desc: When no direction is authored the CSS specification default
 *           "to bottom" (180 degrees) is used.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseDefaultDirection014, TestSize.Level1)
#else
void GradientParserTddTest::ParseDefaultDirection014()
#endif
{
    // Arrange
    GradientInfo info;

    // Act: no direction authored
    bool result = ParseLinearGradient("linear-gradient(red, blue)", &info);

    // Assert: CSS default "to bottom" (180 degrees)
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM);
    EXPECT_NEAR(info.angle, ANGLE_180, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseMaxColorsTruncated015
 * @tc.desc: Boundary case: 18 authored stops are truncated to
 *           LINEAR_GRADIENT_MAX_COLORS (16).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseMaxColorsTruncated015, TestSize.Level1)
#else
void GradientParserTddTest::ParseMaxColorsTruncated015()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient(
        "linear-gradient(red, green, blue, yellow, cyan, magenta, orange, purple, pink, gray,"
        " lime, navy, teal, maroon, olive, aqua, fuchsia, white)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.colorCount, LINEAR_GRADIENT_MAX_COLORS);
}

/* ==========================================================================
 * Boundary conditions and error handling.
 * ========================================================================== */

/**
 * @tc.name: ParseNullOutput016
 * @tc.desc: Error handling: a nullptr output pointer returns false without crashing.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseNullOutput016, TestSize.Level1)
#else
void GradientParserTddTest::ParseNullOutput016()
#endif
{
    // Act
    bool result = ParseLinearGradient("linear-gradient(red, blue)", nullptr);
    // Assert
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ParseWrongPrefix017
 * @tc.desc: Error handling: values that are not a linear-gradient() call
 *           (radial-gradient, a plain color, an empty string) return false.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseWrongPrefix017, TestSize.Level1)
#else
void GradientParserTddTest::ParseWrongPrefix017()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act & Assert: non-linear-gradient inputs are rejected
    result = ParseLinearGradient("radial-gradient(red, blue)", &info);
    EXPECT_FALSE(result);
    EXPECT_FALSE(info.isValid);

    result = ParseLinearGradient("red", &info);
    EXPECT_FALSE(result);

    result = ParseLinearGradient("", &info);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ParseCaseInsensitivePrefix038
 * @tc.desc: The function name is matched case-insensitively: LINEAR-GRADIENT and
 *           Linear-Gradient are accepted exactly like the canonical lowercase form,
 *           including the direction keyword and hex colors.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseCaseInsensitivePrefix038, TestSize.Level1)
#else
void GradientParserTddTest::ParseCaseInsensitivePrefix038()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act & Assert: case-insensitive function name matching

    result = ParseLinearGradient("LINEAR-GRADIENT(red, blue)", &info);
    ASSERT_TRUE(result);
    EXPECT_TRUE(info.isValid);

    result = ParseLinearGradient("Linear-Gradient(to right, red, blue)", &info);
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_RIGHT);

    result = ParseLinearGradient("LINEAR-GRADIENT(to right, #ff0000, #0000ff)", &info);
    ASSERT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_RIGHT);
    EXPECT_EQ(static_cast<unsigned>(info.colorCount), static_cast<unsigned>(2));
}

/**
 * @tc.name: ParseMissingCloseParen018
 * @tc.desc: Error handling: a missing closing parenthesis returns false.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseMissingCloseParen018, TestSize.Level1)
#else
void GradientParserTddTest::ParseMissingCloseParen018()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act & Assert: missing close paren returns false
    result = ParseLinearGradient("linear-gradient(red, blue", &info);
    EXPECT_FALSE(result);

    result = ParseLinearGradient("linear-gradient(", &info);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ParseTooFewColors019
 * @tc.desc: Boundary case: fewer than two valid color stops returns false.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseTooFewColors019, TestSize.Level1)
#else
void GradientParserTddTest::ParseTooFewColors019()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    // Act & Assert: fewer than two valid color stops returns false
    /* A single argument. */
    result = ParseLinearGradient("linear-gradient(red)", &info);
    EXPECT_FALSE(result);
    /* Two arguments but one of the colors is invalid. */
    result = ParseLinearGradient("linear-gradient(notacolor, blue)", &info);
    EXPECT_FALSE(result);
    /* Every color is invalid. */
    result = ParseLinearGradient("linear-gradient(notacolor, alsbad)", &info);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ParseInvalidColorSkipped020
 * @tc.desc: Fault tolerance: one invalid stop inside a multi color list is skipped
 *           while the remaining stops still parse.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseInvalidColorSkipped020, TestSize.Level1)
#else
void GradientParserTddTest::ParseInvalidColorSkipped020()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(to right, red, notacolor, blue)", &info);

    // Assert: invalid stop dropped, red and blue survive
    ASSERT_TRUE(result);
    ASSERT_EQ(info.colorCount, TWO_COLORS);
    EXPECT_EQ(info.colorStops[0].color.full, 0xFFFF0000u);
    EXPECT_EQ(info.colorStops[1].color.full, 0xFF0000FFu);
}

/**
 * @tc.name: ParseInvalidAngleRejected021
 * @tc.desc: W3C CSS Images compliance: a malformed angle such as "abcdeg" is not a
 *           valid <angle>, therefore the entire linear-gradient() declaration is invalid
 *           and ignored (no style generated) instead of degrading to a default direction.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseInvalidAngleRejected021, TestSize.Level1)
#else
void GradientParserTddTest::ParseInvalidAngleRejected021()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(abcdeg, red, blue)", &info);

    // Assert: declaration invalid, gradient not produced (no direction/style)
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ParseInvalidAngleVariants039
 * @tc.desc: Illegal <angle> values across all supported units (deg/rad/turn) and a
 *           partially-numeric token must reject the whole declaration per W3C.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseInvalidAngleVariants039, TestSize.Level1)
#else
void GradientParserTddTest::ParseInvalidAngleVariants039()
#endif
{
    // Arrange
    GradientInfo info;

    // Assert: non-numeric angle tokens are not valid <angle>, declaration invalid
    EXPECT_FALSE(ParseLinearGradient("linear-gradient(abcdeg, red, blue)", &info));
    EXPECT_FALSE(ParseLinearGradient("linear-gradient(abcrad, red, blue)", &info));
    EXPECT_FALSE(ParseLinearGradient("linear-gradient(xyzturn, red, blue)", &info));
    // Partially-consumed number is not a complete decimal number.
    EXPECT_FALSE(ParseLinearGradient("linear-gradient(123abcdeg, red, blue)", &info));
    // Sanity: a well formed angle still succeeds.
    EXPECT_TRUE(ParseLinearGradient("linear-gradient(45deg, red, blue)", &info));
}

/**
 * @tc.name: ParseInvalidAngleMissingParam040
 * @tc.desc: Missing / insufficient parameters make the declaration invalid per W3C: an
 *           empty gradient body (no color stops) and a single color stop (needs >= 2)
 *           are both rejected by the argument-count guard.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseInvalidAngleMissingParam040, TestSize.Level1)
#else
void GradientParserTddTest::ParseInvalidAngleMissingParam040()
#endif
{
    // Arrange
    GradientInfo info;

    // Assert: empty body (0 stops) and a lone stop (1 stop) are invalid declarations
    EXPECT_FALSE(ParseLinearGradient("linear-gradient()", &info));
    EXPECT_FALSE(ParseLinearGradient("linear-gradient(red)", &info));
    // Sanity: a valid color-first gradient (no explicit direction, >= 2 stops) is accepted.
    EXPECT_TRUE(ParseLinearGradient("linear-gradient(red, blue)", &info));
}

/**
 * @tc.name: ParseValidAngleBoundary041
 * @tc.desc: Valid <angle> boundary values: 0deg, 360deg, negative and over-360 angles
 *           are legal CSS and stored verbatim as CUSTOM_ANGLE (wrapping is the adapter's
 *           job). Confirms legal angles are accepted while illegal ones are rejected.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseValidAngleBoundary041, TestSize.Level1)
#else
void GradientParserTddTest::ParseValidAngleBoundary041()
#endif
{
    // Arrange
    GradientInfo info;
    bool result;

    result = ParseLinearGradient("linear-gradient(0deg, red, blue)", &info);
    EXPECT_TRUE(result);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_0, ANGLE_TOLERANCE);

    result = ParseLinearGradient("linear-gradient(360deg, red, blue)", &info);
    EXPECT_TRUE(result);
    EXPECT_NEAR(info.angle, 360.0f, ANGLE_TOLERANCE);

    result = ParseLinearGradient("linear-gradient(-45deg, red, blue)", &info);
    EXPECT_TRUE(result);
    EXPECT_NEAR(info.angle, -ANGLE_45, ANGLE_TOLERANCE);

    result = ParseLinearGradient("linear-gradient(720deg, red, blue)", &info);
    EXPECT_TRUE(result);
    EXPECT_NEAR(info.angle, 720.0f, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseInvalidGradientBrokenToken042
 * @tc.desc: W3C CSS: a gradient function token whose name is split by whitespace
 *           ("linear-  gradient(...)", "linear- gradient(...)") breaks tokenization
 *           and the whole declaration is invalid. IsBrokenGradientFunctionToken
 *           must flag it, ParseLinearGradient must reject it, and a valid gradient
 *           (single space only between args, no space inside the name) must still
 *           parse. URLs that merely contain "linear- gradient" must NOT be flagged.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseInvalidGradientBrokenToken042, TestSize.Level1)
#else
void GradientParserTddTest::ParseInvalidGradientBrokenToken042()
#endif
{
    // Arrange
    GradientInfo info;

    // Assert: whitespace-split function token is detected as broken.
    EXPECT_TRUE(IsBrokenGradientFunctionToken("linear-  gradient(45deg, red, blue)"));
    EXPECT_TRUE(IsBrokenGradientFunctionToken("linear- gradient(45deg, red, blue)"));
    EXPECT_TRUE(IsBrokenGradientFunctionToken("LINEAR-  GRADIENT(45deg, red, blue)"));
    // Assert: a correct, contiguous token is NOT broken.
    EXPECT_FALSE(IsBrokenGradientFunctionToken("linear-gradient(45deg, red, blue)"));
    // Assert: an image URL that merely contains "linear- gradient" is NOT flagged.
    EXPECT_FALSE(IsBrokenGradientFunctionToken("background: url(linear- gradient.png)"));
    EXPECT_FALSE(IsBrokenGradientFunctionToken("linear-gradient.png"));
    // Assert: ParseLinearGradient rejects the broken-token declaration outright.
    EXPECT_FALSE(ParseLinearGradient("linear-  gradient(45deg, red, blue)", &info));
    // Sanity: a valid gradient with normal argument spacing still parses.
    EXPECT_TRUE(ParseLinearGradient("linear-gradient(45deg, red, blue)", &info));
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_45, ANGLE_TOLERANCE);
}

/* ==========================================================================
 * Helper functions.
 * ========================================================================== */

/**
 * @tc.name: ParseColorFormats022
 * @tc.desc: Color literal parsing: named colors, #RGB, #RRGGBB, #RRGGBBAA,
 *           rgb() and rgba().
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseColorFormats022, TestSize.Level1)
#else
void GradientParserTddTest::ParseColorFormats022()
#endif
{
    // Arrange
    ColorType color;
    bool parseResult;

    // Act & Assert: various color formats
    /* Named color, case insensitive. */
    parseResult = ParseColorToColorType("RED", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.full, 0xFFFF0000u);

    /* #RGB is expanded into #RRGGBB. */
    parseResult = ParseColorToColorType("#f00", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.red, FULL);
    EXPECT_EQ(color.green, ZERO);
    EXPECT_EQ(color.blue, ZERO);
    EXPECT_EQ(color.alpha, FULL);

    /* #RRGGBB */
    parseResult = ParseColorToColorType("#00ff00", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.green, FULL);
    EXPECT_EQ(color.alpha, FULL);

    /* #RRGGBBAA carries an alpha channel. */
    parseResult = ParseColorToColorType("#ff000080", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.red, FULL);
    EXPECT_EQ(color.alpha, ALPHA_HALF);

    /* rgb() */
    parseResult = ParseColorToColorType("rgb(0, 0, 255)", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.blue, FULL);
    EXPECT_EQ(color.alpha, FULL);

    /* rgba() alpha 0.5 maps to 127. */
    parseResult = ParseColorToColorType("rgba(255, 0, 0, 0.5)", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.red, FULL);
    EXPECT_NEAR(color.alpha, 127, 1);

    /* transparent */
    parseResult = ParseColorToColorType("transparent", color);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(color.full, 0u);
}

/**
 * @tc.name: ParseColorInvalid023
 * @tc.desc: Error handling: malformed color literals return false.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseColorInvalid023, TestSize.Level1)
#else
void GradientParserTddTest::ParseColorInvalid023()
#endif
{
    // Arrange
    ColorType color;
    bool parseResult;

    // Act & Assert: malformed color literals return false
    parseResult = ParseColorToColorType("", color);
    EXPECT_FALSE(parseResult);
    parseResult = ParseColorToColorType("notacolor", color);
    EXPECT_FALSE(parseResult);
    parseResult = ParseColorToColorType("#ff00", color);     /* Invalid hex length. */
    EXPECT_FALSE(parseResult);
    parseResult = ParseColorToColorType("#gggggg", color);   /* Invalid hex digit. */
    EXPECT_FALSE(parseResult);
    parseResult = ParseColorToColorType("rgb(1, 2)", color); /* Missing component. */
    EXPECT_FALSE(parseResult);
}

/**
 * @tc.name: ParseColorStopOffset024
 * @tc.desc: Color stop offset parsing: explicit percentage, omitted offset (-1,
 *           resolved later by NormalizeColorStops) and out of range positions,
 *           which W3C requires to be preserved verbatim.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseColorStopOffset024, TestSize.Level1)
#else
void GradientParserTddTest::ParseColorStopOffset024()
#endif
{
    // Arrange
    GradientColorStop stop;
    bool parseResult;

    // Act & Assert: offset parsing in various scenarios
    parseResult = ParseColorStop("red 50%", stop);
    EXPECT_TRUE(parseResult);
    EXPECT_EQ(stop.color.full, 0xFFFF0000u);
    EXPECT_FLOAT_EQ(stop.offset, OFFSET_HALF);

    /* An omitted offset is stored as -1 and resolved by NormalizeColorStops. */
    parseResult = ParseColorStop("blue", stop);
    EXPECT_TRUE(parseResult);
    EXPECT_FLOAT_EQ(stop.offset, -1.0f);

    /*
     * W3C CSS Images Module Level 3, 3.4.1: out of range positions are legal and
     * must survive parsing, they are projected onto the painted segment later by
     * ClipColorStopsToGradientLine().
     */
    parseResult = ParseColorStop("red -20%", stop);
    EXPECT_TRUE(parseResult);
    EXPECT_FLOAT_EQ(stop.offset, OFFSET_BEFORE_START);
    parseResult = ParseColorStop("red 150%", stop);
    EXPECT_TRUE(parseResult);
    EXPECT_FLOAT_EQ(stop.offset, OFFSET_BEYOND_END);

    /* -100% collides with the "omitted" sentinel and must stay distinguishable. */
    parseResult = ParseColorStop("red -100%", stop);
    EXPECT_TRUE(parseResult);
    EXPECT_FALSE(IsGradientOffsetUnspecified(stop.offset));
    EXPECT_NEAR(stop.offset, -1.0f, OFFSET_TOLERANCE);

    /* Empty string and invalid color. */
    parseResult = ParseColorStop("", stop);
    EXPECT_FALSE(parseResult);
    parseResult = ParseColorStop("notacolor 50%", stop);
    EXPECT_FALSE(parseResult);
}

/**
 * @tc.name: NormalizeColorStops025
 * @tc.desc: Normalization: an omitted first offset becomes 0 and an omitted last
 *           offset becomes 1; omitted offsets in between are spread evenly.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, NormalizeColorStops025, TestSize.Level1)
#else
void GradientParserTddTest::NormalizeColorStops025()
#endif
{
    // Arrange
    ColorType red;
    red.full = 0xFFFF0000u;
    GradientStopList stops;
    stops.PushBack(GradientColorStop(red, -1.0f)); /* -> 0    */
    stops.PushBack(GradientColorStop(red, -1.0f)); /* -> 0.25 */
    stops.PushBack(GradientColorStop(red, -1.0f)); /* -> 0.5  */
    stops.PushBack(GradientColorStop(red, -1.0f)); /* -> 0.75 */
    stops.PushBack(GradientColorStop(red, -1.0f)); /* -> 1    */

    // Act
    NormalizeColorStops(stops);

    // Assert
    EXPECT_FLOAT_EQ(stops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(stops[1].offset, OFFSET_QUARTER);
    EXPECT_FLOAT_EQ(stops[2].offset, OFFSET_HALF);
    EXPECT_FLOAT_EQ(stops[3].offset, OFFSET_THREE_QUARTER);
    EXPECT_FLOAT_EQ(stops[4].offset, OFFSET_END);
}

/**
 * @tc.name: NormalizeMonotonic026
 * @tc.desc: Normalization enforces a monotonic sequence: an out of order offset is
 *           raised to its predecessor's value.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, NormalizeMonotonic026, TestSize.Level1)
#else
void GradientParserTddTest::NormalizeMonotonic026()
#endif
{
    // Arrange
    ColorType red;
    red.full = 0xFFFF0000u;
    GradientStopList stops;
    stops.PushBack(GradientColorStop(red, 0.8f));
    stops.PushBack(GradientColorStop(red, 0.2f)); /* Out of order, raised to 0.8. */

    // Act
    NormalizeColorStops(stops);

    // Assert
    EXPECT_FLOAT_EQ(stops[0].offset, 0.8f);
    EXPECT_FLOAT_EQ(stops[1].offset, 0.8f);
}

/**
 * @tc.name: SplitGradientArgsRgba027
 * @tc.desc: Argument splitting: commas nested inside rgba() are preserved because
 *           the splitter is parenthesis depth aware.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, SplitGradientArgsRgba027, TestSize.Level1)
#else
void GradientParserTddTest::SplitGradientArgsRgba027()
#endif
{
    // Act
    GradientArgList parts =
        SplitGradientArgs("to right, rgba(255, 0, 0, 0.5), blue");

    // Assert
    ASSERT_EQ(parts.Size(), 3u);
    EXPECT_STREQ(parts[0].c_str(), "to right");
    EXPECT_STREQ(parts[1].c_str(), "rgba(255, 0, 0, 0.5)");
    EXPECT_STREQ(parts[2].c_str(), "blue");

    /* The whole declaration with an rgba() stop still parses. */
    // Arrange
    GradientInfo info;
    // Act
    bool parseResult = ParseLinearGradient("linear-gradient(to right, rgba(255, 0, 0, 0.5), blue)", &info);
    // Assert
    ASSERT_TRUE(parseResult);
    EXPECT_EQ(info.colorCount, TWO_COLORS);
}

/**
 * @tc.name: TrimToLower028
 * @tc.desc: String helpers: Trim strips leading and trailing whitespace, ToLower
 *           performs an ASCII lowercase conversion.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, TrimToLower028, TestSize.Level1)
#else
void GradientParserTddTest::TrimToLower028()
#endif
{
    // Act & Assert: Trim and ToLower string helpers
    std::string trimmedRed = Trim("  red  ");
    EXPECT_STREQ(trimmedRed.c_str(), "red");

    std::string trimmedBlue = Trim("\t\nblue\r ");
    EXPECT_STREQ(trimmedBlue.c_str(), "blue");

    std::string trimmedEmpty = Trim("   ");
    EXPECT_STREQ(trimmedEmpty.c_str(), "");

    std::string loweredDir = ToLower("To RIGHT");
    EXPECT_STREQ(loweredDir.c_str(), "to right");

    std::string loweredDeg = ToLower("45DEG");
    EXPECT_STREQ(loweredDeg.c_str(), "45deg");
}

/* ==========================================================================
 * Shared direction / angle helpers extracted during the gradient refactor
 * ========================================================================== */

/**
 * @tc.name: ParseDirectionOrAngleKeywords029
 * @tc.desc: ParseDirectionOrAngle() resolves all eight direction keywords, both
 *           word orders of the diagonals, is case insensitive, tolerates
 *           surrounding whitespace and switches to CUSTOM_ANGLE for units.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseDirectionOrAngleKeywords029, TestSize.Level1)
#else
void GradientParserTddTest::ParseDirectionOrAngleKeywords029()
#endif
{
    GradientInfo info;
    // Helper: parse + assert direction (optional angle) + error-log on mismatch.
    auto check = [&info](const char* in, CssGradientDirection dir, float angle = 0.0f,
                    bool chkAngle = false, float tol = 0.0f) {
        bool ok = ParseDirectionOrAngle(in, &info);
        EXPECT_TRUE(ok);
        EXPECT_EQ(info.direction, dir);
        if (chkAngle) {
            if (tol > 0.0f) {
                EXPECT_NEAR(info.angle, angle, tol);
            } else {
                EXPECT_FLOAT_EQ(info.angle, angle);
            }
        }
        if (!ok || info.direction != dir ||
            (chkAngle && std::fabs(info.angle - angle) > (tol > 0.0f ? tol : 1e-5f))) {
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "[GRADIENT_TDD] ParseDirectionOrAngleKeywords029: assertion failed "
                        "(status=failed, input='%s', expectedDir=%d, actualDir=%d, "
                        "expectedAngle=%.2f, actualAngle=%.2f, chkAngle=%d, op=ParseDirectionOrAngle)",
                        in, static_cast<int>(dir), static_cast<int>(info.direction),
                        angle, info.angle, chkAngle ? 1 : 0);
        }
    };
    // 8 cardinal/diagonal keywords
    check("to top", CssGradientDirection::TO_TOP, ANGLE_0, true);
    check("to top right", CssGradientDirection::TO_TOP_RIGHT, ANGLE_45, true);
    check("to right", CssGradientDirection::TO_RIGHT, ANGLE_90, true);
    check("to bottom right", CssGradientDirection::TO_BOTTOM_RIGHT, ANGLE_135, true);
    check("to bottom", CssGradientDirection::TO_BOTTOM, ANGLE_180, true);
    check("to bottom left", CssGradientDirection::TO_BOTTOM_LEFT, ANGLE_225, true);
    check("to left", CssGradientDirection::TO_LEFT, ANGLE_270, true);
    check("to top left", CssGradientDirection::TO_TOP_LEFT, ANGLE_315, true);
    // Reversed word order synonyms resolve to the same enum values
    check("to right top", CssGradientDirection::TO_TOP_RIGHT);
    check("to left bottom", CssGradientDirection::TO_BOTTOM_LEFT);
    // Case + surrounding whitespace are normalized before keyword lookup
    check("  TO RIGHT  ", CssGradientDirection::TO_RIGHT, ANGLE_90, true);
    // Explicit unit switches the payload over to CUSTOM_ANGLE
    check("135DEG", CssGradientDirection::CUSTOM_ANGLE, ANGLE_135, true, ANGLE_TOLERANCE);
}

/**
 * @tc.name: ParseDirectionOrAngleRejectsColor030
 * @tc.desc: ParseDirectionOrAngle() returns false for a nullptr payload, empty
 *           input and anything that is really a color stop, leaving the
 *           direction and angle fields untouched so the caller can retry the
 *           argument as a color.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseDirectionOrAngleRejectsColor030, TestSize.Level1)
#else
void GradientParserTddTest::ParseDirectionOrAngleRejectsColor030()
#endif
{
    // Act & Assert: nullptr payload must not be dereferenced
    bool nullResult = ParseDirectionOrAngle("to right", nullptr);
    EXPECT_FALSE(nullResult);

    // Arrange
    GradientInfo info;
    const CssGradientDirection initialDirection = info.direction;
    bool parseResult;

    // Act & Assert: non-direction inputs are rejected, payload unchanged
    parseResult = ParseDirectionOrAngle("", &info);
    EXPECT_FALSE(parseResult);
    parseResult = ParseDirectionOrAngle("   ", &info);
    EXPECT_FALSE(parseResult);
    parseResult = ParseDirectionOrAngle("red", &info);
    EXPECT_FALSE(parseResult);
    parseResult = ParseDirectionOrAngle("#ff0000", &info);
    EXPECT_FALSE(parseResult);
    parseResult = ParseDirectionOrAngle("rgba(255, 0, 0, 0.5)", &info);
    EXPECT_FALSE(parseResult);
    /* "to nowhere" is not in the keyword table and carries no angle unit. */
    parseResult = ParseDirectionOrAngle("to nowhere", &info);
    EXPECT_FALSE(parseResult);
    /* A bare number without a unit is not a CSS angle. */
    parseResult = ParseDirectionOrAngle("45", &info);
    EXPECT_FALSE(parseResult);
    /* Trailing garbage after the unit must not be silently consumed. */
    parseResult = ParseDirectionOrAngle("45degx", &info);
    EXPECT_FALSE(parseResult);

    /* Rejected calls leave the payload exactly as it was. */
    EXPECT_EQ(info.direction, initialDirection);
}

/**
 * @tc.name: NormalizeDirectionKeywordToAngle031
 * @tc.desc: NormalizeDirectionKeywordToAngle() rewrites the leading direction
 *           keyword into "<deg>deg" syntax. The whole first argument is matched,
 *           which is the regression guard for the former prefix match that
 *           clipped "to right bottom" down to "to right".
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, NormalizeDirectionKeywordToAngle031, TestSize.Level1)
#else
void GradientParserTddTest::NormalizeDirectionKeywordToAngle031()
#endif
{
    // Arrange
    std::string normalized;
    bool normResult;

    // Act & Assert: direction keyword → angle normalization
    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(to right, red, blue)", normalized);
    EXPECT_TRUE(normResult);
    EXPECT_STREQ(normalized.c_str(), "linear-gradient(90deg, red, blue)");

    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(to bottom, red, blue)", normalized);
    EXPECT_TRUE(normResult);
    EXPECT_STREQ(normalized.c_str(), "linear-gradient(180deg, red, blue)");

    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(to top left, red, blue)", normalized);
    EXPECT_TRUE(normResult);
    EXPECT_STREQ(normalized.c_str(), "linear-gradient(315deg, red, blue)");

    /* Regression: the reversed diagonal must not be clipped to "to right". */
    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(to right bottom, red, blue)", normalized);
    EXPECT_TRUE(normResult);
    EXPECT_STREQ(normalized.c_str(), "linear-gradient(135deg, red, blue)");

    /* The rewritten declaration still parses into the same geometry. */
    // Arrange
    GradientInfo info;
    // Act
    bool parseResult = ParseLinearGradient(normalized, &info);
    // Assert
    ASSERT_TRUE(parseResult);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_NEAR(info.angle, ANGLE_135, ANGLE_TOLERANCE);

    /* Nothing to rewrite: the caller keeps using the original declaration. */
    normalized.clear();
    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(45deg, red, blue)", normalized);
    EXPECT_FALSE(normResult);
    EXPECT_TRUE(normalized.empty());
    normResult = NormalizeDirectionKeywordToAngle("radial-gradient(to right, red, blue)", normalized);
    EXPECT_FALSE(normResult);
    normResult = NormalizeDirectionKeywordToAngle("red", normalized);
    EXPECT_FALSE(normResult);
    /* No top level comma means there is no first argument to replace. */
    normResult = NormalizeDirectionKeywordToAngle("linear-gradient(to right)", normalized);
    EXPECT_FALSE(normResult);
    EXPECT_TRUE(normalized.empty());
}

/* ==========================================================================
 * Illegal direction value handling: the gradient must be silently disabled.
 * ========================================================================== */

/**
 * @tc.name: ParseIllegalDirectionToNowhere032
 * @tc.desc: An unknown direction keyword such as "to nowhere" is rejected and
 *           the whole gradient is disabled (returns false, no default fallback).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseIllegalDirectionToNowhere032, TestSize.Level1)
#else
void GradientParserTddTest::ParseIllegalDirectionToNowhere032()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(to nowhere, red, blue)", &info);
    // Assert: "to nowhere" is rejected, gradient is disabled
    EXPECT_FALSE(result);
    EXPECT_FALSE(info.isValid);
}

/**
 * @tc.name: ParseIllegalDirectionMeaningless033
 * @tc.desc: A meaningless token such as "abcdef" sitting in the direction slot
 *           is not a direction, an angle nor a color stop, so the gradient is
 *           silently disabled (returns false).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseIllegalDirectionMeaningless033, TestSize.Level1)
#else
void GradientParserTddTest::ParseIllegalDirectionMeaningless033()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(abcdef, red, blue)", &info);
    // Assert: "abcdef" is not a direction, angle, or color - gradient disabled
    EXPECT_FALSE(result);
    EXPECT_FALSE(info.isValid);
}

/**
 * @tc.name: ParseIllegalDirectionUppercaseSpaces034
 * @tc.desc: An illegal function form with an upper case name and stray spaces
 *           ("LINEAR - GRADIENT (to right, red, blue)") does not match the
 *           required "linear-gradient(" prefix, so the gradient is disabled.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ParseIllegalDirectionUppercaseSpaces034, TestSize.Level1)
#else
void GradientParserTddTest::ParseIllegalDirectionUppercaseSpaces034()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("LINEAR - GRADIENT (to right, red, blue)", &info);
    // Assert: uppercase with stray spaces does not match the prefix
    EXPECT_FALSE(result);
    EXPECT_FALSE(info.isValid);
}

/* ==========================================================================
 * W3C out of range color stops (CSS Images Module Level 3, 3.4.1)
 * ========================================================================== */

/**
 * @tc.name: ClipOutOfRangeStops035
 * @tc.desc: "linear-gradient(to right, #ff0000 -20%, #0000ff 150%)" must paint
 *           #e1001e at the left edge and #4b00b4 at the right edge, which is
 *           what browsers render, instead of the plain red to blue ramp the
 *           previous clamping produced.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ClipOutOfRangeStops035, TestSize.Level1)
#else
void GradientParserTddTest::ClipOutOfRangeStops035()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient("linear-gradient(to right, #ff0000 -20%, #0000ff 150%)", &info);

    // Assert
    ASSERT_TRUE(result);
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_RIGHT);
    ASSERT_EQ(info.colorCount, TWO_COLORS);

    /* Both stops end up on the painted segment. */
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_END);

    /* Left edge: ratio (0 + 0.2) / 1.7 = 0.1176 along red -> blue. */
    EXPECT_NEAR(info.colorStops[0].color.red, CLIPPED_START_RED, CHANNEL_TOLERANCE);
    EXPECT_EQ(info.colorStops[0].color.green, ZERO);
    EXPECT_NEAR(info.colorStops[0].color.blue, CLIPPED_START_BLUE, CHANNEL_TOLERANCE);
    EXPECT_EQ(info.colorStops[0].color.alpha, FULL);

    /* Right edge: ratio (1 + 0.2) / 1.7 = 0.7059 along red -> blue. */
    EXPECT_NEAR(info.colorStops[1].color.red, CLIPPED_END_RED, CHANNEL_TOLERANCE);
    EXPECT_EQ(info.colorStops[1].color.green, ZERO);
    EXPECT_NEAR(info.colorStops[1].color.blue, CLIPPED_END_BLUE, CHANNEL_TOLERANCE);
    EXPECT_EQ(info.colorStops[1].color.alpha, FULL);
}

/**
 * @tc.name: ClipKeepsInnerStops036
 * @tc.desc: An out of range ramp with an inner stop keeps that stop at its own
 *           position, and a ramp entirely outside the box degrades to the flat
 *           color the standard mandates.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ClipKeepsInnerStops036, TestSize.Level1)
#else
void GradientParserTddTest::ClipKeepsInnerStops036()
#endif
{
    /* The green stop is inside the box and must survive the projection. */
    // Arrange
    GradientInfo info;

    // Act
    bool result = ParseLinearGradient(
        "linear-gradient(to right, red -50%, lime 50%, blue 150%)", &info);

    // Assert
    ASSERT_TRUE(result);
    ASSERT_EQ(info.colorCount, THREE_COLORS);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_HALF);
    EXPECT_FLOAT_EQ(info.colorStops[2].offset, OFFSET_END);
    EXPECT_EQ(info.colorStops[1].color.green, FULL);

    /* Every stop sits left of the box: the box shows the last color, flat. */
    // Arrange
    GradientInfo outside;

    // Act
    bool resultOutside = ParseLinearGradient("linear-gradient(to right, red -80%, blue -20%)", &outside);

    // Assert
    ASSERT_TRUE(resultOutside);
    ASSERT_EQ(outside.colorCount, TWO_COLORS);
    EXPECT_FLOAT_EQ(outside.colorStops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(outside.colorStops[1].offset, OFFSET_END);
    EXPECT_EQ(outside.colorStops[0].color.full, outside.colorStops[1].color.full);
    EXPECT_EQ(outside.colorStops[0].color.blue, FULL);
}

/**
 * @tc.name: ClipPreservesInRangeRamp037
 * @tc.desc: A ramp that already lives inside [0, 1] must come out of the
 *           clipping step byte for byte identical (no regression for the
 *           ordinary gradients that make up almost every style sheet).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientParserTddTest, ClipPreservesInRangeRamp037, TestSize.Level1)
#else
void GradientParserTddTest::ClipPreservesInRangeRamp037()
#endif
{
    // Arrange
    ColorType red;
    red.full = 0xFFFF0000u;
    ColorType blue;
    blue.full = 0xFF0000FFu;
    GradientStopList stops;
    stops.PushBack(GradientColorStop(red, OFFSET_START));
    stops.PushBack(GradientColorStop(blue, OFFSET_THREE_QUARTER));

    // Act
    ClipColorStopsToGradientLine(stops);

    // Assert
    ASSERT_EQ(stops.Size(), static_cast<uint8_t>(TWO_COLORS));
    EXPECT_FLOAT_EQ(stops[0].offset, OFFSET_START);
    EXPECT_FLOAT_EQ(stops[1].offset, OFFSET_THREE_QUARTER);
    EXPECT_EQ(stops[0].color.full, red.full);
    EXPECT_EQ(stops[1].color.full, blue.full);
}

#ifndef TDD_ASSERTIONS
void GradientParserTddTest::RunTests()
{
    ParseAngle0Deg001();
    ParseAngle45Deg002();
    ParseAngle90Deg003();
    ParseAngle135Deg004();
    ParseKeywordDiagonal005();
    ParseRadTurnAngle006();
    ParseNegativeAndLargeAngle007();
    DirectionToCssAngle008();
    ParseTwoColor009();
    ParseThreeColor010();
    ParseFiveColorWithOffsets011();
    ParseHorizontalDirection012();
    ParseVerticalDirection013();
    ParseDefaultDirection014();
    ParseMaxColorsTruncated015();
    ParseNullOutput016();
    ParseWrongPrefix017();
    ParseCaseInsensitivePrefix038();
    ParseMissingCloseParen018();
    ParseTooFewColors019();
    ParseInvalidColorSkipped020();
    ParseInvalidAngleRejected021();
    ParseColorFormats022();
    ParseColorInvalid023();
    ParseColorStopOffset024();
    NormalizeColorStops025();
    NormalizeMonotonic026();
    SplitGradientArgsRgba027();
    TrimToLower028();
    ParseDirectionOrAngleKeywords029();
    ParseDirectionOrAngleRejectsColor030();
    NormalizeDirectionKeywordToAngle031();
    ParseIllegalDirectionToNowhere032();
    ParseIllegalDirectionMeaningless033();
    ParseIllegalDirectionUppercaseSpaces034();
    ClipOutOfRangeStops035();
    ClipKeepsInnerStops036();
    ClipPreservesInRangeRamp037();
    ParseInvalidAngleVariants039();
    ParseInvalidAngleMissingParam040();
    ParseValidAngleBoundary041();
    ParseInvalidGradientBrokenToken042();
}
#endif
} // namespace ACELite
} // namespace OHOS
