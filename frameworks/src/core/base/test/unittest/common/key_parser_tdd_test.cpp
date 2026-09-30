/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "key_parser_tdd_test.h"
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
#include <cstring>
#include "key_parser.h"
#include "keys.h"
#include "test_common.h"
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

namespace OHOS {
namespace ACELite {
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
using testing::ext::TestSize;
/**
 * @tc.name: ACELite_Key_Parser_Gap_001
 * @tc.desc: test parsing the gap keys added by RM018 gap requirement
 */
void KeyParserTddTest::KeyParserGapTest001()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. parse "gap", "columnGap" and "rowGap" with length
     * @tc.expected: step1. return K_GAP, K_COLUMN_GAP and K_ROW_GAP
     */
    EXPECT_EQ(KeyParser::ParseKeyId("gap", 3), K_GAP);         // 3: length of "gap"
    EXPECT_EQ(KeyParser::ParseKeyId("columnGap", 9), K_COLUMN_GAP); // 9: length of "columnGap"
    EXPECT_EQ(KeyParser::ParseKeyId("rowGap", 6), K_ROW_GAP);  // 6: length of "rowGap"

    /**
     * @tc.steps: step2. parse "gap", "columnGap" and "rowGap" without length
     * @tc.expected: step2. return K_GAP, K_COLUMN_GAP and K_ROW_GAP
     */
    EXPECT_EQ(KeyParser::ParseKeyId("gap"), K_GAP);
    EXPECT_EQ(KeyParser::ParseKeyId("columnGap"), K_COLUMN_GAP);
    EXPECT_EQ(KeyParser::ParseKeyId("rowGap"), K_ROW_GAP);
    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_002
 * @tc.desc: test key strings which are similar to gap keys but should not match
 */
void KeyParserTddTest::KeyParserGapTest002()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. parse prefix/suffix variants and case-changed variants of gap keys
     * @tc.expected: step1. all of them return K_UNKNOWN
     */
    EXPECT_EQ(KeyParser::ParseKeyId("g"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("ga"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("gaps"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("Gap"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("column"), K_COLUMN);       // exact shorter key still matches its own id
    EXPECT_EQ(KeyParser::ParseKeyId("columngap"), K_UNKNOWN);   // case sensitive, lowercase g not match
    EXPECT_EQ(KeyParser::ParseKeyId("columnGaps"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("rowgap"), K_UNKNOWN);      // case sensitive, lowercase g not match
    EXPECT_EQ(KeyParser::ParseKeyId("rowGaps"), K_UNKNOWN);

    /**
     * @tc.steps: step2. parse empty string and nullptr
     * @tc.expected: step2. return K_UNKNOWN
     */
    EXPECT_EQ(KeyParser::ParseKeyId("", 0), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId(nullptr, 3), K_UNKNOWN); // 3: dummy length
    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_003
 * @tc.desc: test neighbor keys of the new gap branches are parsed as before
 */
void KeyParserTddTest::KeyParserGapTest003()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. parse keys next to the new gap branches
     * @tc.expected: step1. all of them return their original key ids
     */
    EXPECT_EQ(KeyParser::ParseKeyId("column"), K_COLUMN);
    EXPECT_EQ(KeyParser::ParseKeyId("column-reverse"), K_COLUMN_REVERSE);
    EXPECT_EQ(KeyParser::ParseKeyId("row"), K_ROW);
    EXPECT_EQ(KeyParser::ParseKeyId("row-reverse"), K_ROW_REVERSE);
    // keys next to the new 'g' branch
    EXPECT_EQ(KeyParser::ParseKeyId("forwards"), K_FORWARDS);
    EXPECT_EQ(KeyParser::ParseKeyId("height"), K_HEIGHT);
    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_004
 * @tc.desc: test the new gap key ids are valid key ids
 */
void KeyParserTddTest::KeyParserGapTest004()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. check the new gap key ids with IsKeyValid
     * @tc.expected: step1. all of them are valid, K_UNKNOWN and KEYWORDS_MAX are invalid
     */
    EXPECT_TRUE(KeyParser::IsKeyValid(K_GAP));
    EXPECT_TRUE(KeyParser::IsKeyValid(K_COLUMN_GAP));
    EXPECT_TRUE(KeyParser::IsKeyValid(K_ROW_GAP));
    EXPECT_FALSE(KeyParser::IsKeyValid(K_UNKNOWN));
    EXPECT_FALSE(KeyParser::IsKeyValid(KEYWORDS_MAX));
    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_Key_Parser_Position_005
 * @tc.desc: test parsing the position keys added by RM020 absolute requirement
 */
void KeyParserTddTest::KeyParserPositionTest005()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. parse "position", "absolute" and "bottom" with length
     * @tc.expected: step1. return K_POSITION, K_ABSOLUTE and K_BOTTOM
     */
    EXPECT_EQ(KeyParser::ParseKeyId("position", 8), K_POSITION); // 8: length of "position"
    EXPECT_EQ(KeyParser::ParseKeyId("absolute", 8), K_ABSOLUTE); // 8: length of "absolute"
    EXPECT_EQ(KeyParser::ParseKeyId("bottom", 6), K_BOTTOM);     // 6: length of "bottom"

    /**
     * @tc.steps: step2. parse "position", "absolute" and "bottom" without length
     * @tc.expected: step2. return K_POSITION, K_ABSOLUTE and K_BOTTOM
     */
    EXPECT_EQ(KeyParser::ParseKeyId("position"), K_POSITION);
    EXPECT_EQ(KeyParser::ParseKeyId("absolute"), K_ABSOLUTE);
    EXPECT_EQ(KeyParser::ParseKeyId("bottom"), K_BOTTOM);

    /**
     * @tc.steps: step3. check the new position key ids with IsKeyValid
     * @tc.expected: step3. all of them are valid key ids
     */
    EXPECT_TRUE(KeyParser::IsKeyValid(K_POSITION));
    EXPECT_TRUE(KeyParser::IsKeyValid(K_ABSOLUTE));
    EXPECT_TRUE(KeyParser::IsKeyValid(K_BOTTOM));
    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_Key_Parser_Position_006
 * @tc.desc: test key strings similar to position keys but should not match,
 *           and neighbor keys of the new branches are parsed as before
 */
void KeyParserTddTest::KeyParserPositionTest006()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. parse prefix/suffix variants and case-changed variants of position keys
     * @tc.expected: step1. all of them return K_UNKNOWN
     */
    EXPECT_EQ(KeyParser::ParseKeyId("positions"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("Position"), K_UNKNOWN);  // case sensitive
    EXPECT_EQ(KeyParser::ParseKeyId("absolut"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("absolutes"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("Absolute"), K_UNKNOWN);  // case sensitive
    EXPECT_EQ(KeyParser::ParseKeyId("bottoms"), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("Bottom"), K_UNKNOWN);    // case sensitive

    /**
     * @tc.steps: step2. parse keys next to the new position branches
     * @tc.expected: step2. all of them return their original key ids
     */
    // keys next to K_ABSOLUTE in the 'a' branch
    EXPECT_EQ(KeyParser::ParseKeyId("alignItems"), K_ALIGN_ITEMS);
    // keys next to K_BOTTOM in the 'b' branch
    EXPECT_EQ(KeyParser::ParseKeyId("borderWidth"), K_BORDER_WIDTH);
    EXPECT_EQ(KeyParser::ParseKeyId("break"), K_BREAK);
    // keys next to K_POSITION in the 'p' branch
    EXPECT_EQ(KeyParser::ParseKeyId("percent"), K_PERCENT);
    EXPECT_EQ(KeyParser::ParseKeyId("picker-view"), K_PICKER_VIEW);
    // pre-existing offset keys are parsed as before
    EXPECT_EQ(KeyParser::ParseKeyId("left"), K_LEFT);
    EXPECT_EQ(KeyParser::ParseKeyId("top"), K_TOP);
    EXPECT_EQ(KeyParser::ParseKeyId("right"), K_RIGHT);
    TDD_CASE_END();
}

void KeyParserTddTest::FlexKeyParse001()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. flex related keys should be parsed to the correct key ids
     */
    EXPECT_EQ(KeyParser::ParseKeyId("flexDirection", std::strlen("flexDirection")), K_FLEX_DIRECTION);
    EXPECT_EQ(KeyParser::ParseKeyId("justifyContent", std::strlen("justifyContent")), K_JUSTIFY_CONTENT);
    EXPECT_EQ(KeyParser::ParseKeyId("alignItems", std::strlen("alignItems")), K_ALIGN_ITEMS);
    EXPECT_EQ(KeyParser::ParseKeyId("flexWrap", std::strlen("flexWrap")), K_FLEX_WRAP);
    EXPECT_EQ(KeyParser::ParseKeyId("row", std::strlen("row")), K_ROW);
    EXPECT_EQ(KeyParser::ParseKeyId("column", std::strlen("column")), K_COLUMN);
    EXPECT_EQ(KeyParser::ParseKeyId("flex-start", std::strlen("flex-start")), K_FLEX_START);
    EXPECT_EQ(KeyParser::ParseKeyId("flex-end", std::strlen("flex-end")), K_FLEX_END);
    EXPECT_EQ(KeyParser::ParseKeyId("center", std::strlen("center")), K_CENTER);
    EXPECT_EQ(KeyParser::ParseKeyId("space-between", std::strlen("space-between")), K_SPACE_BETWEEN);
    EXPECT_EQ(KeyParser::ParseKeyId("space-around", std::strlen("space-around")), K_SPACE_AROUND);
    EXPECT_EQ(KeyParser::ParseKeyId("space-evenly", std::strlen("space-evenly")), K_SPACE_EVENLY);
    EXPECT_EQ(KeyParser::ParseKeyId("wrap", std::strlen("wrap")), K_WRAP);
    EXPECT_EQ(KeyParser::ParseKeyId("stretch", std::strlen("stretch")), K_STRETCH);
    /* *
     * @tc.steps: step2. illegal keys should be parsed as K_UNKNOWN (exact match, no prefix match)
     */
    EXPECT_EQ(KeyParser::ParseKeyId("flexDirectionX", std::strlen("flexDirectionX")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("justifyContentX", std::strlen("justifyContentX")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("alignItemsX", std::strlen("alignItemsX")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("rows", std::strlen("rows")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("columns", std::strlen("columns")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("wraps", std::strlen("wraps")), K_UNKNOWN);
    /* truncated keys should not be matched as substrings */
    EXPECT_EQ(KeyParser::ParseKeyId("flexDirectio", std::strlen("flexDirectio")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("space-betwee", std::strlen("space-betwee")), K_UNKNOWN);
    /* key matching is case sensitive */
    EXPECT_EQ(KeyParser::ParseKeyId("FlexDirection", std::strlen("FlexDirection")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("ROW", std::strlen("ROW")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("stretchable", std::strlen("stretchable")), K_UNKNOWN);
    TDD_CASE_END();
}

void KeyParserTddTest::KeyParseOverflowAuto002()
{
    TDD_CASE_BEGIN();
    /* *
     * @tc.steps: step1. parse the style key and value strings used by RM.017
     * @tc.expected: step1. all of them are parsed to valid key ids instead of K_UNKNOWN
     */
    EXPECT_EQ(KeyParser::ParseKeyId("auto", std::strlen("auto")), K_AUTO);
    EXPECT_EQ(KeyParser::ParseKeyId("hidden", std::strlen("hidden")), K_HIDDEN);
    EXPECT_EQ(KeyParser::ParseKeyId("overflow", std::strlen("overflow")), K_OVERFLOW);
    EXPECT_EQ(KeyParser::ParseKeyId("visible", std::strlen("visible")), K_VISIBLE);
    /* *
     * @tc.expected: step2. pre-existing neighbor keys are not affected
     */
    EXPECT_EQ(KeyParser::ParseKeyId("height", std::strlen("height")), K_HEIGHT);
    EXPECT_EQ(KeyParser::ParseKeyId("options", std::strlen("options")), K_OPTIONS);
    EXPECT_EQ(KeyParser::ParseKeyId("vertical", std::strlen("vertical")), K_VERTICAL);
    /* *
     * @tc.expected: step3. look-alike strings still return K_UNKNOWN
     */
    EXPECT_EQ(KeyParser::ParseKeyId("autox", std::strlen("autox")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("visiblex", std::strlen("visiblex")), K_UNKNOWN);
    EXPECT_EQ(KeyParser::ParseKeyId("hid", std::strlen("hid")), K_UNKNOWN);
    TDD_CASE_END();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ACELite_Key_Parser_Gap_001
 * @tc.desc: test parsing the gap keys added by RM018 gap requirement
 */
HWTEST_F(KeyParserTddTest, keyParserGapTest001, TestSize.Level1)
{
    KeyParserTddTest::KeyParserGapTest001();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_002
 * @tc.desc: test key strings which are similar to gap keys but should not match
 */
HWTEST_F(KeyParserTddTest, keyParserGapTest002, TestSize.Level1)
{
    KeyParserTddTest::KeyParserGapTest002();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_003
 * @tc.desc: test neighbor keys of the new gap branches are parsed as before
 */
HWTEST_F(KeyParserTddTest, keyParserGapTest003, TestSize.Level1)
{
    KeyParserTddTest::KeyParserGapTest003();
}

/**
 * @tc.name: ACELite_Key_Parser_Gap_004
 * @tc.desc: test the new gap key ids are valid key ids
 */
HWTEST_F(KeyParserTddTest, keyParserGapTest004, TestSize.Level1)
{
    KeyParserTddTest::KeyParserGapTest004();
}

/**
 * @tc.name: ACELite_Key_Parser_Position_005
 * @tc.desc: test parsing the position keys added by RM020 absolute requirement
 */
HWTEST_F(KeyParserTddTest, keyParserPositionTest005, TestSize.Level1)
{
    KeyParserTddTest::KeyParserPositionTest005();
}

/**
 * @tc.name: ACELite_Key_Parser_Position_006
 * @tc.desc: test key strings similar to position keys but should not match
 */
HWTEST_F(KeyParserTddTest, keyParserPositionTest006, TestSize.Level1)
{
    KeyParserTddTest::KeyParserPositionTest006();
}

/* *
 * @tc.name: FlexKeyParse001
 * @tc.desc: Verify flex related keys are parsed to the correct key ids,
 *           and illegal keys are parsed as K_UNKNOWN.
 */
HWTEST_F(KeyParserTddTest, keyParserFlex001, TestSize.Level1)
{
    KeyParserTddTest::FlexKeyParse001();
}

/* *
 * @tc.name: KeyParseOverflowAuto002
 * @tc.desc: Verify auto/hidden/overflow/visible strings are parsed to valid key ids,
 *           which is the precondition of the RM.017 overflow and margin auto features.
 */
HWTEST_F(KeyParserTddTest, keyParseOverflowAuto002, TestSize.Level1)
{
    KeyParserTddTest::KeyParseOverflowAuto002();
}
#endif
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT

#ifndef TDD_ASSERTIONS
void KeyParserTddTest::RunTests()
{
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    KeyParserGapTest001();
    KeyParserGapTest002();
    KeyParserGapTest003();
    KeyParserGapTest004();
    KeyParserPositionTest005();
    KeyParserPositionTest006();
    FlexKeyParse001();
    KeyParseOverflowAuto002();
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
}
#endif
} // namespace ACELite
} // namespace OHOS
