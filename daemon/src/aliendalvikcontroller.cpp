#include "aliendalvikcontroller.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QTimer>

AliendalvikController::AliendalvikController(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::systemBus().connect(
                QString(),
                QStringLiteral("/org/freedesktop/systemd1/unit/aliendalvik_2eservice"),
                QStringLiteral("org.freedesktop.DBus.Properties"),
                QStringLiteral("PropertiesChanged"),
                this, SLOT(aliendalvikChanged(QString, QVariantMap, QStringList)));

    QDBusMessage msg = QDBusMessage::createMethodCall(
                QStringLiteral("org.freedesktop.systemd1"),
                QStringLiteral("/org/freedesktop/systemd1/unit/aliendalvik_2eservice"),
                QStringLiteral("org.freedesktop.DBus.Properties"),
                QStringLiteral("Get"));
    QDBusPendingReply<QVariant> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    QDBusPendingCallWatcher *msgWatcher =new QDBusPendingCallWatcher(msgPending);
    connect(msgWatcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher){
        watcher->deleteLater();
        QDBusPendingReply<QVariant> reply = *watcher;
        if (reply.isError()) {
            return;
        }
        const QString result = reply.value().toString();
        const bool isActive = result == QLatin1String("active");
        if (isActive) {
            serviceStarted();
        }
    });
}

void AliendalvikController::aliendalvikChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    const QString activeState = properties.value(QStringLiteral("ActiveState")).toString();
    if (activeState == QLatin1String("active")) {
        serviceStarted();
    } else if (activeState == QLatin1String("deactivating")) {
        serviceStopped();
    }
}
