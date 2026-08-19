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

#include "svg_tdd_test.h"

#include "acelite_config.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"

#if (FEATURE_COMPONENT_SVG == 1)
#include "ace_mem_base.h"
#include "app_style_item.h"
#include "key_parser.h"
#include "keys.h"
#include "svg_component_utils.h"
#include "svg_element_component.h"
#endif // FEATURE_COMPONENT_SVG

namespace OHOS {
namespace ACELite {
const char * const BUNDLE1 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '300px', height: '300px'},\n"
    "                attrs:{\n"
    "                    ref: 'svg1',\n"
    "                    viewBox: '0 0 100 100'\n"
    "                }\n"
    "            },[_c('rect',{\n"
    "                attrs:{\n"
    "                    x: '10',\n"
    "                    y: '10',\n"
    "                    width: '50',\n"
    "                    height: '50',\n"
    "                    fill: '#ff0000'\n"
    "                }\n"
    "            })])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

const char * const BUNDLE2 =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container']\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '300px', height: '300px'},\n"
    "                attrs:{\n"
    "                    ref: 'svg2',\n"
    "                    viewBox: '0 0 100 100'\n"
    "                }\n"
    "            },[_c('defs',{},[\n"
    "                _c('linearGradient',{attrs:{id:'grad1'}},[\n"
    "                    _c('stop',{attrs:{'offset':'0%','stop-color':'red'}}),\n"
    "                    _c('stop',{attrs:{'offset':'100%','stop-color':'blue'}})\n"
    "                ]),\n"
    "                _c('rect',{attrs:{id:'shape1',x:'5',y:'5',width:'20',height:'20',fill:'green'}})\n"
    "            ]),_c('rect',{\n"
    "                attrs:{\n"
    "                    x: '10',\n"
    "                    y: '10',\n"
    "                    width: '30',\n"
    "                    height: '30',\n"
    "                    fill: 'url(#grad1)'\n"
    "                }\n"
    "            },[_c('animate',{attrs:{attributeName:'x',from:'10',to:'50',dur:'2s',repeatCount:'indefinite'}})\n"
    "            ]),_c('use',{\n"
    "                attrs:{\n"
    "                    'xlink:href': '#shape1',\n"
    "                    x: '60',\n"
    "                    y: '10'\n"
    "                }\n"
    "            },[_c('animateTransform',{attrs:{"
    "attributeName:'transform',type:'rotate',"
    "from:'0 70 20',to:'360 70 20',dur:'3s',"
    "repeatCount:'indefinite'}})\n"
    "            ])])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: {\n"
    "                    width: '454px',\n"
    "                    height: '454px',\n"
    "                    justifyContent: 'center',\n"
    "                    alignItems: 'center'\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

void SvgTddTest::SvgRectRenderTest001()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE1, strlen(BUNDLE1));
    UIView *svgView = GetViewByRef(page, "svg1");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
    TDD_CASE_END();
}

void SvgTddTest::SvgGradientAnimationUseTest002()
{
    TDD_CASE_BEGIN();
    JSValue page = CreatePage(BUNDLE2, strlen(BUNDLE2));
    UIView *svgView = GetViewByRef(page, "svg2");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
    TDD_CASE_END();
}

