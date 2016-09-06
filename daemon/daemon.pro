TARGET = aliendalvik-control
target.path = /usr/bin

INSTALLS += target

QT += dbus

SOURCES += \
    src/main.cpp \
    src/dbusmain.cpp \
    src/mimehandleradaptor.cpp

HEADERS += \
    src/dbusmain.h \
    src/mimehandleradaptor.h

dbus.files = dbus/org.coderus.aliendalvikcontrol.service
dbus.path = /usr/share/dbus-1/services

INSTALLS += dbus

desktop.files = \
#    desktop/aliendalvik-downloads.desktop \
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
    settings/icon-m-aliendalvikcontrol.png \
    settings/icon-m-aliendalvik-back.png
settingspng.path = /usr/share/jolla-settings/pages/aliendalvikcontrol

INSTALLS += settingspng

#icons.files = hicolor
#icons.path = /usr/share/icons
#INSTALLS += icons
