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
    data() {
        return {
            title: '',
            items: [],
            pickerData: [],
        };
    },
    onInit() {
        this.title = 'Page List';
        this.initializeItems();
        this.initializePickerData();
    },
    onShow() {
        this.$refs.swiperObj.rotation({ focus: true });
    },
    initializeItems() {
        const fruits = [
            'apple', 'banana', 'orange', 'grape', 'watermelon',
            'pear', 'peach', 'cherry', 'mango', 'pineapple',
            'strawberry', 'blueberry', 'raspberry', 'blackberry',
            'kiwi', 'plum', 'apricot', 'nectarine', 'lychee', 'dragonFruit'
        ];
        this.items = [...fruits];
    },
    initializePickerData() {
        const numbers = Array.from({ length: 9 }, (_, i) => (i + 1) * 10);
        this.pickerData = [...numbers];
    },
    backPage() {
        router.replaceUrl({
            uri: 'pages/index/index',
        });
    },
    onSelectSwiper() {
        console.log('onSelectSwiper------');
        this.focusComponent('swiperObj');
    },
    onSelectList() {
        console.log('onSelectList------');
        this.focusComponent('listObj');
    },
    onSelectPickerView() {
        console.log('onSelectPickerView------');
        this.focusComponent('pickerObj');
    },
    onSelectSlider(index) {
        console.log(`onSelectSlider------ index: ${index}`);
        const sliderRefs = ['sliderObj1', 'sliderObj2', 'sliderObj3'];
        const refName = sliderRefs[index - 1];
        if (refName && this.$refs[refName]) {
            this.$refs[refName].rotation({ focus: true });
        } else {
            console.warn(`Slider with index ${index} not found.`);
        }
    },
    focusComponent(refName) {
        if (this.$refs[refName]) {
            console.log('focusComponent------refName=', refName);
            this.$refs[refName].rotation({ focus: true });
        } else {
            console.warn(`Component with ref "${refName}" not found.`);
        }
    },
};