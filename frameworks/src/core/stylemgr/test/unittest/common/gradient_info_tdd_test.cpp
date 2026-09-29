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

#include "gradient_info_tdd_test.h"
#include "gfx_utils/gradient_info.h"
#include "app_style_gradient_parser.h"

#include <cmath>
#include <utility>

namespace OHOS {
namespace ACELite {
namespace {
/* Color channel extremes used to build the red/green/blue/yellow/cyan stops. */
const uint8_t CH_FULL = 0xFF;
const uint8_t CH_ZERO = 0x00;
const float OFFSET_START = 0.0f;
const float OFFSET_HALF = 0.5f;
const float OFFSET_END = 1.0f;
const uint8_t TWO_COLORS = 2;
const uint8_t THREE_COLORS = 3;
const uint8_t OVER_LIMIT_COLORS = 18; /* Above LINEAR_GRADIENT_MAX_COLORS (16) */
const float CUSTOM_ANGLE_45 = 45.0f;

/* Standard CSS angles exercised by the direction and normalization tests. */
const float ANGLE_0 = 0.0f;
const float ANGLE_45 = 45.0f;
const float ANGLE_90 = 90.0f;
const float ANGLE_135 = 135.0f;
const float ANGLE_315 = 315.0f;
/* Three full turns, used to prove that wrapping is not limited to one turn. */
const float ANGLE_1080 = 1080.0f;

/* Build an opaque RGB color stop. */
GradientColorStop MakeStop(uint8_t r, uint8_t g, uint8_t b, float offset)
{
    return GradientColorStop(r, g, b, CH_FULL, offset);
}

/*
 * Produce a quiet NaN without relying on the NAN macro, which some lite
 * toolchains do not expose when building with -ffast-math disabled headers.
 */
float MakeNan()
{
    return nanf("");
}
} // namespace

/**
 * @tc.name: DefaultState001
 * @tc.desc: A default constructed GradientInfo is invalid (safety baseline).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, DefaultState001, TestSize.Level1)
#else
void GradientInfoTddTest::DefaultState001()
#endif
{
    // Arrange: default-constructed GradientInfo
    GradientInfo info;

    // Act: no explicit action needed (default state)

    // Assert: every field is in its initial safe state
    EXPECT_EQ(info.type, GradientType::LINEAR);
    EXPECT_EQ(info.direction, CssGradientDirection::NONE);
    EXPECT_FLOAT_EQ(info.angle, 0.0f);
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
    EXPECT_FALSE(info.isValid);
}

/**
 * @tc.name: SetTwoColorStops002
 * @tc.desc: Two color stops (two color scenario); the deep copy matches the input.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetTwoColorStops002, TestSize.Level1)
#else
void GradientInfoTddTest::SetTwoColorStops002()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop stops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START), /* red  @0%   */
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)    /* blue @100% */
    };

    // Act
    info.SetColorStops(stops, TWO_COLORS);

    // Assert: valid, deep copy, mutation isolation
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.colorCount, TWO_COLORS);
    ASSERT_NE(info.colorStops, nullptr);
    /* Deep copy: the internal array is a different allocation. */
    EXPECT_NE(info.colorStops, stops);
    /* Contents are identical. */
    EXPECT_EQ(info.colorStops[0].color.red, CH_FULL);
    EXPECT_EQ(info.colorStops[0].color.blue, CH_ZERO);
    EXPECT_FLOAT_EQ(info.colorStops[0].offset, OFFSET_START);
    EXPECT_EQ(info.colorStops[1].color.blue, CH_FULL);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_END);
    /* Mutating the source array must not affect the stored stops. */
    stops[0].color.red = CH_ZERO;
    EXPECT_EQ(info.colorStops[0].color.red, CH_FULL);
}

/**
 * @tc.name: SetMultiColorStops003
 * @tc.desc: Multi color stops (>= 3 colors); count and contents are correct.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetMultiColorStops003, TestSize.Level1)
#else
void GradientInfoTddTest::SetMultiColorStops003()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop stops[THREE_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),  /* red   @0%   */
        MakeStop(CH_ZERO, CH_FULL, CH_ZERO, OFFSET_HALF),   /* green @50%  */
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)     /* blue  @100% */
    };

    // Act
    info.SetColorStops(stops, THREE_COLORS);

    // Assert
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.colorCount, THREE_COLORS);
    EXPECT_EQ(info.colorStops[1].color.green, CH_FULL);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_HALF);
}

