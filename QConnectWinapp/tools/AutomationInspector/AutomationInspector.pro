QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Check for the target platform
win32 {
    # Configuration for Windows
    debug {
        LIBS += -L..\vcpkg\packages\jsoncpp_x64-windows\debug\lib \
                -ljsoncpp \
                -lGdi32 \
                -luser32
    }

    release {
        LIBS += -L..\vcpkg\packages\jsoncpp_x64-windows\lib \
                -ljsoncpp \
                -lGdi32 \
                -luser32
    }

    INCLUDEPATH += ..\vcpkg\packages\jsoncpp_x64-windows\include \

    DEFINES += BUILD_ON_WINDOWS_X64
}

unix {
    # Configuration for Linux
    LIBS += -lfltk -ljsoncpp -lX11 -ldl
    DEFINES += BUILD_ON_LINUX
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../common/AutoObject.cpp \
    ../common/utilities.cpp \
    adddefinitiondialog.cpp \
    automationinspector.cpp \
    elementtreemodel.cpp \
    main.cpp \
    pluginmanager.cpp \
    viewerautoobject.cpp

HEADERS += \
    ../common/AutoObject.h \
    ../common/MessageType.h \
    ../common/constants.h \
    ../common/plugininterface.h \
    ../common/utilities.h \
    adddefinitiondialog.h \
    automationinspector.h \
    elementtreemodel.h \
    pluginmanager.h \
    viewerautoobject.h

FORMS += \
    adddefinitiondialog.ui \
    automationinspector.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
unix {
    message("Unix scope executed")
    target.path = $$PWD/../bin/
    QMAKE_POST_LINK += mkdir -p $$shell_quote($$target.path) &&
    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$OUT_PWD/AutomationInspector $$target.path/AutomationInspector
}

win32 {
    message("Win32 scope executed")

    !isEmpty(CONFIG):CONFIG(debug, debug|release) {
        target.path = $$PWD/../bin/debug/
        QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) $$shell_path($$target.path) (mkdir $$shell_path($$target.path))) &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path($$OUT_PWD/debug/AutomationInspector.exe) $$shell_path($$target.path)  &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path(..\vcpkg\packages\jsoncpp_x64-windows\debug\bin\jsoncpp.dll) $$shell_path($$target.path)  &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path(..\vcpkg\packages\cpprestsdk_x64-windows\debug\bin\cpprest_2_10d.dll) $$shell_path($$target.path)
    } else {
        target.path = $$PWD/../bin/release/
        QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) $$shell_path($$target.path) (mkdir $$shell_path($$target.path))) &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path($$OUT_PWD/release/AutomationInspector.exe) $$shell_path($$target.path) &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path(..\vcpkg\packages\jsoncpp_x64-windows\bin\jsoncpp.dll) $$shell_path($$target.path)  &&
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path(..\vcpkg\packages\cpprestsdk_x64-windows\bin\cpprest_2_10.dll) $$shell_path($$target.path)
    }
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
