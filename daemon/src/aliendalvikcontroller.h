#ifndef ALIENDALVIKCONTROLLER_H
#define ALIENDALVIKCONTROLLER_H

#include <QObject>

class AliendalvikController : public QObject
{
    Q_OBJECT
public:
    explicit AliendalvikController(QObject *parent = nullptr);

private slots:
    virtual void serviceStopped() = 0;
    virtual void serviceStarted() = 0;
};

#endif // ALIENDALVIKCONTROLLER_H
