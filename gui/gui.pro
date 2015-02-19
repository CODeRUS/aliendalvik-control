TARGET = powermenu2-gui
target.path = /usr/bin

QT += dbus

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

desktops.files = powermenu2.desktop
desktops.path = /usr/share/applications

icons.files = powermenu2.png
icons.path = /usr/share/icons/hicolor/86x86/apps

qmls.files = qmls/*
qmls.path = /usr/share/powermenu2/qml

images.files = images
images.path = /usr/share/powermenu2

translations.files = translations/powermenu_en.qm
translations.path = /usr/share/powermenu2/translations

INSTALLS += target desktops icons qmls images translations

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/shortcutshelper.cpp

HEADERS += \
    src/shortcutshelper.h

OTHER_FILES += \
    qmls/main.qml \
    qmls/pages/AboutPage.qml \
    qmls/pages/ConfigurationPage.qml \
    qmls/pages/MainPage.qml \
    qmls/pages/ShortcutsPage.qml \
    qmls/cover/CoverPage.qml \
    qmls/ShaderTiledBackground.qml