void SvgTddTest::RunTests()
{
    SvgRectRenderTest001();
    SvgGradientAnimationUseTest002();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:SvgRectRenderTest001
 * @tc.desc: Verify declarative SVG with rect renders a non-null view.
 */
HWTEST_F(SvgTddTest, SvgRectRender001, TestSize.Level1)
{
    SvgTddTest::SvgRectRenderTest001();
}

/**
 * @tc.name:SvgGradientAnimationUseTest002
 * @tc.desc: Verify declarative SVG with gradient, animate, and use renders a non-null view.
 */
HWTEST_F(SvgTddTest, SvgGradientAnimationUse002, TestSize.Level1)
{
    SvgTddTest::SvgGradientAnimationUseTest002();
}

#if (FEATURE_COMPONENT_SVG == 1)
/*
 * The tag mapping helpers below have external linkage but are intentionally not exported by
 * svg_element_component.h. Declare them here so the switch branches can be covered directly.
 */
SvgElementType TagToSvgShapeType(uint16_t tag);
SvgElementType TagToSvgStructType(uint16_t tag);
SvgElementType TagToSvgType(uint16_t tag);

namespace {
// Bundle without any attrs on <svg> and on <rect>, and with a non-SVG child inside <svg>.
const char * const BUNDLE_DEFAULT_ATTRS =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box3' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '200px', height: '200px'}\n"
    "            },[_c('rect',{}),\n"
    "              _c('text',{attrs:{value: 'not-svg'}})\n"
    "            ])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle covering percent sizes, zero sizes, every private attribute and an unusable attr value.
const char * const BUNDLE_PRIVATE_ATTRS =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box4' }\n"
    "            },[_c('svg',{\n"
    "                attrs:{\n"
    "                    ref: 'svg4',\n"
    "                    width: '100%',\n"
    "                    height: '100%',\n"
    "                    viewBox: '0 0 10 10',\n"
    "                    fill: 'red',\n"
    "                    stroke: 'blue',\n"
    "                    strokeWidth: '2',\n"
    "                    strokeLinecap: 'round',\n"
    "                    strokeLinejoin: 'bevel',\n"
    "                    strokeMiterlimit: '4',\n"
    "                    strokeDasharray: '4 2',\n"
    "                    strokeDashoffset: '1',\n"
    "                    fillOpacity: '0.5',\n"
    "                    strokeOpacity: '0.5',\n"
    "                    visibility: 'visible',\n"
    "                    textAnchor: 'middle',\n"
    "                    fontSize: '12',\n"
    "                    opacity: '0.5',\n"
    "                    id: 'svgRoot',\n"
    "                    unknownAttr: null\n"
    "                }\n"
    "            },[_c('circle',{attrs:{cx:'5',cy:'5',r:'4'}})]),\n"
    "            _c('svg',{attrs:{ref:'svg4b', width:'0', height:'0'}})]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle covering every ApplyStyle branch, attr/style overwrite and the animateColor color hint.
const char * const BUNDLE_ELEMENT_STYLE =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box5' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '200px', height: '200px'},\n"
    "                attrs:{ ref: 'svg5' }\n"
    "            },[_c('rect',{\n"
    "                staticStyle:{\n"
    "                    fill: '#00ff00',\n"
    "                    stroke: '#0000ff',\n"
    "                    strokeWidth: 2,\n"
    "                    strokeLinecap: 'round',\n"
    "                    strokeLinejoin: 'bevel',\n"
    "                    strokeMiterlimit: 4,\n"
    "                    strokeDasharray: '4 2',\n"
    "                    strokeDashoffset: 1,\n"
    "                    fillOpacity: '0.5',\n"
    "                    strokeOpacity: '0.5',\n"
    "                    visibility: 'visible',\n"
    "                    textAnchor: 'middle',\n"
    "                    fontSize: 12,\n"
    "                    opacity: 0.5,\n"
    "                    margin: 2\n"
    "                },\n"
    "                attrs:{x:'1',y:'1',width:'10',height:'10',fill:'#ff0000',badValue:null}\n"
    "            },[_c('animateColor',{attrs:{"
    "attributeName:'fill',from:'#ff0000',to:'#0000ff',dur:'1s'}}),\n"
    "              _c('text',{attrs:{value:'child'}})\n"
    "            ])])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle placing SVG elements outside of any <svg> root, so no document can be resolved.
const char * const BUNDLE_ORPHAN_ELEMENT =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box6' }\n"
    "            },[_c('g',{attrs:{ref:'orphanGroup', id:'orphanGroup'}},[\n"
    "                _c('rect',{attrs:{x:'1',y:'1',width:'5',height:'5'}})\n"
    "            ])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle driving startAnimation/stopAnimation, including a call on an object without a component.
const char * const BUNDLE_JS_ANIMATION =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box7' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '120px', height: '120px'},\n"
    "                attrs:{ ref: 'svg7' }\n"
    "            },[_c('rect',{attrs:{x:'0',y:'0',width:'10',height:'10'}},[\n"
    "                _c('animate',{attrs:{"
    "attributeName:'x',from:'0',to:'10',dur:'1s',repeatCount:'indefinite'}})\n"
    "            ])])]);\n"
    "        },\n"
    "        onShow: function () {\n"
    "            var el = this.$refs.svg7;\n"
    "            if (!el) { return; }\n"
    "            if (el.startAnimation) { el.startAnimation(); }\n"
    "            if (el.stopAnimation) { el.stopAnimation(); }\n"
    "            var start = el.startAnimation;\n"
    "            var stop = el.stopAnimation;\n"
    "            if (start) { start.call({}); }\n"
    "            if (stop) { stop.call({}); }\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle binding a private attribute to observed data so the update path is triggered.
const char * const BUNDLE_ATTR_UPDATE =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box8' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '120px', height: '120px'},\n"
    "                attrs:{ ref: 'svg8', fill: function () { return _vm.color; } }\n"
    "            },[_c('rect',{attrs:{x:'0',y:'0',width:'10',height:'10'}})])]);\n"
    "        },\n"
    "        data: { color: '#ff0000' },\n"
    "        onShow: function () { this.color = '#00ff00'; },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle passing JS null as the attrs object of <svg> and <rect>, which previously triggered
// jerry_get_object_keys(null) undefined behavior in CaptureAttrs / CreateNativeViews.
const char * const BUNDLE_NULL_ATTRS =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'boxNull' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '100px', height: '100px'},\n"
    "                attrs: null\n"
    "            },[_c('rect',{attrs: null})])]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

// Bundle covering both an SVG <image> (href) and a UI <image> that carries only x/y (no src, so no
// image is loaded). The UI image is the case the disambiguation used to misclassify as SVG because
// x/y were part of the SVG detection set; after narrowing it must route to ImageComponent.
const char * const BUNDLE_IMAGE_ROUTING =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'boxImg' }\n"
    "            },[\n"
    "              _c('image',{staticStyle:{width:'20px', height:'20px'}, attrs:{ref:'uiImage', x:'0', y:'0'}}),\n"
    "              _c('svg',{attrs:{ref:'svgImg', width:'100', height:'100', viewBox:'0 0 10 10'}},\n"
    "                [_c('image',{attrs:{ref:'svgImage', href:'/img.png', x:'1', y:'1'}})]\n"
    "              )\n"
    "            ]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

