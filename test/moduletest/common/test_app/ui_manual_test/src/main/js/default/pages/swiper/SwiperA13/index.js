/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

export default {
    data: {
        index: 0,
        title: "",
        text_a: "暂无数据",
        text_b: "更多详情",
    },
    onInit() {
        this.title = this.$t('strings.world');
        this.first_text = 5;
        this.first_percent = this.first_text;
        this.second_text = 15;
        this.second_percent = this.second_text;
        this.third_text = 30;
        this.third_percent = this.third_text;
        this.fourth_text = 55;
        this.fourth_percent = this.fourth_text;
    },
    updateIndex(e) {
        console.log("e.index = " + e.direction + " this.index=" + this.index);
        if (this.index < 3 || this.index >= 0) {
            if (e.direction == "up") {
                this.index++;
            } else {
                this.index--;
            }
        } else {
            this.index = 0;
        }
    }
}



