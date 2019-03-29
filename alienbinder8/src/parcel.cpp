#include "intent.h"
#include "intentsender.h"
#include "parcel.h"

#define LOGME LOGGING(Parcel)".parcel"

Parcel::Parcel(GBinderLocalRequest *request)
    : LoggingClassWrapper(LOGME)
    , m_request(request)
    , m_writer(new GBinderWriter)
{
    gbinder_local_request_init_writer(m_request, m_writer);
}

Parcel::Parcel(GBinderRemoteReply *reply)
    : LoggingClassWrapper(LOGME)
    , m_reply(reply)
    , m_reader(new GBinderReader)
{
    gbinder_remote_reply_init_reader(m_reply, m_reader);
}

Parcel::Parcel(GBinderRemoteRequest *remote)
    : LoggingClassWrapper(LOGME)
    , m_remote(remote)
    , m_reader(new GBinderReader)
{
    gbinder_remote_request_init_reader(m_remote, m_reader);
}

Parcel::~Parcel()
{
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
        qCDebug(logging) << it.key() << it.value();
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
    case QVariant::UserType: {
        if (value.canConvert<Intent>()) {
            writeInt(static_cast<int>(VAL_PARCELABLE));
            Intent intent = value.value<Intent>();
            writeParcelable(intent);
        } else if (value.canConvert<IntentSender>()) {
            writeInt(static_cast<int>(VAL_PARCELABLE));
            IntentSender intentSender = value.value<IntentSender>();
            writeParcelable(intentSender);
        } else if (value.canConvert<QList<Intent>>()) {
            writeInt(static_cast<int>(VAL_PARCELABLEARRAY));
            QList<Intent> intentArray = value.value<QList<Intent>>();
            writeInt(intentArray.length());
            for (const Intent &intent : intentArray) {
                writeParcelable(intent);
            }
        } else {
            qCCritical(logging) << Q_FUNC_INFO << "Unsupported user value type:" << value.userType();
        }
        break;
    }
    default:
        qCCritical(logging) << Q_FUNC_INFO << "Unsupported value type:" << QMetaType::typeName(value.type()) << value.type();
    }
}

void Parcel::writeParcelable(const Parcelable &parcelable)
{
    // writeParcelableCreator
    writeString(parcelable.creator);
    parcelable.writeToParcel(this);
}

int Parcel::readInt() const
{
    int result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_int32(m_reader, &result)) {
        qCCritical(logging) << Q_FUNC_INFO << "Error reading value!";
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

QVariantHash Parcel::readBundle() const
{
    QVariantHash result;
    const int length = readInt();

    if (length <= 0) {
        return result;
    }

    const int magic = readInt();
    if (magic != BUNDLE_MAGIC) {
        qCCritical(logging) << Q_FUNC_INFO << "Bundle magic does not match:" << magic;
    }

    const int size = readInt();
    for (int i = 0; i < size; i++) {
        const QString key(readString());
        const QVariant value(readValue());
        result.insert(key, value);
    }
    return result;
}

qlonglong Parcel::readLong() const
{
    qlonglong result = 0;
    if (!m_reader) {
        return result;
    }

    if (!gbinder_reader_read_int64(m_reader, &result)) {
        qCCritical(logging) << Q_FUNC_INFO << "Error reading value!";
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
        qCCritical(logging) << Q_FUNC_INFO << "Error reading value!";
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
    case VAL_PARCELABLE:
        return readParcelable();
    case VAL_PARCELABLEARRAY:
        return readParcelableArray();
    default:
        qCritical(logging) << Q_FUNC_INFO << "Unsupported value type:" << type;
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
        qCCritical(logging) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

GBinderRemoteObject *Parcel::readStrongBinder() const
{
    GBinderRemoteObject* result = nullptr;
    if (!m_reader) {
        return result;
    }
    if (!gbinder_reader_read_nullable_object(m_reader, &result)) {
        qCCritical(logging) << Q_FUNC_INFO << "Error reading value!";
    }
    return result;
}

QVariant Parcel::readParcelable() const
{
    const QString creator = readString();
    if (creator == QLatin1String("android.content.Intent")) {
        return QVariant::fromValue(Intent(this));
    } else if (creator == QLatin1String("android.net.Uri")) {
        const int typeId = readInt();
        if (typeId == StringUri_TYPE_ID) {
            return QVariant(readString());
        } else {
            qCDebug(logging) << Q_FUNC_INFO << "Unsupported uri type id:" << typeId;
        }
        return QVariant();
    } else {
        qCDebug(logging) << Q_FUNC_INFO << "Unsupported creator:" << creator;
        return QVariant();
    }
}

QVariantList Parcel::readParcelableArray() const
{
    QVariantList result;
    const int length = readInt();
    for (int i = 0; i < length; i++) {
        result.append(readParcelable());
    }
    return result;
}

GBinderLocalRequest *Parcel::request()
{
    return m_request;
}
