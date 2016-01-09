import QtQuick 2.1
import Sailfish.Silica 1.0

import "../components"

Page {
    id: page

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
            }
        }

        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Folder player")
            }

            EntryItem {
                text: qsTr("Intternal Memory")
                onClicked: {
                    pageStack.pushAttached(Qt.resolvedUrl("FilesystemPage.qml"), {"path": "/home/nemo/Music"})
                    pageStack.navigateForward(PageStackAction.Animated)
                }
            }

            EntryItem {
                text: qsTr("External Memory")
                onClicked: {
                    pageStack.pushAttached(Qt.resolvedUrl("FilesystemPage.qml"), {"path": "/media/sdcard"})
                    pageStack.navigateForward(PageStackAction.Animated)
                }
            }

            EntryItem {
                text: qsTr("All filesystem")
                onClicked: {
                    pageStack.pushAttached(Qt.resolvedUrl("FilesystemPage.qml"), {"path": "/"})
                    pageStack.navigateForward(PageStackAction.Animated)
                }
            }
        }
    }
}


