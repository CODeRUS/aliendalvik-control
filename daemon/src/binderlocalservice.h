#ifndef BINDERLOCALSERVICE_H
#define BINDERLOCALSERVICE_H

#include "aliendalvikcontroller.h"
#include "loggingclasswrapper.h"

#include <QObject>

#include <gbinder.h>

class BinderLocalService: public AliendalvikController, public LoggingClassWrapper
{
    Q_OBJECT
public:
    explicit BinderLocalService(const char *serviceName,
                                const char *interfaceName,
                                QObject *parent = nullptr,
                                const char *loggingCategoryName = LOGGING(BinderLocalService)".interface");
    virtual ~BinderLocalService();

signals:
    void binderConnected();
    void binderDisconnected();

private slots:
    void serviceStopped();
    void serviceStarted();

protected:
    virtual GBinderLocalReply *onTransact(
            GBinderLocalObject *obj,
            GBinderRemoteRequest *req,
            guint code,
            guint flags,
            int *status) = 0;

private:
    void binderConnect();
    void binderDisconnect();

    static GBinderLocalReply *handler(
            GBinderLocalObject *obj,
            GBinderRemoteRequest *req,
            guint code,
            guint flags,
            int *status,
            void *user_data);

    const char *m_serviceName;
    const char *m_interfaceName;

    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderServiceName *m_binderServiceName = nullptr;
    GBinderLocalObject *m_localObject = nullptr;
    GBinderRemoteObject *m_remoteObject = nullptr;

};

#endif // BINDERLOCALSERVICE_H
