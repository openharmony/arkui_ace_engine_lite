TEMPLATE = app
CONFIG += console c++11
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES -= UNICODE
DEFINES += QT_COMPILER
DEFINES += Q_OBJECT
DEFINES += QT_DEPRECATED_WARNINGS \
"ENABLE_ICU=1" \
"ENABLE_VECTOR_FONT=1"
DESTDIR = ../libs

ROOT_PATH = ../../../../../../../..
FOUNDATION_PATH = ../../../../../../..
ACELITE_PATH = ../../../../..
ACELITE_FRAMEWORK_PATH = ../../../..
UIDESTDIR = $${ROOT_PATH}/foundation/arkui/ui_lite/tools/qt/simulator

INCLUDEPATH += \
    $${ACELITE_PATH}/interfaces/inner_api/builtin/async \
    $${ACELITE_PATH}/interfaces/inner_api/builtin/base \
    $${ACELITE_PATH}/interfaces/inner_api/builtin/jsi \
    $${ACELITE_FRAMEWORK_PATH}/include/base \
    $${FOUNDATION_PATH}/graphic/graphic_utils_lite/interfaces/innerkits \
    $${FOUNDATION_PATH}/graphic/graphic_utils_lite/interfaces/kits \
    $${FOUNDATION_PATH}/../third_party/freetype/include \
    $${FOUNDATION_PATH}/arkui/ui_lite/interfaces/innerkits \
    $${FOUNDATION_PATH}/arkui/ui_lite/frameworks \
    $${FOUNDATION_PATH}/arkui/ui_lite/test/framework \
    $${FOUNDATION_PATH}/arkui/ui_lite/interfaces/kits \
    $${FOUNDATION_PATH}/arkui/ui_lite/interfaces/innerkits/common \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/uitest \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev \
    $${FOUNDATION_PATH}/ability/ability_lite/interfaces/kits/ability_lite/slite \
    $${ACELITE_FRAMEWORK_PATH}/targets/simulator \
    $${ACELITE_FRAMEWORK_PATH}/include/context/ \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/utils \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/jsthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/vsyncthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/amsthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/timerthread \
    $${FOUNDATION_PATH}/../commonlibrary/utils_lite/timer_task/include/ \
    $${FOUNDATION_PATH}/../third_party/bounds_checking_function/include \
    $${FOUNDATION_PATH}/../third_party/bounds_checking_function/src \
    $${FOUNDATION_PATH}/arkui/ui_lite/test/autotest/include \
    $${FOUNDATION_PATH}/arkui/ui_lite/test/autotest/src

HEADERS += \
    $${FOUNDATION_PATH}/arkui/ui_lite/interfaces/innerkits/font/ui_font_vector.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/config.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/monitor.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/task_thread.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/gui_thread.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev/mouse_input.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev/mousewheel_input.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/uitest/main_widget.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/uitest/ui_mainwidget.h \
    $${ACELITE_FRAMEWORK_PATH}/include/context/js_ability.h \
    $${FOUNDATION_PATH}/arkui/ui_lite/test/framework/common/ui_text_language.h \
    child_widget.h \
    simulator_config.h

SOURCES += \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/gui_thread.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/monitor.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/display/task_thread.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev/mouse_input.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev/mousewheel_input.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/test/framework/common/ui_text_language.cpp \
    $${FOUNDATION_PATH}/arkui/ui_lite/tools/qt/simulator/drivers/indev/key_input.cpp \
    main_widget.cpp \
    child_widget.cpp \
    main.cpp \
    simulator_config.cpp

LIBS += $$DESTDIR/jsfwk.dll
LIBS += $$DESTDIR/jerryScript.dll
LIBS += $$UIDESTDIR/libs/libui.dll
LIBS += $$UIDESTDIR/libs/test.dll
LIBS += $$UIDESTDIR/libs/auto_test.dll

COPY_DEST = $$replace(OUT_PWD, /, \\)
UILIBS = $$FOUNDATION_PATH/arkui/ui_lite/tools/qt/simulator/libs
UI_LIBS = $$replace(UILIBS, /, \\)
QMAKE_POST_LINK += copy $$UI_LIBS\\freetype.dll $$COPY_DEST\\..\\libs\\freetype.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\icu.dll $$COPY_DEST\\..\\libs\\icu.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libjpeg-turbo.dll $$COPY_DEST\\..\\libs\\libjpeg-turbo.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libpng.dll $$COPY_DEST\\..\\libs\\libpng.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libui.dll $$COPY_DEST\\..\\libs\\libui.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\giflib.dll $$COPY_DEST\\..\\libs\\giflib.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\qrcodegen.dll $$COPY_DEST\\..\\libs\\qrcodegen.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\zlib.dll $$COPY_DEST\\..\\libs\\zlib.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\test.dll $$COPY_DEST\\..\\libs\\test.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\auto_test.dll $$COPY_DEST\\..\\libs\\auto_test.dll
