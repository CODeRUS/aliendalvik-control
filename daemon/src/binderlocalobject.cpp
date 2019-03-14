#include "binderlocalobject.h"
#include "binderinterfaceabstract.h"

BinderLocalObject::BinderLocalObject(const char *name, BinderInterfaceAbstract *interface, QObject *parent, const char *loggingCategoryName)
    : QObject(parent)
    , LoggingClassWrapper(loggingCategoryName)
    , m_interface(interface)
{
    m_localHandler = gbinder_servicemanager_new_local_object(
                m_interface->manager(),
                name,
                &BinderLocalObject::onTransact,
                this);

    qCDebug(logging) << Q_FUNC_INFO << "Creating handler for" << name << m_localHandler;
}

BinderLocalObject::~BinderLocalObject()
{
    if (m_localHandler) {
        qCDebug(logging) << Q_FUNC_INFO << "Dropping handler" << m_localHandler;
        gbinder_local_object_drop(m_localHandler);
        m_localHandler = nullptr;
    }
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
