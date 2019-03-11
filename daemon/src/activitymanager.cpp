#include "activitymanager.h"
#include "intent.h"
#include "parcel.h"

#include <QCoreApplication>
#include <QTimer>

#define AM_SERVICE_NAME "activity"
#define AM_INTERFACE_NAME "android.app.IActivityManager"

static ActivityManager *s_instance = nullptr;

ActivityManager::ActivityManager(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(AM_SERVICE_NAME,
                              AM_INTERFACE_NAME,
                              "",
                              parent,
                              loggingCategoryName)
{
}

ActivityManager::~ActivityManager()
{
    if (m_receiver) {
        gbinder_local_object_drop(m_receiver);
        m_receiver = nullptr;
    }

    s_instance = nullptr;
}

ActivityManager *ActivityManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new ActivityManager(qApp);
    }
    return s_instance;
}

void ActivityManager::startActivity(Intent intent)
{
    ActivityManager *manager = ActivityManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << intent.action;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // IBinder b; ApplicationThreadNative.asInterface(b);
    parcel->writeString(QString()); // callingPackage
    parcel->writeInt(1); // const value
    intent.writeToParcel(parcel.data());
    parcel->writeString(QString()); // resolvedType
    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // resultTo
    parcel->writeString(QString()); // resultWho
    parcel->writeInt(0); // requestCode
    parcel->writeInt(0); // startFlags
    parcel->writeInt(0); // profilerInfo disable
    parcel->writeInt(0); // options disable
    int status = 0;
    manager->sendTransaction(TRANSACTION_startActivity, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
}

void ActivityManager::forceStopPackage(const QString &package)
{
    ActivityManager *manager = ActivityManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << package << TRANSACTION_forceStopPackage;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(package);
    parcel->writeInt(USER_OWNER);
    int status = 0;
    manager->sendTransaction(TRANSACTION_forceStopPackage, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
}

GBinderRemoteObject *ActivityManager::getIntentSender(Intent intent)
{
    GBinderRemoteObject *result = nullptr;

    ActivityManager *manager = ActivityManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << intent.action;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return result;
    }

    parcel->writeInt(INTENT_SENDER_ACTIVITY); // intent sender type
    parcel->writeString(QString()); // packageName
    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // token
    parcel->writeString(QString()); // resultWho
    parcel->writeInt(123); // requestCode
    parcel->writeInt(1); // intents
    parcel->writeInt(1); // intent
    intent.writeToParcel(parcel.data());
    parcel->writeInt(1); // resolved types
    parcel->writeString(QString()); // resolved type
    parcel->writeInt(0); // flags
    parcel->writeInt(USER_CURRENT); // userId
    int status = 0;
    QSharedPointer<Parcel> in = manager->sendTransaction(TRANSACTION_getIntentSender, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
    const int exception = in->readInt();
    qCDebug(manager->logging) << Q_FUNC_INFO << "Exception:" << exception;
    if (exception != 0) {
        return result;
    }

    result = in->readStrongBinder();
    qCWarning(manager->logging) << Q_FUNC_INFO << "IntentSender:" << result;

    return result;

    if (!result) {
        return result;
    }

//    oneway interface IIntentSender {
//        void send(int code, in Intent intent, String resolvedType, in IBinder whitelistToken,
//                IIntentReceiver finishedReceiver, String requiredPermission, in Bundle options);
//    }

    GBinderClient *client = gbinder_client_new(result, "android.content.IIntentSender");
    qCWarning(manager->logging) << Q_FUNC_INFO << "Client:" << client;
    GBinderLocalRequest* req = gbinder_client_new_request(client);
    Parcel out(req);
    out.writeInt(123); // code
    out.writeInt(0); // new intent
//    out.writeInt(1); // intent
//    intent.writeToParcel(&out);
    out.writeString(QString()); // resolvedType
    out.writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // whitelistToken
    out.writeStrongBinder(manager->m_receiver); // finishedReceiver
    out.writeString(QString()); // requiredPermission
    out.writeInt(0); // options

    int sstatus = gbinder_client_transact_sync_oneway(client, 1, req);
    qCWarning(manager->logging) << Q_FUNC_INFO << "Status:" << sstatus;

//    QSharedPointer<Parcel> parcel2 = manager->createTransaction();
//    if (!parcel2) {
//        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
//        return;
//    }
//    parcel2->writeStrongBinder(object); // IIntentSender target
//    parcel2->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // whitelistToken

    gbinder_client_unref(client);
}


//oneway interface IIntentReceiver {
//    void performReceive(in Intent intent, int resultCode, String data,
//            in Bundle extras, boolean ordered, boolean sticky, int sendingUser);
//}

GBinderLocalReply *ActivityManager::intentReceiver(GBinderLocalObject *obj, GBinderRemoteRequest *req, guint code, guint flags, int *status, void *user_data)
{
    const char *iface = gbinder_remote_request_interface(req);
    qWarning() << Q_FUNC_INFO;
    qWarning() << "Interface:" << iface;
    qWarning() << "GBinderLocalObject:" << obj;
    qWarning() << "GBinderRemoteRequest:" << req;
    qWarning() << "Code:" << code;
    qWarning() << "Flags:" << flags;


    Parcel in(req);
    const int haveIntent = in.readInt();
    qWarning() << "Have intent:" << haveIntent;
    if (haveIntent != 0) {
        Intent intent(&in);
        qWarning() << "intent:" << intent.action;
    }

    const int resultCode = in.readInt();
    qWarning() << "resultCode:" << resultCode;
    const QString data = in.readString();
    qWarning() << "data:" << data;

    const int haveBundle = in.readInt();
    qWarning() << "Have bundle:" << haveBundle;

    if (haveBundle != 0) {
        //
    }
    const int ordered = in.readInt();
    qWarning() << "ordered:" << ordered;
    const int sticky = in.readInt();
    qWarning() << "sticky:" << sticky;
    const int sendingUser = in.readInt();
    qWarning() << "sendingUser:" << sendingUser;

    GBinderLocalReply *reply = NULL;
    *status = GBINDER_STATUS_OK;
    return reply;
}

void ActivityManager::registrationCompleted()
{
    qCDebug(logging) << Q_FUNC_INFO;

    if (!m_receiver) {
        m_receiver = gbinder_servicemanager_new_local_object(
                    manager(),
                    "android.content.IIntentReceiver",
                    ActivityManager::intentReceiver,
                    this);

        qWarning() << Q_FUNC_INFO << "Receiver:" << m_receiver;

//        QSharedPointer<Parcel> parcel = createTransaction();
//        if (!parcel) {
//            qCCritical(logging) << Q_FUNC_INFO << "Null Parcel!";
//            return;
//        }

//        parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // IApplicationThread caller
//        parcel->writeString(QString()); // String callerPackage
//        parcel->writeStrongBinder(m_receiver); // IIntentReceiver receiver
//        parcel->writeInt(0); // IntentFilter filter
//        parcel->writeString(QString()); // String requiredPermission
//        parcel->writeInt(USER_CURRENT); // int userId
//        parcel->writeInt(0); // int flags
//        int status = 0;
//        QSharedPointer<Parcel> in = sendTransaction(TRANSACTION_registerReceiver, parcel, &status);
//        qCDebug(logging) << Q_FUNC_INFO << "Status:" << status;
//        const int exception = in->readInt();
//        qCDebug(logging) << Q_FUNC_INFO << "Exception:" << exception;
//        if (exception != 0) {
//            return;
//        }
//        const int haveIntent = in->readInt();
//        qCDebug(logging) << Q_FUNC_INFO << "Have intent:" << haveIntent;
//        if (haveIntent != 0) {
//            //
//        }
    }

}
