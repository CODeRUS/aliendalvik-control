#ifndef BINDERINTERFACEABSTRACT_H
#define BINDERINTERFACEABSTRACT_H

#include <QObject>

#include <QSharedPointer>
#include <gbinder.h>

class Parcel;
class BinderInterfaceAbstract : public QObject
{
    Q_OBJECT
public:
    explicit BinderInterfaceAbstract(const char *serviceName,
                                     const char *interfaceName,
                                     QObject *parent = nullptr);
    virtual ~BinderInterfaceAbstract();

    QSharedPointer<Parcel> createTransaction();
    QSharedPointer<Parcel> sendTransaction(int code, QSharedPointer<Parcel> parcel, int *status);

    GBinderServiceManager *manager();
    GBinderLocalObject *localHandler();

    static GBinderLocalReply* onTransact(
            GBinderLocalObject *obj,
            GBinderRemoteRequest *req,
            guint code,
            guint flags,
            int *status,
            void *user_data);

public slots:
    void reconnect();

protected:
    virtual void registrationCompleted() = 0;

private:
    void binderConnect();
    void binderDisconnect();

    static void registrationHandler(GBinderServiceManager* sm, const char* name, void* user_data);
    void registerManager();

    static void deathHandler(GBinderRemoteObject *obj, void *user_data);

    const char *m_serviceName;
    const char *m_interfaceName;

    GBinderRemoteObject *m_remote = nullptr;
    GBinderLocalObject *m_local = nullptr;
    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderClient *m_client = nullptr;

    int m_registrationHandler = 0;
    int m_deathHandler = 0;
    int m_localHandler = 0;
};

#endif // BINDERINTERFACEABSTRACT_H
