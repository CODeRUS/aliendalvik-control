import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

Switch {
    id: root
    property string entryPath

    DBusInterface {
        id: dbus

        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
    }

    iconSource: "/usr/share/jolla-settings/pages/aliendalvikcontrol/icon-m-aliendalvik-back.png"
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
