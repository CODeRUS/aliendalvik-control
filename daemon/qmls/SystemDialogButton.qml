/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Petri M. Gerdt <petri.gerdt@jollamobile.com>
**
****************************************************************************/

import QtQuick 2.1
import Sailfish.Silica 1.0

MouseArea {
    id: button

    property alias text: label.text
    property alias iconSource: icon.source

    height: column.y + column.height + Theme.paddingMedium

    Rectangle {
        id: highlightBackground
        visible: parent.pressed && parent.containsMouse
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightDimmerColor, 0) }
            GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightDimmerColor, 0.2) }
        }
    }
    Column {
        id: column

        width: parent.width
        y: Theme.paddingLarge
        Image {
            id: icon
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Label {
            id: label
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
            }
            height: 2*dummyLabel.height
            Label {
                // work-around for getting height of one line
                id: dummyLabel
                text: parent.text
                visible: false
                maximumLineCount: 1
                font.pixelSize: Theme.fontSizeExtraSmall
            }
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.Wrap
            maximumLineCount: 2
            elide: Text.ElideRight
            color: Theme.rgba("black", 0.4)
        }
    }
}
