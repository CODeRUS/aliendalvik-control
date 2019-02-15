#ifndef BINDERINTERFACEABSTRACT_H
#define BINDERINTERFACEABSTRACT_H

#include <QObject>

#include <gbinder.h>

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

class Parcel
{
public:
    Parcel(GBinderLocalRequest *request);
    Parcel(GBinderRemoteReply *reply);
    ~Parcel();

    void writeStrongBinder(GBinderLocalObject *value);
    void writeStrongBinder(GBinderRemoteObject *value);
    void writeString(const QString &value);
    void writeInt(int value);
    void writeBundle(const QVariantHash &value);
    void writeValue(const QVariant &value);

    int readInt() const;
    QString readString() const;
    void readBundle() const;
    qlonglong readLong() const;
    bool readBoolean() const;
    double readDouble() const;
    QStringList readStringList() const;
    QHash<int, QVariant> readSparseArray() const;
    QVariant readValue() const;
    float readFloat() const;

    GBinderLocalRequest *request();

private:
    GBinderLocalRequest *m_request = nullptr;
    GBinderRemoteReply *m_reply = nullptr;
    GBinderWriter *m_writer = nullptr;
    GBinderReader *m_reader = nullptr;
};

class BinderInterfaceAbstract : public QObject
{
    Q_OBJECT
public:
    explicit BinderInterfaceAbstract(const char *serviceName,
                                     const char *interfaceName,
                                     QObject *parent = nullptr);
    virtual ~BinderInterfaceAbstract();

    Parcel *createTransaction();
    Parcel *sendTransaction(int code, Parcel *parcel, int *status);

    GBinderServiceManager *manager();
    GBinderLocalObject *localHandler();

    static GBinderLocalReply* onTransact(
            GBinderLocalObject *obj,
            GBinderRemoteRequest *req,
            guint code,
            guint flags,
            int *status,
            void *user_data);

public slots:
    void reconnect();

protected:
    virtual void registrationCompleted() = 0;

private:
    void binderConnect();
    void binderDisconnect();

    static void registrationHandler(GBinderServiceManager* sm, const char* name, void* user_data);
    void registerManager();

    static void deathHandler(GBinderRemoteObject *obj, void *user_data);

    const char *m_serviceName;
    const char *m_interfaceName;

    GBinderRemoteObject *m_remote = nullptr;
    GBinderLocalObject *m_local = nullptr;
    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderClient *m_client = nullptr;

    int m_registrationHandler = 0;
    int m_deathHandler = 0;
    int m_localHandler = 0;
};

#endif // BINDERINTERFACEABSTRACT_H