/**
 * @tc.name: SetColorStopsNull004
 * @tc.desc: Error path: a nullptr stop array leaves the payload invalid.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetColorStopsNull004, TestSize.Level1)
#else
void GradientInfoTddTest::SetColorStopsNull004()
#endif
{
    // Arrange
    GradientInfo info;

    // Act
    info.SetColorStops(nullptr, TWO_COLORS);

    // Assert: stays invalid, no dangling pointer
    EXPECT_FALSE(info.isValid);
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
}

/**
 * @tc.name: SetColorStopsLessThanTwo005
 * @tc.desc: Boundary: fewer than two stops cannot form a ramp, stays invalid.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetColorStopsLessThanTwo005, TestSize.Level1)
#else
void GradientInfoTddTest::SetColorStopsLessThanTwo005()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop single = MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START);

    // Act: fewer than 2 stops cannot form a ramp
    info.SetColorStops(&single, 1);

    // Assert: stays invalid
    EXPECT_FALSE(info.isValid);
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
}

/**
 * @tc.name: SetColorStopsMaxLimit006
 * @tc.desc: Boundary: more than LINEAR_GRADIENT_MAX_COLORS (16) stops are
 *          truncated to 16 (lite device memory protection).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetColorStopsMaxLimit006, TestSize.Level1)
#else
void GradientInfoTddTest::SetColorStopsMaxLimit006()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop stops[OVER_LIMIT_COLORS];
    for (uint8_t i = 0; i < OVER_LIMIT_COLORS; i++) {
        stops[i] = MakeStop(i * 10, i * 5, i * 2, static_cast<float>(i) / (OVER_LIMIT_COLORS - 1));
    }

    // Act
    info.SetColorStops(stops, OVER_LIMIT_COLORS);

    // Assert: truncated to LINEAR_GRADIENT_MAX_COLORS
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.colorCount, LINEAR_GRADIENT_MAX_COLORS);
    /* Truncation keeps the first 16 stops. */
    EXPECT_EQ(info.colorStops[LINEAR_GRADIENT_MAX_COLORS - 1].color.red,
              (LINEAR_GRADIENT_MAX_COLORS - 1) * 10);
}

/**
 * @tc.name: DirectionAnglePreserved007
 * @tc.desc: Regression: SetColorStops calls Clear() internally and must restore
 *          direction and angle, so a custom 45deg angle is never lost.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, DirectionAnglePreserved007, TestSize.Level1)
#else
void GradientInfoTddTest::DirectionAnglePreserved007()
#endif
{
    // Arrange
    GradientInfo info;
    info.direction = CssGradientDirection::CUSTOM_ANGLE;
    info.angle = CUSTOM_ANGLE_45;

    GradientColorStop stops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };

    // Act: SetColorStops internally calls Clear(), must restore direction/angle
    info.SetColorStops(stops, TWO_COLORS);

    // Assert
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_FLOAT_EQ(info.angle, CUSTOM_ANGLE_45);
}

/**
 * @tc.name: GetBeginEndColor008
 * @tc.desc: GetBeginColor/GetEndColor return the first and last stop colors (legacy GradientColor adapter).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, GetBeginEndColor008, TestSize.Level1)
#else
void GradientInfoTddTest::GetBeginEndColor008()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop stops[THREE_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_FULL, CH_ZERO, OFFSET_HALF),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    info.SetColorStops(stops, THREE_COLORS);

    // Act
    ColorType begin = info.GetBeginColor();
    ColorType end = info.GetEndColor();

    // Assert
    EXPECT_EQ(begin.red, CH_FULL);
    EXPECT_EQ(begin.green, CH_ZERO);
    EXPECT_EQ(end.blue, CH_FULL);
    EXPECT_EQ(end.red, CH_ZERO);
}

/**
 * @tc.name: GetBeginEndColorInvalid009
 * @tc.desc: Error path: an invalid payload yields transparent black (full == 0).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, GetBeginEndColorInvalid009, TestSize.Level1)
#else
void GradientInfoTddTest::GetBeginEndColorInvalid009()
#endif
{
    // Arrange
    GradientInfo info; /* invalid by default */

    // Act
    ColorType begin = info.GetBeginColor();
    ColorType end = info.GetEndColor();

    // Assert: transparent black
    EXPECT_EQ(begin.full, 0u);
    EXPECT_EQ(end.full, 0u);
}

