#ifndef BINDERLOCALOBJECT_H
#define BINDERLOCALOBJECT_H

#include "loggingclasswrapper.h"

#include <gbinder.h>

#include <QObject>

class BinderLocalObject : public QObject , public LoggingClassWrapper
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

private:
    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderLocalObject *m_localHandler = nullptr;
};

#endif // BINDERLOCALOBJECT_H
