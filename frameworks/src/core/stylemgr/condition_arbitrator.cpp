/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "condition_arbitrator.h"

#include "ace_log.h"
#include "js_fwk_common.h"
#include "securec.h"
#include "stdlib.h"
#include "string_util.h"
#include "strings.h"
#include "system_info.h"

namespace OHOS {
namespace ACELite {
constexpr uint32_t LIMIT_CONDITION_LEN = 512;
constexpr int32_t OPERATOR_BRACKETS = 1;
constexpr int32_t OPERATOR_OR = 2;
constexpr int32_t OPERATOR_AND = 3;
constexpr uint32_t SPACE_CHAR_LIMIT_SCOPE = 5;
constexpr int32_t SLICE_FROM_CURRENT_POS = 0;
constexpr uint8_t MAX_LENGTH_PER_CONDITION = 32;
constexpr uint8_t MAX_LENGTH_PER_CONDITION_ADD_BRACKETS = 35;
constexpr uint8_t ASSICLL_DIGIT_START = 48;
constexpr uint8_t ASSICLL_DIGIT_END = 57;

/**
 * @brief Absolute value of x.
 */
#define ABS_VALUE(x) ((x) > 0 ? (x) : (-(x)))

/**
 * @brief parse the whole condition string into single condition items, and check if it matches the current device
 *        environment, and gives the final result of the entire media query expression.
 * @param conditions the input media query condition string
 * @return the result representing if the media query matches the current environment, true for positive result
 *
 */
bool ConditionArbitrator::Decide(const char *conditions) const
{
    bool result = IsValid(conditions);
    if (!result) {
        return false;
    }
    if (strstr(conditions, "and") == nullptr && strstr(conditions, "or") == nullptr) {
        return JudgeCondition(conditions);
    }
    LinkQueue queue;
    LinkQueue expressionQueue;
    result = DecomPositionConditions(conditions, &queue);
    if (!result || queue.IsEmpty()) {
        FreeMallocData(&queue);
        return false;
    }
    TransformExpression(&queue, &expressionQueue);
    result = Calculate(&expressionQueue);
    FreeMallocData(&queue);
    return result;
}

bool ConditionArbitrator::JudgeCondition(const char *condition) const
{
    if (condition == nullptr || strlen(condition) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the condition is invalid");
        return false;
    }

    // treat screen as one single media query feature, and it always is true for any devices
    if (strcmp(condition, "screen") == 0) {
        return true;
    }

    uint8_t conditionLen = strlen(condition);
    // the condition must start with '(', end with ')'
    if (condition[0] != '(' || (conditionLen <= 1) || (condition[conditionLen - 1] != ')')) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error format, condition is not properly packed with ( )");
        return false;
    }
    const uint8_t minTargetLen = 2;
    char *targetCondition = StringUtil::Slice(condition, 1, conditionLen - minTargetLen + 1);
    if (targetCondition == nullptr) {
        return false;
    }
    // devide the target condition into two parts : conditionName and value  through ":"
    char *targetValue = nullptr;
    char *conditionNameStr = strtok_s(targetCondition, ":", &targetValue);
    // judge the condition is success or not
    ConditionName conditionId = GetConditionName(StringUtil::Trim(conditionNameStr));
    char *trimedTargetValue = StringUtil::Trim(targetValue);
    if (conditionId == ConditionName::UNKOWN || trimedTargetValue == nullptr) {
        ACE_FREE(targetCondition);
        targetCondition = nullptr;
        return false;
    }
    bool result = JudgeConditionAction(conditionId, trimedTargetValue);
    ACE_FREE(targetCondition);
    targetCondition = nullptr;
    return result;
}

bool ConditionArbitrator::JudgeConditionAction(ConditionName conditionId, const char *trimedTargetValue) const
{
    if (trimedTargetValue == nullptr || strlen(trimedTargetValue) == 0) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::DEVICE_TYPE: // fall through
        case ConditionName::ROUND_SCREEN:
            return JudgeConditionByStrValue(conditionId, trimedTargetValue);
        case ConditionName::WIDTH: // fall through
        case ConditionName::MIN_WIDTH: // fall through
        case ConditionName::MAX_WIDTH: // fall through
        case ConditionName::HEIGHT: // fall through
        case ConditionName::MIN_HEIGHT: // fall through
        case ConditionName::MAX_HEIGHT: // fall through
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return JudgeConditionByNumberValue(conditionId, trimedTargetValue);
        }
        default: {
            HILOG_ERROR(HILOG_MODULE_ACE, "not supported condition feature %{public}d", conditionId);
            return false;
        }
    }
}

