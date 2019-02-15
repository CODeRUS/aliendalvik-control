#include "binderinterfaceabstract.h"

#include <QDebug>
#include <QEventLoop>
#include <QLoggingCategory>
#include <QTimer>

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
        binderConnect();

        QEventLoop loop;
        QTimer timer;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(2000);
        loop.exec();
    }

    if (!m_client) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "No client!";
        return nullptr;
    }

    GBinderLocalRequest* req = gbinder_client_new_request(m_client);
    return new Parcel(req);
}

Parcel *BinderInterfaceAbstract::sendTransaction(int code, Parcel *parcel, int *status)
{
    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply
            (m_client, code, parcel->request(), status);

    return new Parcel(reply);
}

GBinderServiceManager *BinderInterfaceAbstract::manager()
{
    return m_serviceManager;
}

GBinderLocalObject *BinderInterfaceAbstract::localHandler()
{
    return m_local;
}

GBinderLocalReply *BinderInterfaceAbstract::onTransact(GBinderLocalObject *obj, GBinderRemoteRequest *req, guint code, guint flags, int *status, void *user_data)
{
    const char *iface = gbinder_remote_request_interface(req);
    qCDebug(binderinterface) << Q_FUNC_INFO << iface << obj << req << code << flags << status << user_data;
    GBinderLocalReply *reply = NULL;
    *status = GBINDER_STATUS_OK;
    return reply;
}

void BinderInterfaceAbstract::reconnect()
{
    qCDebug(binderinterface) << Q_FUNC_INFO;

    if (m_client) {
        return;
    }

    binderConnect();
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

    if (m_localHandler) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing local handler";
        gbinder_servicemanager_cancel(m_serviceManager, m_localHandler);
        m_localHandler = 0;
    }

    if (m_local) {
        qCDebug(binderinterface) << Q_FUNC_INFO << "Removing local";
        gbinder_local_object_drop(m_local);
        m_local = nullptr;
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

//    m_local = gbinder_servicemanager_new_local_object(m_serviceManager,
//                                                      "android.app.IApplicationThread",
//                                                      BinderInterfaceAbstract::onTransact,
//                                                      this);

//    m_localHandler = gbinder_servicemanager_add_service_sync(m_serviceManager,
//                                            "activity",
//                                            m_local);

//    qCDebug(binderinterface) << Q_FUNC_INFO << "Local handler created:" /*<< m_localHandler*/ << m_local;

    registrationCompleted();
}

Parcel::Parcel(GBinderLocalRequest *request)
    : m_request(request)
    , m_writer(new GBinderWriter)
{
    qCDebug(binderinterface) << Q_FUNC_INFO << this << m_request << m_writer;

    gbinder_local_request_init_writer(m_request, m_writer);
}

Parcel::Parcel(GBinderRemoteReply *reply)
    : m_reply(reply)
    , m_reader(new GBinderReader)
{
    qCDebug(binderinterface) << Q_FUNC_INFO << this << m_reply << m_reader;

    gbinder_remote_reply_init_reader(m_reply, m_reader);
}

Parcel::~Parcel()
{
    qCDebug(binderinterface) << Q_FUNC_INFO << this << m_request;

    if (m_request) {
        gbinder_local_request_unref(m_request);
    }

    if (m_reply) {
        gbinder_remote_reply_unref(m_reply);
    }
}

void Parcel::writeStrongBinder(GBinderLocalObject *value)
{
    if (!m_writer) {
        return;
    }

    gbinder_writer_append_local_object(m_writer, value);
}

void Parcel::writeStrongBinder(GBinderRemoteObject *value)
{
    if (!m_writer) {
        return;
    }

    gbinder_writer_append_remote_object(m_writer, value);
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
    if (!m_writer) {
        return;
    }

    gbinder_writer_append_int32(m_writer, value);
}

void Parcel::writeBundle(const QVariantHash &value)
{
    if (!m_writer) {
        return;
    }

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
    if (!m_writer) {
        return;
    }

    switch (value.type()) {
    case QMetaType::QUrl:
        writeInt(static_cast<int>(VAL_PARCELABLE));
        // writeParcelableCreator
        writeString(QStringLiteral("android.net.Uri"));
        writeInt(static_cast<int>(StringUri_TYPE_ID));
        writeString(value.toString());
        break;
    case QMetaType::QString:
        writeInt(static_cast<int>(VAL_STRING));
        writeString(value.toString());
        break;
    default:
        qCCritical(binderinterface) << Q_FUNC_INFO << "Unsupported value type:" << QMetaType::typeName(value.type()) << value.type();
    }
}

int Parcel::readInt() const
{
    int result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_int32(m_reader, &result)) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

QString Parcel::readString() const
{
    QString result;
    if (!m_reader) {
        return result;
    }

    gsize length;
    const ushort* string = gbinder_reader_read_string16_utf16(m_reader, &length);
    if (!string) {
        return result;
    }

    return QString::fromUtf16(string, length);
}

void Parcel::readBundle() const
{
    const int length = readInt();
    qCDebug(binderinterface) << Q_FUNC_INFO << "Bundle length:" << length;
}

qlonglong Parcel::readLong() const
{
    qlonglong result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_int64(m_reader, &result)) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

bool Parcel::readBoolean() const
{
    return readInt() != 0;
}

double Parcel::readDouble() const
{
    double result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_double(m_reader, &result)) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

QStringList Parcel::readStringList() const
{
    QStringList result;
    if (!m_reader) {
        return result;
    }
    const int length = readInt();
    for (int i = 0; i < length; i++) {
        result.append(readString());
    }
    return result;
}

QHash<int, QVariant> Parcel::readSparseArray() const
{
    QHash<int, QVariant> result;
    if (!m_reader) {
        return result;
    }
    const int length = readInt();
    for (int i = 0; i < length; i++) {
        const int key = readInt();
        const QVariant value = readValue();
        result.insert(key, value);
    }
    return result;
}

QVariant Parcel::readValue() const
{
    QVariant result;
    if (!m_reader) {
        return result;
    }
    const int type = readInt();
    switch (type) {
    case VAL_NULL:
      return QVariant();
    case VAL_STRING:
      return QVariant(readString());
    case VAL_INTEGER:
      return QVariant(readInt());
    case VAL_LONG:
      return QVariant(readLong());
    case VAL_BOOLEAN:
      return QVariant(readBoolean());
    case VAL_DOUBLE:
      return QVariant(readDouble());
    case VAL_STRINGARRAY:
      return QVariant(readStringList());
    default:
      qCritical(binderinterface) << Q_FUNC_INFO << "Unsupported value type:" << type;
      return result;
    }
}

float Parcel::readFloat() const
{
    float result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_float(m_reader, &result)) {
        qCCritical(binderinterface) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

GBinderLocalRequest *Parcel::request()
{
    return m_request;
}
