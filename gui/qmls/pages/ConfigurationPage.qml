import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

Page {
    id: page
    objectName: "configurationPage"

    property var actionsList: ["blank", "tklock", "devlock", "shutdown", "unblank", "tkunlock", "dbus1", "dbus2"]

    function dummy() {
        qsTr("blank")
        qsTr("tklock")
        qsTr("devlock")
        qsTr("shutdown")
        qsTr("unblank")
        qsTr("tkunlock")
        qsTr("dbus1")
        qsTr("dbus2")
    }

    function selectShortcut1(path) {
        configurationPowermenu.applicationShortcut1 = path
    }

    function selectShortcut2(path) {
        configurationPowermenu.applicationShortcut2 = path
    }

    function updateActionList(actions, action) {
        var temp = actions.split(",")
        if (temp.indexOf(action) < 0) {
            var temp2 = []
            for (var i = 0; i < actionsList.length; i++) {
                if (temp.indexOf(actionsList[i]) != -1 || actionsList[i] == action) {
                    temp2.splice(temp2.length, 0, actionsList[i])
                }
            }
            temp = temp2
        }
        else {
            temp.splice(temp.indexOf(action), 1)
        }
        return temp.join(",")
    }

    function translateActions(acts) {
        var tmp = []
        var actsList = acts.split(",")
        for (var i = 0; i < actsList.length; i++) {
            tmp.splice(tmp.length, 0, qsTr(actsList[i]))
        }
        if (tmp.length > 0) {
            return tmp.join(", ")
        }
        else {
            return qsTr("No action")
        }
    }

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: column.height

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Configuration")
            }

            ComboBox {
                id: shortActionOn
                width: parent.width
                label: qsTr("Shorttap action display on")
                value: translateActions(helper.shortPressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.shortPressActionOn.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.shortPressActionOn = updateActionList(helper.shortPressActionOn, actionsList[index])
                        }
                    }
                }
            }

            ComboBox {
                id: shortActionOff
                width: parent.width
                label: qsTr("Shorttap action display off")
                value: translateActions(helper.shortPressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.shortPressActionOff.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.shortPressActionOff = updateActionList(helper.shortPressActionOff, actionsList[index])
                        }
                    }
                }
            }

            ComboBox {
                id: longActionOn
                width: parent.width
                label: qsTr("Longtap action display on")
                value: translateActions(helper.longPressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.longPressActionOn.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.longPressActionOn = updateActionList(helper.longPressActionOn, actionsList[index])
                        }
                    }
                }
            }

            ComboBox {
                id: longActionOff
                width: parent.width
                label: qsTr("Longtap action display off")
                value: translateActions(helper.longPressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.longPressActionOff.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.longPressActionOff = updateActionList(helper.longPressActionOff, actionsList[index])
                        }
                    }
                }
            }

            ComboBox {
                id: doubleActionOn
                width: parent.width
                label: qsTr("Doubletap action display on")
                value: translateActions(helper.doublePressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.doublePressActionOn.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.doublePressActionOn = updateActionList(helper.doublePressActionOn, actionsList[index])
                        }
                    }
                }
            }

            ComboBox {
                id: doubleActionOff
                width: parent.width
                label: qsTr("Doubletap action display off")
                value: translateActions(helper.doublePressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: 8
                        TextSwitch {
                            property var actions: helper.doublePressActionOff.split(",")
                            text: qsTr(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.doublePressActionOff = updateActionList(helper.doublePressActionOff, actionsList[index])
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut1)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut1], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut1)
                }
            }

            /*ValueButton {
                label: qsTr("Application 2")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut2)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut2], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut2)
                }
            }*/

            ComboBox {
                width: parent.width
                label: qsTr("Shutdown controls")
                currentIndex: configurationPowermenu.showShutdown ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Shown")
                        onClicked: {
                            configurationPowermenu.showShutdown = true
                        }
                    }
                    MenuItem {
                        text: qsTr("Hidden")
                        onClicked: {
                            configurationPowermenu.showShutdown = false
                        }
                    }
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Show hidden shortcuts")
                currentIndex: configurationPowermenu.showHiddenShortcuts ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Yes")
                        onClicked: {
                            configurationPowermenu.showHiddenShortcuts = true
                        }
                    }
                    MenuItem {
                        text: qsTr("No")
                        onClicked: {
                            configurationPowermenu.showHiddenShortcuts = false
                        }
                    }
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Fancy menu background")
                currentIndex: configurationPowermenu.fancyBackground ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Yes")
                        onClicked: {
                            configurationPowermenu.fancyBackground = true
                        }
                    }
                    MenuItem {
                        text: qsTr("No")
                        onClicked: {
                            configurationPowermenu.fancyBackground = false
                        }
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
