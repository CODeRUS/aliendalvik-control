#ifndef SYSTEMDCONTROLLER_H
#define SYSTEMDCONTROLLER_H

#include <QObject>

class SystemdController : public QObject
{
    Q_OBJECT
public:
    explicit SystemdController(
            const QString &unit,
            QObject *parent = nullptr);
    virtual ~SystemdController();

    static SystemdController *GetInstance(const QString &unit);
    bool isActive() const;

signals:
    void serviceStopped();
    void serviceStarted();

private slots:
    void propertiesChanged(const QString &, const QVariantMap &properties, const QStringList &);

private:
    void connectProperties();
    void getActiveState();

    void setActiveState(const QString &activeState, bool init = false);

    QString m_activeState;
    QString m_path;
    QString m_unit;
};

#endif // SYSTEMDCONTROLLER_H
