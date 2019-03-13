#include "adaptor.h"
#include "aliendalvikcontrol_interface.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

static const QString s_introspect_xml = QStringLiteral(
"  <interface name=\"org.coderus.aliendalvikcontrol\">\n"
"  </interface>\n"
);


static const QString c_dbus_service = QStringLiteral("org.coderus.aliendalvikcontrol");
static const QString c_dbus_path = QStringLiteral("/");

Adaptor::Adaptor(QObject *parent)
    : QDBusVirtualObject(parent)
    , m_iface(new DBusInterface(c_dbus_service, c_dbus_path, QDBusConnection::systemBus(), this))
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

    m_iface->componentActivity(interface, className, data);

    connection.send(message.createReply());
    return true;
}
