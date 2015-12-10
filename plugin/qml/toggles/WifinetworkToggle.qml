import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import com.jolla.connection 1.0
import org.nemomobile.configuration 1.0
import org.nemomobile.dbus 2.0
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    settingsPage: "system_settings/connectivity/wlan"
    name: qsTr("Wireless")
    icon: {
        // WLAN off
        if (!wifiTechnology.powered)
            return "image://theme/icon-m-wlan-no-signal";

        // WLAN tethering
        if (wifiTechnology.tethering)
            return "image://theme/icon-m-wlan-hotspot";

        // WLAN connected
        if (wifiTechnology.connected) {
            if (networkManager.instance.defaultRoute.type !== "wifi" && networkManager.instance.defaultRoute.type !== "")
                return "image://theme/icon-m-wlan-0"

            if (networkManager.instance.defaultRoute.strength >= 59) {
                return "image://theme/icon-m-wlan-4"
            } else if (networkManager.instance.defaultRoute.strength >= 55) {
                return "image://theme/icon-m-wlan-3"
            } else if (networkManager.instance.defaultRoute.strength >= 50) {
                return "image://theme/icon-m-wlan-2"
            } else if (networkManager.instance.defaultRoute.strength >= 40) {
                return "image://theme/icon-m-wlan-1"
            } else {
                return "image://theme/icon-m-wlan-0"
            }
        }

        // WLAN not connected, network available
        if (networkManager.servicesList("wifi").length > 0)
            return "image://theme/icon-m-wlan"

        // WLAN no signal
        return "image://theme/icon-m-wlan-no-signal"
    }
    active: wifiTechnology.powered && !wifiTechnology.tethering
    property bool busy: busyTimer.running

    onClicked: {
        if (wifiTechnology.tethering) {
            connectionAgent.stopTethering(true)
        } else {
            wifiTechnology.powered = !wifiTechnology.powered
            if (wifiTechnology.powered) {
                busyTimer.stop()
            } else if (connDialogConfig.rise) {
                busyTimer.restart()
            }
        }
    }

    Timer {
        id: busyTimer
        interval: connDialogConfig.scanningWait
        onTriggered: connectionSelector.openConnection()
        onRunningChanged: {
            if (running) {
                wifiTechnology.connectedChanged.connect(stop)
            } else {
                wifiTechnology.connectedChanged.disconnect(stop)
            }
        }
    }

    ConfigurationGroup {
        id: connDialogConfig

        path: "/apps/jolla-settings/wlan_fav_switch_connection_dialog"

        property bool rise: true
        property int scanningWait: 5000
    }

    NetworkTechnology {
        id: wifiTechnology
        path: networkManager.instance.technologyPathForType("wifi")
        function updateTechnologies() {
            if (networkManager.instance.available && networkManager.instance.technologiesEnabled) {
                wifiTechnology.path = networkManager.instance.technologyPathForType("wifi")
            }
        }
    }

    NetworkManagerFactory {
        id: networkManager
    }

    Connections {
        target: networkManager.instance
        onTechnologiesChanged: wifiTechnology.updateTechnologies()
        onTechnologiesEnabledChanged: wifiTechnology.updateTechnologies()
        onAvailableChanged: wifiTechnology.updateTechnologies()
    }

    ConnectionAgent { id: connectionAgent }

    DBusInterface {
        id: connectionSelector

        service: "com.jolla.lipstick.ConnectionSelector"
        path: "/"
        iface: "com.jolla.lipstick.ConnectionSelectorIf"

        function openConnection() {
            call('openConnectionNow', 'wifi')
        }
    }
}

