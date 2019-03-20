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

    if (isServiceRunning()) {
        QTimer::singleShot(0, this, &AliendalvikController::serviceStarted);
    }
}

bool AliendalvikController::isServiceRunning() const
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
                QStringLiteral("org.freedesktop.systemd1"),
                QStringLiteral("/org/freedesktop/systemd1/unit/aliendalvik_2eservice"),
                QStringLiteral("org.freedesktop.DBus.Properties"),
                QStringLiteral("Get"));
    msg.setArguments({QStringLiteral("org.freedesktop.systemd1.Unit"), QStringLiteral("ActiveState")});
    QDBusReply<QVariant> reply = QDBusConnection::systemBus().call(msg);
    if (reply.error().type() != QDBusError::NoError) {
        return false;
    }
    const QString result = reply.value().toString();
    return result == QLatin1String("active");
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
