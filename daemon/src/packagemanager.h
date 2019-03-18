#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include "binderinterfaceabstract.h"
#include "loggingclasswrapper.h"

#include <QSharedPointer>

enum {
    TRANSACTION_getPackageUid = 5,
    TRANSACTION_queryIntentActivities = 45,
};

class Intent;
class ResolveInfo;
class PackageManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit PackageManager(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(PackageManager)".manager");
    virtual ~PackageManager();

    static PackageManager *GetInstance();

    static int getPackageUid(const QString &packageName);
    static QList<QSharedPointer<ResolveInfo> > queryIntentActivities(Intent intent);

protected:
    void registrationCompleted() override;
};

#endif // PACKAGEMANAGER_H