void CheckCanonicalName(const char *jsName, const char *expected)
{
    char *result = SvgComponentUtils::CanonicalName(jsName);
    ASSERT_TRUE(result != nullptr);
    EXPECT_STREQ(result, expected);
    ace_free(result);
}

void CheckValueFromJerry(jerry_value_t value, bool isColor, const char *expected)
{
    char *result = SvgComponentUtils::ValueFromJerry(value, isColor);
    if (expected == nullptr) {
        EXPECT_TRUE(result == nullptr);
        ace_free(result);
        jerry_release_value(value);
        return;
    }
    ASSERT_TRUE(result != nullptr);
    EXPECT_STREQ(result, expected);
    ace_free(result);
    jerry_release_value(value);
}

void CheckValueFromStyle(const char *keyName, jerry_value_t value, bool isColor, const char *expected)
{
    uint16_t keyId = KeyParser::ParseKeyId(keyName);
    AppStyleItem *item = AppStyleItem::CreateStyleItem(keyId, value);
    jerry_release_value(value);
    ASSERT_TRUE(item != nullptr);
    char *result = SvgComponentUtils::ValueFromStyle(item, isColor);
    if (expected == nullptr) {
        EXPECT_TRUE(result == nullptr);
        ace_free(result);
        delete item;
        return;
    }
    ASSERT_TRUE(result != nullptr);
    EXPECT_STREQ(result, expected);
    ace_free(result);
    delete item;
}

