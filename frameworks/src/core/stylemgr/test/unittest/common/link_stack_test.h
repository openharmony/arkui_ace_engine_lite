/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_LINK_STACK_TEST_H
#define OHOS_ACELITE_LINK_STACK_TEST_H
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
class LinkStackTest : public testing::Test {
#else
class LinkStackTest {
#endif
public:
    void SetUp() {}
    void TearDown() {}
    void InitStack001();
    void FreeNode002();
    void Push003();
    void Pop004();
    void IsFull005();
    void IsEmpty006();
    void Peak007();
#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
}
}
#endif
