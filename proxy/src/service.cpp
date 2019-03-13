#include "adaptor.h"
#include "service.h"

#include <QDBusConnection>
#include <QCoreApplication>
#include <QDebug>

Service::Service(QObject *parent)
    : QObject(parent)
{

}

void Service::start()
{
    const QString service = QStringLiteral("org.coderus.aliendalvikcontrol");
    qDebug() << "Starting dbus service" << service << "...";
    bool success = QDBusConnection::sessionBus().registerService(service);
    if (!success) {
        qWarning() << "Register service fails!";
        QCoreApplication::exit(0);
        return;
    }

    qDebug() << "Service registered successfully!";

    Adaptor *adaptor = new Adaptor(this);
    bool handler = QDBusConnection::sessionBus().registerVirtualObject(QStringLiteral("/"), adaptor);
    if (handler) {
        qDebug() << "D-Bus handler registered successfully!";
    } else {
        qWarning() << "Register hanler fails!";
        QCoreApplication::exit(0);
        return;
    }
}
