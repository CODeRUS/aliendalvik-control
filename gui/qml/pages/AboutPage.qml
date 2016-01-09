import QtQuick 2.1
import Sailfish.Silica 1.0
import QtMultimedia 5.4
import harbour.folderplayer 1.0

import "../components"

Page {
    id: page

    property string path
    property string lowerPath

    Connections {
        target: player
        onPlaybackStateChanged: {
            if (player.playbackState == Audio.StoppedState) {
                player.source = folderListModel.getNextFile(player.path)
                player.play()
            }
        }
    }

    SilicaListView {
        id: listView

        model: FolderListModel {
            id: folderListModel
            path: page.path
        }

        anchors.fill: parent
        header: Column {
            width: page.width
            PageHeader {
                title: folderListModel.name
            }
            EntryItem {
                visible: !folderListModel.isRoot && folderListModel.path != page.lowerPath
                text: "[..]"
                bold: true
                onClicked: folderListModel.cdUp()
            }
        }

        delegate: EntryItem {
            id: delegate

            readonly property bool isPlaying: player.path == filepath
            highlighted: down || isPlaying

            text: isdir ? ("[" + filename + "]") : filename
            bold: isdir

            onClicked: {
                if (isdir) {
                    folderListModel.cd(filename)
                }
                else if (isPlaying) {
                    if (player.playbackState == Audio.PlayingState) {
                        player.pause()
                    }
                    else {
                        player.play()
                    }
                }
                else {
                    if (player.playbackState == Audio.PlayingState) {
                        player.stop()
                    }
                    player.source = filepath
                    player.play()
                }
            }
        }
        VerticalScrollDecorator {}
    }
}





