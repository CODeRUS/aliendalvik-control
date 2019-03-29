#include "binderlocalobject.h"
#include "binderinterfaceabstract.h"

BinderLocalObject::BinderLocalObject(const char *name, QObject *parent, const char *loggingCategoryName)
    : AliendalvikController(parent)
    , LoggingClassWrapper(loggingCategoryName)
    , m_serviceName(name)
{
}

BinderLocalObject::~BinderLocalObject()
{
    binderDisconnect();
}

GBinderLocalObject *BinderLocalObject::localObject() const
{
    return m_localHandler;
}

GBinderLocalReply *BinderLocalObject::onTransact(GBinderLocalObject *, GBinderRemoteRequest *req, guint code, guint, int *status, void *user_data)
{
    BinderLocalObject *binderLocalObject = static_cast<BinderLocalObject*>(user_data);
    const char *iface = gbinder_remote_request_interface(req);
    qCDebug(binderLocalObject->logging) << Q_FUNC_INFO << code << iface;

    *status = GBINDER_STATUS_OK;

    GBinderLocalReply *reply = nullptr;
    return reply;
}

void BinderLocalObject::serviceStopped()
{
    qCDebug(logging) << Q_FUNC_INFO;
    binderDisconnect();
}

void BinderLocalObject::serviceStarted()
{
    qCDebug(logging) << Q_FUNC_INFO;
    binderConnect();
}

void BinderLocalObject::binderConnect()
{
    qCDebug(logging) << Q_FUNC_INFO << "Creating service manager for" << binderDevice();
    m_serviceManager = gbinder_servicemanager_new(binderDevice());

    if (!m_serviceManager) {
        qCCritical(logging) << Q_FUNC_INFO << "Can't create service manager!";
        return;
    }

    m_localHandler = gbinder_servicemanager_new_local_object(
                m_serviceManager,
                m_serviceName,
                &BinderLocalObject::onTransact,
                this);

    qCDebug(logging) << Q_FUNC_INFO << "Creating handler for" << m_serviceName << m_localHandler;
}

void BinderLocalObject::binderDisconnect()
{
    if (m_localHandler) {
        qCDebug(logging) << Q_FUNC_INFO << "Dropping handler" << m_localHandler;
        gbinder_local_object_drop(m_localHandler);
        m_localHandler = nullptr;
    }
}
