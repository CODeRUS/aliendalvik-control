TEMPLATE = lib

TARGET = $$qtLibraryTarget(aliendalvikshareplugin)
target.path = /usr/lib/nemo-transferengine/plugins

QT += dbus
CONFIG += plugin link_pkgconfig
PKGCONFIG += nemotransferengine-qt5

HEADERS += \
    src/transferiface.h \
    src/plugininfo.h \
    src/mediatransfer.h

SOURCES += \
    src/transferiface.cpp \
    src/plugininfo.cpp \
    src/mediatransfer.cpp

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

EXTRA_CFLAGS=-W -Wall -Wextra -Wpedantic -Werror
QMAKE_CXXFLAGS += $$EXTRA_CFLAGS
QMAKE_CFLAGS += $$EXTRA_CFLAGS

qml.files = qml/AliendalvikShare.qml
qml.path = /usr/share/nemo-transferengine/plugins

INSTALLS += target qml
