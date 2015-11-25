import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.systemsettings 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    settingsPage: "system_settings/look_and_feel/display"

    name: displaySettings.orientationLock[0].toUpperCase() + displaySettings.orientationLock.substr(1)
    icon: {
        if (displaySettings.orientationLock == "portrait") {
            return "image://theme/icon-m-phone"
        }
        else if (displaySettings.orientationLock == "landscape") {
            return "image://theme/icon-m-tablet"
        }
        else {
            return "image://theme/icon-m-rotate"
        }
    }

    onClicked: {
        if (displaySettings.orientationLock == "portrait") {
            displaySettings.orientationLock = "landscape"
        }
        else if (displaySettings.orientationLock == "landscape") {
            displaySettings.orientationLock = "dynamic"
        }
        else {
            displaySettings.orientationLock = "portrait"
        }
    }

    DisplaySettings { id: displaySettings }
}

