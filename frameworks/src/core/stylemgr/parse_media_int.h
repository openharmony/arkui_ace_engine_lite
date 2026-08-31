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

#ifndef OHOS_ACELITE_STYLEMGR_PARSE_MEDIA_INT_H
#define OHOS_ACELITE_STYLEMGR_PARSE_MEDIA_INT_H

#include <charconv>
#include <string>
#include <system_error>

namespace OHOS {
namespace ACELite {
inline bool ParseMediaInt(const char *text, int &out)
{
    if (text == nullptr || *text == '\0') {
        return false;
    }
    int value = 0;
    const char *last = text + std::char_traits<char>::length(text);
    auto result = std::from_chars(text, last, value);
    if (result.ec != std::errc() || result.ptr != last) {
        return false;
    }
    out = value;
    return true;
}

inline bool ParseMediaInt(const std::string &text, int &out)
{
    return ParseMediaInt(text.c_str(), out);
}
} // namespace ACELite
} // namespace OHOS
#endif
