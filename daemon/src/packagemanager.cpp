#include "packagemanager.h"
#include "intent.h"
#include "resolveinfo.h"
#include "parcel.h"

#include <QCoreApplication>

#define PM_SERVICE_NAME "package"
#define PM_INTERFACE_NAME "android.content.pm.IPackageManager"

static PackageManager *s_instance = nullptr;

PackageManager::PackageManager(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(PM_SERVICE_NAME,
                              PM_INTERFACE_NAME,
                              "",
                              parent,
                              loggingCategoryName)
{

}

PackageManager::~PackageManager()
{
    s_instance = nullptr;
}

PackageManager *PackageManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new PackageManager(qApp);
    }
    return s_instance;
}

QList<QSharedPointer<ResolveInfo> > PackageManager::queryIntentActivities(Intent intent)
{
    PackageManager *manager = PackageManager::GetInstance();

    qCDebug(manager->logging) << Q_FUNC_INFO << intent.action;
    QList<QSharedPointer<ResolveInfo> > resolveInfoList;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return resolveInfoList;
    }

    parcel->writeInt(1); // const value
    intent.writeToParcel(parcel.data());
    parcel->writeString(intent.type); // resolvedType
    parcel->writeInt(0); // flags
    parcel->writeInt(USER_OWNER); // userId
    int status = 0;
    QSharedPointer<Parcel> out = manager->sendTransaction(TRANSACTION_queryIntentActivities, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
    const int exception = out->readInt();
    if (exception != 0) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Exception:" << exception;
        return resolveInfoList;
    }
    const int result = out->readInt();
    qCDebug(manager->logging) << Q_FUNC_INFO << "Result:" << result;
    if (result != 0) {
        const int count = out->readInt();
        qCDebug(manager->logging) << Q_FUNC_INFO << "Count:" << count;
        qCDebug(manager->logging) << Q_FUNC_INFO << "Creator:" << out->readString();
        for (int i = 0; i < count; i++) {
            const int present = out->readInt();
            qCDebug(manager->logging) << Q_FUNC_INFO << "Present:" << present;
            if (present != 0) {
                QSharedPointer<ResolveInfo> resolveInfo(new ResolveInfo(out.data()));
                resolveInfoList.append(resolveInfo);
            } else {
                break;
            }
        }
    }
    return resolveInfoList;
}

int PackageManager::getPackageUid(const QString &packageName)
{
    PackageManager *manager = PackageManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << intent.action;

    int uid = -1;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return resolveInfoList;
    }

    parcel->writeString(packageName);
    parcel->writeInt(0); // flags
    parcel->writeInt(USER_OWNER); // userId
    int status = 0;
    QSharedPointer<Parcel> out = manager->sendTransaction(TRANSACTION_getPackageUid, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
    const int exception = out->readInt();
    if (exception != 0) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Exception:" << exception;
        return uid;
    }
    uid = out->readInt();
    qCDebug(manager->logging) << Q_FUNC_INFO << "Result:" << uid;

    return uid;
}

void PackageManager::registrationCompleted()
{
    qCDebug(logging) << Q_FUNC_INFO;
}
