import QtQuick 2.0
import Sailfish.Silica 1.0
import org.coderus.powermenu.controls 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: "Flashlight"
    icon: "image://theme/icon-camera-wb-tungsten"

    onClicked: {
        flashlight.toggle()
    }

    Flashlight {
        id: flashlight
    }
}