/**
 * @tc.name: GetDirectionValue010
 * @tc.desc: GetDirectionValue returns the raw enum value for keywords and degrades CUSTOM_ANGLE to TO_BOTTOM (4).
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, GetDirectionValue010, TestSize.Level1)
#else
void GradientInfoTddTest::GetDirectionValue010()
#endif
{
    // Arrange
    GradientInfo info;
    uint8_t directionValue;

    // Act: TO_RIGHT
    info.direction = CssGradientDirection::TO_RIGHT;
    directionValue = info.GetDirectionValue();
    // Assert
    EXPECT_EQ(directionValue, static_cast<uint8_t>(CssGradientDirection::TO_RIGHT));

    // Act: TO_TOP
    info.direction = CssGradientDirection::TO_TOP;
    directionValue = info.GetDirectionValue();
    // Assert
    EXPECT_EQ(directionValue, static_cast<uint8_t>(CssGradientDirection::TO_TOP));

    // Act: CUSTOM_ANGLE degrades to TO_BOTTOM
    info.direction = CssGradientDirection::CUSTOM_ANGLE;
    directionValue = info.GetDirectionValue();
    // Assert
    EXPECT_EQ(directionValue, static_cast<uint8_t>(CssGradientDirection::TO_BOTTOM));
}

/**
 * @tc.name: DeepCopyIndependence011
 * @tc.desc: DeepCopy produces an independent clone: same contents, separate allocation, mutations do not leak across.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, DeepCopyIndependence011, TestSize.Level1)
#else
void GradientInfoTddTest::DeepCopyIndependence011()
#endif
{
    // Arrange
    GradientInfo info;
    info.direction = CssGradientDirection::TO_BOTTOM_RIGHT; /* 135 degree diagonal */
    GradientColorStop stops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    info.SetColorStops(stops, TWO_COLORS);

    // Act
    GradientInfo* copy = info.DeepCopy();

    // Assert: independent clone with same contents
    ASSERT_NE(copy, nullptr);
    EXPECT_TRUE(copy->isValid);
    EXPECT_EQ(copy->colorCount, TWO_COLORS);
    EXPECT_EQ(copy->direction, CssGradientDirection::TO_BOTTOM_RIGHT);
    /* Separate allocations. */
    EXPECT_NE(copy->colorStops, info.colorStops);
    /* Contents are identical. */
    EXPECT_EQ(copy->colorStops[0].color.red, CH_FULL);
    EXPECT_EQ(copy->colorStops[1].color.blue, CH_FULL);
    /* Mutating the clone must not affect the source. */
    copy->colorStops[0].color.red = CH_ZERO;
    EXPECT_EQ(info.colorStops[0].color.red, CH_FULL);

    delete copy;
}

/**
 * @tc.name: DeepCopyInvalidSource012
 * @tc.desc: Error path: DeepCopy of an invalid source returns nullptr.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, DeepCopyInvalidSource012, TestSize.Level1)
#else
void GradientInfoTddTest::DeepCopyInvalidSource012()
#endif
{
    // Arrange
    GradientInfo info; /* invalid by default */

    // Act
    GradientInfo* copy = info.DeepCopy();

    // Assert
    EXPECT_EQ(copy, nullptr);
}

/**
 * @tc.name: MoveConstructor013
 * @tc.desc: Move construction transfers ownership: the source is detached so the buffer is never freed twice.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, MoveConstructor013, TestSize.Level1)
#else
void GradientInfoTddTest::MoveConstructor013()
#endif
{
    // Arrange
    GradientInfo info;
    GradientColorStop stops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    info.SetColorStops(stops, TWO_COLORS);
    GradientColorStop* rawPtr = info.colorStops;

    // Act
    GradientInfo moved(std::move(info));

    // Assert: ownership transferred, source detached
    EXPECT_TRUE(moved.isValid);
    EXPECT_EQ(moved.colorCount, TWO_COLORS);
    EXPECT_EQ(moved.colorStops, rawPtr); /* ownership moved, no extra copy */
    /* The source is detached, its destructor cannot double free. */
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
    EXPECT_FALSE(info.isValid);
}

