/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_KEYS_H
#define OHOS_ACELITE_KEYS_H

#include "acelite_config.h"

#ifndef KEYWORD
#define KEYWORDS_ENUM_DEFINE
#define KEYWORD(enumkey, keystr) K_##enumkey,

namespace OHOS {
namespace ACELite {
enum {
    K_UNKNOWN, // enum start from 0
#endif         // KEYWORD

    /*
     * Rules to define the KEYWORD:
     * Should not be duplicated,
     * and use xxx_yyy instead the xxx-yyy as enumstr
     * because of enum define limitation
     */
    // clang-format off
    // Reused by the struct begin
    // Sample:KEYWORD(enum_string, real-key-string)
    KEYWORD(ALIGN_ITEMS, alignItems) // layout style
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
    KEYWORD(ANALOG_CLOCK, analog-clock) // tag name
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
    KEYWORD(ANIMATION_DELAY, animationDelay) // animation delay
    KEYWORD(ANIMATION_DURATION, animationDuration) // animation during
    KEYWORD(ANIMATION_FILL_MODE, animationFillMode) // animation fill mode
    KEYWORD(ANIMATION_ITERATION_COUNT, animationIterationCount) // animation iteration count
    KEYWORD(ANIMATION_NAME, animationName) // animation name
    KEYWORD(ANIMATION_TIMING_FUNCTION, animationTimingFunction) // animation timing function
    KEYWORD(ARC, arc) // circle progress
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(AUTOPLAY, autoplay) // video component autoplay attribute
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(BACKGROUND_COLOR, backgroundColor) // chart line color
    KEYWORD(BACKGROUND_IMAGE, backgroundImage) // common style, but only button, checkbox, radio support
#if (FEATURE_COMPONENT_GRADIENT == 1)
    KEYWORD(BACKGROUND, background)
#endif // FEATURE_COMPONENT_GRADIENT
    KEYWORD(BLOCK_COLOR, blockColor)
    KEYWORD(BORDER_BOTTOM_COLOR, borderBottomColor) // common style border bottom color
    KEYWORD(BORDER_LEFT_COLOR, borderLeftColor) // common style border left color
    KEYWORD(BORDER_RIGHT_COLOR, borderRightColor) // common style border right color
    KEYWORD(BORDER_TOP_COLOR, borderTopColor) // common style border top color
    KEYWORD(BORDER_COLOR, borderColor) // common style, horizon progress style
    KEYWORD(BORDER_RADIUS, borderRadius) // horizon progress style
    KEYWORD(BORDER_BOTTOM_WIDTH, borderBottomWidth) // common style border bottom width
    KEYWORD(BORDER_LEFT_WIDTH, borderLeftWidth) // common style border left width
    KEYWORD(BORDER_RIGHT_WIDTH, borderRightWidth) // common style border right width
    KEYWORD(BORDER_TOP_WIDTH, borderTopWidth) // common style border top width
    KEYWORD(BORDER_WIDTH, borderWidth) // common style, horizon progress style
    KEYWORD(BREAK, break) // text overflow type
#if (FEATURE_COMPONENT_CAMERA == 1)
    KEYWORD(CAMERA, camera) // camera component
#endif // FEATURE_COMPONENT_CAMERA
#if (FEATURE_COMPONENT_CANVAS == 1)
    KEYWORD(CANVAS, canvas) // tag name
#endif // FEATURE_COMPONENT_CANVAS
    KEYWORD(CANVASGRADIENT, canvasGradient) // gradient fillstyle or strokestyle
    KEYWORD(CANVASPATTERN, canvasPattern) // pattern fillstyle or strokestyle
    KEYWORD(CENTER, center) // text align type
    KEYWORD(CENTER_X, centerX) // circle progress x
    KEYWORD(CENTER_Y, centerY) // circle progress y
    KEYWORD(CHANGE, change) // the input component event name, picker-view event name
    KEYWORD(CHECKED, checked) // the input and switch component of checkbox attribute
    KEYWORD(CHECKBOX, checkbox) // the input component type value
    KEYWORD(CLICK, click) // click event listener
    KEYWORD(CLIP, clip) // text overflow type
    KEYWORD(CHART, chart) // tag name
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
    KEYWORD(CLOCK_HAND, clock-hand) // tag name
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
    KEYWORD(COLOR, color) // text style type, rectangular clock-hand fill color, horizon progress style
    KEYWORD(COLUMN, column) // layout style
    KEYWORD(COLUMN_REVERSE, column-reverse) // layout style
    KEYWORD(CONTAIN, contain) // image component contain
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(CONTROLS, controls) // video component controls attribute
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(COVER, cover) // image component cover
    KEYWORD(DATASETS, datasets) // chart data
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
    KEYWORD(DATE, date) // picker-view attr value
#endif // FEATURE_COMPONENT_DATE_PICKER
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(DAY, day)
    KEYWORD(DIGIT2, digit2)
#endif
    KEYWORD(DIRECTION, direction) // progress direction
    KEYWORD(DISPLAY, display) // div layout
    KEYWORD(DIV, div) // tag name
    KEYWORD(DRAG, drag) // drag event listener
    KEYWORD(DURATION, duration)
    KEYWORD(EASE_IN, ease-in) // animation time function value
    KEYWORD(EASE_IN_OUT, ease-in-out) // animation time function value
    KEYWORD(EASE_OUT, ease-out) // animation time function value
    KEYWORD(ELLIPSIS, ellipsis) // text overflow type
#if (FEATURE_COMPONENT_EDITTEXT == 1)
    KEYWORD(EDITTEXTTYPE, editTextType) // text overflow type
#endif // FEATURE_COMPONENT_EDITTEXT
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
    KEYWORD(END, end) // date picker-view attr
#endif // FEATURE_COMPONENT_DATE_PICKER
#if ((FEATURE_COMPONENT_CAMERA == 1) || (FEATURE_COMPONENT_VIDEO == 1))
    KEYWORD(ERROR, error) // camera component error event
#endif // FEATURE_COMPONENT_CAMERA
    KEYWORD(EXPAND, expand) // text overflow type
    KEYWORD(FALSE, false) // common attributes
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(FINISH, finish) // video component finish event tag
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(FILL, fill) // image component fill
    KEYWORD(FILL_MODE, fillmode) // image-animator attribtue
    KEYWORD(FIT_ORIGINAL_SIZE, fitOriginalSize) // image component fit original size
    KEYWORD(FIXED_SIZE, fixedsize) // image-animator attribute
    KEYWORD(FLEX_DIRECTION, flexDirection) // layout style
    KEYWORD(FLEX_END, flex-end) // layout style
    KEYWORD(FLEX_START, flex-start) // layout style
    KEYWORD(FLEX_WRAP, flexWrap) // layout style
    KEYWORD(FONT_SIZE, fontSize) // text style type
    KEYWORD(FONT_FAMILY, fontFamily) // text style type
    KEYWORD(FORWARDS, forwards) // animation fill value
    KEYWORD(HEIGHT, height) // common attributes
#if ((FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_DATE_FORMAT == 1))
    KEYWORD(HOUR, hour) // analog-clock hour
#endif // FEATURE_COMPONENT_ANALOG_CLOCK FEATURE_DATE_FORMAT
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(HOUR12, hour12)
#endif
    KEYWORD(ID, id) // common attributes
    KEYWORD(IMAGE, image) // tag name
    KEYWORD(IMAGE_ANIMATOR, image-animator) // tag name
    KEYWORD(IMAGES, images) // image-animator attribute
    KEYWORD(INDEX, index) // swiper attr index
    KEYWORD(INPUT, input) // input component name
    KEYWORD(ITEM_SELECTED, itemselected) // scroll item selected event listener
    KEYWORD(ITERATION, iteration) // image-animator attribute
    KEYWORD(JUSTIFY_CONTENT, justifyContent) // layout style
#ifdef JS_EXTRA_EVENT_SUPPORT
    KEYWORD(KEY, key) // onkey event
#endif
    KEYWORD(LABELS, labels) // chart labels
    KEYWORD(LARGER, larger) // text font size type
    KEYWORD(LEFT, left) // text align type
    KEYWORD(LETTER_SPACING, letterSpacing) // text style type
    KEYWORD(LINE_HEIGHT, lineHeight) // text style type
    KEYWORD(LIST, list) // list tag name
    KEYWORD(LIST_ITEM, list-item) // list-item tag name
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(LONG, long)
#endif
    KEYWORD(LONGPRESS, longpress) // long progress event listener
    KEYWORD(LOOP, loop) // swiper loop attribute
    KEYWORD(MARGIN, margin) // common style margin
    KEYWORD(MARGIN_BOTTOM, marginBottom) // common style margin
    KEYWORD(MARGIN_LEFT, marginLeft) // common style margin left
    KEYWORD(MARGIN_RIGHT, marginRight) // common style margin right
    KEYWORD(MARGIN_TOP, marginTop) // common style margin top
    KEYWORD(MARQUEE, marquee) // marquee tag name
    KEYWORD(MAX, max) // common attributes
#if (FEATURE_COMPONENT_EDITTEXT == 1)
    KEYWORD(MAX_LENGTH, maxlength) // maxlength of input
#endif // FEATURE_COMPONENT_EDITTEXT
    KEYWORD(MIN, min) // common attributes, analog-clock minute
    KEYWORD(MINI, mini) // text font size type
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(MINUTE, minute)
    KEYWORD(MONTH, month)
#endif
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(MUTED, muted) // video component muted attribute
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(NAME, name) // the attribute name
    KEYWORD(NONE, none) // animation fill value
    KEYWORD(NO_REPEAT, no-repeat) // no-repeat pattern type
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(NUMERIC, numeric)
#endif
    KEYWORD(OBJECT_FIT, objectFit) // image component object-fit
#if FEATURE_PATH_ANIMATOR
    KEYWORD(OFFSET_DISTANCE, offsetDistance) // motion path progress 0%~100%
    KEYWORD(OFFSET_PATH, offsetPath) // motion path: path("M ... L/Q/C ... Z")
    KEYWORD(OFFSET_ROTATE, offsetRotate) // motion path rotate (angle only)
#endif // FEATURE_PATH_ANIMATOR
    KEYWORD(OPACITY, opacity) // common style opacity
    KEYWORD(OPTIONS, options) // chart options
    KEYWORD(PADDING, padding)
    KEYWORD(PADDING_BOTTOM, paddingBottom)
    KEYWORD(PADDING_LEFT, paddingLeft)
    KEYWORD(PADDING_RIGHT, paddingRight)
    KEYWORD(PADDING_TOP, paddingTop)
#if (FEATURE_COMPONENT_EDITTEXT == 1)
    KEYWORD(PASSWORD, password) // type of input component
#endif // FEATURE_COMPONENT_EDITTEXT
    KEYWORD(PERCENT, percent)
    KEYWORD(PICKER_VIEW, picker-view) // tag name
    KEYWORD(REF, ref) // common attribute
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(PREPARED, prepared) // video component prepared event
#endif // FEATURE_COMPONENT_VIDEO
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
    KEYWORD(PIVOT_X, pivotX) // clock-hand pivot-x
    KEYWORD(PIVOT_Y, pivotY) // clock-hand pivot-y
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_EDITTEXT == 1)
    KEYWORD(PLACEHOLDER, placeholder) // placeholder of edittext
    KEYWORD(PLACEHOLDER_COLOR, placeholderColor) // placeholder-color of edittext
#endif // FEATURE_COMPONENT_EDITTEXT
    KEYWORD(PROGRESS, progress) // progress tag name
#if (FEATURE_COMPONENT_QRCODE == 1)
    KEYWORD(QRCODE, qrcode) // qrcode tag name
#endif
    KEYWORD(RADIO, radio) // the type in input
    KEYWORD(RADIUS, radius) // circle progress radius
    KEYWORD(RANGE, range) // picker-view attr
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(PAUSE, pause) // video component pause event tag
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(REVERSE, reverse) // image-animator attribute
    KEYWORD(REPEAT, repeat) // repeat pattern type
    KEYWORD(REPEAT_X, repeat-x) // repeat-x pattern type
    KEYWORD(REPEAT_Y, repeat-y) // repeat-y pattern type
    KEYWORD(RIGHT, right) // text align type
    KEYWORD(ROTATE, rotate) // animation rotate
    KEYWORD(ROW, row) // layout style
    KEYWORD(ROW_REVERSE, row-reverse) // layout style
#if FEATURE_TRANSITION_ANIMATOR
    KEYWORD(SCALE, scale) // animation scale
#endif // FEATURE_TRANSITION_ANIMATOR
    KEYWORD(SCALE_DOWN, scaleDown) // image component scale down
    KEYWORD(SCROLLAMOUNT, scrollamount) // marquee scroll speed
    KEYWORD(SCROLLBOTTOM, scrollbottom) // scroll bottom event listener
    KEYWORD(SCROLLDELAY, scrolldelay) // marquee scroll delay
    KEYWORD(SCROLLEND, scrollend) // scroll end event listener
    KEYWORD(SCROLLSTART, scrollstart) // scroll start event listener
    KEYWORD(SCROLLTOP, scrolltop) // scroll top event listener
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
    KEYWORD(SEC, sec) // analog-clock second
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(SECOND, second)
#endif
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(SEEKING, seeking) // // video component seeking event tag
    KEYWORD(SEEKED, seeked) // // video component seeked event tag
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(SELECTED, selected) // picker-view attr
    KEYWORD(SELECTED_COLOR, selectedColor) // picker-view style
    KEYWORD(SELECTED_FONT_FAMILY, selectedFontFamily) // picker-view style
    KEYWORD(SELECTED_FONT_SIZE, selectedFontSize) // picker-view style
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(SHORT, short)
#endif
    KEYWORD(SHOW, show)
    KEYWORD(SLIDER, slider) // tag name
    KEYWORD(SPACE_AROUND, space-around) // layout style
    KEYWORD(SPACE_BETWEEN, space-between) // layout style
    KEYWORD(SPACE_EVENLY, space-evenly) // layout style
    KEYWORD(SRC, src) // common attributes, use for image view, video compnent
    KEYWORD(IMAGE_WIDTH, width) // common attributes, use for image view, video compnent
    KEYWORD(IMAGE_HEIGHT, height) // common attributes, use for image view, video compnent
    KEYWORD(STACK, stack) // tag name
    KEYWORD(STANDARD, standard) // text font size type
    KEYWORD(STOP, stop) // image-animator stop event, video component end event tag
#if ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
    KEYWORD(START, start) // date picker-view attr, video component start event tag
#endif // ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
    KEYWORD(START_ANGLE, startAngle) // circle progress start angle
    KEYWORD(STEP, step)
#if (FEATURE_COMPONENT_SVG == 1)
    KEYWORD(STROKE, stroke) // svg/css stroke color
#endif
    KEYWORD(STROKE_WIDTH, strokeWidth) // horizon/arc progress style
    KEYWORD(SWIPE, swipe) // swipe event listener
    KEYWORD(SWIPER, swiper) // tag name
    KEYWORD(SWITCH, switch) // tag name
#if (FEATURE_COMPONENT_TABS == 1)
    KEYWORD(TAB_BAR, tab-bar) // tag name
    KEYWORD(TAB_CONTENT, tab-content) // tag name
    KEYWORD(TABS, tabs) // tag name
#endif // FEATURE_COMPONENT_TABS
    KEYWORD(TEXT, text) // tag name, picker-view attr value
    KEYWORD(TEXT_ALIGN, textAlign) // text style type
    KEYWORD(TEXT_OVERFLOW, textOverflow) // text style type
    KEYWORD(TIME, time) // picker-view attr value
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(TIME_UPDATE, timeupdate) // video component timeupdate event tag
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(TOP, top)
    KEYWORD(TOTAL_ANGLE, total-angle) // circle progress style
    KEYWORD(TOUCHSTART, touchStart) // touch event listener
    KEYWORD(TOUCHMOVE, touchmove) // touchmove event listener
    KEYWORD(TOUCHEND, touchEnd) // touchend event listener
#ifdef JS_EXTRA_EVENT_SUPPORT
    KEYWORD(TOUCHCANCEL, touchcancel) // ontouchcancel event
#endif
    KEYWORD(TRANSLATE_X, translateX) // animation transform x
    KEYWORD(TRANSLATE_Y, translateY) // animation transform y
#if FEATURE_ELEMENT_TRANSITION
    KEYWORD(TRANSITION_DELAY, transitionDelay) // transition delay
    KEYWORD(TRANSITION_DURATION, transitionDuration) // transition duration
    KEYWORD(TRANSITION_TIMING_FUNCTION, transitionTimingFunction) // transition timing function
    KEYWORD(TRANSITION_EFFECT, transitionEffect) // transition effect, arkui own property
#endif
    KEYWORD(TRUE, true) // common attributes
#if ((FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_COMPONENT_EDITTEXT == 1))
    KEYWORD(TYPE, type) // clock-hand type
#endif // (FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_COMPONENT_EDITTEXT == 1)
    KEYWORD(VALUE, value) // common attributes
    KEYWORD(VERTICAL, vertical) // swiper attr vertical
#if (FEATURE_COMPONENT_VIDEO == 1)
    KEYWORD(VIDEO, video) // video tag name
#endif // FEATURE_COMPONENT_VIDEO
    KEYWORD(WIDTH, width) // common attributes
    KEYWORD(WRAP, wrap) // flex wrap
#if (FEATURE_DATE_FORMAT == 1)
    KEYWORD(WEEKDAY, weekday)
    KEYWORD(YEAR, year)
#endif
#if (FEATURE_NUMBER_FORMAT == 1)
    KEYWORD(STYLE, style)
    KEYWORD(USEGROUP, useGroup)
    KEYWORD(MINIMUMFRACTIONDIGITS, minimumFractionDigits)
    KEYWORD(MAXIMUMFRACTIONDIGITS, maximumFractionDigits)
#endif
#if FEATURE_COMPONENT_TEXT_SPANNABLE
    KEYWORD(BACKGROUNDCOLORSPANCOLOR, backgroundcolorspancolor)
    KEYWORD(BACKGROUNDCOLORSPANSTART, backgroundcolorspanstart)
    KEYWORD(BACKGROUNDCOLORSPANEND, backgroundcolorspanend)
    KEYWORD(FOREGROUNDCOLORSPANCOLOR, foregroundcolorspancolor)
    KEYWORD(FOREGROUNDCOLORSPANSTART, foregroundcolorspanstart)
    KEYWORD(FOREGROUNDCOLORSPANEND, foregroundcolorspanend)
    KEYWORD(LINEBACKGROUNDCOLORSPANCOLOR, linebackgroundcolorspancolor)
    KEYWORD(LINEBACKGROUNDCOLORSPANSTART, linebackgroundcolorspanstart)
    KEYWORD(LINEBACKGROUNDCOLORSPANEND, linebackgroundcolorspanend)
    KEYWORD(ABSOLUTESIZESPANSTART, absolutesizespanstart)
    KEYWORD(ABSOLUTESIZESPANEND, absolutesizespanend)
    KEYWORD(ABSOLUTESIZESPANSIZE, absolutesizespansize)
    KEYWORD(RELATIVESIZESPANSTART, relativesizespanstart)
    KEYWORD(RELATIVESIZESPANEND, relativesizespanend)
    KEYWORD(RELATIVESIZESPANSIZE, relativesizespansize)
    KEYWORD(SPANNABLESTYLE, spannablestyle)
    KEYWORD(SPANNABLESTART, spannablestart)
    KEYWORD(SPANNABLEEND, spannableend)
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
#if (GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT == 1)
    KEYWORD(ABSOLUTE, absolute) // position: absolute
    KEYWORD(ALIGN_CONTENT, alignContent) // flex multi-line cross-axis distribution
    KEYWORD(ALIGN_SELF, alignSelf) // layout style, child cross-axis alignment override
    KEYWORD(ASPECT_RATIO, aspectRatio) // flex item preferred width/height ratio
    KEYWORD(AUTO, auto) // margin auto / align-self auto
    KEYWORD(BOTTOM, bottom) // bottom offset
    KEYWORD(COLUMN_GAP, columnGap) // flex layout column gap
    KEYWORD(FLEX_BASIS, flexBasis) // flex item main-axis base size
    KEYWORD(FLEX_GROW, flexGrow) // flex item grow factor
    KEYWORD(FLEX_SHRINK, flexShrink) // flex item shrink factor
    KEYWORD(GAP, gap) // flex layout gap
    KEYWORD(HIDDEN, hidden) // overflow hidden
    KEYWORD(MAX_HEIGHT, maxHeight) // flex item max height constraint
    KEYWORD(MAX_WIDTH, maxWidth) // flex item max width constraint
    KEYWORD(MIN_HEIGHT, minHeight) // flex item min height constraint
    KEYWORD(MIN_WIDTH, minWidth) // flex item min width constraint
    KEYWORD(OVERFLOW, overflow) // overflow control
    KEYWORD(POSITION, position) // position type
    KEYWORD(ROW_GAP, rowGap) // flex layout row gap
    KEYWORD(STRETCH, stretch) // layout style
    KEYWORD(VISIBLE, visible) // overflow visible
#endif // GRAPHIC_ENABLE_FLEX_LAYOUT_ENHANCEMENT
#if (FEATURE_COMPONENT_SVG == 1)
    KEYWORD(SVG, svg) // tag name
    KEYWORD(RECT, rect) // svg shape tag name
    KEYWORD(CIRCLE, circle) // svg shape tag name
    KEYWORD(ELLIPSE, ellipse) // svg shape tag name
    KEYWORD(LINE, line) // svg shape tag name
    KEYWORD(POLYLINE, polyline) // svg shape tag name
    KEYWORD(POLYGON, polygon) // svg shape tag name
    KEYWORD(PATH, path) // svg shape tag name
    KEYWORD(SVG_G, g) // svg container tag name
    KEYWORD(DEFS, defs) // svg container tag name
    KEYWORD(LINEAR_GRADIENT, linearGradient) // svg gradient tag name
    KEYWORD(RADIAL_GRADIENT, radialGradient) // svg gradient tag name
    // note: svg gradient <stop> reuses the existing K_STOP keyword (image-animator stop)
    KEYWORD(SOLID_COLOR, solidColor) // svg solid color tag name
    KEYWORD(ANIMATE, animate) // svg animation tag name
    KEYWORD(ANIMATE_COLOR, animateColor) // svg animation tag name
    KEYWORD(ANIMATE_TRANSFORM, animateTransform) // svg animation tag name
    KEYWORD(ANIMATE_MOTION, animateMotion) // svg animation tag name
    KEYWORD(SET, set) // svg animation tag name
    KEYWORD(MPATH, mpath) // svg animation tag name
    KEYWORD(USE, use) // svg use tag name
    KEYWORD(TEXT_AREA, textArea) // svg text area tag name
    KEYWORD(TSPAN, tspan) // svg tspan tag name
    KEYWORD(STROKE_LINECAP, strokeLinecap) // svg stroke linecap style
    KEYWORD(STROKE_LINEJOIN, strokeLinejoin) // svg stroke linejoin style
    KEYWORD(STROKE_MITERLIMIT, strokeMiterlimit) // svg stroke miterlimit style
    KEYWORD(STROKE_DASHARRAY, strokeDasharray) // svg stroke dasharray style
    KEYWORD(STROKE_DASHOFFSET, strokeDashoffset) // svg stroke dashoffset style
    KEYWORD(FILL_OPACITY, fillOpacity) // svg fill opacity style
    KEYWORD(STROKE_OPACITY, strokeOpacity) // svg stroke opacity style
    KEYWORD(VISIBILITY, visibility) // svg visibility style
    KEYWORD(TEXT_ANCHOR, textAnchor) // svg text anchor style
#endif // FEATURE_COMPONENT_SVG
    // Reused by the struct end
// clang-format on
#ifdef KEYWORDS_ENUM_DEFINE
    KEYWORDS_MAX, // max size is UINT16_MAX
};
#undef KEYWORDS_ENUM_DEFINE
#undef KEYWORD
} // namespace ACELite
} // OHOS namespace
#endif // KEYWORDS_ENUM_DEFINE

#endif // OHOS_ACELITE_KEYS_H
