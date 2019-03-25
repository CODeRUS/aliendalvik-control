#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "binderinterfaceabstract.h"

enum {
    TRANSACTION_setOverscan = 15,
};

class WindowManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit WindowManager(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(WindowManager)".manager");
    virtual ~WindowManager();

    static WindowManager *GetInstance();

    static void setOverscan(int displayId, int left, int top, int right, int bottom);
};

#endif // WINDOWMANAGER_H
