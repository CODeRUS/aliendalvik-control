import QtQuick 2.1
import QtQuick.Window 2.1 as QtQuick
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0

Window {
    id: window

    default property alias _contentChildren: content.data
    property alias contentItem: content

    readonly property bool isPortrait: deviceOrientation == Orientation.Portrait || deviceOrientation == Orientation.PortraitInverted
    readonly property bool isLandscape: deviceOrientation == Orientation.Landscape || deviceOrientation == Orientation.LandscapeInverted
    readonly property int deviceRotation: rotatingItem.rotation


    Item {
        id: root
        width: Screen.width
        height: Screen.height
        anchors.centerIn: parent

        rotation: window.QtQuick.Screen.angleBetween(Qt.PortraitOrientation, window.QtQuick.Screen.primaryOrientation)

        Item {
            id: rotatingItem

            z: 1
            anchors.centerIn: parent
            width: window.isPortrait ? parent.width : parent.height
            height: window.isPortrait ? parent.height : parent.width
            rotation: window.deviceOrientation === Orientation.Landscape
                      ? 90
                      : window.deviceOrientation === Orientation.PortraitInverted
                        ? 180
                        : window.deviceOrientation === Orientation.LandscapeInverted
                          ? 270
                          : 0
            clip: true

            Behavior on rotation {
                SmoothedAnimation { duration: 200 }
            }

            Item {
                id: content
                width: parent.width
                height: parent.height
                y: 0
            }
        }
    }
}
