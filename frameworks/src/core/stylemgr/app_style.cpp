/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "stylemgr/app_style.h"
#include "ace_log.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
void AppStyle::SetStyleName(const char * const name, size_t nameLen)
{
    if ((nameLen <= 0) || (nameLen >= NAME_LENGTH_MAX) || (name == nullptr) || (strlen(name) == 0)) {
        return;
    }
    styleName_ = static_cast<char *>(ace_malloc(sizeof(char) * (nameLen + 1)));
    if (styleName_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create style_name failed.");
        return;
    }
    if (memcpy_s(styleName_, nameLen, name, nameLen) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "style_name set string value error");
        ace_free(styleName_);
        styleName_ = nullptr;
        return;
    }
    *(styleName_ + nameLen) = '\0';
}

void AppStyle::Reset()
{
    const AppStyleItem *next = nullptr;
    while (firstStyleItem_ != nullptr) {
        next = firstStyleItem_->GetNext();
        delete firstStyleItem_;
        firstStyleItem_ = next;
    }
    lastStyleItem_ = nullptr;
#if FEATURE_TRANSITION_ANIMATOR
    AppStyle *nextSegment = nullptr;
    while (firstKeyFrameSegment_ != nullptr) {
        nextSegment = firstKeyFrameSegment_->nextKeyFrameSegment_;
        firstKeyFrameSegment_->nextKeyFrameSegment_ = nullptr;
        delete firstKeyFrameSegment_;
        firstKeyFrameSegment_ = nextSegment;
    }
    lastKeyFrameSegment_ = nullptr;
    keyFrameSegmentCount_ = 0;
#endif // FEATURE_TRANSITION_ANIMATOR
    if (styleName_ != nullptr) {
        ace_free(styleName_);
        styleName_ = nullptr;
    }
}

void AppStyle::AddStyleItem(AppStyleItem *newStyleItem)
{
    if (newStyleItem == nullptr) {
        return;
    }

    if (firstStyleItem_ == nullptr) {
        firstStyleItem_ = newStyleItem;
        lastStyleItem_ = newStyleItem;
    } else {
        newStyleItem->SetPre(lastStyleItem_);
        lastStyleItem_->SetNext(newStyleItem);
        lastStyleItem_ = newStyleItem;
    }
}

#if FEATURE_TRANSITION_ANIMATOR
void AppStyle::AddKeyFrameSegment(AppStyle* segment)
{
    if (segment == nullptr) {
        return;
    }
    if (firstKeyFrameSegment_ == nullptr) {
        firstKeyFrameSegment_ = segment;
        lastKeyFrameSegment_ = segment;
    } else {
        lastKeyFrameSegment_->nextKeyFrameSegment_ = segment;
        lastKeyFrameSegment_ = segment;
    }
    keyFrameSegmentCount_++;
}

void AppStyle::SetKeyFrameTime(uint8_t from, uint8_t to)
{
    keyFrameTimeFrom_ = from;
    keyFrameTimeTo_ = to;
}
#endif // FEATURE_TRANSITION_ANIMATOR

const AppStyleItem *AppStyle::GetStyleItemByName(const char * const stylePropName) const
{
    if (stylePropName == nullptr) {
        return nullptr;
    }
    size_t len = strlen(stylePropName);
    uint16_t keyId = KeyParser::ParseKeyId(stylePropName, len);
    return GetStyleItemByNameId(keyId);
}

const AppStyleItem *AppStyle::GetStyleItemByNameId(uint16_t stylePropNameId) const
{
    if (!KeyParser::IsKeyValid(stylePropNameId)) {
        return nullptr;
    }

    const AppStyleItem *styleItem = firstStyleItem_;
    while (styleItem != nullptr) {
        if (styleItem->GetPropNameId() == stylePropNameId) {
            return styleItem;
        }

        styleItem = styleItem->GetNext();
    }
    return nullptr;
}

