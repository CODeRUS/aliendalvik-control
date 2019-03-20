#include "parcel.h"
#include "windowmanager.h"

#include <QCoreApplication>
#include <QTimer>

#define WM_SERVICE_NAME "window"
#define WM_INTERFACE_NAME "android.view.IWindowManager"

static WindowManager *s_instance = nullptr;

WindowManager::WindowManager(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(WM_SERVICE_NAME,
                              WM_INTERFACE_NAME,
                              "",
                              parent,
                              loggingCategoryName)
{

}

WindowManager::~WindowManager()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

WindowManager *WindowManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new WindowManager(qApp);
    }
    return s_instance;
}

void WindowManager::setOverscan(int displayId, int left, int top, int right, int bottom)
{
    WindowManager *manager = WindowManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << displayId << left << top << right << bottom;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    if (!parcel) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeInt(displayId);
    parcel->writeInt(left);
    parcel->writeInt(top);
    parcel->writeInt(right);
    parcel->writeInt(bottom);
    int status = 0;
    manager->sendTransaction(TRANSACTION_setOverscan, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
}

void WindowManager::registrationCompleted()
{
    qCDebug(logging) << Q_FUNC_INFO;
}
