import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0
import org.coderus.powermenu.controls 1.0
import "components"

MainWindow {
    id: window

    onDisappeared: {
        editMode = false
        view.close()
    }
    property Item remorse
    property bool editMode: false

    function remorseRestart() {
        if (!remorse) {
            remorse = remorseComponent.createObject(root)
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
            remorse = remorseComponent.createObject(root)
        }

        remorse.execute(qsTr("Shutdown device"),
                        function() {
                            dsmeDbus.shutdown()
                            view.close()
                        },
                        3000
        )
    }

    function shutdownDevice() {
        dsmeDbus.call("req_shutdown", [])
    }

    function showSettingsPage(page) {
        if (page && page.length > 0) {
            settingsIface.call("showPage", [page])
            window.disappear()
        }
    }

    function restartHomescreen() {
        lipstickSystemdIface.call("Restart", ["replace"])
    }

    Connections {
        target: view
        onVisibleChanged: {
            if (view.visible) {
                mceRequest.typedCall("get_display_status",
                                      [],
                                      function (result) {
                                          if (result == "on") {
                                              window.appear()
                                          }
                                          else {
                                              view.close()
                                          }
                                      })
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        preventStealing: true
        onClicked: {
            window.disappear()
        }
    }

    Item {
        id: root
        anchors.fill: parent
        anchors.bottomMargin: Theme.horizontalPageMargin * 4

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: Theme.highlightDimmerColor
            height: controlRow1.height + Math.min(grid.height, grid.contentHeight + Theme.horizontalPageMargin)

            MouseArea {
                anchors.fill: parent
            }

            Behavior on height {
                NumberAnimation { duration: 500 }
            }
        }

        Row {
            id: controlRow1
            height: Theme.itemSizeExtraLarge

            BackgroundIconButton {
                width: root.width / 3
                height: parent.height
                iconSource: "image://theme/icon-l-power"
                title: qsTr("Shutdown")
                enabled: !editMode
                onClicked: remorseShutdown()
            }

            BackgroundIconButton {
                width: root.width / 3
                height: parent.height
                iconSource: "image://theme/icon-l-reboot"
                title: qsTr("Reboot")
                enabled: !editMode
                onClicked: remorseRestart()
                onPressAndHold: restartHomescreen()
            }

            BackgroundIconButton {
                width: root.width / 3
                height: parent.height
                iconSource: editMode ? "image://theme/icon-m-developer-mode" : "image://theme/icon-m-device-lock"
                title: editMode ? qsTr("Edit mode") : qsTr("Lock")
                highlighted: down || editMode
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
        }

        GridView {
            id: grid
            anchors {
                top: controlRow1.bottom
                left: parent.left
                leftMargin: (root.width - (grid.cellWidth * Math.floor(root.width / grid.cellWidth))) / 2
                right: parent.right
                rightMargin: leftMargin
                bottom: parent.bottom
                bottomMargin: leftMargin
            }
            pixelAligned: true
            flickDeceleration: Theme.flickDeceleration
            maximumFlickVelocity: Theme.maximumFlickVelocity
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            cacheBuffer: cellHeight
            interactive: contentHeight > height
            cellWidth: Theme.itemSizeExtraLarge
            cellHeight: Theme.itemSizeLarge

            add: Transition {
                SequentialAnimation {
                    NumberAnimation { properties: "z"; to: -1; duration: 1 }
                    NumberAnimation { properties: "opacity"; to: 0.0; duration: 1 }
                    NumberAnimation { properties: "x,y"; duration: 1 }
                    NumberAnimation { properties: "z"; to: 0; duration: 500 }
                    NumberAnimation { properties: "opacity"; from: 0.0; to: 1.0; duration: 300 }
                }
            }
            remove: Transition {
                ParallelAnimation {
                    NumberAnimation { properties: "z"; to: -1; duration: 1 }
                    NumberAnimation { properties: "x"; to: 0; duration: 100 }
                    NumberAnimation { properties: "opacity"; to: 0.0; duration: 100 }
                }
            }
            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 500 }
            }

            model: TogglesModel {
                id: gridModel
                editMode: window.editMode
            }
            delegate: Item {
                id: gridDelegate
                width: grid.cellWidth
                height: grid.cellHeight
                clip: true

                Rectangle {
                    id: itemDelegate
                    width: gridDelegate.width
                    height: gridDelegate.height
                    color: actionDelegate.pressed
                           ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                           : "transparent"
                    x: 0
                    y: 0

                    Loader {
                        id: loaderDelegate
                        anchors.fill: parent
                        source: model.source
                        property var sourceModel: model
                        property bool hiddenProperty: gridModel.hidden.indexOf(model.path) >= 0
                    }

                    MouseArea {
                        id: actionDelegate
                        anchors.fill: parent
                        property int dragIndex: index
                        onDragIndexChanged: {
                            if (drag.target) {
                                gridModel.move(index, dragIndex)
                            }
                        }
                        onPressed: {
                            if (!editMode) {
                                loaderDelegate.item.pressed = true
                            }
                        }
                        onClicked: {
                            if (editMode) {
                                if (!model.icon || model.icon.length == 0) {
                                    gridModel.hideToggle(model.path)
                                }
                            }
                            else if (doubleTimer2.running) {
                                doubleTimer2.stop()
                                loaderDelegate.item.doubleClicked()
                            }
                            else {
                                doubleTimer2.restart()
                            }
                        }
                        Timer {
                            id: doubleTimer2
                            interval: 200
                            onTriggered: {
                                loaderDelegate.item.clicked()
                            }
                        }
                        onReleased: {
                            if (drag.target) {
                                drag.target = null
                                itemDelegate.parent = gridDelegate
                                itemDelegate.x = 0
                                itemDelegate.y = 0
                            }
                            loaderDelegate.item.pressed = false
                        }
                        onPressAndHold: {
                            if (editMode) {
                                drag.target = itemDelegate
                                var newPos = mapToItem(reorderPlaceholder, mouseX, mouseY)
                                itemDelegate.parent = reorderPlaceholder
                                itemDelegate.x = newPos.x - itemDelegate.width / 2
                                itemDelegate.y = newPos.y - itemDelegate.height
                            }
                        }
                        onPositionChanged: {
                            if (drag.target) {
                                var targetIndex = grid.indexAt(itemDelegate.x + itemDelegate.width / 2, itemDelegate.y + itemDelegate.height / 2)
                                if (targetIndex >= 0) {
                                    dragIndex = targetIndex
                                }
                            }
                        }
                        onContainsMouseChanged: {
                            if (!drag.target && !editMode) {
                                loaderDelegate.item.pressed = containsMouse
                            }
                        }
                    }
                }
            }

        }

        Item {
            id: reorderPlaceholder
            anchors.fill: grid
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
        id: mceDbus
        bus: DBus.SystemBus
        service: "com.nokia.mce"
        path: "/com/nokia/mce/signal"
        iface: "com.nokia.mce.signal"
        signalsEnabled: true
        function display_status_ind(status) {
            if (status == "off") {
                view.close()
            }
        }
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
        id: settingsIface
        bus: DBus.SessionBus
        service: 'com.jolla.settings'
        path: '/com/jolla/settings/ui'
        iface: 'com.jolla.settings.ui'
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