bool ConditionArbitrator::JudgeConditionByStrValue(ConditionName conditionId, const char *trimedTargetValue) const
{
    bool result = false;
    switch (conditionId) {
        case ConditionName::DEVICE_TYPE: {
            result = (strcmp(trimedTargetValue, SystemInfo::GetInstance().GetDeviceType()) == 0);
            break;
        }
        case ConditionName::ROUND_SCREEN: {
            if (!strcmp(trimedTargetValue, "TRUE") || !strcmp(trimedTargetValue, "true") ||
                !strcmp(trimedTargetValue, "1")) {
                result = (SystemInfo::GetInstance().IsRoundScreen() == true);
            } else if (!strcmp(trimedTargetValue, "FALSE") || !strcmp(trimedTargetValue, "false") ||
                       !(strcmp(trimedTargetValue, "0"))) {
                result = (SystemInfo::GetInstance().IsRoundScreen() == false);
            } else {
                result = false;
            }
            break;
        }
        default: {
            return false;
        }
    }
    return result;
}

bool ConditionArbitrator::JudgeConditionByNumberValue(ConditionName conditionId, const char *targetValue) const
{
    // must be started with number character
    if (!(targetValue[0] >= ASSICLL_DIGIT_START && targetValue[0] <= ASSICLL_DIGIT_END)) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::WIDTH: // fall through
        case ConditionName::MIN_WIDTH: // fall through
        case ConditionName::MAX_WIDTH: // fall through
        case ConditionName::HEIGHT: // fall through
        case ConditionName::MIN_HEIGHT: // fall through
        case ConditionName::MAX_HEIGHT: {
            return CompareIntDimension(conditionId, targetValue);
        }
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return CompareFloatDimension(conditionId, targetValue);
        }
        default: {
            return false;
        }
    }
}

bool ConditionArbitrator::CompareIntDimension(ConditionName conditionId, const char *targetValue) const
{
    int dimensionValue = atoi(targetValue);
    if (dimensionValue <= 0 || dimensionValue >= UINT16_MAX) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::WIDTH: {
            return SystemInfo::GetInstance().GetScreenWidth() == dimensionValue;
        }
        case ConditionName::MIN_WIDTH: {
            // the device screen width must be larger than the requirement
            return SystemInfo::GetInstance().GetScreenWidth() >= dimensionValue;
        }
        case ConditionName::MAX_WIDTH: {
            // the device screen width must be smaller than the requirement
            return SystemInfo::GetInstance().GetScreenWidth() <= dimensionValue;
        }
        case ConditionName::HEIGHT: {
            return SystemInfo::GetInstance().GetScreenHeight() == dimensionValue;
        }
        case ConditionName::MIN_HEIGHT: {
            // the device screen height must be larger than the requirement
            return SystemInfo::GetInstance().GetScreenHeight() >= dimensionValue;
        }
        case ConditionName::MAX_HEIGHT: {
            // the device screen height must be smaller than the requirement
            return SystemInfo::GetInstance().GetScreenHeight() <= dimensionValue;
        }
        default:
            return false;
    }
}

bool ConditionArbitrator::IsFloatValueEqual(float left, float right, float precision) const
{
    return (ABS_VALUE(left - right) < precision);
}

bool ConditionArbitrator::CompareFloatDimension(ConditionName conditionId, const char *targetValue) const
{
    float floatValue = atof(targetValue);
    switch (conditionId) {
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return CompareAspectRatio(conditionId, floatValue);
        }
        default: {
            return false;
        }
    }
}

bool ConditionArbitrator::CompareAspectRatio(ConditionName conditionId, float targetRatioValue) const
{
    float currentAspectRatio = SystemInfo::GetInstance().GetAspectRatio();
    bool isEqual = IsFloatValueEqual(targetRatioValue, currentAspectRatio, CONDITION_FLOAT_VALUE_EPRECISION);
    if (isEqual) {
        // the equal case matches for all ASPECT_RATIO media feature types
        return true;
    }
    switch (conditionId) {
        case ConditionName::ASPECT_RATIO: {
            return isEqual;
        }
        case ConditionName::MIN_ASPECT_RATIO: {
            return currentAspectRatio > targetRatioValue;
        }
        case ConditionName::MAX_ASPECT_RATIO: {
            return currentAspectRatio < targetRatioValue;
        }
        default: {
            return false;
        }
    }
}

