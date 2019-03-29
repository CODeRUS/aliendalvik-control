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
                              parent,
                              loggingCategoryName)
{
}

ActivityManager::~ActivityManager()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
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
    qCDebug(manager->logging) << Q_FUNC_INFO << package;

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
