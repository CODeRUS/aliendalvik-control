import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import Nemo.DBus 2.0

ShareDialog {
    id: root

    // Hacka hacking hacky-hacky hacked hacku hacka hack.
    Connections {
        target: __silica_applicationwindow_instance
        onApplicationActiveChanged: {
            accept()
        }
    }

    Component.onCompleted: {
        if (aliendalvikServiceIface.isActive()) {
//            shareItem.start()
            control.call("prepareSharing", [root.content])
        }
    }

    canAccept: aliendalvikServiceIface.isActive()

    DBusInterface {
        id: control
        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
        signalsEnabled: true

        function sharingListReady(data, sharing) {
            console.log(root.content)
            console.log(data)
            console.log(JSON.stringify(sharing))
        }
    }

    DBusInterface {
        id: aliendalvikServiceIface
        bus: DBus.SystemBus
        service: 'org.freedesktop.systemd1'
        path: '/org/freedesktop/systemd1/unit/aliendalvik_2eservice'
        iface: 'org.freedesktop.systemd1.Unit'

        function isActive() {
            var activeProperty = getProperty("ActiveState")
            return activeProperty === "active"
        }
    }

    SailfishShare {
        id: shareItem
        source: root.source
        content: root.content
        serviceId: root.methodId
        userData: root.content
    }

    DialogHeader {
        acceptText: qsTrId("Share to Android")
    }

    BusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: aliendalvikServiceIface.isActive()
    }

    Label {
        anchors.centerIn: parent
        width: parent.width - Theme.paddingLarge * 2
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        text: "Aliendalvik not running! Please start any android application first to use this function."
        visible: !aliendalvikServiceIface.isActive()
    }
}