ConditionName ConditionArbitrator::GetConditionName(const char *conditionName) const
{
    if (conditionName == nullptr || strlen(conditionName) == 0) {
        return ConditionName::UNKOWN;
    }
    static const struct {
        const char *nameStr;
        ConditionName name;
    } conditionNamePair[ConditionName::MAX_COUNT] = {
        {"width", ConditionName::WIDTH},
        {"height", ConditionName::HEIGHT},
        {"min-width", ConditionName::MIN_WIDTH},
        {"max-width", ConditionName::MAX_WIDTH},
        {"min-height", ConditionName::MIN_HEIGHT},
        {"max-height", ConditionName::MAX_HEIGHT},
        {"aspect-ratio", ConditionName::ASPECT_RATIO},
        {"min-aspect-ratio", ConditionName::MIN_ASPECT_RATIO},
        {"max-aspect-ratio", ConditionName::MAX_ASPECT_RATIO},
        {"device-type", ConditionName::DEVICE_TYPE},
        {"round-screen", ConditionName::ROUND_SCREEN}
    };
    ConditionName targetName = ConditionName::UNKOWN;
    uint8_t index = 0;
    for (; index < ConditionName::UNKOWN; index++) {
        if (strcmp(conditionName, conditionNamePair[index].nameStr) == 0) {
            targetName = conditionNamePair[index].name;
            break;
        }
    }
    if (index == ConditionName::UNKOWN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the condition name is not supported [%{public}s]", conditionName);
    }
    return targetName;
}

bool ConditionArbitrator::IsValid(const char *conditions) const
{
    if (conditions == nullptr || strlen(conditions) >= LIMIT_CONDITION_LEN) {
        return false;
    }

    LinkStack stack;
    while (*conditions != '\0') {
        if (*conditions == '(') {
            stack.Push("(");
        } else if (*conditions == ')') {
            if (stack.IsEmpty() || strcmp("(", stack.Peak()) != 0) {
                return false;
            }
            stack.Pop(nullptr);
        }
        conditions++;
    }
    return stack.IsEmpty() ? true : false;
}

const char *ConditionArbitrator::FindFirstPos(const char *conditions, uint8_t *size) const
{
    if (conditions == nullptr || size == nullptr) {
        return nullptr;
    }

    const char *recordPos = conditions;
    while (*recordPos != '\0') {
        if (*recordPos == '(' || *recordPos == ')' || IsOperator(recordPos)) {
            return recordPos;
        }
        recordPos++;
        (*size)++;
    }
    return nullptr;
}

const char *ConditionArbitrator::FindNoSpacePos(const char *conditions) const
{
    if (conditions == nullptr) {
        return nullptr;
    }

    while (*conditions != '\0' && ((*conditions) == ' ' ||
        static_cast<uint32_t>(*conditions) - '\t' < SPACE_CHAR_LIMIT_SCOPE)) {
        conditions++;
    }
    return conditions;
}

bool ConditionArbitrator::DecomPositionConditions(const char *conditions, LinkQueue* queue) const
{
    conditions = FindNoSpacePos(conditions);
    if (conditions == nullptr || strlen(conditions) == 0 || queue == nullptr || !queue->IsEmpty()) {
        return false;
    }
    const char *recordPos = nullptr;
    char *buff = nullptr;
    uint8_t len = 0;
    bool result = false;
    while ((recordPos = FindFirstPos(conditions, &len)) != nullptr) {
        if (len == 0) {
            switch (*recordPos) {
                case '(' :
                case ')' :
                    buff = StringUtil::Slice(recordPos, SLICE_FROM_CURRENT_POS, OPERATOR_BRACKETS);
                    recordPos++;
                    break;
                case 'a' :
                    buff = StringUtil::Slice(recordPos, SLICE_FROM_CURRENT_POS, OPERATOR_AND);
                    recordPos = recordPos + OPERATOR_AND;
                    break;
                case 'o' :
                    buff = StringUtil::Slice(recordPos, SLICE_FROM_CURRENT_POS, OPERATOR_OR);
                    recordPos = recordPos + OPERATOR_OR;
                    break;
                default :
                    break;
            }
            recordPos = FindNoSpacePos(recordPos);
        } else {
            buff = StringUtil::Slice(conditions, SLICE_FROM_CURRENT_POS, len * sizeof(char));
        }

        if (buff != nullptr) {
            result = queue->Enqueue(buff);
            if (!result) {
                return false;
            }
        }
        conditions = recordPos;
        len = 0;
    }
    if (conditions && *conditions != '\0' && strlen(conditions) > 0) {
        buff = StringUtil::Slice(conditions, SLICE_FROM_CURRENT_POS);
        if (buff != nullptr) {
            result = queue->Enqueue(buff);
        }
    }
    return result;
}

