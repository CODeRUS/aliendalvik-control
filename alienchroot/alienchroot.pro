TEMPLATE = lib

QT = core dbus network
CONFIG += plugin

HEADERS += \
    ../common/src/alienabstract.h \
    src/alienchroot.h

SOURCES += \
    ../common/src/alienabstract.cpp \
    src/alienchroot.cpp

TARGET = aliendalvikcontrolplugin-chroot
target.path = /usr/lib

INSTALLS = target
