#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include "binderinterfaceabstract.h"

#include <QVariantHash>

enum {
    TRANSACTION_startActivity = 3,
    TRANSACTION_registerReceiver = 6,
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

protected:
    void registrationCompleted() override;

private:

};

#endif // ACTIVITYMANAGER_H
