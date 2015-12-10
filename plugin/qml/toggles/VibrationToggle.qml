import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.systemsettings 1.0
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    name: qsTr("Vibration")
    icon: "image://theme/icon-m-vibration"

    Image {
        id: modeIcon
        source: {
            switch (soundSettings.vibraMode) {
            case ProfileControl.VibraAlways:
            case ProfileControl.VibraNever:
            default:
                return ""
            case ProfileControl.VibraSilent:
                return "image://theme/icon-system-ringtone-mute"
            case ProfileControl.VibraNormal:
                return "image://theme/icon-system-ringtone"
            }
        }
        cache: true
        anchors.centerIn: iconItem
        anchors.horizontalCenterOffset: Math.min(iconItem.width, iconItem.height) / 2 - width / 2
        anchors.verticalCenterOffset: Math.min(iconItem.width, iconItem.height) / 2 - height / 2
        visible: soundSettings.vibraMode == ProfileControl.VibraSilent
                    || soundSettings.vibraMode == ProfileControl.VibraNormal
    }

    Rectangle {
        height: 2
        width: Math.sqrt(Math.pow(iconItem.sourceSize.width, 2) + Math.pow(iconItem.sourceSize.height, 2)) - Theme.paddingMedium
        anchors.centerIn: iconItem
        rotation: 45
        visible: soundSettings.vibraMode == ProfileControl.VibraNever
        color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
    }

    settingsPage: "system_settings/look_and_feel/sounds"

    onClicked: {
        var nextmode = ProfileControl.VibraAlways
        switch (soundSettings.vibraMode) {
        case ProfileControl.VibraAlways:
            nextmode = ProfileControl.VibraSilent
            break
        case ProfileControl.VibraSilent:
            nextmode = ProfileControl.VibraNormal
            break
        case ProfileControl.VibraNormal:
            nextmode = ProfileControl.VibraNever
            break
        case ProfileControl.VibraNever:
        default:
            nextmode = ProfileControl.VibraAlways
            break
        }
        soundSettings.vibraMode = nextmode
    }

    ProfileControl { id: soundSettings}
}
