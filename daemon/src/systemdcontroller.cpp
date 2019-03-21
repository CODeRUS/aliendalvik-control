#include "systemdcontroller.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingReply>

#include <QCoreApplication>

#include <QDebug>

static const QString s_serviceName = QStringLiteral("org.freedesktop.systemd1");
static const QString s_servicePath = QStringLiteral("/org/freedesktop/systemd1");

static const QString s_getMethod = QStringLiteral("Get");
static const QString s_getUnitMethod = QStringLiteral("GetUnit");

static const QString s_propertiesSignal = QStringLiteral("PropertiesChanged");

static const QString s_propertiesIface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString s_unitIface = QStringLiteral("org.freedesktop.systemd1.Unit");
static const QString s_managerIface = QStringLiteral("org.freedesktop.systemd1.Manager");

static const QString s_propertyActiveState = QStringLiteral("ActiveState");

static const QString s_valueStateActive = QStringLiteral("active");
static const QString s_valueStateDeactivating = QStringLiteral("deactivating");

static QHash<QString, SystemdController*> s_instances;

SystemdController::SystemdController(const QString &unit, QObject *parent)
    : QObject(parent)
    , m_unit(unit)
{
    qDebug() << Q_FUNC_INFO << m_unit;

    QDBusMessage msg = QDBusMessage::createMethodCall(
                s_serviceName,
                s_servicePath,
                s_managerIface,
                s_getUnitMethod);
    msg.setArguments({m_unit});
    QDBusPendingReply<QDBusObjectPath> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    QDBusPendingCallWatcher *msgWatcher = new QDBusPendingCallWatcher(msgPending);
    connect(msgWatcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher){
        watcher->deleteLater();
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << Q_FUNC_INFO << reply.error().message();
            return;
        }
        const QDBusObjectPath objectPath = reply.value();
        m_path = objectPath.path();
        qDebug() << Q_FUNC_INFO << "Resolved" << m_unit << "to" << m_path;
        connectProperties();
        getActiveState();
    });
}

SystemdController::~SystemdController()
{
    qDebug() << Q_FUNC_INFO << m_unit;

    if (s_instances.contains(m_unit)) {
        s_instances.remove(m_unit);
    }
}

SystemdController *SystemdController::GetInstance(const QString &unit)
{
    qDebug() << Q_FUNC_INFO << unit;

    if (s_instances.contains(unit)) {
        return s_instances.value(unit);
    }

    SystemdController *instance = new SystemdController(unit, qApp);
    s_instances.insert(unit, instance);
    return instance;
}

void SystemdController::propertiesChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    const QString activeState = properties.value(s_propertyActiveState, QString()).toString();
    setActiveState(activeState);
}

void SystemdController::getActiveState()
{
    if (m_path.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Object path is empty!";
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(
                s_serviceName,
                m_path,
                s_propertiesIface,
                s_getMethod);
    msg.setArguments({s_unitIface, s_propertyActiveState});
    QDBusPendingReply<QVariant> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    QDBusPendingCallWatcher *msgWatcher = new QDBusPendingCallWatcher(msgPending);
    connect(msgWatcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher){
        watcher->deleteLater();
        QDBusPendingReply<QVariant> reply = *watcher;
        if (reply.isError()) {
            qWarning() << Q_FUNC_INFO << reply.error().message();
            return;
        }
        const QString activeState = reply.value().toString();
        qDebug() << Q_FUNC_INFO << "ActiveState:" << activeState;
        setActiveState(activeState, true);
    });
}

void SystemdController::setActiveState(const QString &activeState, bool init)
{
    if (m_activeState == activeState) {
        return;
    }
    if (activeState.isEmpty() || activeState.isNull()) {
        return;
    }
    m_activeState = activeState;
    if (m_activeState == s_valueStateActive) {
        emit serviceStarted();
    } else if (m_activeState == s_valueStateDeactivating && !init) {
        emit serviceStopped();
    }
}

void SystemdController::connectProperties()
{
    if (m_path.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Object path is empty!";
        return;
    }

    bool ok =
        QDBusConnection::systemBus().connect(
            QString(),
            m_path,
            s_propertiesIface,
            s_propertiesSignal,
            this,
            SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
    qDebug() << Q_FUNC_INFO << "Signal conncted" << s_propertiesSignal << ok;
}
