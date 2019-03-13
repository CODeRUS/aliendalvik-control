TARGET = aliendalvik-control
target.path = /usr/bin

INSTALLS += target

QT += dbus
CONFIG += link_pkgconfig
PKGCONFIG += libgbinder

SOURCES += \
    src/main.cpp \
    src/dbusmain.cpp \
    src/inotifywatcher.cpp \
    src/mimehandleradaptor.cpp \
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
    src/alienservice.cpp

HEADERS += \
    src/dbusmain.h \
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
    src/alienservice.h

dbus.files = dbus/org.coderus.aliendalvikcontrol.service
dbus.path = /usr/share/dbus-1/services

INSTALLS += dbus

desktop.files = \
    desktop/android-open-url.desktop \
    desktop/android-open-url-selector.desktop
desktop.path = /usr/share/applications

INSTALLS += desktop

systemd.files = systemd/aliendalvik-control.service
systemd.path = /usr/lib/systemd/user

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

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/gbinder
