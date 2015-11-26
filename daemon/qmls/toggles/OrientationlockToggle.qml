import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.systemsettings 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    settingsPage: "system_settings/look_and_feel/display"

    name: {
        if (displaySettings.orientationLock === "portrait") {
            return qsTr("Portrait")
        }
        else if (displaySettings.orientationLock === "landscape") {
            return qsTr("Landscape")
        }
        else if (displaySettings.orientationLock === "dynamic") {
            return qsTr("Dynamic")
        }
    }

    icon: {
        if (displaySettings.orientationLock === "portrait") {
            return "image://theme/icon-m-phone"
        }
        else if (displaySettings.orientationLock === "landscape") {
            return "image://theme/icon-m-tablet"
        }
        else if (displaySettings.orientationLock === "dynamic") {
            return "image://theme/icon-m-rotate"
        }
    }

    onClicked: {
        if (displaySettings.orientationLock === "portrait") {
            displaySettings.orientationLock = "landscape"
        }
        else if (displaySettings.orientationLock === "landscape") {
            displaySettings.orientationLock = "dynamic"
        }
        else if (displaySettings.orientationLock === "dynamic") {
            displaySettings.orientationLock = "portrait"
        }
    }

    DisplaySettings {
        id: displaySettings
        onOrientationLockChanged: console.log("[" + orientationLock + "]")
    }
}

