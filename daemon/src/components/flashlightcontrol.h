#ifndef FLASHLIGHTCONTROL_H
#define FLASHLIGHTCONTROL_H

#include <QObject>

class FlashlightControl : public QObject
{
    Q_OBJECT
public:
    explicit FlashlightControl(QObject *parent = 0);

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    bool active();

    Q_INVOKABLE void toggle();

private:
    bool _active;

signals:
    void activeChanged();

};

#endif // FLASHLIGHTCONTROL_H
