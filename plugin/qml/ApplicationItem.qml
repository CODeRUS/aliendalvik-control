import QtQuick 2.0
import org.nemomobile.lipstick 0.1

IconItem {
    icon: sourceModel.icon
    name: sourceModel.name
    settingsPage: "applications/%1".arg(sourceModel.path.substr(sourceModel.path.lastIndexOf("/") + 1))

    onClicked: {
        launcher.launchApplication()
        //window.disappearAnimation()
    }

    LauncherItem {
        id: launcher
        filePath: sourceModel.path
    }
}

