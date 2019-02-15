#include "packagemanager.h"
#include "intent.h"
#include "resolveinfo.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(pmInterface, "packagemanager.interface", QtDebugMsg)

#define PM_SERVICE_NAME "package"
#define PM_INTERFACE_NAME "android.content.pm.IPackageManager"

static PackageManager *s_instance = nullptr;

PackageManager::PackageManager(QObject *parent)
    : BinderInterfaceAbstract(PM_SERVICE_NAME, PM_INTERFACE_NAME, parent)
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

void PackageManager::queryIntentActivities(Intent intent)
{
    qCDebug(pmInterface) << Q_FUNC_INFO << intent.action;

    PackageManager *manager = PackageManager::GetInstance();
    QScopedPointer<Parcel, QScopedPointerDeleter<Parcel>> parcel(manager->createTransaction());
    if (!parcel) {
        qCCritical(pmInterface) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeInt(1); // const value
    intent.writeToParcel(parcel.data());
    parcel->writeString(intent.type); // resolvedType
    parcel->writeInt(0); // flags
    parcel->writeInt(USER_OWNER); // userId
    int status = 0;
    QScopedPointer<Parcel, QScopedPointerDeleter<Parcel>> out(
                manager->sendTransaction(TRANSACTION_queryIntentActivities, parcel.data(), &status));
    const int exception = out->readInt();
    if (exception != 0) {
        qCCritical(pmInterface) << Q_FUNC_INFO << "Exception:" << exception;
        return;
    }
    const int result = out->readInt();
    qCDebug(pmInterface) << Q_FUNC_INFO << "Result:" << result;
    if (result != 0) {
        const int count = out->readInt();
        qCDebug(pmInterface) << Q_FUNC_INFO << "Count:" << count;
        qCDebug(pmInterface) << Q_FUNC_INFO << "Creator:" << out->readString();
        for (int i = 0; i < count; i++) {
            const int present = out->readInt();
            qCDebug(pmInterface) << Q_FUNC_INFO << "Present:" << present;
            if (present != 0) {
                ResolveInfo info(out.data());
            } else {
                break;
            }
        }
    }
    qCDebug(pmInterface) << Q_FUNC_INFO << "Status:" << status;
}

void PackageManager::registrationCompleted()
{
    qCDebug(pmInterface) << Q_FUNC_INFO;
}
