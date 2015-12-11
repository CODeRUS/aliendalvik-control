import QtQuick 2.0
import Sailfish.Silica 1.0
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Screenshot")
    icon: "image://theme/icon-m-device-download"
    disabled: Screenshot.busy
    hideAfterClick: true

    onClicked: {
        Screenshot.save(800)
    }
}
