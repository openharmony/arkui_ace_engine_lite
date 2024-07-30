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

const COLOR_RED = '#FF0000';
const COLOR_ORANGE = '#FFA500';
const COLOR_GREEN = '#00fa9a';
const COLOR_BLACK = '#000000';
const COLOR_BLUE = '#0000FF';
const COLOR_YELLOW = ' #FFFF00';

const MAX_ITEM_NUMBER = 20;
const GO_TO_SPECIAL_ITEM = 3;
export default {
    data: {
        planList: [],
        goToEnd: '',
        goToFront: '',
        goToSpecial: '',
        inputText: '',
        addItem: '',
        deleteAll: '',
        changeBG: '',
        backGroundColor: '',
    },
    onInit() {
        this.onInitList();
        this.goToEnd = 'to the end';
        this.goToFront = 'to front';
        this.goToSpecial = 'go to ' + GO_TO_SPECIAL_ITEM;
        this.deleteAll = 'deleteAll';
        this.addItem = 'addItem';
        this.deleteItem = 'deleteItem';
        this.changeBG = 'change bg';
        this.backGroundColor = COLOR_YELLOW;

    },
    onInitList() {
        for (let i = 0; i < MAX_ITEM_NUMBER; i++) {
            let data = {id: i,
                title: `plan-${i}`,
                date: this.getCurrentDateTime(),
                color: COLOR_ORANGE,
                borderColor: COLOR_BLACK,
                backgroundColor: COLOR_BLUE,
            };
            this.planList.push(data);
        }
    },
    onGoToEnd() {
        console.log('onGoToEnd');
        if (this.planList.length <= 0) {
            console.log('Element is empty');
            return;
        }
        let end = this.planList.length - 1;
        this.$refs.listObj.scrollTo({ index: end});
    },
    onGoFront() {
        console.log('onGoFront');
        this.$refs.listObj.scrollTo({ index: 0 });
    },
    onGoSpecial(item) {
        console.log('onGoSpecial');
        let length = this.planList.length - 1;
        let index = item;
        if (length < item) {
            index = 0;
        }
        this.$refs.listObj.scrollTo({ index: index });
    },
    onAddItem() {
        console.log('onAddItem');
        let length = this.planList.length;
        let data = { id: length,
            title: 'plan-addItem',
            date: '2021-12-31 10:00:00',
            color: COLOR_ORANGE};

        data.date = this.getCurrentDateTime();
        this.planList.push(data);
        this.onGoSpecial(this.planList.length - 1);
    },
    onDeleteItem() {
        console.log('onDeleteItem');
        if (this.planList.length <= 0) {
            console.log('Element is empty');
            return;
        }
        this.planList.pop();
        if (this.planList.length > 0) {
            this.onGoSpecial(this.planList.length - 1);
        }
    },
    onSelectItem(item) {
        console.log('onSelectItem=' + item);
        this.planList[item].title = 'select';
        this.planList[item].color = COLOR_RED;
        this.planList[item].backgroundColor = COLOR_YELLOW;
        this.planList[item].date = this.getCurrentDateTime();
        this.onInitItem(item);
    },
    onLongPressItem(item) {
        console.log('onLongPressItem = ' + item);
        this.planList[item].title = 'onLongPress';
        this.planList[item].color = COLOR_GREEN;
        this.onInitItem(item);
    },
    onDragItem(item) {
        console.log('onDragItem = ' + item);
    },
    onDeleteAll() {
        console.log('onDeleteAll');
        while (this.planList.length > 0) {
            this.planList.pop();
        }
    },
    onChangeBackgroundColor() {
        console.log('onChangeBackgroundColor');
        if (this.backGroundColor === COLOR_BLUE) {
            this.backGroundColor = COLOR_YELLOW;
        } else if (this.backGroundColor === COLOR_YELLOW) {
            this.backGroundColor = COLOR_GREEN;
        } else if (this.backGroundColor === COLOR_GREEN) {
            this.backGroundColor = COLOR_ORANGE;
        } else {
            this.backGroundColor = COLOR_BLUE;
        }
    },
    getCurrentDateTime() {
        let now = new Date();
        let year = now.getFullYear();
        let month = now.getMonth() + 1;
        let day = now.getDate();
        let hours = now.getHours();
        let minutes = now.getMinutes();
        let seconds = now.getSeconds();

        month = (month < 10 ? '0' : '') + month;
        day = (day < 10 ? '0' : '') + day;
        hours = (hours < 10 ? '0' : '') + hours;
        minutes = (minutes < 10 ? '0' : '') + minutes;
        seconds = (seconds < 10 ? '0' : '') + seconds;

        return year + '-' + month + '-' + day + ' ' + hours + ':' + minutes + ':' + seconds;
    },
    onInitItem(item) {
        for (let i = 0; i < this.planList.length; i++) {
            if (this.planList[i].id === item) {
                continue;
            }

            this.planList[i].title = `plan-${i}`;
            this.planList[i].color = COLOR_ORANGE;
        }
    }
}