/**
 * @tc.name: ClearResets014
 * @tc.desc: Clear releases the buffer and resets every field.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, ClearResets014, TestSize.Level1)
#else
void GradientInfoTddTest::ClearResets014()
#endif
{
    // Arrange
    GradientInfo info;
    info.direction = CssGradientDirection::TO_RIGHT;
    GradientColorStop stops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    info.SetColorStops(stops, TWO_COLORS);
    ASSERT_TRUE(info.isValid);

    // Act
    info.Clear();

    // Assert: every field is reset
    EXPECT_FALSE(info.isValid);
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
    EXPECT_EQ(info.direction, CssGradientDirection::NONE);
}

/**
 * @tc.name: SharedDirectionAngleTable015
 * @tc.desc: CssGradientDirectionToAngle is the single source of truth for the
 *           keyword table; every standard angle (0/45/90/135 and the diagonals)
 *           must resolve exactly, and non keyword values fall back to the CSS
 *           default (180 degrees, "to bottom").
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SharedDirectionAngleTable015, TestSize.Level1)
#else
void GradientInfoTddTest::SharedDirectionAngleTable015()
#endif
{
    // Arrange
    float angle;

    // Act: every keyword maps to its expected CSS angle
    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_TOP);
    // Assert
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_TOP);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_TOP_RIGHT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_TOP_RIGHT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_RIGHT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_RIGHT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_BOTTOM_RIGHT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_BOTTOM_RIGHT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_BOTTOM);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_BOTTOM);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_BOTTOM_LEFT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_BOTTOM_LEFT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_LEFT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_LEFT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::TO_TOP_LEFT);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_TO_TOP_LEFT);

    /* Neither CUSTOM_ANGLE nor NONE can be expressed as a keyword. */
    angle = CssGradientDirectionToAngle(CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_DEFAULT);

    angle = CssGradientDirectionToAngle(CssGradientDirection::NONE);
    EXPECT_FLOAT_EQ(angle, CSS_ANGLE_DEFAULT);

    /* Horizontal and vertical shortcuts must stay mutually consistent. */
    float angleRight = CssGradientDirectionToAngle(CssGradientDirection::TO_RIGHT);
    float angleLeft = CssGradientDirectionToAngle(CssGradientDirection::TO_LEFT);
    EXPECT_FLOAT_EQ(angleRight + CSS_ANGLE_HALF_TURN, angleLeft);

    float angleTop = CssGradientDirectionToAngle(CssGradientDirection::TO_TOP);
    float angleBottom = CssGradientDirectionToAngle(CssGradientDirection::TO_BOTTOM);
    EXPECT_FLOAT_EQ(angleTop + CSS_ANGLE_HALF_TURN, angleBottom);
}

/**
 * @tc.name: NormalizeCssAngleDegrees016
 * @tc.desc: Arbitrary angles wrap into [0, 360); NaN degrades to the CSS
 *           default instead of poisoning the downstream trigonometry.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, NormalizeCssAngleDegrees016, TestSize.Level1)
#else
void GradientInfoTddTest::NormalizeCssAngleDegrees016()
#endif
{
    // Arrange
    float normalized;

    // Act: angles wrap into [0, 360), NaN degrades to default
    /* Already inside the range: returned untouched. */
    normalized = NormalizeCssAngleDegrees(ANGLE_0);
    // Assert
    EXPECT_FLOAT_EQ(normalized, ANGLE_0);

    normalized = NormalizeCssAngleDegrees(ANGLE_45);
    EXPECT_FLOAT_EQ(normalized, ANGLE_45);

    normalized = NormalizeCssAngleDegrees(ANGLE_135);
    EXPECT_FLOAT_EQ(normalized, ANGLE_135);

    /* A full turn maps back onto zero, not onto 360. */
    normalized = NormalizeCssAngleDegrees(CSS_ANGLE_FULL_TURN);
    EXPECT_FLOAT_EQ(normalized, ANGLE_0);

    /* Beyond one turn. */
    normalized = NormalizeCssAngleDegrees(CSS_ANGLE_FULL_TURN + ANGLE_45);
    EXPECT_FLOAT_EQ(normalized, ANGLE_45);

    normalized = NormalizeCssAngleDegrees(ANGLE_1080 + ANGLE_90);
    EXPECT_FLOAT_EQ(normalized, ANGLE_90);

    /* Negative angles wrap forward. */
    normalized = NormalizeCssAngleDegrees(-ANGLE_45);
    EXPECT_FLOAT_EQ(normalized, ANGLE_315);

    normalized = NormalizeCssAngleDegrees(-ANGLE_90);
    EXPECT_FLOAT_EQ(normalized, CSS_ANGLE_TO_LEFT);

    normalized = NormalizeCssAngleDegrees(-(CSS_ANGLE_FULL_TURN + ANGLE_90));
    EXPECT_FLOAT_EQ(normalized, CSS_ANGLE_TO_LEFT);

    /* Non finite input degrades gracefully. */
    normalized = NormalizeCssAngleDegrees(MakeNan());
    EXPECT_FLOAT_EQ(normalized, CSS_ANGLE_DEFAULT);
}

