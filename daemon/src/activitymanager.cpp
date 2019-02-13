#include "activitymanager.h"

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
    Parcel *parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(amInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeStrongBinder(NULL);
    parcel->writeString(QString()); // callingPackage
    parcel->writeInt(1); // const value
    intent.writeToParcel(parcel);
    parcel->writeString(QString()); // resolvedType
    parcel->writeString(QString()); // callingPackage
    parcel->writeString(QString()); // resultWho
    parcel->writeInt(0); // requestCode
    parcel->writeInt(0); // startFlags
    parcel->writeInt(0); // profilerInfo disable
    parcel->writeInt(0); // options disable
    int status = 0;
    manager->sendTransaction(TRANSACTION_startActivity, parcel, &status);
    qCDebug(amInterface) << Q_FUNC_INFO << "Status:" << status;
    delete parcel;
}

void ActivityManager::forceStopPackage(const QString &package)
{
    qCDebug(amInterface) << Q_FUNC_INFO << package << TRANSACTION_forceStopPackage;
    ActivityManager *manager = ActivityManager::GetInstance();

    Parcel *parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(amInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(package);
    parcel->writeInt(USER_OWNER);
    int status = 0;
    manager->sendTransaction(TRANSACTION_forceStopPackage, parcel, &status);
    qCDebug(amInterface) << Q_FUNC_INFO << "Status:" << status;
    delete parcel;
}

void Intent::writeToParcel(Parcel *parcel)
{
    if (!parcel) {
        qCCritical(amInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(action);

    // Uri.writeToParcel
    if (data.isNull()) {
        parcel->writeInt(0); // NULL_TYPE_ID
    } else {
        parcel->writeInt(1); // TYPE_ID UriString = 1
        parcel->writeString(data);
    }

    parcel->writeString(type);

    parcel->writeInt(flags);

    parcel->writeString(package);

    // ComponentName.writeToParcel
    if (classPackage.isEmpty() && className.isEmpty()) {
        parcel->writeString(QString());
    } else {
        parcel->writeString(classPackage);
        parcel->writeString(className);
    }

    parcel->writeInt(0); // mSourceBounds disable
    parcel->writeInt(0); // mCategories disable
    parcel->writeInt(0); // mSelector disable
    parcel->writeInt(0); // mClipData disable

    parcel->writeInt(contentUserHint);

    if (extras.isEmpty()) {
        parcel->writeInt(-1);
    } else {
        parcel->writeBundle(extras);
    }
}
