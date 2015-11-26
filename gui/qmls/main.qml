import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import "pages"

ApplicationWindow
{
    initialPage: Component { MainPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    property alias configurationPowermenu: configuration
    ConfigurationGroup {
        id: configuration
        path: "/apps/powermenu"
        property string applicationShortcut1: ""
        property string applicationShortcut2: ""
        property string applicationShortcut3: ""
        property bool showHiddenShortcuts: false
    }
}