/**
 * @tc.name: ResolveCssAngleKeyword017
 * @tc.desc: ResolveCssAngle forwards every keyword to the shared table, so the
 *           drawing adapter no longer needs its own switch statement.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, ResolveCssAngleKeyword017, TestSize.Level1)
#else
void GradientInfoTddTest::ResolveCssAngleKeyword017()
#endif
{
    // Arrange
    GradientInfo info;
    /* An angle left over from a previous parse must be ignored for keywords. */
    info.angle = CUSTOM_ANGLE_45;
    float resolved;

    // Act: keyword direction takes priority over stored angle
    info.direction = CssGradientDirection::TO_TOP;
    resolved = info.ResolveCssAngle();
    // Assert
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_TO_TOP);

    info.direction = CssGradientDirection::TO_RIGHT;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_TO_RIGHT);

    info.direction = CssGradientDirection::TO_BOTTOM;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_TO_BOTTOM);

    info.direction = CssGradientDirection::TO_BOTTOM_RIGHT;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_TO_BOTTOM_RIGHT);

    /* An unresolved direction still yields a drawable angle. */
    info.direction = CssGradientDirection::NONE;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_DEFAULT);
}

/**
 * @tc.name: ResolveCssAngleCustom018
 * @tc.desc: With CUSTOM_ANGLE the explicit angle wins and is normalized, so an
 *           out of range or NaN angle can never reach the endpoint computation.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, ResolveCssAngleCustom018, TestSize.Level1)
#else
void GradientInfoTddTest::ResolveCssAngleCustom018()
#endif
{
    // Arrange
    GradientInfo info;
    info.direction = CssGradientDirection::CUSTOM_ANGLE;
    float resolved;

    // Act: explicit angle is normalized

    info.angle = CUSTOM_ANGLE_45;
    resolved = info.ResolveCssAngle();
    // Assert
    EXPECT_FLOAT_EQ(resolved, CUSTOM_ANGLE_45);

    info.angle = CSS_ANGLE_FULL_TURN + ANGLE_135;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, ANGLE_135);

    info.angle = -ANGLE_45;
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, ANGLE_315);

    info.angle = MakeNan();
    resolved = info.ResolveCssAngle();
    EXPECT_FLOAT_EQ(resolved, CSS_ANGLE_DEFAULT);
}

/**
 * @tc.name: MoveAssignment019
 * @tc.desc: Move assignment releases the destination buffer before stealing the
 *           source one (no leak, no double free) and tolerates self assignment.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, MoveAssignment019, TestSize.Level1)
#else
void GradientInfoTddTest::MoveAssignment019()
#endif
{
    // Arrange
    GradientColorStop twoStops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    GradientColorStop threeStops[THREE_COLORS] = {
        MakeStop(CH_ZERO, CH_FULL, CH_ZERO, OFFSET_START),
        MakeStop(CH_FULL, CH_FULL, CH_ZERO, OFFSET_HALF),
        MakeStop(CH_ZERO, CH_FULL, CH_FULL, OFFSET_END)
    };

    /* Destination already owns a buffer, it must be freed by the assignment. */
    GradientInfo dst;
    dst.direction = CssGradientDirection::TO_TOP;
    dst.SetColorStops(twoStops, TWO_COLORS);
    ASSERT_TRUE(dst.isValid);

    GradientInfo src;
    src.direction = CssGradientDirection::CUSTOM_ANGLE;
    src.angle = CUSTOM_ANGLE_45;
    src.SetColorStops(threeStops, THREE_COLORS);
    GradientColorStop* srcRaw = src.colorStops;

    // Act
    dst = std::move(src);

    // Assert: ownership transferred to dst, source detached
    EXPECT_TRUE(dst.isValid);
    EXPECT_EQ(dst.colorCount, THREE_COLORS);
    EXPECT_EQ(dst.colorStops, srcRaw); /* ownership transferred, no copy */
    EXPECT_EQ(dst.direction, CssGradientDirection::CUSTOM_ANGLE);
    EXPECT_FLOAT_EQ(dst.angle, CUSTOM_ANGLE_45);
    EXPECT_FLOAT_EQ(dst.ResolveCssAngle(), CUSTOM_ANGLE_45);

    /* Source detached, its destructor must not free the stolen buffer. */
    EXPECT_EQ(src.colorStops, nullptr);
    EXPECT_EQ(src.colorCount, 0);
    EXPECT_FALSE(src.isValid);

    /* Self assignment must be a no-op rather than a use after free. */
    GradientInfo& alias = dst;
    dst = std::move(alias);
    EXPECT_TRUE(dst.isValid);
    EXPECT_EQ(dst.colorCount, THREE_COLORS);
    EXPECT_EQ(dst.colorStops, srcRaw);
}

