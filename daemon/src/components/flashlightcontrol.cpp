#include "flashlightcontrol.h"

#include <QFile>

FlashlightControl::FlashlightControl(QObject *parent) : QObject(parent)
{
    state = false;
}

void FlashlightControl::toggle()
{
    QFile flash("/sys/kernel/debug/flash_adp1650/mode");
    if (flash.open(QFile::WriteOnly)) {
        flash.write(state ? "0" : "1");
        flash.close();
        state = !state;
    }
}
