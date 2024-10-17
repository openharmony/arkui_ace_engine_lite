/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
import {calc, isOperator} from '../../common/calculator.js';
import app from '@system.app';

let pressedEqual = false;

export default {
    data: {
        expression: '',
        result: ''
    },
    onInit() {
        this.$watch(() => this.expression, (value)=> {
            if (value !== '') {
                this.result = calc(value).toString();
            }
        });
    },
    handleClear() {
        this.expression = '';
        this.result = '';
    },
    handleInput(value) {
        const isOp = isOperator(value);
        const isEmptyExpression = !this.expression;

        if (pressedEqual) {
            this.expression = value;
            pressedEqual = false;
        } else {
            if (isOp) {
                // Remove the last operator if the current input is an operator
                if (this.expression.length > 0 &&
                        isOperator(this.expression.charAt(this.expression.length - 1))) {
                    this.expression = this.expression.slice(0, -1);
                }
                // Prevent leading division or multiplication
                if (isEmptyExpression && (value === '*' || value === '/')) {
                    return;
                }
                this.expression += value;
            } else {
                this.expression += value;
            }
        }
    },
    handleBackspace() {
        if (pressedEqual) {
            this.expression = '';
            this.result = '';
            pressedEqual = false;
        } else {
            this.expression = this.expression.slice(0, -1);
            if (!this.expression.length) {
                this.result = '';
            }
        }
    },
    handleEqual() {
        if (this.result !== '') {
            this.expression = this.result;
            this.result = '';
            pressedEqual = true;
        }
    },
    handleTerminate(e) {
        if (e.direction === 'right') {
            app.terminate();
        }
    },
    handleExist() {
        app.terminate();
    },
};