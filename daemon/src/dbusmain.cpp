#include "dbusmain.h"
#include "mimehandleradaptor.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusError>
#include <QCoreApplication>

DBusMain::DBusMain(QObject *parent) :
    QObject(parent)
{
}

void DBusMain::start()
{
    QString service("org.coderus.aliendalvikcontrol");
    qDebug() << "Starting dbus service" << service << "...";
    bool success = QDBusConnection::sessionBus().registerService(service);
    if (!success) {
        qWarning() << "Register service fails!";
        QCoreApplication::exit(0);
        return;
    }

    qDebug() << "Service registered successfully!";

    MimeHandlerAdaptor *mimeHandlerAdaptor = new MimeHandlerAdaptor(this);

    bool handler = QDBusConnection::sessionBus().registerVirtualObject("/", mimeHandlerAdaptor);
    if (handler) {
        qDebug() << "Handler registered successfully!";
    }
    else {
        qWarning() << "Register hanler fails!";
        QCoreApplication::exit(0);
        return;
    }
}
