#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include "binderinterfaceabstract.h"

#include <QVariantHash>

enum {
    TRANSACTION_startActivity = 3,
    TRANSACTION_registerReceiver = 6,
    TRANSACTION_getIntentSender = 54,
    TRANSACTION_forceStopPackage = 72,
    TRANSACTION_sendIntentSender = 271,
};

enum {
    INTENT_SENDER_BROADCAST = 1,
    INTENT_SENDER_ACTIVITY = 2,
    INTENT_SENDER_ACTIVITY_RESULT = 3,
    INTENT_SENDER_SERVICE = 4,
    INTENT_SENDER_FOREGROUND_SERVICE = 5,
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
