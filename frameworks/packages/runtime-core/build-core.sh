#!/bin/bash
#Copyright (c) 2020-2023 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
set -e

echo "=================================================="
echo "== this tool is just for linux host environment =="
echo "== prerequires: node.js and python3 installed   =="
echo "=================================================="

# step 1: npm install and build
npm install
npm run build

# step 2: clone jerryscript repo
rm -rf jerryscript
{
  timeout 120 git clone git@gitee.com:openharmony/third_party_jerryscript.git jerryscript
} || {
  rm -rf jerryscript
  echo "git clone from gitee failed, try copy from current workspace"
  cp -rf ../../../../../../third_party/jerryscript ./
}

# step 3: assign es6 profile path
cp ./es6.profile ./jerryscript/jerry-core/profiles/es6.profile

# step 4: compile jerry snapshot tool
pushd ./jerryscript
python tools/build.py --mem-heap=512 --snapshot-exec=ON --snapshot-save=ON \
 --profile=es6 --error-messages=ON --logging=ON --mem-stats=ON \
 --jerry-cmdline-snapshot=ON --compile-flag="-Wno-unused"

# step 5: convert framework.min.js to byte code file
popd
./jerryscript/build/bin/jerry-snapshot generate -o ./build/framework.min.bc ./build/framework.min.js

echo "snapshot generating done."

# step 6: convert framework.min.js and framework.min.bc into C files
python3 ./../../tools/snapshot/framework2char.py
