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
    desktop/android-open-url.desktop \
    desktop/android-open-url-selector.desktop
desktop.path = /usr/share/applications

INSTALLS += desktop
