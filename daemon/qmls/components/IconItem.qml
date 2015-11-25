import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    anchors.fill: parent

    property alias name: label.text
    property alias icon: image.source
    property bool active: false
    property bool pressed: false
    property bool highlighted: !window.editMode && (active || pressed)
    property bool disabled: false
    property bool hidden: false

    property alias iconItem: image
    property alias labelItem: label

    property string settingsPage

    enabled: window.editMode ? !hidden : !disabled

    onDoubleClicked: {
        window.showSettingsPage(settingsPage)
    }

    signal clicked
    signal doubleClicked

    opacity: enabled ? 1.0 : Theme.highlightBackgroundOpacity

    Rectangle {
        id: highlightItem
        anchors.fill: parent
        color: root.highlighted
               ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity / (root.active ? 2 : 1))
               : "transparent"
    }

    Image {
        id: image
        anchors.top: parent.top
        anchors.bottom: label.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.paddingMedium
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        fillMode: Image.PreserveAspectFit
        smooth: true
        cache: true
        layer.effect: ShaderEffect {
            property color color: Theme.highlightColor

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
        layer.enabled: root.highlighted
        layer.samplerName: "source"
    }

    Label {
        id: label
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        font.pixelSize: Theme.fontSizeExtraSmall
        color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        horizontalAlignment: Text.AlignHCenter
    }
}
