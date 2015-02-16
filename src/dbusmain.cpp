#include "dbusmain.h"
#include "adaptor.h"

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
    QString service("org.coderus.aliendalvik.control");
    bool success = QDBusConnection::sessionBus().registerService(service);
    if (!success) {
        QCoreApplication::exit(0);
        return;
    }

    Adaptor *adaptor = new Adaptor(this);

    QDBusConnection::sessionBus().registerObject("/", this);
}
