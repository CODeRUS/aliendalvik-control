TARGET = aliendalvik-gui
target.path = /usr/bin

INSTALLS += target

CONFIG += sailfishapp

SOURCES += \
    src/main.cpp

OTHER_FILES += \
    qml/main.qml \
    qml/pages/MainPage.qml \
    qml/pages/AboutPage.qml \
    qml/cover/CoverPage.qml
