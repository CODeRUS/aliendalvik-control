#ifndef ALIENSERVICE_H
#define ALIENSERVICE_H

#include "binderinterfaceabstract.h"
#include "loggingclasswrapper.h"

#include <QSharedPointer>

enum {
    TRANSACTION_getPrettyName = 7,
};

class AlienService : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit AlienService(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(AlienService)".manager");
    virtual ~AlienService();

    static AlienService *GetInstance();

    static QString getPrettyName(int uid);

protected:
    void registrationCompleted() override;
};

#endif // ALIENSERVICE_H
