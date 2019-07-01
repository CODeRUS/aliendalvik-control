import QtQuick 2.1
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

ApplicationWindow
{
    id: appWindow
    cover: Qt.resolvedUrl("aliendalvik-control-share-cover.qml")

    property string titleText: "Share from Android"

    BusyIndicator {
        id: busyInicator
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: visible
        visible: false

        Timer {
            id: busyTimer
            running: true
            repeat: false
            interval: 300
            onTriggered: {
                if (pageStack.currentPage == null) {
                    busyInicator.visible = true
                }
            }
        }
    }

    Component {
       id: expiredPage
       Page {
           id: mainPage

           SilicaFlickable {
               anchors.fill: parent
               contentHeight: content.height

               Column {
                   id: content
                   width: parent.width

                   PageHeader {
                       title: titleText
                   }
               }

               ViewPlaceholder {
                   enabled: visible
                   text: "Application started without share parameters"
               }
           }
       }
    }

    Timer {
        id: expireTimer
        running: true
        repeat: false
        interval: 1000
        onTriggered: {
            busyInicator.visible = false
            pageStack.animatorPush(expiredPage)
        }
    }

    DBusAdaptor {
        id: adaptor
        bus: DBus.SessionBus
        service: "org.coderus.aliendalvikshare"
        path: "/"
        iface: "org.coderus.aliendalvikshare"

        xml: '  <interface name="' + iface + '">\n' +
             '    <method name="share">\n' +
             '      <arg name="mimeType" type="s" direction="in"/>' +
             '      <arg name="data" type="s" direction="in"/>' +
             '      <arg name="fileName" type="s" direction="in"/>' +
             '    </method>\n' +
             '  </interface>\n'

        function share(mimeType, data, fileName) {
            console.log("share called:", mimeType, data, fileName)
            busyInicator.visible = false
            expireTimer.stop()

            appWindow.activate()
            pageStack.clear()

            var shareData = {
                "mimeType": mimeType
            }

            if (fileName && fileName.length > 0) {
                shareData["source"] = Qt.resolvedUrl(fileName)
            } else {
                shareData["data"] = data
            }

            pageStack.animatorPush(
                "Sailfish.TransferEngine.SharePage", shareData)
        }
    }
}
