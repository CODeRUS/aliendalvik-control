import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.systemsettings 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: displaySettings.brightness + "%"
    icon: "image://theme/icon-m-day"
    settingsPage: "system_settings/look_and_feel/display"

    onClicked: {
        if (displaySettings.brightness == 100) {
            displaySettings.brightness = 1
        }
        else {
            displaySettings.brightness = Math.round((displaySettings.brightness + 10) / 10) * 10
        }
    }

    DisplaySettings {
        id: displaySettings
    }
}