#if FEATURE_TRANSITION_ANIMATOR
static uint8_t GetKeyFrameTime(jerry_value_t frame, uint8_t defaultTime)
{
    jerry_value_t timeKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("time"));
    jerry_value_t timeValue = jerry_get_property(frame, timeKey);
    uint8_t time = defaultTime;
    if (jerry_value_is_number(timeValue)) {
        int16_t value = IntegerOf(timeValue);
        if ((value >= ANIMATION_KEY_FRAME_START_TIME) && (value <= ANIMATION_KEY_FRAME_END_TIME)) {
            time = static_cast<uint8_t>(value);
        }
    }
    ReleaseJerryValue(timeKey, timeValue, VA_ARG_END_FLAG);
    return time;
}

static void SplitTranslateValue(const char *value, char *x, uint8_t xSize, char *y, uint8_t ySize)
{
    if ((value == nullptr) || (x == nullptr) || (y == nullptr) || (xSize == 0) || (ySize == 0)) {
        return;
    }
    x[0] = '\0';
    y[0] = '\0';
    while (*value == ' ') {
        value++;
    }
    const char *split = strchr(value, ' ');
    if (split == nullptr) {
        if (strncpy_s(x, xSize, value, xSize - 1) != EOK) {
            x[0] = '\0';
        }
        if (strncpy_s(y, ySize, "0px", strlen("0px")) != EOK) {
            y[0] = '\0';
        }
        return;
    }
    size_t xLen = static_cast<size_t>(split - value);
    if (xLen >= xSize) {
        xLen = xSize - 1;
    }
    if (memcpy_s(x, xSize, value, xLen) != EOK) {
        x[0] = '\0';
    } else {
        x[xLen] = '\0';
    }
    while (*split == ' ') {
        split++;
    }
    if (strncpy_s(y, ySize, split, ySize - 1) != EOK) {
        y[0] = '\0';
    }
}

void AppStyle::AddKeyFrameStyleItem(AppStyle &newStyle, const char *propName, const char *from, const char *to)
{
    if ((propName == nullptr) || (from == nullptr) || (to == nullptr)) {
        return;
    }
    jerry_value_t propKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(propName));
    jerry_value_t fromValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(from));
    jerry_value_t toValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(to));
    jerry_value_t propValue = ConcatJerryString(fromValue, toValue);
    AppStyleItem *newStyleItem = AppStyleItem::GenerateFromJSValue(propKey, propValue);
    newStyle.AddStyleItem(newStyleItem);
    ReleaseJerryValue(propKey, fromValue, toValue, propValue, VA_ARG_END_FLAG);
}

bool AppStyle::AddTranslateKeyFrameItems(jerry_value_t propValueFrom, jerry_value_t propValueTo, AppStyle &newStyle)
{
    jerry_value_t translateKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("translate"));
    bool isTransformSet = false;
    if (JerryHasProperty(propValueFrom, translateKey) && JerryHasProperty(propValueTo, translateKey)) {
        jerry_value_t translateFromValue = jerry_get_property(propValueFrom, translateKey);
        jerry_value_t translateToValue = jerry_get_property(propValueTo, translateKey);
        char *translateFrom = MallocStringOf(translateFromValue);
        char *translateTo = MallocStringOf(translateToValue);
        if ((translateFrom != nullptr) && (translateTo != nullptr)) {
            constexpr uint8_t translateValueSize = 32;
            char fromX[translateValueSize] = { 0 };
            char fromY[translateValueSize] = { 0 };
            char toX[translateValueSize] = { 0 };
            char toY[translateValueSize] = { 0 };
            SplitTranslateValue(translateFrom, fromX, translateValueSize, fromY, translateValueSize);
            SplitTranslateValue(translateTo, toX, translateValueSize, toY, translateValueSize);
            AddKeyFrameStyleItem(newStyle, "translateX", fromX, toX);
            AddKeyFrameStyleItem(newStyle, "translateY", fromY, toY);
            isTransformSet = true;
        }
        ACE_FREE(translateFrom);
        ACE_FREE(translateTo);
        ReleaseJerryValue(translateFromValue, translateToValue, VA_ARG_END_FLAG);
    }

    jerry_value_t transformKeys = jerry_get_object_keys(propValueFrom);
    uint16_t transformKeyCount = jerry_get_array_length(transformKeys);
    for (uint16_t index = 0; index < transformKeyCount; index++) {
        jerry_value_t transformKey = jerry_get_property_by_index(transformKeys, index);
        char *transformName = MallocStringOf(transformKey);
        if ((transformName != nullptr) && strcmp(transformName, "translate") != 0 &&
            JerryHasProperty(propValueTo, transformKey)) {
            jerry_value_t fromValue = jerry_get_property(propValueFrom, transformKey);
            jerry_value_t toValue = jerry_get_property(propValueTo, transformKey);
            char *from = MallocStringOf(fromValue);
            char *to = MallocStringOf(toValue);
            if ((from != nullptr) && (to != nullptr)) {
                AddKeyFrameStyleItem(newStyle, transformName, from, to);
                isTransformSet = true;
            }
            ACE_FREE(from);
            ACE_FREE(to);
            ReleaseJerryValue(fromValue, toValue, VA_ARG_END_FLAG);
        }
        ACE_FREE(transformName);
        jerry_release_value(transformKey);
    }
    jerry_release_value(transformKeys);
    jerry_release_value(translateKey);
    return isTransformSet;
}
#endif // FEATURE_TRANSITION_ANIMATOR

