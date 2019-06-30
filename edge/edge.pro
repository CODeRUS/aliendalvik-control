TARGET = aliendalvik-control-edge
target.path = /usr/bin

INSTALLS += target

QT += dbus gui-private
CONFIG += sailfishapp c++11

SOURCES += \
    src/main.cpp \
    src/nativewindowhelper.cpp

systemd.files = systemd/aliendalvik-control-edge.service
systemd.path = /usr/lib/systemd/user

INSTALLS += systemd

INCLUDEPATH += /usr/include

HEADERS += \
    src/nativewindowhelper.h
