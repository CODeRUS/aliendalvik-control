import QtQuick 2.1
import org.nemomobile.dbus 1.0
import org.nemomobile.configuration 1.0
import Sailfish.Silica 1.0
import org.coderus.desktopfilemodel 1.0

SystemWindow {
    id: powerMenuDialog

    property Item remorse
    property Item tiledBackground

    Component.onCompleted: {
        console.log("PowerMenuDialog Loaded!")

        var shaderComponent = Qt.createComponent("/usr/share/powermenu2/qml/ShaderTiledBackground.qml");
        tiledBackground = shaderComponent.createObject(powerMenuDialogBackground, {"color": Theme.primaryColor, "visible": configurationPowermenu.fancyBackground});
    }

    function hideDialog() {
        view.close()
    }

    function restart() {
        dsmeDbus.call("req_reboot", [])
        hideDialog()
    }

    function shutdown() {
        dsmeDbus.call("req_shutdown", [])
        hideDialog()
    }

    function remorseRestart() {
        if (!remorse) {
            remorse = remorseComponent.createObject(powerMenuDialog)
        }

        remorse.execute("Reboot phone",
                        function() {
                            powerMenuDialog.restart()
                        },
                        3000
        )
    }

    function remorseShutdown() {
        if (!remorse) {
            remorse = remorseComponent.createObject(powerMenuDialog)
        }

        remorse.execute("Shutdown phone",
                        function() {
                            powerMenuDialog.shutdown()
                        },
                        3000
        )
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightDimmerColor, 0.8)
    }

    Rectangle {
        id: powerMenuDialogBackground

        property int maximumHeight: powerMenuDialog.height - column.y - (configurationPowermenu.showShutdown ? (buttonRow.y + buttonRow.height) : 0) - Theme.itemSizeMedium
        width: parent.width
        height: column.height + column.y
        color: Theme.highlightBackgroundColor

        Column {
            id: column
            width: parent.width
            y: Theme.paddingLarge
            spacing: 0
            z: 1

            Label {
                text: "Power Menu"
                maximumLineCount: 2
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width

                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba("black", 0.6)
            }

            Row {
                id: buttonRow
                property real buttonWidth: width / 2
                width: parent.width
                visible: configurationPowermenu.showShutdown

                SystemDialogButton {
                    width: buttonRow.buttonWidth
                    text: "Shutdown"
                    iconSource: "image://theme/icon-l-power?#000000"
                    onClicked: {
                        powerMenuDialog.remorseShutdown()
                    }
                }

                SystemDialogButton {
                    width: buttonRow.buttonWidth
                    text: "Reboot"
                    iconSource: "image://theme/icon-l-backup?#000000"
                    onClicked: {
                        powerMenuDialog.remorseRestart()
                    }
                }
            }

            ListView {
                width: parent.width
                height: Math.min(contentHeight, powerMenuDialogBackground.maximumHeight)
                model: desktopModel
                interactive: contentHeight > height
                clip: true
                delegate: BackgroundItem {
                    id: item
                    width: parent.width
                    height: 100
                    highlightedColor: Theme.rgba(Theme.highlightDimmerColor, Theme.highlightBackgroundOpacity)

                    Image {
                        id: iconImage
                        source: model.icon
                        width: 80
                        height: 80
                        smooth: true
                        anchors {
                            left: parent.left
                            leftMargin: Theme.paddingLarge
                            verticalCenter: parent.verticalCenter
                        }
                    }

                    Label {
                        text: model.name
                        anchors {
                            left: iconImage.right
                            leftMargin: Theme.paddingMedium
                            verticalCenter: parent.verticalCenter
                        }
                        color: item.highlighted ? Theme.highlightColor : Theme.rgba(Theme.highlightDimmerColor, 0.8)
                    }

                    onClicked: {
                        Qt.openUrlExternally(model.path)
                        hideDialog()
                    }
                }
                VerticalScrollDecorator {}
            }
        }
    }

    MouseArea {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: powerMenuDialogBackground.bottom
        }

        onClicked: {
            hideDialog()
        }

        Image {
            width: parent.width
            source: "image://theme/graphic-system-gradient?" + Theme.highlightBackgroundColor
        }
    }

    Behavior on opacity { FadeAnimation {} }

    DBusInterface {
        id: dsmeDbus
        busType: DBusInterface.SystemBus
        destination: "com.nokia.dsme"
        path: "/com/nokia/dsme/request"
        iface: "com.nokia.dsme.request"
    }

    DBusInterface {
        id: mceDbus
        busType: DBusInterface.SystemBus
        destination: "com.nokia.mce"
        path: "/com/nokia/mce/signal"
        iface: "com.nokia.mce.signal"
        signalsEnabled: true
        function display_status_ind(status) {
            if (status == "off") {
                hideDialog()
            }
        }
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: configurationPowermenu.shortcuts
        onlySelected: true
        showHidden: true
    }

    ConfigurationGroup {
        id: configurationPowermenu
        path: "/apps/powermenu"
        property bool showShutdown: true
        property variant shortcuts
        property bool fancyBackground: true
        onFancyBackgroundChanged: tiledBackground.visible = fancyBackground
    }

    Component {
        id: remorseComponent

        RemorsePopup {
            z: 100
        }
    }
}