void AppStyle::AddItemsInLoop(jerry_value_t object, AppStyle &newStyle)
{
    jerry_value_t propKeys = jerry_get_object_keys(object);
    uint16_t propKeySize = jerry_get_array_length(propKeys);
    for (uint16_t index = 0; index < propKeySize; index++) {
        // convert prop key into char
        jerry_value_t propKey = jerry_get_property_by_index(propKeys, index);
        jerry_value_t propValue = jerry_get_property(object, propKey);
        AppStyleItem *newStyleItem = AppStyleItem::GenerateFromJSValue(propKey, propValue);
        newStyle.AddStyleItem(newStyleItem);
        ReleaseJerryValue(propKey, propValue, VA_ARG_END_FLAG);
    }
    jerry_release_value(propKeys);
}

#if FEATURE_TRANSITION_ANIMATOR
void AppStyle::AddKeyFrameItem(jerry_value_t objFrom,
                               jerry_value_t objTo,
                               jerry_value_t propKeyFrom,
                               AppStyle& newStyle)
{
    jerry_value_t propValueFrom = jerry_get_property(objFrom, propKeyFrom);
    jerry_value_t propValueTo = jerry_get_property(objTo, propKeyFrom);
    jerry_value_t propValue = UNDEFINED;
    bool isItemNeedToAdd = false;
    if (JerryHasProperty(objTo, propKeyFrom)) { // key exist in to object
        char *keyFrom = MallocStringOf(propKeyFrom);
        const char * const transitionTransform = "transform";
        // transform type, include "translateX, translateY, rotate"
        if ((keyFrom != nullptr) &&
            !strcmp(keyFrom, transitionTransform)) {
            propValue = AddKeyFramesTransformValue(propValueFrom, propValueTo, isItemNeedToAdd, propKeyFrom);
            if (AddTranslateKeyFrameItems(propValueFrom, propValueTo, newStyle)) {
                isItemNeedToAdd = false;
                jerry_release_value(propValue);
                propValue = UNDEFINED;
            }
        } else {
            isItemNeedToAdd = true;
            propValue = ConcatJerryString(propValueFrom, propValueTo);
        }
        if (keyFrom != nullptr) {
            ace_free(keyFrom);
            keyFrom = nullptr;
        }
    }

    if (isItemNeedToAdd) {
        AppStyleItem *newStyleItem = AppStyleItem::GenerateFromJSValue(propKeyFrom, propValue);
        newStyle.AddStyleItem(newStyleItem);
        jerry_release_value(propValue);
    }
    ReleaseJerryValue(propKeyFrom, propValueFrom, propValueTo, VA_ARG_END_FLAG);
}

void AppStyle::AddKeyFrameItemsInLoopInternal(jerry_value_t objFrom,
                                              jerry_value_t objTo,
                                              jerry_value_t propKeysFrom,
                                              AppStyle& newStyle)
{
    uint16_t propKeySizeFrom = jerry_get_array_length(propKeysFrom);
    for (uint16_t i = 0; i < propKeySizeFrom; i++) {
        jerry_value_t propKeyFrom = jerry_get_property_by_index(propKeysFrom, i);
        AddKeyFrameItem(objFrom, objTo, propKeyFrom, newStyle);
    }
}

