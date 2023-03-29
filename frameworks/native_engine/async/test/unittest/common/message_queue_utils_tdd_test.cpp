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
#include "message_queue_utils_tdd_test.h"
#include "product_adapter.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
bool g_restoreSystemHandlerFlag = false;
void RestoreSystemInterface(const char *crashMessage)
{
    g_restoreSystemHandlerFlag = true;
    HILOG_ERROR(HILOG_MODULE_ACE, crashMessage);
}

/**
 * @tc.name: MessageTddTest01
 * @tc.desc: Test RegRestoreSystemHandler and RestoreSystemWrapper function.
 */
HWTEST_F(MessageQueueUtilsTddTest, MessageTddTest01, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    g_restoreSystemHandlerFlag = false;
    ProductAdapter::RegRestoreSystemHandler(RestoreSystemInterface);
    ProductAdapter::RestoreSystemWrapper("Message Queue Utils Tdd Test");
    EXPECT_TRUE(g_restoreSystemHandlerFlag);
    TDD_CASE_END();
}
} // namespace ACELite
} // namespace OHOS
