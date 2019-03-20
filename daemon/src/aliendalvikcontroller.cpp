#include "aliendalvikcontroller.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QTimer>

static const QString s_serviceName = QStringLiteral("org.freedesktop.systemd1");
static const QString s_servicePath = QStringLiteral("/org/freedesktop/systemd1/unit/aliendalvik_2eservice");
static const QString s_serviceIface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString s_serviceMethod = QStringLiteral("Get");
static const QString s_serviceSignal = QStringLiteral("PropertiesChanged");

static const QString s_propertyIface = QStringLiteral("org.freedesktop.systemd1.Unit");

static const QString s_propertyName = QStringLiteral("ActiveState");

static const QLatin1String s_valueStateActive = QLatin1String("active");
static const QLatin1String s_valueStateDeactivating = QLatin1String("deactivating");

AliendalvikController::AliendalvikController(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::systemBus().connect(
                QString(),
                s_servicePath,
                s_serviceIface,
                s_serviceSignal,
                this,
                SLOT(aliendalvikChanged(QString, QVariantMap, QStringList)));

    QDBusMessage msg = QDBusMessage::createMethodCall(
                s_serviceName,
                s_servicePath,
                s_serviceIface,
                s_serviceMethod);
    msg.setArguments({s_propertyIface, s_propertyName});
    QDBusPendingReply<QVariant> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    QDBusPendingCallWatcher *msgWatcher = new QDBusPendingCallWatcher(msgPending);
    connect(msgWatcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher){
        watcher->deleteLater();
        QDBusPendingReply<QVariant> reply = *watcher;
        if (reply.isError()) {
            qWarning() << Q_FUNC_INFO << reply.error().message();
            return;
        }
        const QString result = reply.value().toString();
        const bool isActive = result == s_valueStateActive;
        if (isActive) {
            serviceStarted();
        }
    });
}

void AliendalvikController::aliendalvikChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    const QString activeState = properties.value(s_propertyName).toString();
    if (activeState == s_valueStateActive) {
        serviceStarted();
    } else if (activeState == s_valueStateDeactivating) {
        serviceStopped();
    }
}
