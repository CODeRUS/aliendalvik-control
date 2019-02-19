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
    MimeHandlerAdaptor *mimeHandlerAdaptor = new MimeHandlerAdaptor(this);

    QString service("org.coderus.aliendalvikcontrol");
    qDebug() << "Starting dbus service" << service << "...";
    bool success = QDBusConnection::sessionBus().registerService(service);
    if (!success) {
        qWarning() << "Register service fails!";
        mimeHandlerAdaptor->shareFile(QStringLiteral("/home/nemo/Pictures/Default/All_green.jpg"),
                                      QStringLiteral("image/jpeg"));
//        QCoreApplication::exit(0);
        return;
    }

    qDebug() << "Service registered successfully!";

    bool handler = QDBusConnection::sessionBus().registerVirtualObject("/", mimeHandlerAdaptor);
    if (handler) {
        qDebug() << "D-Bus handler registered successfully!";
    }
    else {
        qWarning() << "Register hanler fails!";
        QCoreApplication::exit(0);
        return;
    }
}
