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
        }

        VerticalScrollDecorator {
            flickable: flick
        }
    }
}