jerry_value_t MakeString(const char *value)
{
    return jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
}
} // namespace

/**
 * @tc.name:SvgIsColorAttrTest003
 * @tc.desc: Verify IsColorAttr covers the null guard, every color name and a non-color name.
 */
HWTEST_F(SvgTddTest, SvgIsColorAttr003, TestSize.Level1)
{
    EXPECT_FALSE(SvgComponentUtils::IsColorAttr(nullptr));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("fill"));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("stroke"));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("stop-color"));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("solid-color"));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("color"));
    EXPECT_TRUE(SvgComponentUtils::IsColorAttr("flood-color"));
    EXPECT_FALSE(SvgComponentUtils::IsColorAttr("stroke-width"));
    EXPECT_FALSE(SvgComponentUtils::IsColorAttr(""));
}

/**
 * @tc.name:SvgCanonicalNameSpecialTest004
 * @tc.desc: Verify CanonicalName returns the preserved spelling for every special cased name.
 */
HWTEST_F(SvgTddTest, SvgCanonicalNameSpecial004, TestSize.Level1)
{
    EXPECT_TRUE(SvgComponentUtils::CanonicalName(nullptr) == nullptr);
    CheckCanonicalName("viewbox", "viewBox");
    CheckCanonicalName("viewBox", "viewBox");
    CheckCanonicalName("attributename", "attributeName");
    CheckCanonicalName("repeatcount", "repeatCount");
    CheckCanonicalName("calcmode", "calcMode");
    CheckCanonicalName("keytimes", "keyTimes");
    CheckCanonicalName("keysplines", "keySplines");
    CheckCanonicalName("keypoints", "keyPoints");
}

/**
 * @tc.name:SvgCanonicalNameCamelCaseTest005
 * @tc.desc: Verify CanonicalName converts camelCase to kebab-case and keeps plain names as is.
 */
HWTEST_F(SvgTddTest, SvgCanonicalNameCamelCase005, TestSize.Level1)
{
    CheckCanonicalName("strokeWidth", "stroke-width");
    CheckCanonicalName("strokeDashoffset", "stroke-dashoffset");
    CheckCanonicalName("fill", "fill");
    CheckCanonicalName("", "");
    CheckCanonicalName("ABC", "-a-b-c");
}

/**
 * @tc.name:SvgValueFromJerryNumberTest006
 * @tc.desc: Verify number values are formatted as integer, floating and masked color strings.
 */
HWTEST_F(SvgTddTest, SvgValueFromJerryNumber006, TestSize.Level1)
{
    const double integerValue = 42;
    const double floatingValue = 1.5;
    const double redValue = 16711680;    // 0xFF0000
    const double maskedValue = 305419896; // 0x12345678, only the lowest 24 bits are kept
    CheckValueFromJerry(jerry_create_number(integerValue), false, "42");
    CheckValueFromJerry(jerry_create_number(floatingValue), false, "1.5");
    CheckValueFromJerry(jerry_create_number(redValue), true, "#FF0000");
    CheckValueFromJerry(jerry_create_number(maskedValue), true, "#345678");
}

/**
 * @tc.name:SvgValueFromJerryOthersTest007
 * @tc.desc: Verify string and boolean values are converted and unsupported types return null.
 */
HWTEST_F(SvgTddTest, SvgValueFromJerryOthers007, TestSize.Level1)
{
    CheckValueFromJerry(MakeString("url(#grad1)"), false, "url(#grad1)");
    CheckValueFromJerry(MakeString("red"), true, "red");
    CheckValueFromJerry(jerry_create_boolean(true), false, "true");
    CheckValueFromJerry(jerry_create_boolean(false), false, "false");
    CheckValueFromJerry(jerry_create_undefined(), false, nullptr);
    CheckValueFromJerry(jerry_create_null(), false, nullptr);
    CheckValueFromJerry(jerry_create_object(), false, nullptr);
}

/**
 * @tc.name:SvgValueFromStyleSupportedTest008
 * @tc.desc: Verify number and string style items are converted for plain and color properties.
 */
