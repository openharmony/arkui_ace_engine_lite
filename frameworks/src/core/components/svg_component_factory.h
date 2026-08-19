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

#ifndef OHOS_ACELITE_SVG_COMPONENT_FACTORY_H
#define OHOS_ACELITE_SVG_COMPONENT_FACTORY_H

#include "acelite_config.h"
#include "component.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"
#include "non_copyable.h"
#include "svg_component.h"
#include "svg_component_utils.h"
#include "svg_element_component.h"

namespace OHOS {
namespace ACELite {

class SvgComponentFactory final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SvgComponentFactory);
    ~SvgComponentFactory() = default;

    static Component* CreateComponent(uint16_t componentNameId,
                                      jerry_value_t options,
                                      jerry_value_t children);
};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_SVG_COMPONENT_FACTORY_H
