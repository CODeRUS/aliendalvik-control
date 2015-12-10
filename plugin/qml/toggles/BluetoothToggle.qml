import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Bluetooth 1.0
import MeeGo.Connman 0.2
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Bluetooth")
    icon: "image://theme/icon-m-bluetooth"
    settingsPage: "system_settings/connectivity/bluetooth"

    active: btTechModel.powered && adapter.powered
    onActiveChanged: busy = false
    property bool busy

    onClicked: {
        if (!busy) {
            busy = true
            btTechModel.powered = !btTechModel.powered
        }
    }

    TechnologyModel {
        id: btTechModel
        name: "bluetooth"
    }

    BluetoothAdapter {
        id: adapter
    }
}
