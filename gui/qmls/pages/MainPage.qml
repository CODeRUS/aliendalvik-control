import QtQuick 2.1
import Sailfish.Silica 1.0
import org.coderus.powermenu.desktopfilemodel 1.0
import org.nemomobile.configuration 1.0
import ".."

Page {
    id: page
    objectName: "mainPage"

    function addShortcut(path) {
        var list = shortcutsConfig.value
        list.splice(list.length, 0, path)
        shortcutsConfig.value = list
    }

    SilicaListView {
        id: view
        anchors.fill: page
        delegate: shortcutDelegate
        model: desktopModel
        header: PageHeader {
            id: title
            title: qsTr("Selected shortcuts")
        }

        PullDownMenu {
            background: Component { ShaderTiledBackground {} }
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                }
            }

            MenuItem {
                text: qsTr("How to use")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("UsagePage.qml"))
                }
            }

            MenuItem {
                text: qsTr("Configuration")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ConfigurationPage.qml"))
                }
            }
        }

        PushUpMenu {
            background: Component { ShaderTiledBackground {} }
            MenuItem {
                text: qsTr("Add shortcut")
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: shortcutsConfig.value, showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.addShortcut)
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: view.count == 0
            text: qsTr("No shortcuts selected")

            Component {
                id: activeContent
                PulleyAnimationHint {
                    flickable: placeholder.flickable
                    width: parent.width
                    height: width
                    anchors.centerIn: parent
                    pushUpHint: true
                }
            }

            Component.onCompleted: {
                placeholder._content = activeContent.createObject(placeholder)
            }
        }

        VerticalScrollDecorator { flickable: view }
    }

    ConfigurationValue {
        id: shortcutsConfig
        key: "/apps/powermenu/shortcuts"
        defaultValue: []
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: shortcutsConfig.value
        onlySelected: true
        showHidden: true
    }

    Component {
        id: shortcutDelegate
        ListItem {
            id: item
            width: ListView.view.width
            contentHeight: 110
            ListView.onRemove: animateRemoval(item)
            menu: contextMenu

            function removeShortcut() {
                var itemname = model.path
                remorseAction(qsTr("Delete shortcut"),
                                     function() {
                                         var list = shortcutsConfig.value
                                         for (var i = 0; i < list.length; i++) {
                                            if (list[i] == itemname) {
                                                list.splice(i, 1);
                                                break;
                                            }
                                         }
                                         shortcutsConfig.value = list
                                     },
                                     3000
                )
            }

            Image {
                id: iconImage
                source: model.icon
                width: 86
                height: 86
                smooth: true
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
            }

            Label {
                text: model.name
                anchors {
                    left: iconImage.right
                    leftMargin: Theme.paddingMedium
                    verticalCenter: parent.verticalCenter
                }
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: {
                            removeShortcut()
                        }
                    }
                }
            }
        }
    }
}


