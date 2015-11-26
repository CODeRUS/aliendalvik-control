import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import com.jolla.settings.system 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Location")
    icon: "image://theme/icon-m-gps"
    settingsPage: "system_settings/connectivity/gps_and_location"

    active: locationSettings.locationEnabled && gpsTechModel.powered

    onClicked: {
        locationSettings.locationEnabled = !active
        gpsTechModel.powered = !active
    }

    TechnologyModel {
        id: gpsTechModel
        name: "gps"
    }

    LocationSettings {
        id: locationSettings
    }
}

