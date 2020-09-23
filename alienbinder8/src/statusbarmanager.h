#ifndef STATUSBARMANAGER_H
#define STATUSBARMANAGER_H

#include "binderinterfaceabstract.h"
#include "../common/src/loggingclasswrapper.h"

#include <QSharedPointer>

enum {
    TRANSACTION_expand = 1,
    TRANSACTION_collapse = 2,
};


class StatusBarManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit StatusBarManager(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(PackageManager)".manager");
    virtual ~StatusBarManager();

    static StatusBarManager *GetInstance();

    static void expand();
    static void collapse();
};

#endif // STATUSBARMANAGER_H
