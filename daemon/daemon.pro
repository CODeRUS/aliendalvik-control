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

LIBS += -L ../libpowermenutools -lpowermenutools

INSTALLS = target systemd dbus qmls quickactions

SOURCES += \
    src/dbuslistener.cpp \
    src/main.cpp \
    src/screenshotanimation.cpp

HEADERS += \
    src/dbuslistener.h \
    src/screenshotanimation.h

DISTFILES += \
    qmls/dialog.qml \
    qmls/MainWindow.qml \
    qmls/screenshot.qml
