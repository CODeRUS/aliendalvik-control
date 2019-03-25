#include "binderlocalservice.h"

#include <QTimer>

BinderLocalService::BinderLocalService(const char *serviceName,
                                       const char *interfaceName,
                                       QObject *parent,
                                       const char *loggingCategoryName)
    : AliendalvikController(parent)
    , LoggingClassWrapper(loggingCategoryName)
    , m_serviceName(serviceName)
    , m_interfaceName(interfaceName)
{
}

BinderLocalService::~BinderLocalService()
{
    binderDisconnect();
}

void BinderLocalService::serviceStopped()
{
    qCDebug(logging) << Q_FUNC_INFO;
    binderDisconnect();
}

void BinderLocalService::serviceStarted()
{
    qCDebug(logging) << Q_FUNC_INFO;
    binderConnect();
}

void BinderLocalService::binderConnect()
{
    qCDebug(logging) << Q_FUNC_INFO << "Binder connect" << m_serviceName << m_interfaceName;

    if (!m_serviceManager) {
        qCWarning(logging) << Q_FUNC_INFO << "Creating service manager for" << binderDevice();
        m_serviceManager = gbinder_servicemanager_new(binderDevice());
    }

    if (!m_serviceManager) {
        qCCritical(logging) << Q_FUNC_INFO << "Can't create service manager!";
        return;
    }

    if (!m_localObject) {
        qCWarning(logging) << Q_FUNC_INFO << "Creating local object";
        m_localObject = gbinder_servicemanager_new_local_object(
                    m_serviceManager,
                    m_interfaceName,
                    &BinderLocalService::handler,
                    this);
    }

    if (!m_binderServiceName) {
        qCWarning(logging) << Q_FUNC_INFO << "Creating service name";
        m_binderServiceName = gbinder_servicename_new(m_serviceManager, m_localObject, m_serviceName);
    }

    emit binderConnected();
}

void BinderLocalService::binderDisconnect()
{
    qCWarning(logging) << Q_FUNC_INFO << "Binder disconnect" << m_serviceName << m_interfaceName;

    if (m_binderServiceName) {
        qCWarning(logging) << Q_FUNC_INFO << "Removing service name";
        gbinder_servicename_unref(m_binderServiceName);
        m_binderServiceName = nullptr;
    }

    if (m_localObject) {
        qCWarning(logging) << Q_FUNC_INFO << "Removing local obbject";
        gbinder_local_object_drop(m_localObject);
        m_localObject = nullptr;
    }

    if (m_serviceManager) {
        qCWarning(logging) << Q_FUNC_INFO << "Removing service manager";
        gbinder_servicemanager_unref(m_serviceManager);
        m_serviceManager = nullptr;
    }

    emit binderDisconnected();
}

GBinderLocalReply *BinderLocalService::handler(GBinderLocalObject *obj,
                                               GBinderRemoteRequest *req,
                                               guint code,
                                               guint flags,
                                               int *status,
                                               void *user_data)
{
    BinderLocalService *instance = static_cast<BinderLocalService *>(user_data);
    return instance->onTransact(obj, req, code, flags, status);
}
