#include "flashlightcontrol.h"

#include <QFile>

FlashlightControl::FlashlightControl(QObject *parent) : QObject(parent)
{
    _active = false;
}

bool FlashlightControl::active()
{
    return _active;
}

void FlashlightControl::toggle()
{
    QFile flash("/sys/kernel/debug/flash_adp1650/mode");
    if (flash.open(QFile::WriteOnly)) {
        flash.write(_active ? "0" : "1");
        flash.close();
        _active = !_active;

        Q_EMIT activeChanged();
    }
}
