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

void ActivityManager::getIntentSender(Intent intent)
{
    ActivityManager *manager = ActivityManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << intent.action;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeInt(2); // ?
    parcel->writeString(QString()); // packageName
    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // token
    parcel->writeString(QString()); // resultWho
    parcel->writeInt(0); // requestCode
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
        return;
    }

    GBinderRemoteObject *object = in->readStrongBinder();
    qCWarning(manager->logging) << Q_FUNC_INFO << "IntentSender:" << object;

    if (!object) {
        return;
    }

    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(5000);
    loop.exec();

    GBinderClient *client = gbinder_client_new(object, "android.content.IIntentSender");
    qCWarning(manager->logging) << Q_FUNC_INFO << "Client:" << client;
    GBinderLocalRequest* req = gbinder_client_new_request(client);
    Parcel out(req);
    out.writeInt(0); // code
    out.writeInt(0); // new intent
//    out.writeInt(1); // intent
//    intent.writeToParcel(&out);
    out.writeString(QString()); // resolvedType
    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // whitelistToken
    parcel->writeStrongBinder(manager->m_receiver); // finishedReceiver
    out.writeString(QString()); // requiredPermission
    out.writeInt(0); // options

    int sstatus = gbinder_client_transact_sync_oneway(client, 1, req);
    qCWarning(manager->logging) << Q_FUNC_INFO << "Status:" << sstatus;

    gbinder_client_unref(client);
}

GBinderLocalReply *ActivityManager::intentReceiver(GBinderLocalObject *obj, GBinderRemoteRequest *req, guint code, guint flags, int *status, void *user_data)
{
    const char *iface = gbinder_remote_request_interface(req);
    qWarning() << Q_FUNC_INFO;
    qWarning() << "Interface:" << iface;
    qWarning() << "GBinderLocalObject:" << obj;
    qWarning() << "GBinderRemoteRequest:" << req;
    qWarning() << "Code:" << code;
    qWarning() << "Flags:" << flags;
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
    }

}
