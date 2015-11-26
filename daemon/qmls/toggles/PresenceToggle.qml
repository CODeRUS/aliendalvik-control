import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Contacts 1.0
import org.nemomobile.contacts 1.0
import "../components"

ToggleItem {
    id: root
    anchors.fill: parent

    name: PresenceListener.presenceStateText(PresenceListener.globalPresenceState)
    icon: "image://theme/icon-m-presence"
    settingsPage: "system_settings/connectivity/presence"
    disabled: PresenceListener.globalPresenceState == Person.PresenceUnknown

    onClicked: {
        if (PresenceListener.globalPresenceState == Person.PresenceOffline) {
            presenceUpdate.setGlobalPresence(Person.PresenceAway)
        }
        else if (PresenceListener.globalPresenceState == Person.PresenceAway) {
            presenceUpdate.setGlobalPresence(Person.PresenceAvailable)
        }
        else if (PresenceListener.globalPresenceState == Person.PresenceAvailable) {
            presenceUpdate.setGlobalPresence(Person.PresenceOffline)
        }
    }

    ContactPresenceUpdate {
        id: presenceUpdate
    }
}
