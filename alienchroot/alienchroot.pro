TEMPLATE = lib

QT = core dbus network
CONFIG += plugin

HEADERS += \
    ../abstract/src/alienabstract.h \
    src/alienchroot.h

SOURCES += \
    ../abstract/src/alienabstract.cpp \
    src/alienchroot.cpp

TARGET = aliendalvikcontrolplugin-chroot
target.path = /usr/lib

INSTALLS = target
