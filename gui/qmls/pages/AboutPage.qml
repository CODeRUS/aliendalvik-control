import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

Page {
    id: page
    objectName: "aboutPage"

    Item {
        id: mainItem
        width: parent.width
        height: width
        anchors.bottom: flick.top
        anchors.bottomMargin: flick.contentY + 150
        clip: true

        Item {
            id: item

            x: Theme.paddingLarge
            height: img.height
            width: parent.width - (Theme.paddingLarge * 2)
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0 - height / 2

            Image {
                id: img
                source: "/usr/share/powermenu2/images/sailfish.png"
                property color color: Theme.highlightColor
                layer.effect: ShaderEffect {
                    id: shaderItem
                    property color color: Qt.rgba(colorTimer.cr, colorTimer.cg, colorTimer.cb, colorTimer.ca)

                    fragmentShader: "
                        varying mediump vec2 qt_TexCoord0;
                        uniform highp float qt_Opacity;
                        uniform lowp sampler2D source;
                        uniform highp vec4 color;
                        void main() {
                            highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                            gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                        }
                    "
                }
                layer.enabled: true
                layer.samplerName: "source"

                Timer {
                    id: colorTimer
                    running: true
                    interval: 50
                    repeat: true

                    property real cr: 1.0
                    property real cg: 0.0
                    property real cb: 0.0
                    property real ca: 0.5

                    property bool cru: true
                    property bool cgu: true
                    property bool cbu: true
                    property bool cau: true

                    onTriggered: {
                        if (cru) {
                            cr += 0.11
                        }
                        else {
                            cr -= 0.11
                        }
                        if (cr >= 1) {
                            cru = false
                        }
                        else if (cr <= 0) {
                            cru = true
                        }

                        if (cgu) {
                            cg += 0.12
                        }
                        else {
                            cg -= 0.12
                        }
                        if (cg >= 1) {
                            cgu = false
                        }
                        else if (cg <= 0) {
                            cgu = true
                        }

                        if (cbu) {
                            cb += 0.13
                        }
                        else {
                            cb -= 0.13
                        }
                        if (cb >= 1) {
                            cbu = false
                        }
                        else if (cb <= 0) {
                            cbu = true
                        }

                        if (cau) {
                            ca += 0.05
                        }
                        else {
                            ca -= 0.05
                        }
                        if (ca >= 1) {
                            cau = false
                        }
                        else if (ca <= 0.1) {
                            cau = true
                        }
                    }
                }
            }

            Timer {
                interval: 20
                repeat: true
                running: true
                onTriggered: {
                    if (item.rotation + 2 < 360)
                        item.rotation += 2
                    else
                        item.rotation = 0
                }
            }

        }
    }

    GlassItem {
        anchors.verticalCenter: mainItem.bottom
        height: 2*Theme.paddingLarge
        width: img.width*4
        x: 0 - (width / 2) + (img.width / 2) + Theme.paddingLarge
        radius: 0.7
        color: Qt.rgba(colorTimer.cr, colorTimer.cg, colorTimer.cb, colorTimer.ca)
        falloffRadius: 0.2
        brightness: 1.0
        visible: (item.rotation >= 0 && item.rotation < 30) || item.rotation > 330
    }

    GlassItem {
        anchors.verticalCenter: mainItem.bottom
        height: 2*Theme.paddingLarge
        width: img.width*4
        x: parent.width - (img.width / 2) - (width / 2) - Theme.paddingLarge
        radius: 0.7
        color: Qt.rgba(colorTimer.cr, colorTimer.cg, colorTimer.cb, colorTimer.ca)
        falloffRadius: 0.2
        brightness: 1.0
        visible: item.rotation > 150 && item.rotation < 210
    }

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
