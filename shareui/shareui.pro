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

qml.files = qml/AliendalvikShare.qml
qml.path = /usr/share/nemo-transferengine/plugins

INSTALLS += target qml
