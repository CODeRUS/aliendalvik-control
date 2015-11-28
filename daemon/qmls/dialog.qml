import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0
import org.coderus.powermenu.controls 1.0
import "components"

MainWindow {
    id: window

    property Item remorse
    property bool editMode: false

    function hideWithCare() {
        testAppear.stop()
        testDisappear.stop()
        editMode = false
        testItem1.y = -testItem1.height
        testItem2.y = -testItem2.height
        view.close()
    }

    function disappearAnimation() {
        testDisappear.start()
    }

    function afterShow() {
        mceRequest.typedCall("get_display_status",
                              [],
                              function (result) {
                                  if (result == "on") {
                                      testAppear.start()
                                  }
                                  else {
                                      hideWithCare()
                                  }
                              })
    }

    function remorseRestart() {
        if (!remorse) {
            remorse = remorseComponent.createObject(contentItem)
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
            remorse = remorseComponent.createObject(contentItem)
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

    function showSettingsPage(page) {
        if (page && page.length > 0) {
            settingsIface.call("showPage", [page])
            window.hideWithAnimation()
        }
    }

    function restartHomescreen() {
        lipstickSystemdIface.call("Restart", ["replace"])
    }

    Connections {
        target: view
        onVisibleChanged: {
            if (view.visible) {
                afterShow()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        preventStealing: true
        onClicked: {
            testDisappear.start()
        }
    }

    SequentialAnimation {
        id: testAppear
        NumberAnimation {
            target: testItem2
            property: "y"
            from: -testItem2.height
            to: 0
            duration: Math.max(testItem2.height, 0)
        }
        NumberAnimation {
            target: testItem1
            property: "y"
            from: -testItem1.height
            to: testItem2.height
            duration: Math.max(testItem1.height, 0)
        }
    }

    SequentialAnimation {
        id: testDisappear
        NumberAnimation {
            target: testItem1
            property: "y"
            from: testItem2.height
            to: -testItem1.height
            duration: Math.max(testItem1.height, 0)
        }
        NumberAnimation {
            target: testItem2
            property: "y"
            from: 0
            to: -testItem2.height
            duration: Math.max(testItem2.height, 0)
        }
        ScriptAction {
            script: {
                editMode = false
                view.close()
            }
        }
    }

    Rectangle {
        id: testItem1
        width: parent.width
        color: Theme.highlightDimmerColor
        height: Math.min(grid.contentHeight + Theme.horizontalPageMargin, grid.height)
        y: -Screen.height

        Behavior on height {
            NumberAnimation { duration: 200 }
        }

        GridView {
            id: grid
            property int sideMargin: (parent.width - (grid.cellWidth * Math.floor(parent.width / grid.cellWidth))) / 2
            width: parent.width - sideMargin
            height: window.contentItem.height - testItem2.height - Theme.horizontalPageMargin * 4
            x: sideMargin
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
                    NumberAnimation { properties: "z"; to: 0; duration: 200 }
                    NumberAnimation { properties: "opacity"; from: 0.0; to: 1.0; duration: 100 }
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
                NumberAnimation { properties: "x,y"; duration: 200 }
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
                                var newPos = mapToItem(grid, mouseX, mouseY)
                                itemDelegate.parent = grid
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
    }

    Rectangle {
        id: testItem2
        width: parent.width
        height: controlRow1.height
        y: -testItem2.height
        color: Theme.highlightDimmerColor

        Row {
            id: controlRow1
            height: btn1.height

            BackgroundIconButton {
                id: btn1
                width: testItem2.width / 3
                iconSource: "image://theme/graphic-power-off"
                title: qsTr("Shutdown")
                enabled: !editMode
                onClicked: remorseShutdown()
            }

            BackgroundIconButton {
                width: testItem2.width / 3
                iconSource: "image://theme/graphic-reboot"
                title: qsTr("Reboot")
                enabled: !editMode
                onClicked: remorseRestart()
                onPressAndHold: restartHomescreen()
            }

            BackgroundIconButton {
                width: testItem2.width / 3
                iconSource: editMode ? "image://theme/icon-m-developer-mode" : "image://theme/graphic-display-blank"
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
                hideWithCare()
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
