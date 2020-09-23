TARGET = aliendalvik-control-selector
target.path = /usr/bin

INSTALLS += target

QT += dbus gui-private
CONFIG += sailfishapp c++11

SOURCES += \
    src/main.cpp

dbus.files = dbus/org.coderus.aliendalvikselector.service
dbus.path = /usr/share/dbus-1/services/

INSTALLS += dbus
