TARGET = aliendalvik-control
target.path = /usr/bin

INSTALLS += target

QT += dbus network

DEFINES += RPM_VERSION=\\\"$$RPM_VERSION\\\"
DEFINES += HELPER_VERSION=\\\"$$HELPER_VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/dbusservice.cpp \
    src/inotifywatcher.cpp \
    ../common/src/aliendalvikcontroller.cpp \
    ../common/src/systemdcontroller.cpp

HEADERS += \
    src/dbusservice.h \
    src/inotifywatcher.h \
    ../common/src/aliendalvikcontroller.h \
    ../common/src/systemdcontroller.h

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

EXTRA_CFLAGS=-W -Wall -Wextra -Wpedantic -Werror
QMAKE_CXXFLAGS += $$EXTRA_CFLAGS
QMAKE_CFLAGS += $$EXTRA_CFLAGS

dbus.files = dbus/org.coderus.aliendalvikcontrol.service
dbus.path = /usr/share/dbus-1/system-services/

INSTALLS += dbus

dbusConf.files = dbus/org.coderus.aliendalvikcontrol.conf
dbusConf.path = /etc/dbus-1/system.d/

INSTALLS += dbusConf

desktop.files = \
    desktop/android-open-url.desktop \
    desktop/android-open-url-selector.desktop
desktop.path = /usr/share/applications

INSTALLS += desktop

systemd.files = \
    systemd/aliendalvik-control.service
systemd.path = /lib/systemd/system/

INSTALLS += systemd

settingsjson.files = settings/aliendalvikcontrol.json
settingsjson.path = /usr/share/jolla-settings/entries

INSTALLS += settingsjson

settingsqml.files = \
    settings/main.qml \
    settings/NavbarToggle.qml
settingsqml.path = /usr/share/jolla-settings/pages/aliendalvikcontrol

INSTALLS += settingsqml

apk.files = apk/app-release.apk
apk.path = /usr/share/aliendalvik-control/apk

INSTALLS += apk

env.files = environment/10-debug.conf
env.path = /var/lib/environment/aliendalvik-control

INSTALLS += env

ad_dbus_adaptor.files = ../dbus/org.coderus.aliendalvikcontrol.xml
ad_dbus_adaptor.source_flags = -c DBusAdaptor
ad_dbus_adaptor.header_flags = -c DBusAdaptor
DBUS_ADAPTORS += ad_dbus_adaptor
