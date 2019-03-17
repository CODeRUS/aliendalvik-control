TARGET = aliendalvik-control
target.path = /usr/bin

INSTALLS += target

QT += dbus network
CONFIG += link_pkgconfig
PKGCONFIG += libgbinder

SOURCES += \
    src/main.cpp \
    src/mimehandleradaptor.cpp \
    src/inotifywatcher.cpp \
    src/binderinterfaceabstract.cpp \
    src/activitymanager.cpp \
    src/packagemanager.cpp \
    src/intent.cpp \
    src/resolveinfo.cpp \
    src/activityinfo.cpp \
    src/componentinfo.cpp \
    src/packageiteminfo.cpp \
    src/applicationinfo.cpp \
    src/windowlayout.cpp \
    src/intentfilter.cpp \
    src/parcelable.cpp \
    src/parcel.cpp \
    src/loggingclasswrapper.cpp \
    src/appopsservice.cpp \
    src/intentsender.cpp \
    src/alienservice.cpp \
    src/binderlocalobject.cpp \
    src/binderlocalservice.cpp

HEADERS += \
    src/mimehandleradaptor.h \
    src/inotifywatcher.h \
    src/binderinterfaceabstract.h \
    src/activitymanager.h \
    src/packagemanager.h \
    src/intent.h \
    src/resolveinfo.h \
    src/activityinfo.h \
    src/componentinfo.h \
    src/packageiteminfo.h \
    src/applicationinfo.h \
    src/windowlayout.h \
    src/intentfilter.h \
    src/parcelable.h \
    src/parcel.h \
    src/loggingclasswrapper.h \
    src/appopsservice.h \
    src/intentsender.h \
    src/alienservice.h \
    src/binderlocalobject.h \
    src/binderlocalservice.h

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

systemd.files = systemd/aliendalvik-control.service
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

settingspng.files = \
    settings/icon-m-aliendalvik-back.png
settingspng.path = /usr/share/jolla-settings/pages/aliendalvikcontrol

INSTALLS += settingspng

ad_dbus_adaptor.files = ../dbus/org.coderus.aliendalvikcontrol.xml
ad_dbus_adaptor.source_flags = -c DBusAdaptor
ad_dbus_adaptor.header_flags = -c DBusAdaptor
DBUS_ADAPTORS += ad_dbus_adaptor

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/gbinder
