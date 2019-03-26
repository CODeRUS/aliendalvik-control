#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

class DBusInterface;
class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

public slots:
    void start();

private:
    DBusInterface *m_iface;
};

#endif // SERVICE_H
