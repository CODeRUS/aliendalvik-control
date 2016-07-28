import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0

Page {
    id: page

    DBusInterface {
        id: dbus

        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikcontrol"
        path: "/"
        iface: "org.coderus.aliendalvikcontrol"

        signalsEnabled: true

        function imeAvailable(imeList) {
            imeRepeater.model = imeList
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Activating) {
            dbus.call("getImeList", [])
        }
    }

    SilicaFlickable {
        id: flick

        anchors.fill: parent

        Column {
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: "Aliendalvik Control"
            }

            SectionHeader {
                text: "Reset android handler"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "HTTP"
                onClicked: {
                    dbus.call("uriActivitySelector", ["https://openrepos.net"])
                }
            }

            SectionHeader {
                text: "Downloads"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Open"
                onClicked: {
                    dbus.call("openDownloads", [])
                }
            }

            SectionHeader {
                text: "Navigation bar"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Hide"
                onClicked: {
                    dbus.call("hideNavBar", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Show"
                onClicked: {
                    dbus.call("showNavBar", [])
                }
            }

            SectionHeader {
                text: "Input methods"
            }

            Repeater {
                id: imeRepeater
                width: parent.width
                model: []
                delegate: Component {
                    BackgroundItem {
                        width: parent.width
                        Label {
                            anchors.centerIn: parent
                            width: parent.width - Theme.horizontalPageMargin * 2
                            text: modelData
                        }
                        onClicked: {
                            dbus.call("setImeMethod", [modelData])
                        }
                    }
                }
            }

            Item {
                width: 1; height: Theme.itemSizeSmall
            }
        }

        VerticalScrollDecorator {
            flickable: flick
        }
    }
}

