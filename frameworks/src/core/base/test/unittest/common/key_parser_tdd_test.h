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

#ifndef OHOS_ACELITE_KEY_PARSER_TDD_TEST
#define OHOS_ACELITE_KEY_PARSER_TDD_TEST
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
class KeyParserTddTest : public testing::Test {
#else
class KeyParserTddTest {
#endif
public:
    void SetUp() {}
    void TearDown() {}
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    void KeyParserGapTest001();
    void KeyParserGapTest002();
    void KeyParserGapTest003();
    void KeyParserGapTest004();
    void KeyParserPositionTest005();
    void KeyParserPositionTest006();
    void FlexKeyParse001();
    void KeyParseOverflowAuto002();
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
} // namespace ACELite
} // namespace OHOS
#endif
