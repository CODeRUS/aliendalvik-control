import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

MouseArea
{
    id: root

    width: Screen.width
    height: Screen.height

    onClicked: {
        console.log("clicked")
        Qt.quit()
    }

    Rectangle {
        anchors.fill: parent
        color: "red"
        opacity: 0.2
    }

    DBusAdaptor {
        id: adaptor
        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikselector"
        path: "/"
        iface: "org.coderus.aliendalvikselector"

        xml: '  <interface name="' + iface + '">\n' +
             '    <method name="openUrl">\n' +
             '      <arg name="candidates" type="a(v)" direction="in"/>' +
             '    </method>\n' +
             '  </interface>\n'

        function openUrl(candidates) {
            console.log("openUrl called:", JSON.stringify(candidates))
        }
    }
}
