import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import Nemo.DBus 2.0

ShareDialog {
    id: root

    property string sourceString: source
    property var contentVariant: root.content || {}

    property bool ready: false

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
            width: ListView.view.width

            Image {
                id: icon
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.width: Theme.itemSizeSmall - Theme.paddingMedium
                sourceSize.height: Theme.itemSizeSmall - Theme.paddingMedium
                source: "data:image/png;base64," + modelData.icon
            }

            Label {
                anchors.left: icon.right
                anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.prettyName
                color: parent.pressed && parent.down ? Theme.highlightColor : Theme.primaryColor
            }

            onClicked: {
                control.call("doShare", [modelData.mimeType, modelData.fileName, modelData.data, modelData.packageName, modelData.className, modelData.launcherClass])
                pageStack.pop()
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
            __silica_applicationwindow_instance.activate()
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

