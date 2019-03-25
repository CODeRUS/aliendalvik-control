#include "appopsservice.h"
#include "parcel.h"

#include <QCoreApplication>

#define AO_SERVICE_NAME "appops"
#define AO_INTERFACE_NAME "com.android.internal.app.IAppOpsService"

static AppOpsService *s_instance = nullptr;

AppOpsService::AppOpsService(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(AO_SERVICE_NAME,
                              AO_INTERFACE_NAME,
                              parent,
                              loggingCategoryName)
{
}

AppOpsService::~AppOpsService()
{
    s_instance = nullptr;
}

AppOpsService *AppOpsService::GetInstance()
{
    if (!s_instance) {
        s_instance = new AppOpsService(qApp);
    }
    return s_instance;
}

GBinderRemoteObject *AppOpsService::getToken(GBinderLocalObject *clientToken)
{
    GBinderRemoteObject *result = nullptr;

    AppOpsService *service = AppOpsService::GetInstance();
    qCDebug(service->logging) << Q_FUNC_INFO << clientToken;

    QSharedPointer<Parcel> parcel = service->createTransaction();
    if (!parcel) {
        qCCritical(service->logging) << Q_FUNC_INFO << "Null Parcel!";
        return result;
    }

    parcel->writeStrongBinder(clientToken);

    int status = 0;
    QSharedPointer<Parcel> in = service->sendTransaction(TRANSACTION_getToken, parcel, &status);
    qCDebug(service->logging) << Q_FUNC_INFO << "Status:" << status;
    const int exception = in->readInt();
    qCDebug(service->logging) << Q_FUNC_INFO << "Exception:" << exception;
    if (exception != 0) {
        return result;
    }
    result = in->readStrongBinder();

    return result;
}
