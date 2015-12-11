import QtQuick 2.1
import Sailfish.Silica 1.0
import com.jolla.settings.system 1.0
import org.nemomobile.systemsettings 1.0
import org.coderus.powermenu 1.0

ToggleItem {
    id: root
    anchors.fill: parent

    name: currentModeLabel(usbSettings.configMode)
    icon: "image://theme/icon-m-usb"
    settingsPage: "system_settings/connectivity/usb"
    disabled: !itemsPopulated

    onClicked: {
        for (var i = 0; i < availableModes.length; i++) {
            var item = availableModes[i]
            if (item.mode == usbSettings.configMode) {
                usbSettings.configMode = availableModes[((i + 1) == availableModes.length) ? 0 : (i + 1)].mode
                break
            }
        }
    }

    property bool itemsPopulated: false

    USBSettings {
        id: usbSettings
        onSupportedModesChanged: createItems()
        //onConfigModeChanged: updateConfigModes()
    }

    property var availableModes: []

    readonly property var usbModes: [
        {
            mode: usbSettings.MODE_CHARGER,
            //% "Charger"
            menuText: qsTrId("settings_usb-me-charger"),
            // Dedicated charger but net effect is charging only
            currentLabel: qsTrId("settings_usb-la-charging")
        },{
            mode: usbSettings.MODE_CHARGING,
            //% "Charging only"
            menuText: qsTrId("settings_usb-me-charging"),
            //% "Currently charging only"
            currentLabel: qsTrId("settings_usb-la-charging")
        },{
            mode:  usbSettings.MODE_MASS_STORAGE,
            //% "Mass storage"
            menuText: qsTrId("settings_usb-me-mass_storage"),
            //% "Mass storage mode in use"
            currentLabel: qsTrId("settings_usb-la-mass_storage")
        },{
            mode: usbSettings.MODE_MTP,
            //% "Media transfer (MTP)"
            menuText: qsTrId("settings_usb-me-mtp"),
            //% "Media transfer (MTP) mode in use"
            currentLabel: qsTrId("settings_usb-la-mtp")
        },{
            mode: usbSettings.MODE_CONNECTION_SHARING,
            //% "USB tethering"
            menuText: qsTrId("settings_usb-me-connshare"),
            //% "USB tethering"
            currentLabel: qsTrId("settings_usb-la-connshare")
        },{
            mode: usbSettings.MODE_DEVELOPER,
            //% "Developer Mode"
            menuText: qsTrId("settings_usb-me-developer"),
            //% "Developer mode in use"
            currentLabel: qsTrId("settings_usb-la-developer")
        },{
            mode: usbSettings.MODE_PC_SUITE,
            //% "PC connection mode"
            menuText: qsTrId("settings_usb-me-pc_suite"),
            //% "PC connection mode in use"
            currentLabel: qsTrId("settings_usb-la-pc_suite")
        },{
            mode: usbSettings.MODE_ADB,
            //% "Adb mode"
            menuText: qsTrId("settings_usb-me-adb"),
            //% "Android debug bridge mode"
            currentLabel: qsTrId("settings_usb-la-adb")
        },{
            mode: usbSettings.MODE_DIAG,
            //% "Diag mode"
            menuText: qsTrId("settings_usb-me-diag"),
            //% "Diag mode"
            currentLabel: qsTrId("settings_usb-la-diag")
        },{
            mode: usbSettings.MODE_ASK,
            //% "Always ask"
            menuText: qsTrId("settings_usb-me-always_ask")
        },{
            mode: usbSettings.MODE_HOST,
            //% "Host mode"
            menuText: qsTrId("settings_usb-me-host")
        }
    ]

    function createItem(index, mode)
    {
        var temp = availableModes

        for (var i = 0; i < usbModes.length; i++) {
            var item = usbModes[i]
            if (item.mode == mode) {
                if (item.menuText) {
                    temp.splice(index, 0, item)
                    availableModes = temp
                    return
                }
                break
            }
        }

        //% "Mode %1"
        temp.splice(index, 0, {"mode": mode, "menuText": qsTrId("settings_usb-me-other").arg(mode)})
        availableModes = temp
    }

    function currentModeLabel(mode)
    {
        if (mode && mode != usbSettings.MODE_UNDEFINED) {
            for (var i = 0; i < usbModes.length; i++) {
                var item = usbModes[i]
                if (item.mode == mode) {
                    if (item.menuText) {
                        return item.menuText
                    }
                    break;
                }
            }
        }
        //% "Currently not connected"
        return qsTrId("settings_usb-la-not_connected")
    }

    function createItems()
    {
        itemsPopulated = false
        availableModes = []

        var askFound = false
        var chargingFound = false
        var modes = usbSettings.supportedModes
        for (var i = 0; i < modes.length; i++) {
            var mode = modes[i]
            createItem(i, mode)
            chargingFound |= (mode == usbSettings.MODE_CHARGING)
            askFound |= (mode == usbSettings.MODE_ASK)
        }

        if (!chargingFound) {
            createItem(0, usbSettings.MODE_CHARGING)
        }
        if (!askFound) {
            createItem(0, usbSettings.MODE_ASK)
        }

        itemsPopulated = true
    }

    Component.onCompleted: createItems()
}

