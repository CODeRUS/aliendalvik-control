TEMPLATE = lib

TARGET = powermenutools
target.path = /usr/lib

QT += quick qml gui-private dbus
CONFIG += plugin link_pkgconfig
PKGCONFIG += mlite5

INSTALLS += target

DEFINES += POWERMENUTOOLS_LIBRARY

HEADERS += \
    src/screenshotcontrol.h \
    src/flashlightcontrol.h

SOURCES += \
    src/screenshotcontrol.cpp \
    src/flashlightcontrol.cpp
