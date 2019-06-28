import QtQuick 2.6
import Sailfish.Silica 1.0
import Nemo.DBus 2.0
import org.coderus.aliendalvikcontrol.edge 1.0

Item {
    id: root

    width: Screen.width
    height: Screen.height

    property bool isTopmostAndroid: false
    onIsTopmostAndroidChanged: {
        setTouchRegion(isTopmostAndroid)
    }

    DBusInterface {
        id: control
        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
        signalsEnabled: true

        function isTopmostAndroidChanged(value) {
            isTopmostAndroid = value
        }

        Component.onCompleted: {
            typedCall("isTopmostAndroid", [],
                      function(value) {
                          isTopmostAndroid = value
                          setTouchRegion(isTopmostAndroid)
                      })
        }
    }

    function setTouchRegion(enabled) {
        NativeWindowHelper.setTouchRegion(mouseArea, enabled)
    }

    function progressBetween(progress, minValue, maxValue) {
        return minValue - (progress * (minValue - maxValue))
    }

    MouseArea {
        id: mouseArea

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.itemSizeHuge

        height: Screen.height / 2
        width: Theme.horizontalPageMargin

        enabled: isTopmostAndroid

        Rectangle {
            anchors.fill: parent
            color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            visible: isTopmostAndroid
        }

        property point startPoint

        onPressed: {
            startPoint = Qt.point(mouse.x, mouse.y)
            pointItem.progressX = Math.min(1, (startPoint.x - mouse.x) / (Screen.width / 2 - Theme.horizontalPageMargin))
            pointItem.offsetY = Math.max(0, mouse.y - startPoint.y)
            pointItem.visible = true
        }

        onClicked: {
            //
        }

        onPositionChanged: {
            pointItem.progressX = Math.min(1, (startPoint.x - mouse.x) / (Screen.width / 2 - Theme.horizontalPageMargin))
            pointItem.offsetY = Math.max(0, mouse.y - startPoint.y)
        }

        onReleased: {
            pointItem.visible = false
            var posx = parseInt(pointItem.x + pointItem.width / 2)
            var posy = parseInt(pointItem.y + pointItem.height / 2)
            control.call("sendTap", [posx, posy])
        }

        onCanceled: {
            pointItem.visible = false
        }
    }

    Rectangle {
        id: pointItem
        property int offsetY
        property real progressX

        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall

        x: Screen.width - progressBetween(progressX, width, Screen.width)
        y: offsetY

        radius: width / 2
        color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)

        visible: false
    }
}
