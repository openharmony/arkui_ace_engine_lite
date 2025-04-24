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
        defaultTime: '',
        time: '',
    },
    onInit() {
        this.defaultTime = this.now();
    },
    onShow() {
        if (this.$refs.pickerObj) {
            this.$refs.pickerObj.rotation({ focus: true });
        }
    },
    handleChange(data) {
        this.time = this.concat(data.hour, data.minute);
    },
    now() {
        const date = new Date();
        const hours = date.getHours();
        const minutes = date.getMinutes();
        return this.concat(hours, minutes);
    },

    fill(value) {
        return (value > 9 ? '' : '0') + value;
    },
    concat(hours, minutes) {
        return `${this.fill(hours)}:${this.fill(minutes)}`;
    },
    backPage() {
        router.replaceUrl({
            uri: 'pages/index/index',
        });
    }
}
