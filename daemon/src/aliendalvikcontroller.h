#ifndef ALIENDALVIKCONTROLLER_H
#define ALIENDALVIKCONTROLLER_H

#include <QObject>

class AliendalvikController : public QObject
{
    Q_OBJECT
public:
    explicit AliendalvikController(QObject *parent = nullptr);
    bool isServiceRunning() const;

private slots:
    void aliendalvikChanged(const QString &, const QVariantMap &properties, const QStringList &);

    virtual void serviceStopped() = 0;
    virtual void serviceStarted() = 0;
};

#endif // ALIENDALVIKCONTROLLER_H
