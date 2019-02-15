#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include "binderinterfaceabstract.h"

enum {
    TRANSACTION_queryIntentActivities = 45,
};

class Intent;
class PackageManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit PackageManager(QObject *parent = nullptr);
    virtual ~PackageManager();

    static PackageManager *GetInstance();

    static void queryIntentActivities(Intent intent);

protected:
    void registrationCompleted() override;
};

#endif // PACKAGEMANAGER_H
