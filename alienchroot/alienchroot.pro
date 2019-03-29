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

TARGET = aliendalvikcontrolplugin-chroot
target.path = /usr/lib

INSTALLS = target
