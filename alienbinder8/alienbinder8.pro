TEMPLATE = lib

QT = core dbus network
CONFIG += plugin
CONFIG += c++11
CONFIG += link_pkgconfig
PKGCONFIG += libgbinder

HEADERS += \
    ../common/src/alienabstract.h \
    ../common/src/aliendalvikcontroller.h \
    ../common/src/loggingclasswrapper.h \
    ../common/src/systemdcontroller.h \
    src/alienbinder8.h \
    src/binderinterfaceabstract.h \
    src/activitymanager.h \
    src/packagemanager.h \
    src/intent.h \
    src/resolveinfo.h \
    src/activityinfo.h \
    src/componentinfo.h \
    src/packageiteminfo.h \
    src/applicationinfo.h \
    src/windowlayout.h \
    src/intentfilter.h \
    src/parcelable.h \
    src/parcel.h \
    src/appopsservice.h \
    src/intentsender.h \
    src/alienservice.h \
    src/binderlocalobject.h \
    src/binderlocalservice.h \
    src/windowmanager.h \
    src/inputmanager.h \
    src/bitset.h \
    src/input.h \
    src/errors.h

SOURCES += \
    ../common/src/alienabstract.cpp \
    ../common/src/aliendalvikcontroller.cpp \
    ../common/src/loggingclasswrapper.cpp \
    ../common/src/systemdcontroller.cpp \
    src/alienbinder8.cpp \
    src/binderinterfaceabstract.cpp \
    src/activitymanager.cpp \
    src/packagemanager.cpp \
    src/intent.cpp \
    src/resolveinfo.cpp \
    src/activityinfo.cpp \
    src/componentinfo.cpp \
    src/packageiteminfo.cpp \
    src/applicationinfo.cpp \
    src/windowlayout.cpp \
    src/intentfilter.cpp \
    src/parcelable.cpp \
    src/parcel.cpp \
    src/appopsservice.cpp \
    src/intentsender.cpp \
    src/alienservice.cpp \
    src/binderlocalobject.cpp \
    src/binderlocalservice.cpp \
    src/windowmanager.cpp \
    src/inputmanager.cpp \
    src/input.cpp


DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

EXTRA_CFLAGS=-W -Wall -Wextra -Wpedantic -Werror
QMAKE_CXXFLAGS += $$EXTRA_CFLAGS
QMAKE_CFLAGS += $$EXTRA_CFLAGS

TARGET = aliendalvikcontrolplugin-binder8
target.path = /usr/lib

INSTALLS = target
