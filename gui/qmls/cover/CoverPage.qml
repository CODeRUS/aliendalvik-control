import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

CoverBackground {
    ShaderTiledBackground {}

    Label {
        id: label
        anchors.centerIn: parent
        text: qsTr("Powermenu\nconfigurator\n9002")
        width: parent.width
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }
}
