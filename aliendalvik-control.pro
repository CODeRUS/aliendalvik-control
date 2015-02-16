TARGET = aliendalvik-control

QT += dbus

SOURCES += \
    src/main.cpp \
    src/dbusmain.cpp \
    src/adaptor.cpp

HEADERS += \
    src/dbusmain.h \
    src/adaptor.h

OTHER_FILES += \
    rpm/aliendalvik-control.spec \
