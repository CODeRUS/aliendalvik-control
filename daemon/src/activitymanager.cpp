#include "activitymanager.h"
#include "intent.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(amInterface, "activitymanager.interface", QtDebugMsg)

#define AM_SERVICE_NAME "activity"
#define AM_INTERFACE_NAME "android.app.IActivityManager"

static ActivityManager *s_instance = nullptr;

ActivityManager::ActivityManager(QObject *parent)
    : BinderInterfaceAbstract(AM_SERVICE_NAME, AM_INTERFACE_NAME, parent)
{
}

ActivityManager::~ActivityManager()
{
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
    qCDebug(amInterface) << Q_FUNC_INFO << intent.action;

    ActivityManager *manager = ActivityManager::GetInstance();
    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(amInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // IBinder b; ApplicationThreadNative.asInterface(b);
    parcel->writeString(QString()); // callingPackage
    parcel->writeInt(1); // const value
    intent.writeToParcel(parcel.data());
    parcel->writeString(QString()); // resolvedType
//    parcel->writeStrongBinder(static_cast<GBinderLocalObject*>(NULL)); // resultTo
    parcel->writeStrongBinder(manager->localHandler()); // resultTo
    parcel->writeString(QStringLiteral("intent_receiver")); // resultWho
    parcel->writeInt(123); // requestCode
    parcel->writeInt(0); // startFlags
    parcel->writeInt(0); // profilerInfo disable
    parcel->writeInt(0); // options disable
    int status = 0;
    manager->sendTransaction(TRANSACTION_startActivity, parcel, &status);
    qCDebug(amInterface) << Q_FUNC_INFO << "Status:" << status;
}

void ActivityManager::forceStopPackage(const QString &package)
{
    qCDebug(amInterface) << Q_FUNC_INFO << package << TRANSACTION_forceStopPackage;
    ActivityManager *manager = ActivityManager::GetInstance();

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(amInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(package);
    parcel->writeInt(USER_OWNER);
    int status = 0;
    manager->sendTransaction(TRANSACTION_forceStopPackage, parcel, &status);
    qCDebug(amInterface) << Q_FUNC_INFO << "Status:" << status;
}

void ActivityManager::registrationCompleted()
{
    qCDebug(amInterface) << Q_FUNC_INFO;
}
