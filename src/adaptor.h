#ifndef ADAPTOR_H
#define ADAPTOR_H

#include <QObject>
#include <QDBusAbstractAdaptor>

class Adaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.aliendalvik.control")

public:
    explicit Adaptor(QObject *parent = 0);
    virtual ~Adaptor();

public slots:
    void sendKeyevent(int code);

};

#endif // ADAPTOR_H
