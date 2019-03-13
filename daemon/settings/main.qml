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
                text: "Reset android handler"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "HTTP"
                onClicked: {
                    dbus.call("uriActivitySelector", ["http://openrepos.net"])
                }
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

            SectionHeader {
                text: "Android settings"
            }

            TextSwitch {
                width: parent.width
                text: "Allow mock location"
                checked: false
                enabled: false
                onClicked: {
                    dbus.typedCall("putSettings", [{"type": "s", "value": "secure"},
                                                   {"type": "s", "value": "mock_location"},
                                                   {"type": "s", "value": checked ? "1" : "0"}])
                }
                Component.onCompleted: {
                    dbus.typedCall("getSettings", [{"type": "s", "value": "secure"},
                                                   {"type": "s", "value": "mock_location"}],
                                   function(value) {
                                       checked = value == 1
                                       enabled = true
                                   })
                }
            }

            TextSwitch {
                width: parent.width
                text: "Allow install non-market apps"
                checked: false
                enabled: false
                onClicked: {
                    dbus.typedCall("putSettings", [{"type": "s", "value": "global"},
                                                   {"type": "s", "value": "install_non_market_apps"},
                                                   {"type": "s", "value": checked ? "1" : "0"}])
                }
                Component.onCompleted: {
                    dbus.typedCall("getSettings", [{"type": "s", "value": "global"},
                                                   {"type": "s", "value": "install_non_market_apps"}],
                                   function(value) {
                                       checked = value == 1
                                       enabled = true
                                   })
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
                    ListItem {
                        property bool imeEnabled: modelData.enabled
                        highlighted: down || imeEnabled
                        showMenuOnPressAndHold: imeEnabled
                        width: parent.width
                        menu: contextMenu
                        contentHeight: imeLabel.height
                        Component {
                            id: contextMenu
                            ContextMenu {
                                MenuItem {
                                    text: "Select"
                                    onClicked: {
                                        dbus.call("setImeMethod", [modelData.name])
                                    }
                                }
                            }
                        }
                        Label {
                            id: imeLabel
                            x: Theme.horizontalPageMargin
                            width: parent.width - Theme.horizontalPageMargin * 2
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            text: modelData.name
                        }
                        onClicked: {
                            dbus.call("triggerImeMethod", [modelData.name, !imeEnabled])
                            imeEnabled = !imeEnabled
                            dbus.call("getImeList", [])
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

