#include "statusbarmanager.h"

#include <QCoreApplication>

#define SBM_SERVICE_NAME "statusbar"
#define SBM_INTERFACE_NAME "com.android.internal.statusbar.IStatusBarService"

static StatusBarManager *s_instance = nullptr;

StatusBarManager::StatusBarManager(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(SBM_SERVICE_NAME,
                              SBM_INTERFACE_NAME,
                              parent,
                              loggingCategoryName)
{

}

StatusBarManager::~StatusBarManager()
{
    s_instance = nullptr;
}

StatusBarManager *StatusBarManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new StatusBarManager(qApp);
    }
    return s_instance;
}

void StatusBarManager::expand()
{
    StatusBarManager *manager = StatusBarManager::GetInstance();

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    int status = 0;
    manager->sendTransaction(TRANSACTION_expand, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
}

void StatusBarManager::collapse()
{
    StatusBarManager *manager = StatusBarManager::GetInstance();

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    int status = 0;
    manager->sendTransaction(TRANSACTION_collapse, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
}
