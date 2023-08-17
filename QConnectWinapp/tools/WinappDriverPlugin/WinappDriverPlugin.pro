QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += WINAPPDRIVERPLUGIN_LIBRARY

win32 {
    # Configuration for Windows
    !isEmpty(CONFIG):CONFIG(debug, debug|release) {
        LIBS += -L..\vcpkg\packages\jsoncpp_x64-windows\debug\lib \
                -ljsoncpp \
                -L..\vcpkg\packages\cpprestsdk_x64-windows\debug\lib \
                -lcpprest_2_10d \
                -lws2_32
    } else {
        LIBS += -LD:\Project\Out\vcpkg\packages\jsoncpp_x64-windows\lib \
                -ljsoncpp \
                -LD:\Project\Out\vcpkg\packages\cpprestsdk_x64-windows\lib \
                -lcpprest_2_10 \
                -lws2_32
    }

    INCLUDEPATH += D:\Project\Out\vcpkg\packages\jsoncpp_x64-windows\include \
                   D:\Project\Out\vcpkg\packages\cpprestsdk_x64-windows\include \

    DEFINES += BUILD_ON_WINDOWS_X64
}

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../common/AutoObject.cpp \
    winappdriverclient.cpp \
    winappdriverplugin.cpp

HEADERS += \
    ../common/AutoObject.h \
    ../common/constants.h \
    ../common/plugininterface.h \
    ../common/rapidxml.hpp \
    WinappDriverPlugin_global.h \
    winappdriverclient.h \
    winappdriverplugin.h

# Default rules for deployment.
win32 {
    message("Win32 scope executed")

    !isEmpty(CONFIG):CONFIG(debug, debug|release) {
        target.path = $$PWD/../bin/debug/Plugins/
        QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) $$shell_path($$target.path) (mkdir $$shell_path($$target.path))) &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path($$OUT_PWD/debug/WinappDriverPlugin.dll) $$shell_path($$target.path)
    } else {
        target.path = $$PWD/../bin/release/Plugins/
        QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) $$shell_path($$target.path) (mkdir $$shell_path($$target.path))) &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path($$OUT_PWD/release/WinappDriverPlugin.dll) $$shell_path($$target.path)
    }
}
!isEmpty(target.path): INSTALLS += target
