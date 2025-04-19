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

const TEST_CASES = [
    { castName: 'Rotation PickView', uri: 'pages/pickView/index' },
    { castName: 'Rotation Slider', uri: 'pages/slider/index' },
    { castName: 'Rotation Swiper', uri: 'pages/swiper/index' },
    { castName: 'Empty Item_1' },
    { castName: 'Empty Item_2' },
    { castName: 'Empty Item_3' },
    { castName: 'Empty Item_4' },
    { castName: 'Empty Item_5' },
    { castName: 'Empty Item_6' },
    { castName: 'Empty Item_7' },
    { castName: 'Empty Item_8' }
];

export default {
    data: {
        title: '',
        caseList: [],
        optionName: '关闭旋转表冠'
    },
    onInit() {
        this.title = 'Test rotating';
        this.onInitTestCast();
        this.$refs.listObj.rotation({focus: true});
    },
    onShow() {
        this.$refs.listObj.rotation({focus: true});
    },
    onInitTestCast() {
        this.caseList = TEST_CASES.map(item => ({ castName: item.castName }));
        console.log('onInitTestCast---length' + this.caseList.length);
    },
    onListItemClick(item) {
        const targetCase = TEST_CASES.find(test => test.castName === item.castName);
        if (!targetCase || !targetCase.uri) {
            return;
        }

        router.replaceUrl({
            uri: targetCase.uri,
        });
    },
    onSelectList() {
    },
    onChangeOption() {
        if (this.optionName === '关闭旋转表冠') {
            this.optionName = '开启旋转表冠';
            this.$refs.listObj.rotation({focus: false});
        } else {
            this.optionName = '开启旋转表冠';
            this.$refs.listObj.rotation({focus: true});
        }
    }
};
