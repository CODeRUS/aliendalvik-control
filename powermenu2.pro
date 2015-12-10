TEMPLATE = subdirs
SUBDIRS = \
    libpowermenutools \
    plugin \
    daemon \
    gui \
    $${NULL}

plugin.depends = libpowermenutools
daemon.depends = libpowermenutools plugin
gui.depends = daemon

OTHER_FILES = \
    rpm/powermenu2.spec \
    $${NULL}
