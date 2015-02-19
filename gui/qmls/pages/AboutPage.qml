import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

Page {
    id: page
    objectName: "aboutPage"

    SilicaFlickable {
        id: flick
        anchors.fill: page
        boundsBehavior: Flickable.DragAndOvershootBounds
        contentHeight: column.height

        ShaderTiledBackground {
            anchors.topMargin: - page.height
            anchors.bottomMargin: - page.height
        }

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("About Powermenu 2")
            }

            Label {
                text: "v" + Qt.application.version
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Label {
                text: "Fancy menu on power key\nwritten by coderus in 0x7DE\nis dedicated to my beloved"
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: helper.bannerPath
                asynchronous: true
                cache: true
            }

            Label {
                text: "We accept donations via"
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Button {
                text: "PayPal"
                width: 300
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    Qt.openUrlExternally("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=ovi.coderus%40gmail%2ecom&lg=en&lc=US&item_name=Donation%20for%20coderus%20powermenu%20EUR&no_note=0&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest")
                }
            }

            Label {
                text: "Me and my beloved would be grateful for every cent.\nYour donations makes application better and i can spend more time for development."
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Label {
                text: "Thanks to Morpog for application icon."
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Button {
                text: qsTr("Activate product")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    codeField.visible = true
                    codeField.forceActiveFocus()
                }
            }

            TextField {
                id: codeField
                width: parent.width
                inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase
                placeholderText: qsTr("Enter your PayPal e-mail")
                label: qsTr("PayPal e-mail")
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: {
                    helper.checkActivation(text.toLowerCase())
                    page.forceActiveFocus()
                    codeField.visible = false
                }
                visible: false
            }
        }

        VerticalScrollDecorator {}
    }
}
