#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "binderinterfaceabstract.h"

#include <QVariantHash>

enum {
    TRANSACTION_getInputDevice = 1,
    TRANSACTION_getInputDeviceIds = 2,
    TRANSACTION_injectInputEvent = 8,
};

enum {
    ACTION_DOWN = 0,
    ACTION_UP,
    ACTION_MULTIPLE
};

class Intent;
class InputManager : public BinderInterfaceAbstract
{
    Q_OBJECT
public:
    explicit InputManager(QObject *parent = nullptr, const char *loggingCategoryName = LOGGING(InputManager)".manager");
    virtual ~InputManager();

    static InputManager *GetInstance();

    // hacks
    static void sendTap(int posx, int posy, quint64 uptime);
    static void injectTapEvent(int action, float posx, float posy, float pressure, quint64 uptime);
    static void keyevent(int keycode, quint64 uptime);
};

#endif // INPUTMANAGER_H
