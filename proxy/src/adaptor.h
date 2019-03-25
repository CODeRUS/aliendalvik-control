#ifndef ADAPTOR_H
#define ADAPTOR_H

#include <QDBusVirtualObject>

class DBusInterface;
class QDBusConnection;
class QDBusMessage;

class Adaptor : public QDBusVirtualObject
{
public:
    explicit Adaptor(QObject *parent = nullptr);
    virtual ~Adaptor();

    QString introspect(const QString &) const override;
    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection) override;

private:
    DBusInterface *m_iface;
};

#endif // ADAPTOR_H
