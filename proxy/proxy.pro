TARGET = aliendalvik-control-proxy
target.path = /usr/bin

INSTALLS += target

QT += dbus

SOURCES += \
    src/main.cpp \
    src/adaptor.cpp \
    src/service.cpp

HEADERS += \
    src/adaptor.h \
    src/service.h

dbus.files = dbus/org.coderus.aliendalvikcontrol.service
dbus.path = /usr/share/dbus-1/services/

INSTALLS += dbus

ad_dbus_interface.files = ../dbus/org.coderus.aliendalvikcontrol.xml
ad_dbus_interface.source_flags = -c DBusInterface
ad_dbus_interface.header_flags = -c DBusInterface
DBUS_INTERFACES += ad_dbus_interface
