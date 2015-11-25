import QtQuick 2.1
import Sailfish.Silica 1.0

MouseArea {
    id: root
    height: parent.height
    width: height
    preventStealing: true

    property alias iconSource: image.source
    property bool down: pressed && containsMouse
    property bool highlighted: down
    property alias title: label.text
    opacity: enabled ? 1.0 : Theme.highlightBackgroundOpacity

    Rectangle {
        anchors.fill: parent
        color: highlighted
               ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
               : "transparent"
    }

    Image {
        id: image

        property color highlightColor: Theme.highlightColor
        property bool highlighted: root.highlighted
        property string _highlightSource

        anchors.top: parent.top
        anchors.bottom: label.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.paddingLarge
        anchors.bottomMargin: Theme.paddingSmall
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
        cache: true
        smooth: true

        source: highlighted ? _highlightSource : source

        _highlightSource: {
            if (source != "") {
                var tmpSource = image.source.toString()
                var index = tmpSource.lastIndexOf("?")
                if (index !== -1) {
                    tmpSource = tmpSource.substring(0, index)
                }
                return tmpSource + "?" + highlightColor
            } else {
                return ""
            }
        }
    }

    Label {
        id: label
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingMedium
        anchors.left: parent.left
        anchors.right: parent.right
        font.pixelSize: Theme.fontSizeSmall
        color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        horizontalAlignment: Text.AlignHCenter
    }
}

