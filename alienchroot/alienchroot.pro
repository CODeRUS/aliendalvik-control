TEMPLATE = lib

QT = core dbus network
CONFIG += plugin

HEADERS += \
    ../common/src/alienabstract.h \
    ../common/src/aliendalvikcontroller.h \
    ../common/src/loggingclasswrapper.h \
    ../common/src/systemdcontroller.h \
    src/alienchroot.h

SOURCES += \
    ../common/src/alienabstract.cpp \
    ../common/src/aliendalvikcontroller.cpp \
    ../common/src/loggingclasswrapper.cpp \
    ../common/src/systemdcontroller.cpp \
    src/alienchroot.cpp

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

EXTRA_CFLAGS=-W -Wall -Wextra -Wpedantic -Werror
QMAKE_CXXFLAGS += $$EXTRA_CFLAGS
QMAKE_CFLAGS += $$EXTRA_CFLAGS

TARGET = aliendalvikcontrolplugin-chroot
target.path = /usr/lib

INSTALLS = target
