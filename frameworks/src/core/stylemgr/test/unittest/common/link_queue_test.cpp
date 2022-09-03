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

#include "link_queue_test.h"
#include "link_queue.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
constexpr uint32_t QUEUE_MAX_SIZE = 8;
const char inputQueue1[] = "firstInput";
const char inputQueue2[] = "secondInput";

/**
 * @tc.name: InitQueue001
 * @tc.desc: test the init queue success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, InitQueue001, TestSize.Level1)
{
    LinkQueue queue;
    queue.InitQueue();
    
    EXPECT_TRUE(queue.GetFront() != nullptr);
    EXPECT_TRUE(queue.GetRear() != nullptr);
    EXPECT_EQ(queue.LengthQueue(), 0);

    queue.FreeNode();
}

/**
 * @tc.name: FreeNode002
 * @tc.desc: test the free node success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, FreeNode002, TestSize.Level1)
{
    LinkQueue queue;

    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
        queue.Enqueue("queueTest");
    }
    queue.FreeNode();
    EXPECT_EQ(queue.LengthQueue(), 0);
    EXPECT_TRUE(queue.GetFront() == nullptr);
}

/**
 * @tc.name: Enqueue003
 * @tc.desc: test the enqueue success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, Enqueue003, TestSize.Level1)
{
    LinkQueue queue;
    queue.Enqueue(inputQueue1);
    EXPECT_STREQ(queue.GetRear()->GetNodeData(), "firstInput");
    EXPECT_EQ(queue.LengthQueue(), 1);

    queue.Enqueue(inputQueue2);
    EXPECT_STREQ(queue.GetRear()->GetNodeData(), "secondInput");
    EXPECT_EQ(queue.LengthQueue(), 2);
}

/**
 * @tc.name: Dequeue004
 * @tc.desc: test the Dequeue success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, Dequeue004, TestSize.Level1)
{
    LinkQueue queue;
    const char *getvalue = nullptr;
    queue.Dequeue(&getvalue);
    EXPECT_TRUE(getvalue == nullptr);
    queue.Enqueue(inputQueue1);
    queue.Enqueue(inputQueue2);
    EXPECT_EQ(queue.LengthQueue(), 2);

    queue.Dequeue(&getvalue);
    EXPECT_TRUE(getvalue != nullptr);
    EXPECT_STREQ(getvalue, "firstInput");
    EXPECT_EQ(queue.LengthQueue(), 1);

    queue.Dequeue(&getvalue);
    EXPECT_TRUE(getvalue != nullptr);
    EXPECT_STREQ(getvalue, "secondInput");
    EXPECT_EQ(queue.LengthQueue(), 0);
}

/**
 * @tc.name: LimitMaxSizeEnQueue005
 * @tc.desc: test set the maximum size of the queue, and push more nodes than the maximum size
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, LimitMaxSizeEnQueue005, TestSize.Level2)
{
    LinkQueue queue(QUEUE_MAX_SIZE);

    for (int i = 0; i < QUEUE_MAX_SIZE + 1; i++) {
        queue.Enqueue("queueTest");
    }
    EXPECT_EQ(queue.LengthQueue(), QUEUE_MAX_SIZE);
}

/**
 * @tc.name: GetNext006
 * @tc.desc: test get next node data success
 * @tc.type: FUNC
 * @tc.require: I5NCRX
 */
HWTEST_F(LinkQueueTest, GetNext006, TestSize.Level1)
{
    LinkQueue queue;
    QueueNode *node = nullptr;
    node = queue.GetNext();
    EXPECT_TRUE(node == nullptr);

    queue.Enqueue(inputQueue1);
    node = queue.GetNext();
    EXPECT_TRUE(node != nullptr);

    EXPECT_STREQ(node->GetNodeData(), "firstInput");
}

#ifndef TDD_ASSERTIONS
void LinkQueueTest::RunTests()
{
    InitQueue001();
    FreeNode002();
    Enqueue003();
    Dequeue004();
    LimitMaxSizeEnQueue005();
    GetNext006();
}
#endif
}
}
