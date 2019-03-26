#include "adaptor.h"
#include "handler.h"
#include "service.h"
#include "aliendalvikcontrol_interface.h"

#include <QDBusConnection>
#include <QCoreApplication>
#include <QDebug>


static const QString c_dbus_service = QStringLiteral("org.coderus.aliendalvikcontrol");
static const QString c_dbus_path = QStringLiteral("/");

Service::Service(QObject *parent)
    : QObject(parent)
    , m_iface(new DBusInterface(c_dbus_service, c_dbus_path, QDBusConnection::systemBus(), this))
{

}

void Service::start()
{
    const QString service = QStringLiteral("org.coderus.aliendalvikcontrol");
    qDebug() << "Starting dbus service" << service << "...";
    bool success = QDBusConnection::sessionBus().registerService(service);
    if (!success) {
        qWarning() << "Register service failed!";
        QCoreApplication::exit(0);
        return;
    }
    qDebug() << "Service registered successfully!";

    Handler *urlHandler = new Handler(m_iface, this);
    const bool urlHandlerSuccess =
            QDBusConnection::sessionBus().registerObject(QStringLiteral("/urlHandler"), urlHandler, QDBusConnection::ExportAllSlots);
    if (!urlHandlerSuccess) {
        qWarning() << "Register urlHandler failed!";
        QCoreApplication::exit(0);
        return;
    }
    qDebug() << "urlHandler registered successfully!";

    Adaptor *adaptor = new Adaptor(m_iface, this);
    const bool adaptorSuccess =
            QDBusConnection::sessionBus().registerVirtualObject(QStringLiteral("/"), adaptor);
    if (adaptorSuccess) {
        qDebug() << "D-Bus handler registered successfully!";
    } else {
        qWarning() << "Register hanler failed!";
        QCoreApplication::exit(0);
        return;
    }
}
