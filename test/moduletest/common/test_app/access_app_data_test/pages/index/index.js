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

import app from '@system.app';
import router from '@system.router';

export default {
    data: {
        title: "",
        jsonData: ""
    },
    onInit() {
        if (typeof this.data === 'undefined') {
            console.log("Correct, in index.js, can NOT access data through this.data.xxx")
        } else {
            console.error("Error, in index.js, should NOT be able to access data through this.data.xxx")
        }

        if (typeof getApp !== 'undefined') {
            var appData = getApp().data
            if (typeof appData !== 'undefined') {
                this.title = appData.name
                this.jsonData = JSON.stringify(appData)
            }
        } else {
            console.log("no getApp exist")
        }
    },
    terminateApp() {
        app.terminate();
    },
    modifyAppData() {
        if (typeof getApp !== 'undefined') {
            var appData = getApp().data
            appData.name = "David"
            console.log("latest app data: " + JSON.stringify(appData))
            this.title = appData.name
            this.jsonData = JSON.stringify(appData)
        } else {
            console.log("no getApp exist")
        }
    },
    addNewDataToAppData() {
        if (typeof getApp !== 'undefined') {
            var appData = getApp().data
            appData.children = "tong tong"
            console.log("latest app data: " + JSON.stringify(appData))
            this.title = appData.children
            this.jsonData = JSON.stringify(appData)
        } else {
            console.log("no getApp exist")
        }
    },
    nextPage() {
        router.replace({
            uri: "pages/second/index"
        })
    }
}