void AppStyle::AddTransitionKeyFramesItemsInLoop(jerry_value_t objFrom, jerry_value_t objTo, AppStyle &newStyle)
{
    if (jerry_value_is_null(objFrom) || jerry_value_is_null(objTo)) {
        return;
    }
    jerry_value_t propKeysFrom = jerry_get_object_keys(objFrom);
    AddKeyFrameItemsInLoopInternal(objFrom, objTo, propKeysFrom, newStyle);
    jerry_release_value(propKeysFrom);
}
#endif // FEATURE_TRANSITION_ANIMATOR

void AppStyle::AddKeyFramesItemsInLoop(jerry_value_t objFrom, jerry_value_t objTo, AppStyle &newStyle)
{
    if (jerry_value_is_null(objFrom) || jerry_value_is_null(objTo)) {
        return;
    }
    jerry_value_t propKeysFrom = jerry_get_object_keys(objFrom);
    uint16_t propKeySizeFrom = jerry_get_array_length(propKeysFrom);

    jerry_value_t propKeyFrom;
    jerry_value_t propValueFrom;
    jerry_value_t propValueTo;
    jerry_value_t propValue = UNDEFINED;

    char *keyFrom = nullptr;
    AppStyleItem *newStyleItem = nullptr;
    for (uint16_t i = 0; i < propKeySizeFrom; i++) {
        bool isItemNeedToAdd = false;
        propKeyFrom = jerry_get_property_by_index(propKeysFrom, i); // transform, height, background-color
        propValueFrom = jerry_get_property(objFrom, propKeyFrom);   // {\"translateX\":\"50px\"}, -5px, #f76160
        propValueTo = jerry_get_property(objTo, propKeyFrom);

        if (JerryHasProperty(objTo, propKeyFrom)) { // key exist in to object
            keyFrom = MallocStringOf(propKeyFrom);
            const char * const transitionTransform = "transform";
            if ((keyFrom != nullptr) &&
                !strcmp(keyFrom, transitionTransform)) { // transform type, include "translateX, translateY, rotate"
                propValue = AddKeyFramesTransformValue(propValueFrom, propValueTo, isItemNeedToAdd, propKeyFrom);
            } else {
                isItemNeedToAdd = true;
                propValue = ConcatJerryString(propValueFrom, propValueTo);
            }
            if (keyFrom != nullptr) {
                ace_free(keyFrom);
                keyFrom = nullptr;
            }
        }

        if (isItemNeedToAdd) {
            newStyleItem = AppStyleItem::GenerateFromJSValue(propKeyFrom, propValue);
            newStyle.AddStyleItem(newStyleItem);
            jerry_release_value(propValue);
        }
        ReleaseJerryValue(propKeyFrom, propValueFrom, propValueTo, VA_ARG_END_FLAG);
    }

    jerry_release_value(propKeysFrom);
}

jerry_value_t AppStyle::AddKeyFramesTransformValue(jerry_value_t propValueFrom,
                                                   jerry_value_t propValueTo,
                                                   bool &isItemNeedToAdd,
                                                   jerry_value_t &propKeyFrom)
{
    jerry_value_t propValue = UNDEFINED;
    if (jerry_value_is_undefined(propValueFrom) || jerry_value_is_undefined(propValueTo)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "transform prop value must be set.");
        return propValue;
    }
    jerry_value_t transformKeysFrom = jerry_get_object_keys(propValueFrom);
    jerry_value_t transformKeysTo = jerry_get_object_keys(propValueTo);
    if ((!jerry_value_is_undefined(transformKeysFrom)) && (!jerry_value_is_undefined(transformKeysTo))) {
        int8_t valueIndex = 0;
        jerry_value_t transformKeyFrom = jerry_get_property_by_index(transformKeysFrom, valueIndex);
        jerry_value_t transformKeyTo = jerry_get_property_by_index(transformKeysTo, valueIndex);
        if (transformKeyFrom == transformKeyTo) {
            jerry_value_t transformValueFrom = jerry_get_property(propValueFrom, transformKeyFrom);
            jerry_value_t transformValueTo = jerry_get_property(propValueTo, transformKeyTo);
            if ((!jerry_value_is_undefined(transformValueFrom)) && (!jerry_value_is_undefined(transformValueTo))) {
                jerry_release_value(propKeyFrom);
                propKeyFrom = jerry_acquire_value(transformKeyFrom);
                isItemNeedToAdd = true;
                propValue = ConcatJerryString(transformValueFrom, transformValueTo);
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "None style to parse.");
            }
            ReleaseJerryValue(transformValueFrom, transformValueTo, VA_ARG_END_FLAG);
        }
        ReleaseJerryValue(transformKeyFrom, transformKeyTo, VA_ARG_END_FLAG);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "None style to parse.");
    }
    ReleaseJerryValue(transformKeysFrom, transformKeysTo, VA_ARG_END_FLAG);

    return propValue;
}

