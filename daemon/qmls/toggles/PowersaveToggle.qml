import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0
import org.freedesktop.contextkit 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: batteryChargePercentageContextProperty.value === undefined ? "" : (batteryChargePercentageContextProperty.value + "%")
    icon: "image://theme/icon-m-battery"
    active: enablePowersave || forcePowersave
    settingsPage: "system_settings/info/powersave"

    onClicked: {
        console.log("cicked")
        if (forcePowersave) {
            mceRequestIface.setValue(key_powersave_force, false)
            mceRequestIface.setValue(key_powersave_enable, false)
        }
        else if (enablePowersave) {
            mceRequestIface.setValue(key_powersave_force, true)
        }
        else {
            mceRequestIface.setValue(key_powersave_enable, true)
        }
    }

    property string key_powersave_enable: "/system/osso/dsm/energymanagement/enable_power_saving"
    property string key_powersave_force: "/system/osso/dsm/energymanagement/force_power_saving"

    readonly property bool enablePowersave: values[key_powersave_enable]
    readonly property bool forcePowersave: values[key_powersave_force]

    property var values: {
        "/system/osso/dsm/energymanagement/enable_power_saving": true,
        "/system/osso/dsm/energymanagement/force_power_saving": true
    }

    Image {
        id: psmIcon
        source: "image://theme/icon-status-powersave"
        cache: true
        anchors.centerIn: iconItem
        anchors.horizontalCenterOffset: Math.min(iconItem.width, iconItem.height) / 2 - width / 2
        anchors.verticalCenterOffset: Math.min(iconItem.width, iconItem.height) / 2 - height / 2
        visible: systemPowerSaveModeContextProperty.value
    }

    Label {
        anchors.left: psmIcon.right
        anchors.verticalCenter: psmIcon.verticalCenter
        visible: !forcePowersave && enablePowersave
        text: "A"
    }

    ContextProperty {
        id: batteryChargePercentageContextProperty
        key: "Battery.ChargePercentage"
    }

    ContextProperty {
        id: systemPowerSaveModeContextProperty
        key: "System.PowerSaveMode"
    }

    DBusInterface {
        id: mceRequestIface
        service: 'com.nokia.mce'
        path: '/com/nokia/mce/request'
        iface: 'com.nokia.mce.request'
        bus: DBus.SystemBus

        function setValue(key, value) {
            typedCall('set_config', [{"type":"s", "value":key}, {"type":"v", "value":value}])
        }

        function getValue(key) {
            typedCall('get_config', [{"type":"s", "value":key}], function (value) {
                var temp = values
                temp[key] = value
                values = temp
            })
        }

        Component.onCompleted: {
            getValue(key_powersave_enable)
            getValue(key_powersave_force)
        }
    }

    DBusInterface {
        id: mceSignalIface
        service: 'com.nokia.mce'
        path: '/com/nokia/mce/signal'
        iface: 'com.nokia.mce.signal'
        bus: DBus.SystemBus

        signalsEnabled: true

        function config_change_ind(key, value) {
            if (key in values) {
                var temp = values
                temp[key] = value
                values = temp
            }
        }
    }
}
