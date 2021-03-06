#ifndef PARCEL_H
#define PARCEL_H

#include <QString>
#include <QVariant>
#include <gbinder.h>

#include "../common/src/loggingclasswrapper.h"

enum {
    StringUri_TYPE_ID = 1
};

// Keep in sync with frameworks/base/core/java/android/os/Parcel.java.
enum {
    VAL_NULL = -1,
    VAL_STRING = 0,
    VAL_INTEGER = 1,
    VAL_MAP = 2,
    VAL_BUNDLE = 3,
    VAL_PARCELABLE = 4,
    VAL_SHORT = 5,
    VAL_LONG = 6,
    VAL_DOUBLE = 8,
    VAL_BOOLEAN = 9,
    VAL_BYTEARRAY = 13,
    VAL_STRINGARRAY = 14,
    VAL_IBINDER = 15,
    VAL_PARCELABLEARRAY = 16,
    VAL_INTARRAY = 18,
    VAL_LONGARRAY = 19,
    VAL_BYTE = 20,
    VAL_SERIALIZABLE = 21,
    VAL_BOOLEANARRAY = 23,
    VAL_PERSISTABLEBUNDLE = 25,
    VAL_DOUBLEARRAY = 28,
};

enum {
    // Keep them in sync with BUNDLE_MAGIC* in frameworks/base/core/java/android/os/BaseBundle.java.
    BUNDLE_MAGIC = 0x4C444E42,
    BUNDLE_MAGIC_NATIVE = 0x4C444E44,
};

class Intent;
class Parcelable;
class Parcel : public LoggingClassWrapper
{
public:
    Parcel(GBinderLocalRequest *request);
    Parcel(GBinderRemoteReply *reply);
    Parcel(GBinderRemoteRequest *remote);
    ~Parcel();

    void writeStrongBinder(GBinderLocalObject *value);
    void writeStrongBinder(GBinderRemoteObject *value);
    void writeString(const QString &value);
    void writeInt(int value);
    void writeFloat(float value);
    void writeDouble(double value);
    void writeInt64(quint64 value);
    void writeBundle(const QVariantHash &value);
    void writeValue(const QVariant &value);
    void writeParcelable(const Parcelable &parcelable);

    int readInt() const;
    quint64 readInt64() const;
    QString readString() const;
    QVariantHash readBundle() const;
    qlonglong readLong() const;
    bool readBoolean() const;
    double readDouble() const;
    QStringList readStringList() const;
    QList<int> readIntList() const;
    QHash<int, QVariant> readSparseArray() const;
    QVariant readValue() const;
    float readFloat() const;
    GBinderRemoteObject *readStrongBinder() const;
    QVariant readParcelable() const;
    QVariantList readParcelableArray() const;

    GBinderLocalRequest *request();

private:
    GBinderLocalRequest *m_request = nullptr;
    GBinderRemoteReply *m_reply = nullptr;
    GBinderRemoteRequest *m_remote = nullptr;
    GBinderWriter *m_writer = nullptr;
    GBinderReader *m_reader = nullptr;
};

#endif // PARCEL_H
