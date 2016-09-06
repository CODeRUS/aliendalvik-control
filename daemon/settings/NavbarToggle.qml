import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0

Switch {
    id: root
    property string entryPath

    DBusInterface {
        id: dbus

        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
    }

    icon.source: "/usr/share/jolla-settings/pages/aliendalvikcontrol/icon-m-aliendalvik-back.png"
    checked: true
    automaticCheck: true
    onClicked: {
        if (checked) {
            dbus.typedCall("showNavBar", [])
        }
        else {
            dbus.typedCall("hideNavBar", [])
        }
    }

    Behavior on opacity { FadeAnimation { } }
}