bool AppStyle::MergeAnimationString(char *dest, uint16_t destSz, const char * const src1, const char * const src2)
{
    if (strcat_s(dest, destSz, src1) != 0) {
        return false;
    }
    if (strcat_s(dest, destSz, ANIMATION_VALUE_SEP) != 0) {
        return false;
    }
    if (strcat_s(dest, destSz, src2) != 0) {
        return false;
    }
    dest[destSz - 1] = '\0';
    return true;
}

jerry_value_t AppStyle::ConcatJerryString(jerry_value_t strA, jerry_value_t strB)
{
    char *stringA = MallocStringOf(strA);
    if (stringA == nullptr) {
        return jerry_create_null();
    }
    char *stringB = MallocStringOf(strB);
    if (stringB == nullptr) {
        ace_free(stringA);
        stringA = nullptr;
        return jerry_create_null();
    }
    const char * const pixelUnit = "px";
    char *next = nullptr;
    char *strtokA = stringA;
    char *strtokB = stringB;
    if (strstr(stringA, pixelUnit)) {
        strtokA = strtok_s(stringA, pixelUnit, &next);
        next = nullptr;
    }
    if (strstr(stringB, pixelUnit)) {
        strtokB = strtok_s(stringB, pixelUnit, &next);
    }
    if ((strtokA == nullptr) || (strtokB == nullptr)) {
        ace_free(stringA);
        stringA = nullptr;
        ace_free(stringB);
        stringB = nullptr;
        return jerry_create_null();
    }
    size_t strtokALen = strlen(strtokA);
    size_t strtokBLen = strlen(strtokB);
    if ((strtokALen == 0) || (strtokALen >= (UINT16_MAX - 1)) || (strtokBLen == 0) ||
        (strtokBLen >= (UINT16_MAX - 1))) {
        ace_free(stringA);
        stringA = nullptr;
        ace_free(stringB);
        stringB = nullptr;
        return jerry_create_null();
    }
    bool result = true;
    size_t size = strlen(strtokA) + strlen(strtokB) + strlen(ANIMATION_VALUE_SEP) + 1;
    const uint16_t maxBytesNum = 1024;
    if ((size >= maxBytesNum)) { // we only support 1kb
        result = false;
    }
    char *newStr = nullptr;
    if (result) {
        newStr = static_cast<char *>(ace_malloc(size));
        if (newStr == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc space error");
            ace_free(stringA);
            stringA = nullptr;
            ace_free(stringB);
            stringB = nullptr;
            return jerry_create_null();
        }
        newStr[0] = '\0';
        result = MergeAnimationString(newStr, size, strtokA, strtokB);
    }

    jerry_value_t propValue;
    if (result) {
        propValue = jerry_create_string(reinterpret_cast<jerry_char_t *>(newStr));
    } else {
        propValue = jerry_create_null();
    }
    ace_free(stringA);
    stringA = nullptr;
    ace_free(stringB);
    stringB = nullptr;
    ACE_FREE(newStr);
    return propValue;
}

