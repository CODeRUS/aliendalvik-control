import QtQuick 2.1
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "usagePage"

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: column.height

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("How to use")
            }

            Label {
                text: qsTr("Powermenu should be configured before any usage. Translate this string as explanation of basic powermenu usage including configuration of powerkey actions to open powermenu2 instead of default sailfish one, or any other additional hints.")
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width - Theme.paddingLarge * 2
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

        VerticalScrollDecorator {}
    }
}
