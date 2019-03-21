#include "aliendalvikcontroller.h"
#include "systemdcontroller.h"

AliendalvikController::AliendalvikController(QObject *parent)
    : QObject(parent)
{
    SystemdController *controller = SystemdController::GetInstance(QStringLiteral("aliendalvik.service"));

    connect(controller, &SystemdController::serviceStarted, this, &AliendalvikController::serviceStarted);
    connect(controller, &SystemdController::serviceStopped, this, &AliendalvikController::serviceStopped);
}
