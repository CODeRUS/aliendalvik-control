import QtQuick 2.0
import Sailfish.Silica 1.0
import org.coderus.powermenu 1.0

GridView {
    id: grid
    property int sideMargin: (parent.width - (grid.cellWidth * Math.floor(parent.width / grid.cellWidth))) / 2
    x: sideMargin
    pixelAligned: true
    flickDeceleration: Theme.flickDeceleration
    maximumFlickVelocity: Theme.maximumFlickVelocity
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    cacheBuffer: cellHeight
    interactive: contentHeight > height
    cellWidth: Theme.itemSizeExtraLarge
    cellHeight: Theme.itemSizeLarge
    property bool editMode: false

    add: Transition {
        SequentialAnimation {
            NumberAnimation { properties: "z"; to: -1; duration: 1 }
            NumberAnimation { properties: "opacity"; to: 0.0; duration: 1 }
            NumberAnimation { properties: "x,y"; duration: 1 }
            NumberAnimation { properties: "z"; to: 0; duration: 200 }
            NumberAnimation { properties: "opacity"; from: 0.0; to: 1.0; duration: 100 }
        }
    }
    remove: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "z"; to: -1; duration: 1 }
            NumberAnimation { properties: "x"; to: 0; duration: 100 }
            NumberAnimation { properties: "opacity"; to: 0.0; duration: 100 }
        }
    }
    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 200 }
    }

    model: TogglesModel {
        id: gridModel
        editMode: grid.editMode
    }
    delegate: Item {
        id: gridDelegate
        width: grid.cellWidth
        height: grid.cellHeight
        clip: true

        Rectangle {
            id: itemDelegate
            width: gridDelegate.width
            height: gridDelegate.height
            color: actionDelegate.pressed
                   ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                   : "transparent"
            x: 0
            y: 0

            Loader {
                id: loaderDelegate
                anchors.fill: parent
                source: "/usr/lib/qt5/qml/org/coderus/powermenu/" + model.source
                property var sourceModel: model
                property bool hiddenProperty: gridModel.hidden.indexOf(model.path) >= 0
                property bool editMode: grid.editMode
            }

            MouseArea {
                id: actionDelegate
                anchors.fill: parent
                property int dragIndex: index
                onDragIndexChanged: {
                    if (drag.target) {
                        gridModel.move(index, dragIndex)
                    }
                }
                onPressed: {
                    if (!editMode) {
                        loaderDelegate.item.pressed = true
                    }
                }
                onClicked: {
                    if (editMode) {
                        if (!model.icon || model.icon.length == 0) {
                            gridModel.hideToggle(model.path)
                        }
                    }
                    else if (doubleTimer2.running) {
                        doubleTimer2.stop()
                        loaderDelegate.item.doubleClicked()
                        if (loaderDelegate.item.settingsPage && loaderDelegate.item.settingsPage.length > 0) {
                            window.disappearAnimation()
                        }
                    }
                    else {
                        doubleTimer2.restart()
                    }
                }
                Timer {
                    id: doubleTimer2
                    interval: 200
                    onTriggered: {
                        loaderDelegate.item.clicked()
                        if (model.icon && model.icon.length >= 0) {
                            window.disappearAnimation()
                        }
                    }
                }
                onReleased: {
                    if (drag.target) {
                        drag.target = null
                        itemDelegate.parent = gridDelegate
                        itemDelegate.x = 0
                        itemDelegate.y = 0
                    }
                    loaderDelegate.item.pressed = false
                }
                onPressAndHold: {
                    if (editMode) {
                        drag.target = itemDelegate
                        var newPos = mapToItem(grid, mouseX, mouseY)
                        itemDelegate.parent = grid
                        itemDelegate.x = newPos.x - itemDelegate.width / 2
                        itemDelegate.y = newPos.y - itemDelegate.height
                    }
                }
                onPositionChanged: {
                    if (drag.target) {
                        var targetIndex = grid.indexAt(itemDelegate.x + itemDelegate.width / 2, itemDelegate.y + itemDelegate.height / 2)
                        if (targetIndex >= 0) {
                            dragIndex = targetIndex
                        }
                    }
                }
                onContainsMouseChanged: {
                    if (!drag.target && !editMode) {
                        loaderDelegate.item.pressed = containsMouse
                    }
                }
            }
        }
    }
}

