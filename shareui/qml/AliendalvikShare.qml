import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import Nemo.DBus 2.0

ShareDialog {
    id: root

    property string sourceString: source
    property var contentVariant: root.content || {}

    property bool ready: false

    // Hacka hacking hacky-hacky hacked hacku hacka hack.
    Connections {
        target: __silica_applicationwindow_instance
        onApplicationActiveChanged: {
            accept()
        }
    }

    Component.onCompleted: {
        if (aliendalvikServiceIface.isActive()) {
            control.call("shareContent", [root.contentVariant, root.sourceString])
        }
    }

    canAccept: false

    SilicaListView {
        id: sharingView
        anchors.fill: parent
        visible: aliendalvikServiceIface.isActive()
        header: PageHeader {
            title: qsTrId("Share to Android")
        }
        delegate: BackgroundItem {
            id: content
            contentHeight: Theme.itemSizeSmall
            width: parent.width

            Label {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.prettyName
                color: parent.pressed && parent.down ? Theme.highlightColor : Theme.primaryColor
            }

            onClicked: {
                control.call("doShare", [modelData.mimetype, modelData.filename, modelData.data, modelData.packageName, modelData.className])
                accept()
            }
        }

        ViewPlaceholder {
            enabled: ready && sharingView.count == 0
            text: "No candidates for sharing to"
        }
    }

    DBusInterface {
        id: control
        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
        signalsEnabled: true

        function sharingListReady(sharingList) {
            ready = true
            sharingView.model = sharingList
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

    BusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: !ready && aliendalvikServiceIface.isActive()
        visible: running
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

