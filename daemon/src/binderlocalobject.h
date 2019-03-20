#ifndef BINDERLOCALOBJECT_H
#define BINDERLOCALOBJECT_H

#include "aliendalvikcontroller.h"
#include "loggingclasswrapper.h"

#include <gbinder.h>

#include <QObject>

class BinderLocalObject : public AliendalvikController , public LoggingClassWrapper
{
    Q_OBJECT
public:
    explicit BinderLocalObject(const char *name,
                               QObject *parent = nullptr,
                               const char *loggingCategoryName = LOGGING(BinderLocalObject)".interface");
    virtual ~BinderLocalObject();

    GBinderLocalObject *localObject() const;

    static GBinderLocalReply* onTransact(
            GBinderLocalObject *,
            GBinderRemoteRequest *req,
            guint code,
            guint,
            int *status,
            void *user_data);

private slots:
    void serviceStopped();
    void serviceStarted();

private:
    void binderConnect();
    void binderDisconnect();

    const char *m_serviceName;

    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderLocalObject *m_localHandler = nullptr;

};

#endif // BINDERLOCALOBJECT_H
