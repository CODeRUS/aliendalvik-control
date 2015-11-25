import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import com.jolla.connection 1.0
import MeeGo.QOfono 0.2
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: "Tethering"
    icon: "image://theme/icon-m-tether"
    settingsPage: "system_settings/connectivity/tethering"

    active: wifiTechnology.tethering
    disabled: networkManager.instance.offlineMode || wifiTechnology.tetheringId.length == 0
             || wifiTechnology.tetheringPassphrase.length == 0 || busy
             || !roamingDataAllowed()
    property bool busy: false

    // Check if mobile network and roaming conditions allow tethering
    function roamingDataAllowed() {
        return networkRegistration.valid && connectionManager.valid
            && !(networkRegistration.status === "roaming" && !connectionManager.roamingAllowed)
    }

    onClicked: {
        if (wifiTechnology.tethering && !busy) {
            delayedTetheringSwitch.start()
            busy = true
            connectionAgent.stopTethering()
        }
        else {
            if (!wifiTechnology.tethering && !busy) {
                delayedTetheringSwitch.start()
                busy = true
                connectionAgent.startTethering("wifi")
            }
        }
    }

    Timer {
        id: delayedTetheringSwitch
        interval: 15000
        onTriggered: busy = false
    }

    ConnectionAgent {
        id: connectionAgent
        onTetheringFinished: {
            delayedTetheringSwitch.stop()
            busy = false
        }
    }

    NetworkManagerFactory {
        id: networkManager
    }

    NetworkTechnology {
        id: wifiTechnology
        path: networkManager.instance.technologyPathForType("wifi")
    }

    Connections {
        target: networkManager.instance
        onTechnologiesChanged: {
            wifiTechnology.path = networkManager.instance.technologyPathForType("wifi")
        }
        onTechnologiesEnabledChanged: {
            wifiTechnology.path = networkManager.instance.technologyPathForType("wifi")
        }
    }

    OfonoManager { id: manager }

    OfonoNetworkRegistration {
        id: networkRegistration
        modemPath: manager.defaultModem
    }

    OfonoConnMan {
        id: connectionManager
        modemPath: manager.defaultModem
    }
}

