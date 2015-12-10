import QtQuick 2.0
import Sailfish.Silica 1.0
import org.coderus.powermenu 1.0

MainWindow {
    id: window

    Image {
        id: image
        width: parent.width
        height: parent.height
        source: screenshotPath
        anchors.centerIn: parent
    }

    NumberAnimation {
        id: disappear
        target: image
        properties: "width,height,opacity"
        to: 0
        duration: 1000
        onStopped: Screenshot.deleteView(view)
    }

    Component.onCompleted: {
        disappear.start()
    }
}

