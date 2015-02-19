import QtQuick 2.1
import Sailfish.Silica 1.0

Item {
    width: Screen.width
    height: Screen.height

    // rounded corners
    Image {
        // top left
        source: "image://theme/graphic-interface-rounded-corner"
        z: 1
    }

    Image {
        id: topRight
        anchors.right: parent.right
        source: "image://theme/graphic-interface-rounded-corner"
        transform: Rotation {
            angle: 90
            origin.x: topRight.height / 2
            origin.y: topRight.height / 2
        }
        z: 1
    }

    Image {
        id: bottomRight
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        source: "image://theme/graphic-interface-rounded-corner"
        transform: Rotation {
            angle: 180
            origin.x: bottomRight.height / 2
            origin.y: bottomRight.height / 2
        }
        z: 1
    }

    Image {
        id: bottomLeft
        anchors.bottom: parent.bottom
        source: "image://theme/graphic-interface-rounded-corner"
        transform: Rotation {
            angle: 270
            origin.x: bottomLeft.height / 2
            origin.y: bottomLeft.height / 2
        }
        z: 1
    }
}
