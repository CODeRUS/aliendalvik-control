import QtQuick 2.1
import Sailfish.Silica 1.0
import ".."

Page {
    id: page
    objectName: "configurationPage"

    property var actionsList: ["blank", "tklock", "devlock", "shutdown", "unblank", "tkunlock", "vibrate", "dbus1", "dbus2", "dbus3", "dbus4", "dbus5", "dbus6"]
    property var actionValues: ["power-key-menu", "double-power-key", "powermenu2", "flashlight", "screenshot"]

    function dummy() {
        qsTr("blank", "Blank display")
        qsTr("tklock", "Lock screen")
        qsTr("devlock", "Lock device")
        qsTr("shutdown", "Power off")
        qsTr("unblank", "Unblank display")
        qsTr("tkunlock", "Unlock screen")
        qsTr("vibrate", "Vibrate")
        qsTr("dbus1", "Action 1")
        qsTr("dbus2", "Action 2")
        qsTr("dbus3", "Action 3")
        qsTr("dbus4", "Action 4")
        qsTr("dbus5", "Action 5")
        qsTr("dbus6", "Action 6")

        qsTr("power-key-menu", "Sailfish Powermenu")
        qsTr("double-power-key", "Fast unlock")
        qsTr("powermenu2", "Powermenu2")
        qsTr("flashlight", "Flashlight")
        qsTr("screenshot", "Screenshot")
    }

    function selectShortcut1(path) {
        configurationPowermenu.applicationShortcut1 = path
    }

    function selectShortcut2(path) {
        configurationPowermenu.applicationShortcut2 = path
    }

    function selectShortcut3(path) {
        configurationPowermenu.applicationShortcut3 = path
    }

    function selectShortcut4(path) {
        configurationPowermenu.applicationShortcut4 = path
    }

    function selectShortcut5(path) {
        configurationPowermenu.applicationShortcut5 = path
    }

    function selectShortcut6(path) {
        configurationPowermenu.applicationShortcut6 = path
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
            var val = actsList[i]
            val = translateAction(val)
            tmp.splice(tmp.length, 0, val)
        }
        if (tmp.length > 0) {
            return tmp.join(", ")
        }
        else {
            return qsTr("No action")
        }
    }

    function translateAction(value) {
        var val = value
        if (val.substr(0,4) == "dbus") {
            if (val == "dbus1")
                val = helper.action1
            else if (val == "dbus2")
                val = helper.action2
            else if (val == "dbus3")
                val = helper.action3
            else if (val == "dbus4")
                val = helper.action4
            else if (val == "dbus5")
                val = helper.action5
            else if (val == "dbus6")
                val = helper.action6

            if (val.substr(0,5) == "event") {
                var tmp = ""
                if (val == "event1")
                    tmp = configurationPowermenu.applicationShortcut1
                else if (val == "event2")
                    tmp = configurationPowermenu.applicationShortcut2
                else if (val == "event3")
                    tmp = configurationPowermenu.applicationShortcut3
                else if (val == "event4")
                    tmp = configurationPowermenu.applicationShortcut4
                else if (val == "event5")
                    tmp = configurationPowermenu.applicationShortcut5
                else if (val == "event6")
                    tmp = configurationPowermenu.applicationShortcut6
                if (tmp.length > 0) {
                    val = helper.readDesktopName(tmp)
                }
                else {
                    val = qsTr(val)
                }
            }
            else {
                val = qsTr(val)
            }
        }
        else {
            val = qsTr(val)
        }
        return val
    }

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Restore to defaults")
                onClicked: helper.resetToDefaults()
            }
        }

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Configuration")
            }

            SectionHeader {
                text: qsTr("Powerkey configurations")
            }

            ComboBox {
                id: shortActionOn
                width: parent.width
                label: qsTr("Short press (display on)")
                value: translateActions(helper.shortPressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.shortPressActionOn.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.shortPressActionOn = updateActionList(helper.shortPressActionOn, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            ComboBox {
                id: shortActionOff
                width: parent.width
                label: qsTr("Short press (display off)")
                value: translateActions(helper.shortPressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.shortPressActionOff.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.shortPressActionOff = updateActionList(helper.shortPressActionOff, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            ComboBox {
                id: longActionOn
                width: parent.width
                label: qsTr("Long press (display on)")
                value: translateActions(helper.longPressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.longPressActionOn.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.longPressActionOn = updateActionList(helper.longPressActionOn, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            ComboBox {
                id: longActionOff
                width: parent.width
                label: qsTr("Long press (display off)")
                value: translateActions(helper.longPressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.longPressActionOff.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.longPressActionOff = updateActionList(helper.longPressActionOff, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            ComboBox {
                id: doubleActionOn
                width: parent.width
                label: qsTr("Double press (display on)")
                value: translateActions(helper.doublePressActionOn)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.doublePressActionOn.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.doublePressActionOn = updateActionList(helper.doublePressActionOn, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            ComboBox {
                id: doubleActionOff
                width: parent.width
                label: qsTr("Double press (display off)")
                value: translateActions(helper.doublePressActionOff)

                menu: ContextMenu {
                    Repeater {
                        model: actionsList.length
                        TextSwitch {
                            property var actions: helper.doublePressActionOff.split(",")
                            text: translateAction(actionsList[index])
                            checked: actions.indexOf(actionsList[index]) != -1
                            onClicked: helper.doublePressActionOff = updateActionList(helper.doublePressActionOff, actionsList[index])
                        }
                    }
                    onActiveChanged: if (active) flick.interactive = true
                }
            }

            SectionHeader {
                text: qsTr("Timings configurations")
            }

            Slider {
                width: parent.width
                label: qsTr("Longpress delay")
                minimumValue: 300
                maximumValue: 2000
                stepSize: 50
                value: helper.longPressDelay
                valueText: qsTr("%1ms.").arg(parseInt(value))
                onReleased: helper.longPressDelay = parseInt(value)
            }

            Slider {
                width: parent.width
                label: qsTr("Doublepress delay")
                minimumValue: 100
                maximumValue: 500
                stepSize: 50
                value: helper.doublePressDelay
                valueText: qsTr("%1ms.").arg(parseInt(value))
                onReleased: helper.doublePressDelay = parseInt(value)
            }

            SectionHeader {
                text: qsTr("Actions configurations")
            }

            ComboBox {
                id: actionCombo1
                width: parent.width
                label: qsTr("Action 1")
                value: qsTr(helper.action1)
                currentIndex: actionValues.indexOf(helper.action1) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event1", "Application 1")
                        onClicked: {
                            helper.action1 = "event1"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action1 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 1")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut1)
                visible: actionCombo1.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut1], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut1)
                }
            }

            ComboBox {
                id: actionCombo2
                width: parent.width
                label: qsTr("Action 2")
                value: qsTr(helper.action2)
                currentIndex: actionValues.indexOf(helper.action2) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event2", "Application 2")
                        onClicked: {
                            helper.action2 = "event2"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action2 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 2")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut2)
                visible: actionCombo2.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut2], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut2)
                }
            }

            ComboBox {
                id: actionCombo3
                width: parent.width
                label: qsTr("Action 3")
                value: qsTr(helper.action3)
                currentIndex: actionValues.indexOf(helper.action3) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event3", "Application 3")
                        onClicked: {
                            helper.action3 = "event3"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action3 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 3")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut3)
                visible: actionCombo3.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut3], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut3)
                }
            }

            ComboBox {
                id: actionCombo4
                width: parent.width
                label: qsTr("Action 4")
                value: qsTr(helper.action4)
                currentIndex: actionValues.indexOf(helper.action4) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event4", "Application 4")
                        onClicked: {
                            helper.action4 = "event4"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action4 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 4")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut4)
                visible: actionCombo4.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut4], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut4)
                }
            }

            ComboBox {
                id: actionCombo5
                width: parent.width
                label: qsTr("Action 5")
                value: qsTr(helper.action5)
                currentIndex: actionValues.indexOf(helper.action5) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event5", "Application 5")
                        onClicked: {
                            helper.action5 = "event5"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action5 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 5")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut5)
                visible: actionCombo5.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut5], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut5)
                }
            }

            ComboBox {
                id: actionCombo6
                width: parent.width
                label: qsTr("Action 6")
                value: qsTr(helper.action6)
                currentIndex: actionValues.indexOf(helper.action6) + 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("event6", "Application 6")
                        onClicked: {
                            helper.action6 = "event6"
                        }
                    }
                    Repeater {
                        model: actionValues.length
                        MenuItem {
                            text: qsTr(actionValues[index])
                            onClicked: {
                                helper.action6 = actionValues[index]
                            }
                        }
                    }
                }
            }

            ValueButton {
                label: qsTr("Application 6")
                value: helper.readDesktopName(configurationPowermenu.applicationShortcut6)
                visible: actionCombo6.currentIndex == 0
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.applicationShortcut6], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortcut6)
                }
            }

            SectionHeader {
                text: qsTr("Extra stuff")
            }

            ComboBox {
                width: parent.width
                label: qsTr("Show hidden shortcuts")
                description: qsTr("Inside settings selector only")
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
        }

        VerticalScrollDecorator { flickable: flick }
    }
}
