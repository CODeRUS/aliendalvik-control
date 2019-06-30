import QtQuick 2.6
import Sailfish.Silica 1.0
import Nemo.DBus 2.0
import Nemo.Configuration 1.0
import QtSensors 5.0
import org.coderus.aliendalvikcontrol.edge 1.0

Item {
    id: root

    width: Screen.width
    height: Screen.height

    property bool initialized: false
    property bool isTopmostAndroid: false
    property bool isTopUp: orientationSensor.reading.orientation === OrientationReading.TopUp
    readonly property bool isActive: configuration.active && isTopmostAndroid && isTopUp
    onIsActiveChanged: {
        setTouchRegion(isActive)
    }

    OrientationSensor {
        id: orientationSensor
        active: true
    }

    ConfigurationGroup {
        id: configuration
        path: "/org/coderus/aliendalvikcontrol/edge"
        property bool active: false
        property bool leftHanded: false
        onLeftHandedChanged: {
            if (!isActive) {
                return
            }

            setTouchRegionDelayed.start()
        }
    }

    Timer {
        id: setTouchRegionDelayed
        interval: 1000
        repeat: false
        onTriggered: setTouchRegion(isActive)
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
                          initialized = true
                          isTopmostAndroid = value
                      })
        }
    }

    function setTouchRegion(enabled) {
        if (!initialized) {
            return
        }

        NativeWindowHelper.setTouchRegion(mouseArea, enabled)
    }

    function progressBetween(progress, minValue, maxValue) {
        return minValue - (progress * (minValue - maxValue))
    }

    MouseArea {
        id: mouseArea

        x: configuration.leftHanded ? 0 : (parent.width - width)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.itemSizeHuge

        height: Screen.height / 2
        width: Theme.horizontalPageMargin

        enabled: isActive

        Rectangle {
            anchors.fill: parent
            color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            visible: isActive
        }

        property point startPoint

        function processMouse(mouse) {
            if (configuration.leftHanded) {
                pointItem.progressX = Math.min(1, (mouse.x - startPoint.x - width / 2) / (Screen.width / 2))
            } else {
                pointItem.progressX = Math.min(1, (startPoint.x - mouse.x - width / 2) / (Screen.width / 2))
            }

            pointItem.offsetY = Math.max(0, mouse.y - startPoint.y)
        }

        onPressed: {
            startPoint = Qt.point(mouse.x, mouse.y)
            processMouse(mouse)
            pointItem.visible = true
        }

        onClicked: {
            //
        }

        onPositionChanged: {
            processMouse(mouse)
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

        readonly property int posX: progressBetween(progressX, width, Screen.width)

        x: configuration.leftHanded ? posX - width
                                    : Screen.width - posX
        y: offsetY

        radius: width / 2
        color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)

        visible: false
    }
}
