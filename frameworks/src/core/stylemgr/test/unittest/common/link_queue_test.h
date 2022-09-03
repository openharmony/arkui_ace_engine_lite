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

#ifndef OHOS_ACELITE_LINK_QUEUE_TEST_H
#define OHOS_ACELITE_LINK_QUEUE_TEST_H
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
class LinkQueueTest : public testing::Test {
#else
class LinkQueueTest {
#endif
public:
    void SetUp() {}
    void TearDown() {}
    void InitQueue001();
    void FreeNode002();
    void Enqueue003();
    void Dequeue004();
    void LimitMaxSizeEnQueue005();
    void GetNext006();
#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
};
}
}
#endif
