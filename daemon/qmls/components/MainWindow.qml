import QtQuick 2.1
import QtQuick.Window 2.1 as QtQuick
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0

Window {
    id: window

    default property alias _contentChildren: content.data
    property alias contentItem: content

    property bool isPortrait: deviceOrientation == Orientation.Portrait || deviceOrientation == Orientation.PortraitInverted
    property bool isLandscape: deviceOrientation == Orientation.Landscape || deviceOrientation == Orientation.LandscapeInverted

    signal appeared
    signal disappeared

    function appear() {
        appearAnimation.start()
    }

    function disappear() {
        disappearAnimation.start()
    }

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
                y: -Screen.height
            }
        }
    }

    SmoothedAnimation {
        id: appearAnimation
        target: content
        property: "y"
        from: -Screen.height
        to: 0
        alwaysRunToEnd: true
        duration: 200
        onStopped: window.appeared()
    }

    SmoothedAnimation {
        id: disappearAnimation
        target: content
        property: "y"
        from: 0
        to: -Screen.height
        alwaysRunToEnd: true
        duration: 200
        onStopped: window.disappeared()
    }
}
