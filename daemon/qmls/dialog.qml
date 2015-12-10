import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0
import org.coderus.powermenu 1.0

MainWindow {
    id: window

    function hideWithCare() {
        testAppear.stop()
        testDisappear.stop()
        controlRow1.editMode = false
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
                controlRow1.editMode = false
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

        TogglesArea {
            id: grid
            width: parent.width - sideMargin
            height: window.contentItem.height - testItem2.height - Theme.horizontalPageMargin * 4
            editMode: controlRow1.editMode
        }
    }

    Rectangle {
        id: testItem2
        width: parent.width
        height: controlRow1.height
        y: -testItem2.height
        color: Theme.highlightDimmerColor

        ControlRow {
            id: controlRow1
            itemWidth: testItem2.width / 3
        }
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
}
