#ifndef APPOPSSERVICE_H
#define APPOPSSERVICE_H

#include "binderinterfaceabstract.h"

enum {
    TRANSACTION_getToken = 7,
};

class AppOpsService : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit AppOpsService(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(AppOpsService)".manager");
    virtual ~AppOpsService();

    static AppOpsService *GetInstance();

    static GBinderRemoteObject *getToken(GBinderLocalObject *clientToken = nullptr);
};

#endif // APPOPSSERVICE_H
