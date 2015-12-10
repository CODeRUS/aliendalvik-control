TEMPLATE = lib
QT += quick gui-private dbus
CONFIG += qt plugin link_pkgconfig
PKGCONFIG += mlite5

TARGET = $$qtLibraryTarget(powermenu)
target.path = /usr/lib/qt5/qml/org/coderus/powermenu

qml.files = qml/*
qml.path = /usr/lib/qt5/qml/org/coderus/powermenu

LIBS += -L ../libpowermenutools -lpowermenutools

INSTALLS += target qml

SOURCES += \
    src/togglesmodel.cpp \
    src/fileutils.cpp \
    src/powermenuplugin.cpp \
    src/translator.cpp

HEADERS += \
    src/togglesmodel.h \
    src/fileutils.h \
    src/powermenuplugin.h \
    src/translator.h

OTHER_FILES += \
    qml/qmldir \
    qml/BackgroundIconButton.qml \
    qml/IconItem.qml \
    qml/ToggleItem.qml \
    qml/ApplicationItem.qml \
    qml/ControlRow.qml \
    qml/TogglesArea.qml \
    qml/toggles/BluetoothToggle.qml \
    qml/toggles/BrightnessToggle.qml \
    qml/toggles/FlashlightToggle.qml \
    qml/toggles/FlightToggle.qml \
    qml/toggles/LocationToggle.qml \
    qml/toggles/MobilenetworkToggle.qml \
    qml/toggles/OrientationlockToggle.qml \
    qml/toggles/PowersaveToggle.qml \
    qml/toggles/PresenceToggle.qml \
    qml/toggles/RadiomodeToggle.qml \
    qml/toggles/ScreenshotToggle.qml \
    qml/toggles/TetheringToggle.qml \
    qml/toggles/VibrationToggle.qml \
    qml/toggles/WifinetworkToggle.qml

