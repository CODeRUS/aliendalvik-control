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
        property string action1: ""
        property string action2: ""
        property string action3: ""
        property string action4: ""
        property string action5: ""
        property string action6: ""
        property string applicationShortcut1: ""
        property string applicationShortcut2: ""
        property string applicationShortcut3: ""
        property string applicationShortcut4: applicationShortcut1
        property string applicationShortcut5: applicationShortcut2
        property string applicationShortcut6: applicationShortcut3
        property bool showHiddenShortcuts: false
    }
}