#if FEATURE_TRANSITION_ANIMATOR
void AppStyle::GenerateKeyFrameSegments(jerry_value_t styleValue, AppStyle &newStyle)
{
    uint16_t frameCount = jerry_get_array_length(styleValue);
    if (frameCount >= ANIMATION_MIN_KEY_FRAME_COUNT) {
        jerry_value_t fromObj = jerry_get_property_by_index(styleValue, 0);
        jerry_value_t toObj = jerry_get_property_by_index(styleValue, frameCount - 1);
        AddTransitionKeyFramesItemsInLoop(fromObj, toObj, newStyle);
        ReleaseJerryValue(toObj, fromObj, VA_ARG_END_FLAG);
    }
    for (uint16_t index = 0; index + 1 < frameCount; index++) {
        jerry_value_t segmentFrom = jerry_get_property_by_index(styleValue, index);
        jerry_value_t segmentTo = jerry_get_property_by_index(styleValue, index + 1);
        AppStyle *segment = new AppStyle();
        if (segment != nullptr) {
            segment->SetKeyFrameTime(GetKeyFrameTime(segmentFrom, ANIMATION_KEY_FRAME_START_TIME),
                                     GetKeyFrameTime(segmentTo, ANIMATION_KEY_FRAME_END_TIME));
            AddTransitionKeyFramesItemsInLoop(segmentFrom, segmentTo, *segment);
            if (segment->GetFirst() != nullptr) {
                newStyle.AddKeyFrameSegment(segment);
            } else {
                delete segment;
            }
        }
        ReleaseJerryValue(segmentFrom, segmentTo, VA_ARG_END_FLAG);
    }
}
#endif // FEATURE_TRANSITION_ANIMATOR

AppStyle *AppStyle::GenerateFromJS(jerry_value_t styleKey, jerry_value_t styleValue, bool isKeyFrames)
{
    char *styleNameBuffer = MallocStringOf(styleKey);
    size_t strLen = 0;
    if (styleNameBuffer == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "convert style name to char failed, will be dropped\n");
        return nullptr;
    }
    strLen = strlen(styleNameBuffer);
    if (strLen == 0 || (strLen >= UINT16_MAX)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "style name length is 0 or max, will be dropped\n");
        ace_free(styleNameBuffer);
        styleNameBuffer = nullptr;
        return nullptr;
    }

    AppStyle *newStyle = new AppStyle();
    if (newStyle == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create style error");
        ace_free(styleNameBuffer);
        styleNameBuffer = nullptr;
        return nullptr;
    }

    newStyle->SetStyleName(static_cast<const char *>(styleNameBuffer), strLen); // translateX  #aaa
    ace_free(styleNameBuffer);
    styleNameBuffer = nullptr;

    if (isKeyFrames) {
#if FEATURE_TRANSITION_ANIMATOR
        GenerateKeyFrameSegments(styleValue, *newStyle);
#else
        jerry_value_t fromObj = jerry_get_property_by_index(styleValue, 0);
        jerry_value_t toObj = jerry_get_property_by_index(styleValue, 1);
        AddKeyFramesItemsInLoop(fromObj, toObj, *newStyle);
        ReleaseJerryValue(toObj, fromObj, VA_ARG_END_FLAG);
#endif // FEATURE_TRANSITION_ANIMATOR
    } else {
        AddItemsInLoop(styleValue, *newStyle);
    }

    return newStyle;
}

void AppStyle::CombineStyles(AppStyle &dest, const AppStyle &source, bool overwrite)
{
    const AppStyleItem *styleItem = source.GetFirst();
    while (styleItem != nullptr) {
        const AppStyleItem *currentStyleItemInSource = styleItem;
        styleItem = styleItem->GetNext();
        const AppStyleItem *existOneInDest = dest.GetStyleItemByNameId(currentStyleItemInSource->GetPropNameId());

        if (existOneInDest == nullptr) {
            dest.AddStyleItem(AppStyleItem::CopyFrom(currentStyleItemInSource));
            continue;
        }

        if (!overwrite) {
            // exist one has higher priority
            continue;
        }

        // the new one has higher priority, to over wirte exist one
        const_cast<AppStyleItem *>(existOneInDest)->UpdateValueFrom(*currentStyleItemInSource);
    }
}
} // namespace ACELite
} // namespace OHOS
