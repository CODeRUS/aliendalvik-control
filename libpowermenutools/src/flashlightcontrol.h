#ifndef FLASHLIGHTCONTROL_H
#define FLASHLIGHTCONTROL_H

#include <QObject>
#include <mlite5/MGConfItem>

class Q_DECL_EXPORT FlashlightControl : public QObject
{
    Q_OBJECT
public:
    explicit FlashlightControl(QObject *parent = 0);

    static FlashlightControl *GetInstance(QObject *parent = 0);

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    bool active();

    Q_INVOKABLE void toggle();

private:
    MGConfItem *flashlightStatus;

signals:
    void activeChanged();

};

#endif // FLASHLIGHTCONTROL_H