/**
 * @tc.name: SetColorStopsReentry020
 * @tc.desc: Calling SetColorStops twice replaces the payload instead of leaking
 *           the previous array, and a rejected second call leaves the object in
 *           the cleared state rather than keeping a dangling pointer.
 * @tc.type: FUNC
 */
#ifdef TDD_ASSERTIONS
HWTEST_F(GradientInfoTddTest, SetColorStopsReentry020, TestSize.Level1)
#else
void GradientInfoTddTest::SetColorStopsReentry020()
#endif
{
    // Arrange
    GradientColorStop twoStops[TWO_COLORS] = {
        MakeStop(CH_FULL, CH_ZERO, CH_ZERO, OFFSET_START),
        MakeStop(CH_ZERO, CH_ZERO, CH_FULL, OFFSET_END)
    };
    GradientColorStop threeStops[THREE_COLORS] = {
        MakeStop(CH_ZERO, CH_FULL, CH_ZERO, OFFSET_START),
        MakeStop(CH_FULL, CH_FULL, CH_ZERO, OFFSET_HALF),
        MakeStop(CH_ZERO, CH_FULL, CH_FULL, OFFSET_END)
    };

    GradientInfo info;
    info.direction = CssGradientDirection::TO_BOTTOM_RIGHT;
    info.SetColorStops(twoStops, TWO_COLORS);
    ASSERT_TRUE(info.isValid);
    ASSERT_EQ(info.colorCount, TWO_COLORS);

    // Act: second call replaces the previous stops
    info.SetColorStops(threeStops, THREE_COLORS);

    // Assert: three color stops win, direction survives
    EXPECT_TRUE(info.isValid);
    EXPECT_EQ(info.colorCount, THREE_COLORS);
    EXPECT_EQ(info.colorStops[1].color.green, CH_FULL);
    EXPECT_FLOAT_EQ(info.colorStops[1].offset, OFFSET_HALF);
    /* The direction survives the replacement. */
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM_RIGHT);

    // Act: rejected call drops previous stops
    info.SetColorStops(nullptr, THREE_COLORS);

    // Assert: no dangling pointer, direction preserved
    EXPECT_FALSE(info.isValid);
    EXPECT_EQ(info.colorStops, nullptr);
    EXPECT_EQ(info.colorCount, 0);
    EXPECT_EQ(info.direction, CssGradientDirection::TO_BOTTOM_RIGHT);
}

#ifndef TDD_ASSERTIONS
void GradientInfoTddTest::RunTests()
{
    DefaultState001();
    SetTwoColorStops002();
    SetMultiColorStops003();
    SetColorStopsNull004();
    SetColorStopsLessThanTwo005();
    SetColorStopsMaxLimit006();
    DirectionAnglePreserved007();
    GetBeginEndColor008();
    GetBeginEndColorInvalid009();
    GetDirectionValue010();
    DeepCopyIndependence011();
    DeepCopyInvalidSource012();
    MoveConstructor013();
    ClearResets014();
    SharedDirectionAngleTable015();
    NormalizeCssAngleDegrees016();
    ResolveCssAngleKeyword017();
    ResolveCssAngleCustom018();
    MoveAssignment019();
    SetColorStopsReentry020();
}
#endif
} // namespace ACELite
} // namespace OHOS
