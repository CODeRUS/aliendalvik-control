import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0
import Nemo.DBus 2.0
import Nemo.Configuration 1.0

MouseArea
{
    id: root

    width: Screen.width
    height: Screen.height

    property string url

    onClicked: {
        Qt.quit()
    }

    ConfigurationGroup {
        id: config
        path: "/org/coderus/aliendalvikcontrol/selector"
        property int windowPosX: -1
        property int windowPosY: -1
    }

    Wallpaper {
        id: bg
        x: config.windowPosX >= 0 ? config.windowPosX : (root.width - bg.width) / 2
        y: config.windowPosY >= 0 ? config.windowPosY : (root.height - bg.height) / 2
        width: parent.width - Theme.horizontalPageMargin * 2
        height: Math.min(Theme.itemSizeMedium * 5, contentView.contentHeight)
        clip: true
        blending: true

        Rectangle {
            anchors.fill: parent
            border.width: 1
            border.color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            color: "transparent"
        }

        SilicaListView {
            id: contentView
            anchors.fill: parent
            header: Component {
                MouseArea {
                    width: parent.width
                    height: Theme.itemSizeMedium

                    drag.minimumX: 0
                    drag.maximumX: root.width - bg.width
                    drag.minimumY: 0
                    drag.maximumY: root.height - Theme.itemSizeMedium * 5

                    onPressAndHold: {
                        drag.target = bg
                    }

                    onReleased: {
                        drag.target = undefined
                        config.windowPosX = bg.x
                        config.windowPosY = bg.y
                        console.log("### saving posx:", config.windowPosX, "posy:", config.windowPosY)
                    }

                    onCanceled: {
                        drag.target = undefined
                    }

                    Column {
                        width: parent.width
                        anchors.centerIn: parent

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Open url with"
                        }

                        Label {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: Theme.horizontalPageMargin
                            truncationMode: TruncationMode.Fade
                            text: root.url
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.secondaryColor
                        }
                    }
                }
            }
            delegate: Component {
                BackgroundItem {
                    id: content
                    width: ListView.view.width

                    Image {
                        id: icon
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        sourceSize.width: Theme.itemSizeSmall - Theme.paddingMedium
                        sourceSize.height: Theme.itemSizeSmall - Theme.paddingMedium
                        source: "data:image/png;base64," + modelData.icon
                    }

                    Label {
                        anchors.left: icon.right
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.prettyName
                        color: content.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }

                    onClicked: {
                        console.log("###", modelData.packageName)
                        control.call("uriLaunchActivity", [modelData.packageName, modelData.className, modelData.launcherClass, modelData.data])

                        Qt.quit()
                    }
                }
            }
            VerticalScrollDecorator {}
        }
    }

    DBusAdaptor {
        id: adaptor
        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikselector"
        path: "/"
        iface: "org.coderus.aliendalvikselector"

        xml: '  <interface name="' + iface + '">\n' +
             '    <method name="openUrl">\n' +
             '      <arg name="url" type="s" direction="in"/>' +
             '      <arg name="candidates" type="a(v)" direction="in"/>' +
             '    </method>\n' +
             '  </interface>\n'

        function openUrl(url, candidates) {
            root.url = url
            contentView.model = candidates
        }
    }

    DBusInterface {
        id: control
        bus: DBus.SystemBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"
    }
}
