TEMPLATE = subdirs
SUBDIRS = \
    proxy \
    alienchroot \
    daemon \
    selector \
    share \
    shareui \
    icons

daemon.depends = alienchroot

OTHER_FILES += \
    rpm/aliendalvik-control.spec \
