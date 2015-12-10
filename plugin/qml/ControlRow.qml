import QtQuick 2.0
import Sailfish.Silica 1.0
import org.coderus.powermenu 1.0
import org.nemomobile.dbus 2.0

Row {
    id: controlRow1
    height: btn1.height
    property int itemWidth
    property bool editMode: false
    property Item remorse

    function remorseRestart() {
        if (!remorse) {
            remorse = remorseComponent.createObject(controlRow1.parent)
        }

        remorse.execute(qsTr("Reboot device"),
                        function() {
                            restartDevice()
                            view.close()
                        },
                        3000
        )
    }

    function restartDevice() {
        dsmeDbus.call("req_reboot", [])
    }

    function remorseShutdown() {
        if (!remorse) {
            remorse = remorseComponent.createObject(controlRow1.parent)
        }

        remorse.execute(qsTr("Shutdown device"),
                        function() {
                            shutdownDevice()
                            view.close()
                        },
                        3000
        )
    }

    function shutdownDevice() {
        dsmeDbus.call("req_shutdown", [])
    }

    function restartHomescreen() {
        lipstickSystemdIface.call("Restart", ["replace"])
    }

    BackgroundIconButton {
        id: btn1
        width: itemWidth
        iconSource: "image://theme/graphic-power-off"
        title: qsTr("Shutdown")
        enabled: !editMode
        onClicked: remorseShutdown()
    }

    BackgroundIconButton {
        width: itemWidth
        iconSource: "image://theme/graphic-reboot"
        title: qsTr("Reboot")
        enabled: !editMode
        onClicked: remorseRestart()
        onPressAndHold: restartHomescreen()
    }

    BackgroundIconButton {
        width: itemWidth
        iconSource: editMode ? "image://theme/icon-m-developer-mode" : "image://theme/graphic-display-blank"
        title: editMode ? qsTr("Edit mode") : qsTr("Lock")
        highlighted: down || editMode
        imageHeight: btn1.imageHeight
        imageWidth: btn1.imageWidth
        onClicked: {
            if (editMode) {
                editMode = false
            }
            else if (doubleTimer.running) {
                doubleTimer.stop()
                mceRequest.call("req_display_state_off", [])
                lipstickDevicelock.call("setState", [1])
            }
            else {
                doubleTimer.restart()
            }
        }
        onPressAndHold: {
            editMode = !editMode
        }
        Timer {
            id: doubleTimer
            interval: 200
            onTriggered: {
                mceRequest.call("req_display_state_off", [])
            }
        }
    }

    DBusInterface {
        id: dsmeDbus
        bus: DBus.SystemBus
        service: "com.nokia.dsme"
        path: "/com/nokia/dsme/request"
        iface: "com.nokia.dsme.request"
    }

    DBusInterface {
        id: mceRequest
        bus: DBus.SystemBus
        service: 'com.nokia.mce'
        path: '/com/nokia/mce/request'
        iface: 'com.nokia.mce.request'
    }

    DBusInterface {
        id: lipstickDevicelock
        bus: DBus.SystemBus
        service: 'org.nemomobile.lipstick'
        path: '/devicelock'
        iface: 'org.nemomobile.lipstick.devicelock'
    }

    DBusInterface {
        id: lipstickSystemdIface
        bus: DBus.SessionBus
        service: 'org.freedesktop.systemd1'
        path: '/org/freedesktop/systemd1/unit/lipstick_2eservice'
        iface: 'org.freedesktop.systemd1.Unit'
    }

    Component {
        id: remorseComponent

        RemorsePopup {
            id: remorsePopup
            y: 0
            height: controlRow1.height
            _seconds: 0

            Rectangle {
                id: progress
                width: parent.width * remorsePopup._msRemaining / remorsePopup._timeout
                height: Theme.paddingSmall
                color: Theme.highlightBackgroundColor
                opacity: 0.6
            }
        }
    }
}