HWTEST_F(SvgTddTest, SvgValueFromStyleSupported008, TestSize.Level1)
{
    const double strokeWidthValue = 3;
    const double greenValue = 65280; // 0x00FF00
    CheckValueFromStyle("strokeWidth", jerry_create_number(strokeWidthValue), false, "3");
    CheckValueFromStyle("fill", jerry_create_number(greenValue), true, "#00FF00");
    CheckValueFromStyle("fill", MakeString("red"), true, "red");
    CheckValueFromStyle("textAnchor", MakeString("middle"), false, "middle");
}

/**
 * @tc.name:SvgValueFromStyleUnsupportedTest009
 * @tc.desc: Verify floating, boolean and percent style items are rejected by ValueFromStyle.
 */
HWTEST_F(SvgTddTest, SvgValueFromStyleUnsupported009, TestSize.Level1)
{
    const double opacityValue = 0.5;
    CheckValueFromStyle("opacity", jerry_create_number(opacityValue), false, nullptr);
    CheckValueFromStyle("visibility", jerry_create_boolean(true), false, nullptr);
    CheckValueFromStyle("strokeWidth", MakeString("50%"), false, nullptr);
}

/**
 * @tc.name:SvgTagToShapeTypeTest010
 * @tc.desc: Verify every shape tag is mapped and non-shape tags fall back to the unknown type.
 */
HWTEST_F(SvgTddTest, SvgTagToShapeType010, TestSize.Level1)
{
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("rect")), SVG_RECT);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("circle")), SVG_CIRCLE);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("ellipse")), SVG_ELLIPSE);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("line")), SVG_LINE);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("polyline")), SVG_POLYLINE);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("polygon")), SVG_POLYGON);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("path")), SVG_PATH);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("g")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgShapeType(KeyParser::ParseKeyId("unknown")), SVG_UNKNOWN);
}

/**
 * @tc.name:SvgTagToStructTypeContainerTest011
 * @tc.desc: Verify container, gradient and animation tags are mapped to their element types.
 */
HWTEST_F(SvgTddTest, SvgTagToStructTypeContainer011, TestSize.Level1)
{
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("g")), SVG_GROUP);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("defs")), SVG_DEFS);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("linearGradient")), SVG_LINEAR_GRADIENT);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("radialGradient")), SVG_RADIAL_GRADIENT);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("stop")), SVG_STOP);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("solidColor")), SVG_SOLID_COLOR);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("animate")), SVG_ANIMATE);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("animateColor")), SVG_ANIMATE_COLOR);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("animateTransform")), SVG_ANIMATE_TRANSFORM);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("animateMotion")), SVG_ANIMATE_MOTION);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("set")), SVG_SET);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("mpath")), SVG_MPATH);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("switch")), SVG_UNKNOWN);
#if (FEATURE_COMPONENT_VIDEO == 1)
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("video")), SVG_UNKNOWN);
#endif // FEATURE_COMPONENT_VIDEO
}

/**
 * @tc.name:SvgTagToStructTypeTextTest012
 * @tc.desc: Verify reference, text and embedded content tags are mapped to their element types.
 */
HWTEST_F(SvgTddTest, SvgTagToStructTypeText012, TestSize.Level1)
{
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("use")), SVG_USE);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("text")), SVG_TEXT);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("textArea")), SVG_TEXT_AREA);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("tspan")), SVG_TSPAN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("image")), SVG_IMAGE);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("a")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("tbreak")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("tref")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("audio")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("animation")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("foreignObject")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("object")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("iframe")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("applet")), SVG_UNKNOWN);
}

/**
 * @tc.name:SvgTagToStructTypeMetaTest013
 * @tc.desc: Verify removed metadata/font/event tags now map to SVG_UNKNOWN, and unknown tags hit the default.
 */
