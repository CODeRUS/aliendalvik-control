#include "binderinterfaceabstract.h"

#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(binderinterface, "binder.interface", QtDebugMsg)

void BinderInterfaceAbstract::registrationHandler(GBinderServiceManager *sm, const char *name, void *user_data)
{
    qCDebug(binderinterface) << Q_FUNC_INFO << "Service registered" << sm << name << user_data;
    BinderInterfaceAbstract* instance = static_cast<BinderInterfaceAbstract *>(user_data);
    instance->registerManager();
}

void BinderInterfaceAbstract::deathHandler(GBinderRemoteObject *obj, void *user_data)
{
    qCDebug(binderinterface) << Q_FUNC_INFO << "Binder died" << obj << user_data;
    BinderInterfaceAbstract* instance = static_cast<BinderInterfaceAbstract *>(user_data);
    instance->binderDisconnect();
}

BinderInterfaceAbstract::BinderInterfaceAbstract(const char *serviceName, const char *interfaceName, QObject *parent)
    : QObject(parent)
    , m_serviceName(serviceName)
    , m_interfaceName(interfaceName)
{
    binderConnect();
}

BinderInterfaceAbstract::~BinderInterfaceAbstract()
{
    binderDisconnect();
}

Parcel *BinderInterfaceAbstract::createTransaction()
{
    qCDebug(binderinterface) << Q_FUNC_INFO;

    if (!m_client) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "No client!";
        return nullptr;
    }

    GBinderLocalRequest* req = gbinder_client_new_request(m_client);
    return new Parcel(req);
}

void BinderInterfaceAbstract::sendTransaction(int code, Parcel *parcel, int *status)
{
    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply
            (m_client, code, parcel->request(), status);

    gbinder_remote_reply_unref(reply);
}

void BinderInterfaceAbstract::binderConnect()
{
    qCDebug(binderinterface) << Q_FUNC_INFO << "Binder connect";

    if (!m_serviceManager) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Creating service manager";
        m_serviceManager = gbinder_servicemanager_new("/dev/puddlejumper");
    }

    if (!m_serviceManager) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "Can't create service manager!";
        return;
    }


    qCDebug(binderinterface) << Q_FUNC_INFO << "Add registration handler";
    m_registrationHandler = gbinder_servicemanager_add_registration_handler(m_serviceManager,
                                                                            m_serviceName,
                                                                            &BinderInterfaceAbstract::registrationHandler,
                                                                            this);
}

void BinderInterfaceAbstract::binderDisconnect()
{
    qCDebug(binderinterface) << Q_FUNC_INFO << "Binder disconnect";

    if (m_client) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing client";
        gbinder_client_unref(m_client);
        m_client = nullptr;
    }

    if (m_deathHandler && m_remote) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing death handler";
        gbinder_remote_object_remove_handler(m_remote, m_deathHandler);
        m_deathHandler = 0;
    }

    if (m_remote) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing remote";
        gbinder_remote_object_unref(m_remote);
        m_remote = nullptr;
    }

    if (m_registrationHandler && m_serviceManager) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing registration handler";
        gbinder_servicemanager_remove_handler(m_serviceManager, m_registrationHandler);
        m_registrationHandler = 0;
    }

    if (m_serviceManager) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing service manager";
        gbinder_servicemanager_unref(m_serviceManager);
        m_serviceManager = nullptr;
    }
}

void BinderInterfaceAbstract::registerManager()
{
    qCDebug(binderinterface) << Q_FUNC_INFO << "Register manager";

    int status;
    m_remote = gbinder_remote_object_ref(
                gbinder_servicemanager_get_service_sync(m_serviceManager,
                                                        m_serviceName,
                                                        &status));

    qCDebug(binderinterface) << Q_FUNC_INFO << "Service status:" << status;

    if (!m_remote) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "No remote!";
        binderDisconnect();
        return;
    }

    m_deathHandler = gbinder_remote_object_add_death_handler(m_remote,
                                                             &BinderInterfaceAbstract::deathHandler,
                                                             this);

    m_client = gbinder_client_new(m_remote, m_interfaceName);

    if (!m_client) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "No client!";
        binderDisconnect();
        return;
    }

    qCDebug(binderinterface) << Q_FUNC_INFO << "Removing registration handler";

    gbinder_servicemanager_remove_handler(m_serviceManager, m_registrationHandler);
    m_registrationHandler = 0;
}

Parcel::Parcel(GBinderLocalRequest *request)
    : m_request(request)
    , m_writer(new GBinderWriter)
{
    qCDebug(binderinterface) << Q_FUNC_INFO << this << m_request;

    gbinder_local_request_init_writer(m_request, m_writer);
}

Parcel::~Parcel()
{
    qCDebug(binderinterface) << Q_FUNC_INFO << this << m_request;

    gbinder_local_request_unref(m_request);
}

void Parcel::writeStrongBinder(GBinderLocalObject *value)
{
    if (!m_writer) {
        return;
    }

    gbinder_writer_append_local_object(m_writer, value);
}

void Parcel::writeString(const QString &value)
{
    if (!m_writer) {
        return;
    }

    if (value.isNull()) {
        gbinder_writer_append_string16_utf16(m_writer, NULL, 0);
    } else {
        gbinder_writer_append_string16_utf16(m_writer, value.utf16(), value.length());
    }
}

void Parcel::writeInt(int value)
{
    gbinder_writer_append_int32(m_writer, value);
}

void Parcel::writeBundle(const QVariantHash &value)
{
    gsize lengthPos = gbinder_writer_bytes_written(m_writer);
    writeInt(-1); // dummy, will hold length
    writeInt(BUNDLE_MAGIC);
    gsize startPos = gbinder_writer_bytes_written(m_writer);

    // writeArrayMapInternal
    writeInt(value.size());
    QHash<QString, QVariant>::const_iterator it = value.begin();
    while (it != value.end()) {
        qDebug() << it.key() << it.value().toString();
        writeString(it.key());
        writeValue(it.value());
        ++it;
    }

    gsize endPos = gbinder_writer_bytes_written(m_writer);
    gsize length = endPos - startPos;
    gbinder_writer_overwrite_int32(m_writer, lengthPos, length);
}

void Parcel::writeValue(const QVariant &value)
{
    switch (value.type()) {
    case QMetaType::QUrl:
        writeInt(static_cast<int>(VAL_PARCELABLE));
        writeString(value.toString());
        break;
    default:
        qCCritical(binderinterface) << Q_FUNC_INFO << "Unsupported value type:" << QMetaType::typeName(value.type()) << value.type();
    }
}

GBinderLocalRequest *Parcel::request()
{
    return m_request;
}
