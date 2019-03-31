#include "adaptor.h"
#include "aliendalvikcontrol_interface.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

static const QString s_introspect_xml = QStringLiteral(
"  <interface name=\"org.coderus.aliendalvikcontrol\">\n"
"  </interface>\n"
);

Adaptor::Adaptor(DBusInterface *iface, QObject *parent)
    : QDBusVirtualObject(parent)
    , m_iface(iface)
{

}

Adaptor::~Adaptor()
{

}

QString Adaptor::introspect(const QString &) const
{
    return s_introspect_xml;
}

bool Adaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    const QString interface = message.interface();

    if (interface == QLatin1String("org.freedesktop.DBus.Introspectable")) {
        return false;
    }

    const QString member = message.member();
    const QString className = QString::fromLatin1(QByteArray::fromPercentEncoding(member.toLatin1().replace("_", "%")));
    const QVariantList dbusArguments = message.arguments();

    qDebug() << interface << member << dbusArguments;

    QString data;
    if (dbusArguments.size() == 1) {
        data = dbusArguments.first().toString();
    }

    m_iface->launcherActivity(interface, className, data);

    connection.send(message.createReply());
    return true;
}
