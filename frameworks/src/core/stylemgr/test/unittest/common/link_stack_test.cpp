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

#include "link_stack_test.h"
#include "link_stack.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
constexpr uint32_t STACK_EMPTY = 0;
constexpr uint32_t STACK_MAX_SIZE = 8;

const char inputStack1[] = "firstInput";
const char inputStack2[] = "secondInput";

/**
 * @tc.name: InitStack001
 * @tc.desc: test the init stack success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, InitStack001, TestSize.Level1)
{
    LinkStack stack;
    stack.InitStack();

    EXPECT_TRUE(stack.GetTop() != nullptr);
    EXPECT_EQ(stack.StackSize(), STACK_EMPTY);

    stack.FreeNode();
}

/**
 * @tc.name: FreeNode002
 * @tc.desc: test the stack free node success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, FreeNode002, TestSize.Level1)
{
    LinkStack stack;

    for (int i = 0; i < STACK_MAX_SIZE; i++) {
        stack.Push("stackTest");
    }
    stack.FreeNode();
    EXPECT_EQ(stack.StackSize(), STACK_EMPTY);
    EXPECT_TRUE(stack.GetTop() == nullptr);
}

/**
 * @tc.name: Push003
 * @tc.desc: test the stack push node success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, Push003, TestSize.Level1)
{
    LinkStack stack(STACK_MAX_SIZE);
    stack.Push(inputStack1);
    EXPECT_STREQ(stack.GetTop()->GetNodeData(), "firstInput");
    EXPECT_EQ(stack.StackSize(), 1);

    stack.Push(inputStack2);
    EXPECT_STREQ(stack.GetTop()->GetNodeData(), "secondInput");
    EXPECT_EQ(stack.StackSize(), 2);
}

/**
 * @tc.name: Pop004
 * @tc.desc: test the stack pop node success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, Pop004, TestSize.Level1)
{
    LinkStack stack;
    const char *getvalue = nullptr;
    stack.Pop(&getvalue);
    EXPECT_TRUE(getvalue == nullptr);
    stack.Push(inputStack1);
    stack.Push(inputStack2);
    EXPECT_EQ(stack.StackSize(), 2);

    stack.Pop(&getvalue);
    EXPECT_TRUE(getvalue != nullptr);
    EXPECT_STREQ(getvalue, "secondInput");
    EXPECT_EQ(stack.StackSize(), 1);

    stack.Pop(&getvalue);
    EXPECT_TRUE(getvalue != nullptr);
    EXPECT_STREQ(getvalue, "firstInput");
    EXPECT_EQ(stack.StackSize(), 0);
}

/**
 * @tc.name: IsFull005
 * @tc.desc: test set the maximum size of the stack, and push more nodes than the maximum size
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, IsFull005, TestSize.Level1)
{
    LinkStack stack(STACK_MAX_SIZE);

    for (int i = 0; i < STACK_MAX_SIZE + 1; i++) {
        stack.Push("stackTest");
    }
    EXPECT_EQ(stack.StackSize(), STACK_MAX_SIZE);
}

/**
 * @tc.name: IsEmpty006
 * @tc.desc: Test whether the stack is collapsed if there are more nodes in
 * the stack than in the stack
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, IsEmpty006, TestSize.Level2)
{
    LinkStack stack(STACK_MAX_SIZE);

    for (int i = 0; i < STACK_MAX_SIZE; i++) {
        stack.Push("stackTest");
    }

    for (int i = 0; i < STACK_MAX_SIZE + 1; i++) {
        stack.Pop(nullptr);
    }
    EXPECT_EQ(stack.StackSize(), STACK_EMPTY);
}

/**
 * @tc.name: Peak007
 * @tc.desc: Test to see the top element of the stack
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkStackTest, Peak007, TestSize.Level1)
{
    LinkStack stack;
    stack.Push("stackTest");
    EXPECT_EQ(stack.StackSize(), 1);
    const char *p = stack.Peak();
    EXPECT_STREQ("stackTest", p);
    EXPECT_EQ(stack.StackSize(), 1);
}

#ifndef TDD_ASSERTIONS
void LinkStackTest::RunTests()
{
    InitStack001();
    FreeNode002();
    Push003();
    Pop004();
    IsFull005();
    IsEmpty006();
    Peak007();
}
#endif
}
}
