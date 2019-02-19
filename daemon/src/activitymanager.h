#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include "binderinterfaceabstract.h"

#include <QVariantHash>

enum {
    TRANSACTION_startActivity = 3,
    TRANSACTION_getIntentSender = 54,
    TRANSACTION_forceStopPackage = 72,
};

class Intent;
class ActivityManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit ActivityManager(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(ActivityManager)".manager");
    virtual ~ActivityManager();

    static ActivityManager *GetInstance();

    static void startActivity(Intent intent);
    static void forceStopPackage(const QString &package);
    static void getIntentSender(Intent intent);

    GBinderLocalObject *m_receiver = nullptr;

    static GBinderLocalReply *intentReceiver(GBinderLocalObject *obj, GBinderRemoteRequest *req, guint code, guint flags, int *status, void *user_data);

protected:
    void registrationCompleted() override;

private:

};

#endif // ACTIVITYMANAGER_H
