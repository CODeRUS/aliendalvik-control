#ifndef FLASHLIGHTCONTROL_H
#define FLASHLIGHTCONTROL_H

#include <QObject>

class FlashlightControl : public QObject
{
    Q_OBJECT
public:
    explicit FlashlightControl(QObject *parent = 0);

    Q_INVOKABLE void toggle();

private:
    bool state;

};

#endif // FLASHLIGHTCONTROL_H
