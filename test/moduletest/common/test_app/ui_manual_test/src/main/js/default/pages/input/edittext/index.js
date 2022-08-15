/*
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

var typeCount = 0;
var placeholderCount = 0;
var maxlenCount = 0;
var valueCount= 0;
var colorCount=0;
var fontsizeCount = 0;
var phcolorCount = 0;

export default {
    data: {
        text: "",
        placeholder:"placeholder",
        typeofinput:"text",
        inputvalue:"",
        inputcolor:16711680,
        fontsize:12,
        phcolor:16711680,
        maxlen:5
    },
    onvaluechange: function(e) {
        console.info("value changed");
        this.text = "input value:" + e;
        console.info(this.text);
    },
    clickChangeType:function(e)
    {
        this.typeofinput = "password";
    },
    clickChangeValue:function(e)
    {
        if (valueCount % 3 == 0) {
            this.inputvalue = "abcd";
        } else if (valueCount % 3 == 1) {
            this.inputvalue = "abcdqwer";
        } else {
            this.inputvalue = "";
        }

        valueCount++;
    },
    clickChangePlaceholder:function(e)
    {
        if (placeholderCount % 2 == 0) {
            this.placeholder = "input here";
        } else {
            this.placeholder = "placeholder";
        }

        placeholderCount++;
    },
    clickChangeMaxlength:function(e)
    {
        if (maxlenCount % 2 == 0) {
            this.maxlen = 10;
        } else {
            this.maxlen = 5;
        }

        maxlenCount++;
    },
    clickChangeColor:function(e)
    {
        if (colorCount % 2 == 0) {
            this.inputcolor = 255;
        } else {
            this.inputcolor = 32768;
        }

        colorCount++;
    },
    clickChangeFontSize:function(e)
    {
        if (fontsizeCount % 2 == 0) {
            this.fontsize = 36;
        } else {
            this.fontsize = 12;
        }

        fontsizeCount++;
    },
    clickChangePHColor:function(e)
    {
        if (phcolorCount % 2 == 0) {
            this.phcolor = 255;
        } else {
            this.phcolor = 32768;
        }

        phcolorCount++;
    },
}