TARGET = powermenu2-daemon
target.path = /usr/bin

QT += gui-private dbus

CONFIG += sailfishapp
PKGCONFIG += mlite5

dbus.files = dbus/org.coderus.powermenu.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/powermenu.service
systemd.path = /usr/lib/systemd/user

qmls.files = qmls/*
qmls.path = /usr/share/powermenu2/qml

quickactions.files = quickactions/org.coderus.powermenu.conf
quickactions.path = /usr/share/lipstick/quickactions

INSTALLS = target systemd dbus qmls quickactions

SOURCES += \
    src/dbuslistener.cpp \
    src/main.cpp \
    src/components/flashlightcontrol.cpp \
    src/components/togglesmodel.cpp \
    src/components/fileutils.cpp

HEADERS += \
    src/dbuslistener.h \
    src/components/flashlightcontrol.h \
    src/components/togglesmodel.h \
    src/components/fileutils.h

DISTFILES += \
    qmls/dialog.qml \
    qmls/components/MainWindow.qml \
    qmls/components/BackgroundIconButton.qml \
    qmls/components/IconItem.qml \
    qmls/components/ToggleItem.qml \
    qmls/toggles/FlashlightToggle.qml \
    qmls/toggles/PowersaveToggle.qml \
    qmls/toggles/FlightToggle.qml \
    qmls/toggles/MobilenetworkToggle.qml \
    qmls/toggles/WifinetworkToggle.qml \
    qmls/toggles/BluetoothToggle.qml \
    qmls/toggles/OrientationlockToggle.qml \
    qmls/toggles/LocationToggle.qml \
    qmls/toggles/TetheringToggle.qml \
    qmls/toggles/BrightnessToggle.qml \
    qmls/components/ApplicationItem.qml \
    qmls/toggles/RadiomodeToggle.qml \
    qmls/toggles/VibrationToggle.qml \
    qmls/toggles/PresenceToggle.qml
