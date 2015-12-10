import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.QOfono 0.2
import MeeGo.Connman 0.2
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Mobile network")
    icon: "image://theme/icon-m-mobile-network"
    settingsPage: "system_settings/connectivity/mobile"
    active: networkService.autoConnect
    disabled: !valid || networkManager.offlineMode || busy
    property bool processing
    property bool busy: { processing || tapTimer.running; }

    Timer {
        id: tapTimer
        interval: 2000; running: false; repeat: false; triggeredOnStart: false;
    }

    onClicked: {
        if (enabled && !busy && !networkManager.offlineMode) {
            if (networkService.favorite) {
                networkService.autoConnect = !networkService.autoConnect
            } else if (!networkService.connected) {
                //autoconnect depends on favorite which depends on being connected once already
                processing = true
                networkService.requestConnect()
            }
            tapTimer.start()
        }
    }

    property alias networkService: service
    property alias connectionManager: connectionManager
    property alias networkManager: networkFactory
    property bool valid: service.path !== ""

    property string inetContextPath: connectionManager.contexts.length > 0 ? connectionManager.contexts[0] : ""
    onInetContextPathChanged: service.path = getServicePathForContext()

    OfonoManager { id: manager }

    OfonoConnMan {
        id: connectionManager
        modemPath: manager.defaultModem
        filter: "internet"
    }

    OfonoSimManager {
        id: simManager
        modemPath: manager.defaultModem
        onSubscriberIdentityChanged: service.path = getServicePathForContext()
    }

    OfonoContextConnection {
        id: dataContext
        contextPath: inetContextPath
        onNameChanged: root.name = name
        onContextPathChanged: root.name = name
        onReportError: console.log(errorString)
    }

    NetworkManager {
        id: networkFactory
        onAvailabilityChanged: service.path = getServicePathForContext()
        onServicesListChanged: service.path = getServicePathForContext()
    }

    NetworkService {
        id: service
        path: getServicePathForContext()
        onStateChanged: {
            if (!root.processing)
                return

            if (state === "ready" || state === "online")
                networkService.autoConnect = true
            else if (state !== "idle" && state !== "failure")
                return

            root.processing = false
        }
    }

    function constructServicePath(contextPath, subscriberIdentity, cellularServices) {
        if (contextPath === "" || subscriberIdentity === "" || cellularServices.length === 0)
            return ""

        var context = contextPath.substr(contextPath.lastIndexOf("/")+1)
        var sp = "/net/connman/service/cellular_" + subscriberIdentity + "_" + context

        for (var i = 0; i < cellularServices.length; ++i) {
            if (cellularServices[i] === sp)
                return sp
        }

        return ""
    }

    function getServicePathForContext() {
        return constructServicePath(inetContextPath, simManager.subscriberIdentity,
                                    networkFactory.servicesList("cellular"))
    }
}
