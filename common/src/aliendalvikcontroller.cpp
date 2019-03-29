#include "aliendalvikcontroller.h"
#include "systemdcontroller.h"

#include <QTimer>

#define s_newBinderDevice "/dev/puddlejumper"

AliendalvikController::AliendalvikController(QObject *parent)
    : QObject(parent)
    , m_controller(SystemdController::GetInstance(QStringLiteral("aliendalvik.service")))
{
    connect(m_controller, &SystemdController::serviceStarted, this, &AliendalvikController::serviceStarted);
    connect(m_controller, &SystemdController::serviceStopped, this, &AliendalvikController::serviceStopped);
    if (isServiceActive()) {
        QTimer::singleShot(0, this, &AliendalvikController::serviceStarted);
    }
}

const char *AliendalvikController::binderDevice() const
{
    return s_newBinderDevice;
}

bool AliendalvikController::isServiceActive() const
{
    if (!m_controller) {
        return false;
    }

    return m_controller->isActive();
}
