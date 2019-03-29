#ifndef BINDERINTERFACEABSTRACT_H
#define BINDERINTERFACEABSTRACT_H

#include "../common/src/aliendalvikcontroller.h"
#include "../common/src/loggingclasswrapper.h"

#include <QObject>

#include <QSharedPointer>
#include <gbinder.h>

class Parcel;
class BinderInterfaceAbstract : public AliendalvikController, public LoggingClassWrapper
{
    Q_OBJECT
public:
    explicit BinderInterfaceAbstract(const char *serviceName,
                                     const char *interfaceName,
                                     QObject *parent = nullptr,
                                     const char *loggingCategoryName = LOGGING(BinderInterfaceAbstract)".interface");
    virtual ~BinderInterfaceAbstract();

    bool isBinderReady() const;
    bool wait(quint64 timeout = 10000);

    QSharedPointer<Parcel> createTransaction();
    QSharedPointer<Parcel> sendTransaction(int code, QSharedPointer<Parcel> parcel, int *status);

    GBinderServiceManager *manager();

    static GBinderLocalReply* onTransact(
            GBinderLocalObject *obj,
            GBinderRemoteRequest *req,
            guint code,
            guint flags,
            int *status,
            void *user_data);

private slots:
    void serviceStopped();
    void serviceStarted();

signals:
    void binderConnected();
    void binderDisconnected();

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
