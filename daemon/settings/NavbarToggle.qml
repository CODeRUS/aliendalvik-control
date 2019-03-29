import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0
import com.jolla.settings 1.0

SettingsToggle {
    id: root

    property int apiVersion: 0

    DBusInterface {
        id: dbus

        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"

        Component.onCompleted: {
            dbus.typedCall("getApiVersion", [],
                           function(result) {
                               console.log("Api version:", result)
                               apiVersion = result
                           })
        }
    }

    name: "NavBar"
    icon.source: "image://theme/icon-m-aliendalvik-back" + (apiVersion <= 19 ? "4" : "8")
    checked: true

    onToggled: {
        checked = !checked
        if (checked) {
            dbus.typedCall("showNavBar", [])
        } else {
            dbus.typedCall("hideNavBar", [])
        }
    }
}
