TARGET = powermenu2-daemon
target.path = /usr/bin

QT += gui-private dbus

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

dbus.files = dbus/org.coderus.powermenu.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/powermenu.service
systemd.path = /usr/lib/systemd/user

qmls.files = \
    qmls/dialog.qml \
    qmls/SystemWindow.qml \
    qmls/SystemDialogButton.qml
qmls.path = /usr/share/powermenu2/qml

INSTALLS += target systemd dbus qmls

SOURCES += \
    src/dbuslistener.cpp \
    src/main.cpp

HEADERS += \
    src/dbuslistener.h
