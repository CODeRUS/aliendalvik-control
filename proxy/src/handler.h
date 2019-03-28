#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>

class DBusInterface;
class Handler : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.aliendalvikcontrol")
public:
    explicit Handler(DBusInterface *iface, QObject *parent = nullptr);

public slots:
    void open(const QStringList &params);
    void openSelector(const QStringList &params);

private:
    DBusInterface *m_iface;
};

#endif // HANDLER_H
