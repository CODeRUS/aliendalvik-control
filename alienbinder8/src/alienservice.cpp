#include "alienservice.h"
#include "parcel.h"

#include <QCoreApplication>

#define PM_SERVICE_NAME "alien"
#define PM_INTERFACE_NAME "alien.applications.IAlienService"

static AlienService *s_instance = nullptr;

AlienService::AlienService(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(PM_SERVICE_NAME,
                              PM_INTERFACE_NAME,
                              parent,
                              loggingCategoryName)
{

}

AlienService::~AlienService()
{
    s_instance = nullptr;
}

AlienService *AlienService::GetInstance()
{
    if (!s_instance) {
        s_instance = new AlienService(qApp);
    }
    return s_instance;
}

QString AlienService::getPrettyName(int uid)
{
    QString prettyName;

    AlienService *service = AlienService::GetInstance();
    qCDebug(service->logging) << Q_FUNC_INFO << uid;

    QSharedPointer<Parcel> parcel = service->createTransaction();
    if (!parcel) {
        qCCritical(service->logging) << Q_FUNC_INFO << "Null Parcel!";
        return prettyName;
    }

    parcel->writeInt(uid);
    int status = 0;
    QSharedPointer<Parcel> out = service->sendTransaction(TRANSACTION_getPrettyName, parcel, &status);
    qCDebug(service->logging) << Q_FUNC_INFO << "Status:" << status;
    const int exception = out->readInt();
    if (exception != 0) {
        qCCritical(service->logging) << Q_FUNC_INFO << "Exception:" << exception;
        return prettyName;
    }
    prettyName = out->readString();
    qCDebug(service->logging) << Q_FUNC_INFO << "Result:" << prettyName;

    return prettyName;
}

void AlienService::startApp(const QString &packageName)
{
    AlienService *service = AlienService::GetInstance();
    qCDebug(service->logging) << Q_FUNC_INFO << packageName;

    QSharedPointer<Parcel> parcel = service->createTransaction();
    if (!parcel) {
        qCCritical(service->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(packageName);
    int status = 0;
    service->sendTransaction(TRANSACTION_startApp, parcel, &status);
    qCDebug(service->logging) << Q_FUNC_INFO << "Status:" << status;
}
