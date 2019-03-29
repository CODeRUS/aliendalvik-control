TEMPLATE = subdirs
SUBDIRS = \
    proxy \
    alienchroot \
    alienbinder8 \
    daemon \
    selector \
    share \
    shareui \
    icons

daemon.depends = \
    alienchroot \
    alienbinder8

OTHER_FILES += \
    rpm/aliendalvik-control.spec \
