/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "transition_tdd_test.h"

#include <cstring>
#include <unistd.h>

#include "ui_view.h"

namespace OHOS {
namespace ACELite {
namespace {
constexpr uint32_t TRANSITION_WAIT_US = 50000;
constexpr uint32_t TRANSITION_SEQUENCE_MIDDLE_WAIT_US = 250000;
constexpr uint32_t TRANSITION_SEQUENCE_END_WAIT_US = 300000;
constexpr int16_t TRANSITION_END_X = 40;
constexpr int16_t TRANSITION_END_Y = 50;
constexpr int16_t TRANSITION_SEQUENCE_MIDDLE_MIN_X = 50;
constexpr uint8_t TRANSITION_END_OPA = 178;
constexpr int16_t TRANSITION_END_ANGLE = 90;
constexpr float TRANSITION_END_SCALE = 2.0f;
constexpr uint8_t TRANSITION_COLOR_RED = 0x44;
constexpr uint8_t TRANSITION_COLOR_GREEN = 0x55;
constexpr uint8_t TRANSITION_COLOR_BLUE = 0x66;
constexpr uint8_t TRANSITION_SCALE_X_INDEX = 0;
constexpr uint8_t TRANSITION_SCALE_Y_INDEX = 5;

const char *const TRANSITION_ANIMATION_LIST_BUNDLE =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('div', {\n"
    "        staticClass: ['box'],\n"
    "        attrs: {\n"
    "          ref: 'box'\n"
    "        }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          width: '454px',\n"
    "          height: '454px'\n"
    "        },\n"
    "        box: {\n"
    "          width: '20px',\n"
    "          height: '20px',\n"
    "          animationName: 'moveAnim, fadeAnim',\n"
    "          animationDuration: '1ms',\n"
    "          animationDelay: '0ms',\n"
    "          animationTimingFunction: 'linear',\n"
    "          animationFillMode: 'forwards',\n"
    "          animationIterationCount: 1\n"
    "        }\n"
    "      },\n"
    "      '@keyframes': {\n"
    "        moveAnim: [{\n"
    "          time: 0,\n"
    "          transform: {\n"
    "            translateX: '0px'\n"
    "          }\n"
    "        }, {\n"
    "          time: 100,\n"
    "          transform: {\n"
    "            translateX: '40px'\n"
    "          }\n"
    "        }],\n"
    "        fadeAnim: [{\n"
    "          time: 0,\n"
    "          opacity: 0.1\n"
    "        }, {\n"
    "          time: 100,\n"
    "          opacity: 0.7\n"
    "        }]\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char *const TRANSITION_PROPERTIES_BUNDLE =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('div', {\n"
    "        staticClass: ['move'], attrs: { ref: 'move' }\n"
    "      }), _c('div', {\n"
    "        staticClass: ['scale'], attrs: { ref: 'scale' }\n"
    "      }), _c('div', {\n"
    "        staticClass: ['rotate'], attrs: { ref: 'rotate' }\n"
    "      }), _c('div', {\n"
    "        staticClass: ['color'], attrs: { ref: 'color' }\n"
    "      }), _c('div', {\n"
    "        staticClass: ['composite'], attrs: { ref: 'composite' }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: { width: '454px', height: '454px' },\n"
    "        move: { width: '20px', height: '20px',\n"
    "          animationName: 'moveAnim', animationDuration: '1ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' },\n"
    "        scale: { width: '20px', height: '20px',\n"
    "          animationName: 'scaleAnim', animationDuration: '1ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' },\n"
    "        rotate: { width: '20px', height: '20px',\n"
    "          animationName: 'rotateAnim', animationDuration: '1ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' },\n"
    "        color: { width: '20px', height: '20px',\n"
    "          animationName: 'colorAnim', animationDuration: '1ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' },\n"
    "        composite: { width: '20px', height: '20px',\n"
    "          animationName: 'compositeAnim', animationDuration: '1ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' }\n"
    "      },\n"
    "      '@keyframes': {\n"
    "        moveAnim: [{ time: 0, transform: { translate: '0px 0px' },\n"
    "          opacity: 0.1 },\n"
    "          { time: 100, transform: { translate: '40px 50px' },\n"
    "          opacity: 0.7 }],\n"
    "        scaleAnim: [{ time: 0, transform: { scale: '1' } },\n"
    "          { time: 100, transform: { scale: '2' } }],\n"
    "        rotateAnim: [{ time: 0, transform: { rotate: '0deg' } },\n"
    "          { time: 100, transform: { rotate: '90deg' } }],\n"
    "        colorAnim: [{ time: 0, backgroundColor: 1122867 },\n"
    "          { time: 100, backgroundColor: 4478310 }],\n"
    "        compositeAnim: [{ time: 0, transform: { translateX: '0px', rotate: '0deg' } },\n"
    "          { time: 100, transform: { translateX: '40px', rotate: '90deg' } }]\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char *const TRANSITION_SEQUENCE_BUNDLE =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', { staticClass: ['container'] }, [_c('div', {\n"
    "        staticClass: ['box'], attrs: { ref: 'box' }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: { width: '454px', height: '454px' },\n"
    "        box: { width: '20px', height: '20px',\n"
    "          animationName: 'sequenceAnim', animationDuration: '400ms',\n"
    "          animationTimingFunction: 'linear', animationFillMode: 'forwards' }\n"
    "      },\n"
    "      '@keyframes': {\n"
    "        sequenceAnim: [{ time: 0, transform: { translateX: '0px' } },\n"
    "          { time: 50, transform: { translateX: '100px' } },\n"
    "          { time: 100, transform: { translateX: '40px' } }]\n"
    "      }\n"
    "    }\n"
    "  });\n"
    "})();\n";
} // namespace

void TransitionTddTest::TransitionAnimationListTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(TRANSITION_ANIMATION_LIST_BUNDLE, strlen(TRANSITION_ANIMATION_LIST_BUNDLE));
    EXPECT_FALSE(JSUndefined::Is(page));
    if (JSUndefined::Is(page)) {
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    UIView *view = GetViewByRef(page, "box");
    EXPECT_NE(view, nullptr);
    if (view == nullptr) {
        DestroyPage(page);
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    Component::HandlerAnimations();
    usleep(TRANSITION_WAIT_US);
    EXPECT_EQ(view->GetX(), TRANSITION_END_X);
    EXPECT_EQ(view->GetOpaScale(), TRANSITION_END_OPA);

    DestroyPage(page);
    Component::ReleaseAnimations();
    TDD_CASE_END();
}

void TransitionTddTest::TransitionPropertiesTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(TRANSITION_PROPERTIES_BUNDLE, strlen(TRANSITION_PROPERTIES_BUNDLE));
    EXPECT_FALSE(JSUndefined::Is(page));
    if (JSUndefined::Is(page)) {
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    UIView *moveView = GetViewByRef(page, "move");
    UIView *scaleView = GetViewByRef(page, "scale");
    UIView *rotateView = GetViewByRef(page, "rotate");
    UIView *colorView = GetViewByRef(page, "color");
    UIView *compositeView = GetViewByRef(page, "composite");
    EXPECT_NE(moveView, nullptr);
    EXPECT_NE(scaleView, nullptr);
    EXPECT_NE(rotateView, nullptr);
    EXPECT_NE(colorView, nullptr);
    EXPECT_NE(compositeView, nullptr);
    if ((moveView == nullptr) || (scaleView == nullptr) || (rotateView == nullptr) || (colorView == nullptr) ||
        (compositeView == nullptr)) {
        DestroyPage(page);
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    const int16_t compositeStartX = compositeView->GetX();
    Component::HandlerAnimations();
    usleep(TRANSITION_WAIT_US);
    EXPECT_EQ(moveView->GetX(), TRANSITION_END_X);
    EXPECT_EQ(moveView->GetY(), TRANSITION_END_Y);
    EXPECT_EQ(moveView->GetOpaScale(), TRANSITION_END_OPA);
    const float *scaleMatrix = scaleView->GetTransformMap().GetScaleMatrix().GetData();
    EXPECT_FLOAT_EQ(scaleMatrix[TRANSITION_SCALE_X_INDEX], TRANSITION_END_SCALE);
    EXPECT_FLOAT_EQ(scaleMatrix[TRANSITION_SCALE_Y_INDEX], TRANSITION_END_SCALE);
    EXPECT_EQ(rotateView->GetTransformMap().GetRotateAngle(), TRANSITION_END_ANGLE);
    const ColorType expectedColor =
        Color::GetColorFromRGB(TRANSITION_COLOR_RED, TRANSITION_COLOR_GREEN, TRANSITION_COLOR_BLUE);
    EXPECT_EQ(colorView->GetStyle(STYLE_BACKGROUND_COLOR), expectedColor.full);
    EXPECT_EQ(compositeView->GetX(), compositeStartX + TRANSITION_END_X);
    EXPECT_EQ(compositeView->GetTransformMap().GetRotateAngle(), TRANSITION_END_ANGLE);

    DestroyPage(page);
    Component::ReleaseAnimations();
    TDD_CASE_END();
}

void TransitionTddTest::TransitionSequenceTest003()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(TRANSITION_SEQUENCE_BUNDLE, strlen(TRANSITION_SEQUENCE_BUNDLE));
    EXPECT_FALSE(JSUndefined::Is(page));
    if (JSUndefined::Is(page)) {
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    UIView *view = GetViewByRef(page, "box");
    EXPECT_NE(view, nullptr);
    if (view == nullptr) {
        DestroyPage(page);
        Component::ReleaseAnimations();
        TDD_CASE_END();
        return;
    }

    Component::HandlerAnimations();
    usleep(TRANSITION_SEQUENCE_MIDDLE_WAIT_US);
    EXPECT_GT(view->GetX(), TRANSITION_SEQUENCE_MIDDLE_MIN_X);
    usleep(TRANSITION_SEQUENCE_END_WAIT_US);
    EXPECT_EQ(view->GetX(), TRANSITION_END_X);

    DestroyPage(page);
    Component::ReleaseAnimations();
    TDD_CASE_END();
}

void TransitionTddTest::RunTests()
{
    TransitionAnimationListTest001();
    TransitionPropertiesTest002();
    TransitionSequenceTest003();
}

#ifdef TDD_ASSERTIONS
HWTEST_F(TransitionTddTest, TransitionAnimationList001, TestSize.Level1)
{
    TransitionAnimationListTest001();
}

HWTEST_F(TransitionTddTest, TransitionProperties002, TestSize.Level1)
{
    TransitionPropertiesTest002();
}

HWTEST_F(TransitionTddTest, TransitionSequence003, TestSize.Level1)
{
    TransitionSequenceTest003();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
