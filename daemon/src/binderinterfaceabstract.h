#ifndef BINDERINTERFACEABSTRACT_H
#define BINDERINTERFACEABSTRACT_H

#include <QObject>

#include <gbinder.h>

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
    ~Parcel();

    void writeStrongBinder(GBinderLocalObject *value);
    void writeString(const QString &value);
    void writeInt(int value);
    void writeBundle(const QVariantHash &value);
    void writeValue(const QVariant &value);

    GBinderLocalRequest *request();

private:
    GBinderLocalRequest *m_request;
    GBinderWriter *m_writer;
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
    void sendTransaction(int code, Parcel *parcel, int *status);

private:
    void binderConnect();
    void binderDisconnect();

    static void registrationHandler(GBinderServiceManager* sm, const char* name, void* user_data);
    void registerManager();

    static void deathHandler(GBinderRemoteObject *obj, void *user_data);

    const char *m_serviceName;
    const char *m_interfaceName;

    GBinderRemoteObject *m_remote = nullptr;
    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderClient *m_client = nullptr;

    int m_registrationHandler = 0;
    int m_deathHandler = 0;
};

#endif // BINDERINTERFACEABSTRACT_H
