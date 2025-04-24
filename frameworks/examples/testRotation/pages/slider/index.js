/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import router from '@ohos.router';

export default {
    data: {
        value: 34,
        startValue: 0,
        currentValue: 0,
        endValue: 100,
        optionName: '关闭旋转表冠'
    },
    onInit() {

    },
    onShow() {
        this.$refs.sliderObj.rotation({focus: true});
    },
    setValue(e) {
        this.currentValue = e.value;
    },
    onChangeOption() {
        if (this.optionName === '关闭旋转表冠') {
            this.optionName = '开启旋转表冠';
            this.$refs.sliderObj.rotation({focus: false});
        } else {
            this.optionName = '关闭旋转表冠';
            this.$refs.sliderObj.rotation({focus: true});
        }
    },
    backPage() {
        router.replaceUrl({
            uri: 'pages/index/index',
        });
    }
}