HWTEST_F(SvgTddTest, SvgTagToStructTypeMeta013, TestSize.Level1)
{
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("desc")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("title")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("metadata")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("script")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("font")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("font-face")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("font-face-src")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("font-face-uri")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("glyph")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("hkern")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("missing-glyph")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("handler")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("listener")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("prefetch")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("discard")), SVG_UNKNOWN);
    EXPECT_EQ(TagToSvgStructType(KeyParser::ParseKeyId("rect")), SVG_UNKNOWN);
}

/**
 * @tc.name:SvgTagToTypeTest014
 * @tc.desc: Verify TagToSvgType prefers the shape mapping and falls back to the struct mapping.
 */
HWTEST_F(SvgTddTest, SvgTagToType014, TestSize.Level1)
{
    EXPECT_EQ(TagToSvgType(KeyParser::ParseKeyId("circle")), SVG_CIRCLE);
    EXPECT_EQ(TagToSvgType(KeyParser::ParseKeyId("path")), SVG_PATH);
    EXPECT_EQ(TagToSvgType(KeyParser::ParseKeyId("defs")), SVG_DEFS);
    EXPECT_EQ(TagToSvgType(KeyParser::ParseKeyId("tspan")), SVG_TSPAN);
    EXPECT_EQ(TagToSvgType(KeyParser::ParseKeyId("unknown")), SVG_UNKNOWN);
}

/**
 * @tc.name:SvgDefaultAttrsRenderTest015
 * @tc.desc: Verify an <svg> without attrs renders, and a non-SVG child is not attached.
 */
