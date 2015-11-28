import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Flashlight")
    icon: "image://theme/icon-camera-wb-tungsten"
    active: Flashlight.active

    onClicked: {
        Flashlight.toggle()
    }
}

