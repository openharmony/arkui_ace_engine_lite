/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
const OperatorLevels = {
    '+': 0,
    '-': 0,
    '*': 1,
    '/': 1,
};

const OperatorHandlers = {
    '+': (one, other) => one + other,
    '-': (one, other) => one - other,
    '*': (one, other) => one * other,
    '/': (one, other) => one / other,
};

function calcSuffixExpression(expression) {
    const numberStack = [];

    while (expression.length) {
        const element = expression.shift();
        if (!isOperator(element)) {
            numberStack.push(Number(element));
        } else {
            const one = numberStack.pop();
            const other = numberStack.pop();
            const result = OperatorHandlers[element](other, one);
            numberStack.push(result);
        }
    }
    return numberStack[0];
}

function processElement(operatorStack, suffixExpression, element) {
    if (element === '(') {
        operatorStack.push(element);
    } else if (element === ')') {
        while (operatorStack.length && operatorStack[operatorStack.length - 1] !== '(') {
            suffixExpression.push(operatorStack.pop());
        }
        if (operatorStack.length) {
            operatorStack.pop();
        }
    } else if (isOperator(element)) {
        let topOperator = operatorStack[operatorStack.length - 1];
        while (topOperator && !isGrouping(topOperator) && !isPrioritized(element, topOperator)) {
            suffixExpression.push(operatorStack.pop());
            topOperator = operatorStack[operatorStack.length - 1];
        }
        operatorStack.push(element);
    } else {
        suffixExpression.push(element);
    }
}

function toSuffixExpression(expression) {
    const operatorStack = [];
    const suffixExpression = [];

    for (let idx = 0; idx < expression.length; idx++) {
        processElement(operatorStack, suffixExpression, expression[idx]);
    }

    while (operatorStack.length) {
        suffixExpression.push(operatorStack.pop());
    }

    return suffixExpression;
}

function parseInfixExpression(content) {
    const size = content.length;
    let number = '';
    const expression = [];
    for (let idx = 0; idx < size; idx++) {
        const element = content[idx];
        if (isGrouping(element)) {
            number && expression.push(number); number = '';
            expression.push(element);
        } else if (isOperator(element)) {
            if (isSymbol(element) && (idx === 0 || content[idx - 1] === '(')) {
                number += element;
            } else {
                number && expression.push(number); number = '';

                idx !== size - 1 && expression.push(element);
            }
        } else {
            number += element;
        }

        idx === size - 1 && number && expression.push(number);
    }
    return expression;
}

function isPrioritized(one, other) {
    return OperatorLevels[one] > OperatorLevels[other];
}

export function isOperator(operator) {
    return (
        operator === '+' || operator === '-' || operator === '*' || operator === '/'
    );
}

function isSymbol(symbol) {
    return symbol === '+' || symbol === '-';
}

function isGrouping(operator) {
    return operator === '(' || operator === ')';
}

export function calc(content) {
    const infixExpression = parseInfixExpression(content);
    const suffixExpression = toSuffixExpression(infixExpression);
    return calcSuffixExpression(suffixExpression);
}