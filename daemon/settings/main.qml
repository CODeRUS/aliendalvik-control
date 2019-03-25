import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

Page {
    id: page

    DBusInterface {
        id: dbus

        bus: DBus.SystemBus
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
        contentHeight: content.height

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: "Aliendalvik Control"
            }

            SectionHeader {
                text: "Launch"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Downloads"
                onClicked: {
                    dbus.call("openDownloads", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Settings"
                onClicked: {
                    dbus.call("openSettings", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Contacts"
                onClicked: {
                    dbus.call("openContacts", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Gallery"
                onClicked: {
                    dbus.call("openGallery", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Camera"
                onClicked: {
                    dbus.call("openCamera", [])
                }
            }

            SectionHeader {
                text: "Navigation bar"
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium
                height: Theme.itemSizeExtraSmall

                Button {
                    text: "Hide"
                    onClicked: {
                        dbus.call("hideNavBar", [])
                    }
                }

                Button {
                    text: "Show"
                    onClicked: {
                        dbus.call("showNavBar", [])
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