HWTEST_F(SvgTddTest, SvgDefaultAttrsRender015, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_DEFAULT_ATTRS, strlen(BUNDLE_DEFAULT_ATTRS));
    UIView *boxView = GetViewByRef(page, "box3");
    EXPECT_TRUE(boxView != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgPercentAndZeroSizeTest016
 * @tc.desc: Verify percent and zero sizes skip the host view resize and private attrs are set.
 */
HWTEST_F(SvgTddTest, SvgPercentAndZeroSize016, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_PRIVATE_ATTRS, strlen(BUNDLE_PRIVATE_ATTRS));
    UIView *svgView = GetViewByRef(page, "svg4");
    EXPECT_TRUE(svgView != nullptr);
    UIView *emptySvgView = GetViewByRef(page, "svg4b");
    EXPECT_TRUE(emptySvgView != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgElementStyleCaptureTest017
 * @tc.desc: Verify every supported style is captured, an unsupported one is ignored, and an
 *           attribute value is overwritten by the matching style.
 */
HWTEST_F(SvgTddTest, SvgElementStyleCapture017, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_ELEMENT_STYLE, strlen(BUNDLE_ELEMENT_STYLE));
    UIView *svgView = GetViewByRef(page, "svg5");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgElementWithoutSvgParentTest018
 * @tc.desc: Verify SVG elements without an <svg> ancestor cannot resolve a document and are
 *           skipped instead of crashing.
 */
HWTEST_F(SvgTddTest, SvgElementWithoutSvgParent018, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_ORPHAN_ELEMENT, strlen(BUNDLE_ORPHAN_ELEMENT));
    UIView *boxView = GetViewByRef(page, "box6");
    EXPECT_TRUE(boxView != nullptr);
    // An SVG element component never owns a UIView, so it can not be resolved through $refs.
    UIView *groupView = GetViewByRef(page, "orphanGroup");
    EXPECT_TRUE(groupView == nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgJsAnimationApiTest019
 * @tc.desc: Verify startAnimation/stopAnimation work on the SVG element and safely return when
 *           the bound object holds no component.
 */
HWTEST_F(SvgTddTest, SvgJsAnimationApi019, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_JS_ANIMATION, strlen(BUNDLE_JS_ANIMATION));
    UIView *svgView = GetViewByRef(page, "svg7");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgAttributeUpdateTest020
 * @tc.desc: Verify a bound private attribute change triggers the SVG update path.
 */
HWTEST_F(SvgTddTest, SvgAttributeUpdate020, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_ATTR_UPDATE, strlen(BUNDLE_ATTR_UPDATE));
    UIView *svgView = GetViewByRef(page, "svg8");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
}

// Bundle with a <switch> inside an SVG: switch is reserved and should route to the UI
// Switch component, not to SvgElementComponent.
const char * const BUNDLE_SVG_SWITCH =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box' }\n"
    "            },[_c('svg',{\n"
    "                attrs:{ ref: 'svg', width: '100', height: '100', viewBox: '0 0 10 10' }\n"
    "            },[_c('switch',{attrs:{ref: 'svgSwitch', transform: 'translate(1,1)'}})])\n"
    "            ]);\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

/**
 * @tc.name:SvgSwitchRoutingTest021
 * @tc.desc: Verify <switch> is treated as the UI Switch component (resolvable via $ref),
 *           not as an SVG element, since switch is reserved and not routed to SVG.
 */
HWTEST_F(SvgTddTest, SvgSwitchRoutingTest021, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_SVG_SWITCH, strlen(BUNDLE_SVG_SWITCH));
    UIView *boxView = GetViewByRef(page, "box");
    EXPECT_TRUE(boxView != nullptr);
    UIView *svgView = GetViewByRef(page, "svg");
    EXPECT_TRUE(svgView != nullptr);
    // SvgElementComponent owns no UIView; a non-null ref proves the UI Switch path was taken.
    UIView *svgSwitch = GetViewByRef(page, "svgSwitch");
    EXPECT_TRUE(svgSwitch != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgNullAttrsRenderTest022
 * @tc.desc: Verify an <svg> and a child <rect> with JS null attrs render without triggering
 *           undefined behavior in CaptureAttrs / CreateNativeViews.
 */
HWTEST_F(SvgTddTest, SvgNullAttrsRender022, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_NULL_ATTRS, strlen(BUNDLE_NULL_ATTRS));
    UIView *boxView = GetViewByRef(page, "boxNull");
    EXPECT_TRUE(boxView != nullptr);
    DestroyPage(page);
}

/**
 * @tc.name:SvgCanonicalNameSvgPreserveTest023
 * @tc.desc: Verify CanonicalName preserves SVG camelCase attribute names that the engine expects
 *           verbatim (preserveAspectRatio, gradientUnits, gradientTransform, spreadMethod).
 */
HWTEST_F(SvgTddTest, SvgCanonicalNameSvgPreserve023, TestSize.Level1)
{
    CheckCanonicalName("preserveAspectRatio", "preserveAspectRatio");
    CheckCanonicalName("preserveaspectratio", "preserveAspectRatio");
    CheckCanonicalName("gradientUnits", "gradientUnits");
    CheckCanonicalName("gradientunits", "gradientUnits");
    CheckCanonicalName("gradientTransform", "gradientTransform");
    CheckCanonicalName("gradienttransform", "gradientTransform");
    CheckCanonicalName("spreadMethod", "spreadMethod");
    CheckCanonicalName("spreadmethod", "spreadMethod");
    CheckCanonicalName("clipPathUnits", "clipPathUnits");
    CheckCanonicalName("clippathunits", "clipPathUnits");
    CheckCanonicalName("clipPath", "clipPath");
    CheckCanonicalName("clippath", "clipPath");
    CheckCanonicalName("patternUnits", "patternUnits");
    CheckCanonicalName("patternunits", "patternUnits");
    CheckCanonicalName("patternTransform", "patternTransform");
    CheckCanonicalName("patterntransform", "patternTransform");
    CheckCanonicalName("maskUnits", "maskUnits");
    CheckCanonicalName("maskunits", "maskUnits");
    CheckCanonicalName("maskContentUnits", "maskContentUnits");
    CheckCanonicalName("maskcontentunits", "maskContentUnits");
    CheckCanonicalName("lengthAdjust", "lengthAdjust");
    CheckCanonicalName("lengthadjust", "lengthAdjust");
    CheckCanonicalName("startOffset", "startOffset");
    CheckCanonicalName("startoffset", "startOffset");
    CheckCanonicalName("textLength", "textLength");
    CheckCanonicalName("textlength", "textLength");
    CheckCanonicalName("glyphRef", "glyphRef");
    CheckCanonicalName("glyphref", "glyphRef");
    CheckCanonicalName("baseFrequency", "baseFrequency");
    CheckCanonicalName("basefrequency", "baseFrequency");
    CheckCanonicalName("stdDeviation", "stdDeviation");
    CheckCanonicalName("stddeviation", "stdDeviation");
    CheckCanonicalName("filterUnits", "filterUnits");
    CheckCanonicalName("filterunits", "filterUnits");
    CheckCanonicalName("primitiveUnits", "primitiveUnits");
    CheckCanonicalName("primitiveunits", "primitiveUnits");
}

/**
 * @tc.name:SvgImageRoutingTest024
 * @tc.desc: Verify an SVG <image> with href still routes to SvgElementComponent (no UIView, not
 *           resolvable via $ref) after the image disambiguation attribute set was narrowed to
 *           drop x/y. The UI-side benefit (a UI <image> carrying only x/y is no longer
 *           misclassified as SVG) is covered by code review of IsSvgImageElement; UI ImageComponent
 *           is not exercisable through $ref in this headless harness.
 */
HWTEST_F(SvgTddTest, SvgImageRoutingTest024, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_IMAGE_ROUTING, strlen(BUNDLE_IMAGE_ROUTING));
    UIView *boxView = GetViewByRef(page, "boxImg");
    EXPECT_TRUE(boxView != nullptr);
    UIView *svgImage = GetViewByRef(page, "svgImage");
    EXPECT_TRUE(svgImage == nullptr);
    DestroyPage(page);
}

// Bundle exercising the remaining SVG animation JS API: pauseAnimations / unpauseAnimations.
const char * const BUNDLE_PAUSE_ANIMATION =
    "(function () {\n"
    "    return new ViewModel({\n"
    "        render: function render(vm) {\n"
    "            var _vm = vm || this;\n"
    "            return _c('div', {\n"
    "                staticClass: ['container'],\n"
    "                attrs: { ref: 'box9' }\n"
    "            },[_c('svg',{\n"
    "                staticStyle:{width: '120px', height: '120px'},\n"
    "                attrs:{ ref: 'svg9' }\n"
    "            },[_c('rect',{attrs:{x:'0',y:'0',width:'10',height:'10'}},[\n"
    "                _c('animate',{attrs:{"
    "attributeName:'x',from:'0',to:'10',dur:'1s',repeatCount:'indefinite'}})\n"
    "            ])])]);\n"
    "        },\n"
    "        onShow: function () {\n"
    "            var el = this.$refs.svg9;\n"
    "            if (!el) { return; }\n"
    "            if (el.pauseAnimations) { el.pauseAnimations(); }\n"
    "            if (el.unpauseAnimations) { el.unpauseAnimations(); }\n"
    "            var pause = el.pauseAnimations;\n"
    "            var unpause = el.unpauseAnimations;\n"
    "            if (pause) { pause.call({}); }\n"
    "            if (unpause) { unpause.call({}); }\n"
    "        },\n"
    "        styleSheet: {\n"
    "            classSelectors:{\n"
    "                container: { width: '454px', height: '454px' }\n"
    "            }\n"
    "        }\n"
    "    });\n"
    "})();\n";

/**
 * @tc.name:SvgJsPauseUnpauseApiTest025
 * @tc.desc: Verify pauseAnimations/unpauseAnimations are reachable from JS and safely return
 *           when the bound object holds no component.
 */
HWTEST_F(SvgTddTest, SvgJsPauseUnpauseApi025, TestSize.Level1)
{
    JSValue page = CreatePage(BUNDLE_PAUSE_ANIMATION, strlen(BUNDLE_PAUSE_ANIMATION));
    UIView *svgView = GetViewByRef(page, "svg9");
    EXPECT_TRUE(svgView != nullptr);
    DestroyPage(page);
}
#endif // FEATURE_COMPONENT_SVG
#endif
} // namespace ACELite
} // namespace OHOS
