/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
        name: "Jack",
        age: 20,
        isBoy: true,
        home: {
            address: "Hang Zhou",
            country: "China"
        }
    },
    onCreate() {
        console.log("Application onCreate")
        if (typeof this.data === 'undefined') {
            console.error("Error! in app.js, we should be able to access data by this.data.xxx")
        } else {
            console.log("Correct! we can access data through this.data.xxx.")
            console.log("this.data.home.address: [" + this.data.home.address + "]")
        }
        if (typeof this.home !== 'undefined') {
            console.error("Error! in app.js, we should not be able to access data by this.xxx")
        } else {
            console.log("Correct! we can NOT access data through this.xxx.")
        }
    },
    onDestroy() {
        console.info("Application onDestroy")
    }
};
