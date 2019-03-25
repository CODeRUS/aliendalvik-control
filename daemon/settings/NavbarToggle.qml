import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0
import com.jolla.settings 1.0

SettingsToggle {
    id: root

    DBusInterface {
        id: dbus

        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
    }

    name: "NavBar"
    icon.source: "image://theme/icon-m-aliendalvik-back"
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
