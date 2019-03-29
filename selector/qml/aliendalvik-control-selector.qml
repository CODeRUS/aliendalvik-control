import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0
import Nemo.DBus 2.0

MouseArea
{
    id: root

    width: Screen.width
    height: Screen.height

    property string url

    onClicked: {
        Qt.quit()
    }

    Wallpaper {
        anchors.centerIn: parent
        width: parent.width - Theme.horizontalPageMargin * 2
        height: Math.min(Theme.itemSizeMedium * 5, contentView.contentHeight)
        clip: true

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
                Item {
                    width: parent.width
                    height: Theme.itemSizeMedium

                    Column {
                        width: parent.width
                        anchors.centerIn: parent

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Open url with"
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
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
                        control.call("uriActivity", [modelData.packageName, modelData.className, modelData.launcherClass, modelData.data])

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
            console.log("openUrl called:", url, JSON.stringify(candidates))
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
