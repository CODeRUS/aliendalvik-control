import QtQuick 2.1
import Sailfish.Silica 1.0
import QtMultimedia 5.4

import "pages"

ApplicationWindow
{
    initialPage: Component { WelcomePage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: Orientation.All
    _defaultPageOrientations: Orientation.All


    property Audio player: audioPlayer
    Audio {
        id: audioPlayer
        property string path: source.toString().replace("file://", "")
    }
}