bool ConditionArbitrator::IsOperator(const char *str) const
{
    if (str != nullptr) {
        if (!strncmp("and", str, OPERATOR_AND) || !strncmp("or", str, OPERATOR_OR)) {
            return true;
        }
    }
    return false;
}

void ConditionArbitrator::TransformExpression(const LinkQueue *queue, LinkQueue *expressionQueue) const
{
    if (queue == nullptr || expressionQueue == nullptr) {
        return;
    }
    if (queue->IsEmpty()) {
        return;
    }
    const char *getvalue = nullptr;
    LinkStack stack;
    QueueNode *tmp = queue->GetNext();
    while (tmp) {
        if (strcmp(tmp->GetNodeData(), "(") == 0) {
            stack.Push(tmp->GetNodeData());
        } else if (strcmp(tmp->GetNodeData(), ")") == 0) {
            while (!stack.IsEmpty() && strcmp(stack.Peak(), "(") != 0) {
                stack.Pop(&getvalue);
                expressionQueue->Enqueue(getvalue);
            }
            stack.Pop(nullptr);
        } else if (IsOperator(tmp->GetNodeData())) {
            if (!stack.IsEmpty() && strcmp("(", stack.Peak()) != 0) {
                stack.Pop(&getvalue);
                expressionQueue->Enqueue(getvalue);
            }
            stack.Push(tmp->GetNodeData());
        } else {
            expressionQueue->Enqueue(tmp->GetNodeData());
        }
        tmp = tmp->GetNodeNext();
    }

    while (!stack.IsEmpty()) {
        stack.Pop(&getvalue);
        expressionQueue->Enqueue(getvalue);
    }
}

bool ConditionArbitrator::Parse(const char *condition) const
{
    if (strcmp(condition, "screen") == 0 || strcmp(condition, "true") == 0) {
        return true;
    } else if (strcmp(condition, "false") == 0) {
        return false;
    } else if (strlen(condition) > MAX_LENGTH_PER_CONDITION) {
        return false;
    }

    char conditionWithBrackets[MAX_LENGTH_PER_CONDITION_ADD_BRACKETS] = {0};
    if (strcat_s(conditionWithBrackets, MAX_LENGTH_PER_CONDITION_ADD_BRACKETS, "(") != 0) {
        return false;
    }
    if (strcat_s(conditionWithBrackets, MAX_LENGTH_PER_CONDITION_ADD_BRACKETS, condition) != 0) {
        return false;
    }
    if (strcat_s(conditionWithBrackets, MAX_LENGTH_PER_CONDITION_ADD_BRACKETS, ")") != 0) {
        return false;
    }
    return JudgeCondition(conditionWithBrackets);
}

bool ConditionArbitrator::Calculate(LinkQueue *expressionQueue) const
{
    if (expressionQueue == nullptr || expressionQueue->IsEmpty()) {
        return false;
    }

    bool result = false;
    LinkStack stack;
    const char *value = nullptr;
    const char resultTrue[] = "true";
    const char resultFalse[] = "false";
    while (!expressionQueue->IsEmpty()) {
        expressionQueue->Dequeue(&value);
        if (!IsOperator(value)) {
            stack.Push(value);
        } else {
            const char *condition1 = nullptr;
            const char *condition2 = nullptr;
            stack.Pop(&condition1);
            stack.Pop(&condition2);
            if (condition1 == nullptr || condition2 == nullptr) {
                return false;
            }
            char *trimStr1 = StringUtil::Trim(const_cast<char*>(condition1));
            char *trimStr2 = StringUtil::Trim(const_cast<char*>(condition2));
            if (trimStr1 == nullptr || trimStr2 == nullptr) {
                return false;
            }
            switch (*value) {
                case 'a':
                    result = Parse(trimStr1) && Parse(trimStr2);
                    break;
                case 'o':
                    result = Parse(trimStr1) || Parse(trimStr2);
                    break;
                default:
                    break;
            }
            result ? stack.Push(resultTrue) : stack.Push(resultFalse);
        }
    }
    if (stack.StackSize() != 1) {
        return false;
    }
    stack.Pop(&value);
    if (value == nullptr) {
        return false;
    }
    return strcmp(value, "true") == 0 ? true : false;
}

void ConditionArbitrator::FreeMallocData(const LinkQueue *queue) const
{
    if (queue == nullptr) {
        return;
    }
    QueueNode *tmp = queue->GetNext();
    while (tmp) {
        char *nodeValue = const_cast<char *>(tmp->GetNodeData());
        ACE_FREE(nodeValue);
        tmp->SetNodeData(nullptr);
        tmp = tmp->GetNodeNext();
    }
}
} // namespace ACELite
} // namespace OHOS
