TEMPLATE = lib
QT += quick
QT -= gui
CONFIG += qt plugin link_pkgconfig
PKGCONFIG += mlite5

TARGET = $$qtLibraryTarget(desktopfilemodel)
target.path = /usr/lib/qt5/qml/org/coderus/desktopfilemodel

qmldir.files = qmldir
qmldir.path = /usr/lib/qt5/qml/org/coderus/desktopfilemodel

INSTALLS += target qmldir

HEADERS += \
    src/desktopfilemodelplugin.h \
    src/desktopfilemodel.h \
    src/desktopfilesortmodel.h

SOURCES += \
    src/desktopfilemodelplugin.cpp \
    src/desktopfilemodel.cpp \
    src/desktopfilesortmodel.cpp
