import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0
import Nemo.Configuration 1.0

Page {
    id: page

    property int apiVersion: 0
    onApiVersionChanged: {
        if (apiVersion <= 19) {
            dbus.call("getImeList", [])

            dbus.typedCall("getSettings", [{"type": "s", "value": "system"},
                                           {"type": "s", "value": "sound_effects_enabled"}],
                           function(value) {
                               touchSoundsSwitch.checked = value == "1"
                               touchSoundsSwitch.enabled = true
                           })

            dbus.typedCall("getSettings", [{"type": "s", "value": "secure"},
                                           {"type": "s", "value": "mock_location"}],
                           function(value) {
                               mockLocationSwitch.checked = value == "1"
                               mockLocationSwitch.enabled = true
                           })

            dbus.typedCall("getSettings", [{"type": "s", "value": "global"},
                                           {"type": "s", "value": "install_non_market_apps"}],
                           function(value) {
                               nonMarketSwitch.checked = value == "1"
                               nonMarketSwitch.enabled = true
                           })
        }
    }

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

        Component.onCompleted: {
            dbus.typedCall("guessApiVersion", [],
                           function(result) {
                               console.log("Api version:", result)
                               apiVersion = result
                           })
        }
    }

    ConfigurationGroup {
        id: edgeConfiguration
        path: "/org/coderus/aliendalvikcontrol/edge"
        property bool active: false
        property bool leftHanded: false
        onLeftHandedChanged: {
            if (!isActive) {
                return
            }

            setTouchRegionDelayed.start()
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
                visible: apiVersion >= 27
                onClicked: {
                    dbus.call("openSettings", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Contacts"
                visible: apiVersion >= 27
                onClicked: {
                    dbus.call("openContacts", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Gallery"
                visible: apiVersion >= 27
                onClicked: {
                    dbus.call("openGallery", [])
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Camera"
                visible: apiVersion >= 27
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
                visible: apiVersion <= 19
            }

            TextSwitch {
                id: touchSoundsSwitch
                width: parent.width
                text: "Touch sounds"
                checked: false
                enabled: false
                visible: apiVersion <= 19
                onClicked: {
                    dbus.typedCall("putSettings", [{"type": "s", "value": "system"},
                                                   {"type": "s", "value": "sound_effects_enabled"},
                                                   {"type": "s", "value": checked ? "1" : "0"}])
                }
            }

            TextSwitch {
                id: mockLocationSwitch
                width: parent.width
                text: "Allow mock location"
                checked: false
                enabled: false
                visible: apiVersion <= 19
                onClicked: {
                    dbus.typedCall("putSettings", [{"type": "s", "value": "secure"},
                                                   {"type": "s", "value": "mock_location"},
                                                   {"type": "s", "value": checked ? "1" : "0"}])
                }
            }

            TextSwitch {
                id: nonMarketSwitch
                width: parent.width
                text: "Allow install non-market apps"
                checked: false
                enabled: false
                visible: apiVersion <= 19
                onClicked: {
                    dbus.typedCall("putSettings", [{"type": "s", "value": "global"},
                                                   {"type": "s", "value": "install_non_market_apps"},
                                                   {"type": "s", "value": checked ? "1" : "0"}])
                }
            }

            SectionHeader {
                text: "Input methods"
                visible: apiVersion <= 19
            }

            Repeater {
                id: imeRepeater
                width: parent.width
                model: []
                visible: apiVersion <= 19
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

            SectionHeader {
                text: "Onehand control"
            }

            TextSwitch {
                id: edgeActiveSwitch
                text: "Enable onehand control"
                checked: edgeConfiguration.active
                onClicked: edgeConfiguration.active = checked

            }

            TextSwitch {
                id: edgeLeftHandedSwitch
                text: "Left handed control"
                visible: edgeActiveSwitch.checked
                checked: edgeConfiguration.leftHanded
                onClicked: edgeConfiguration.leftHanded = checked
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

