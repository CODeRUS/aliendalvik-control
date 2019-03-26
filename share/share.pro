TARGET = aliendalvik-control-share
target.path = /usr/bin

INSTALLS += target

QT += dbus
CONFIG += sailfishapp c++11

dbus.files = dbus/org.coderus.aliendalvikshare.service
dbus.path = /usr/share/dbus-1/services/

INSTALLS += dbus

SOURCES += \
    src/main.cpp
