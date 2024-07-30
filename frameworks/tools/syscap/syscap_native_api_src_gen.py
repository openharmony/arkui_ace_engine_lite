#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2022 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import stat
import json
import argparse

license_string = """/*
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

"""
header_string = """#include <string.h>

namespace OHOS {
namespace ACELite {
"""

tail_string = """
} // namespace ACELite
} // namespace OHOS"""

variable_define_string = """
enum ConfigStatus {
    ENABLE = 0,
    DISABLE,
};

typedef struct {
    const char *name;
    enum ConfigStatus type;
} SysCapDef;

"""

function_define_string = """
static int GetSysCapNum()
{
    return sizeof(g_syscap) / sizeof(SysCapDef);
}

static bool HasSystemCapability(const char *cap)
{
    int sysCapNum = GetSysCapNum();
    for (int i = 0; i < sysCapNum; i++) {
        if (strcmp(cap, g_syscap[i].name) == 0) {
            if (g_syscap[i].type == ENABLE) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}"""

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--syscap-file',
                        help='syscap json file')
    parser.add_argument('--output-file',
                        help='output app file')
    arguments = parser.parse_args()
    return arguments

def get_syscap_list():
    path = parse_args().syscap_file
    with open(path, 'rb') as syscap_file:
        syscap_json = json.load(syscap_file)['syscap']
        syscap_file.close()
        syscap_list = syscap_json['os']
        if ('private' in syscap_json):
            for i in syscap_json['private']:
                syscap_list.append(i)
    return syscap_list

def assemble_syscap_array():
    string = "static const SysCapDef g_syscap[] = {\n"
    for i in get_syscap_list():
        string = ''.join([string, '    {', '"', i, '", ENABLE},\n'])
    string = ''.join([string.strip(',\n'), '\n};\n'])
    return string

def assemble_cpp_file():
    line = license_string + header_string \
         + variable_define_string + assemble_syscap_array() \
         + function_define_string \
         + tail_string
    outpath = parse_args().output_file
    flags = os.O_WRONLY | os.O_CREAT
    modes = stat.S_IWUSR | stat.S_IRUSR
    with os.fdopen(os.open(outpath, flags, modes), 'w') as f:
        f.writelines(line)
        f.close()

if __name__ == '__main__':
    assemble_cpp_file()
