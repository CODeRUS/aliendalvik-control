import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import org.nemomobile.keepalive 1.1
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: "Flight mode"
    icon: "image://theme/icon-m-airplane-mode"
    active: connMgr.instance.offlineMode
    settingsPage: "system_settings/connectivity/flight"

    onClicked: {
        setActive(!active)
    }

    KeepAlive {
        id: keepAlive
    }

    Timer {
        id: cancelKeepaliveTimer
        interval: 10000
        onTriggered: {
            keepAlive.enabled = false
        }
    }

    function startKeepalive() {
        cancelKeepaliveTimer.stop()
        keepAlive.enabled = true
    }

    function stopKeepalive() {
        cancelKeepaliveTimer.stop()
        keepAlive.enabled = false
    }

    function setActive(_active) {
        if (connMgr.instance.offlineMode === _active) {
            return
        }

        // Hold keepalive session over remorse timer
        // and offlineMode property change ipc
        startKeepalive()


        if (_active) {
            connMgr.instance.offlineMode = true
        } else {
            connMgr.instance.offlineMode = false
        }
    }

    NetworkManagerFactory {
        id: connMgr
    }
    Connections {
        target: connMgr.instance
        onOfflineModeChanged: {
            stopKeepalive()
        }
    }
}

