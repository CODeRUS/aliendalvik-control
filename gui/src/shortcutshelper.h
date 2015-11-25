#ifndef SHORTCUTSHELPER_H
#define SHORTCUTSHELPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDBus>

class ShortcutsHelper : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutsHelper(QObject *parent = 0);

    Q_PROPERTY(int longPressDelay READ getLongPressDelay WRITE setLongPressDelay NOTIFY longPressDelayChanged)
    int getLongPressDelay();
    void setLongPressDelay(int msecs);

    Q_PROPERTY(QString longPressActionOn READ getLongPressActionOn WRITE setLongPressActionOn NOTIFY longPressActionOnChanged)
    QString getLongPressActionOn();
    void setLongPressActionOn(const QString &action);

    Q_PROPERTY(QString longPressActionOff READ getLongPressActionOff WRITE setLongPressActionOff NOTIFY longPressActionOffChanged)
    QString getLongPressActionOff();
    void setLongPressActionOff(const QString &action);

    Q_PROPERTY(int doublePressDelay READ getDoublePressDelay WRITE setDoublePressDelay NOTIFY doublePressDelayChanged)
    int getDoublePressDelay();
    void setDoublePressDelay(int msecs);

    Q_PROPERTY(QString doublePressActionOn READ getDoublePressActionOn WRITE setDoublePressActionOn NOTIFY doublePressActionOnChanged)
    QString getDoublePressActionOn();
    void setDoublePressActionOn(const QString &action);

    Q_PROPERTY(QString doublePressActionOff READ getDoublePressActionOff WRITE setDoublePressActionOff NOTIFY doublePressActionOffChanged)
    QString getDoublePressActionOff();
    void setDoublePressActionOff(const QString &action);

    Q_PROPERTY(QString shortPressActionOn READ getShortPressActionOn WRITE setShortPressActionOn NOTIFY shortPressActionOnChanged)
    QString getShortPressActionOn();
    void setShortPressActionOn(const QString &action);

    Q_PROPERTY(QString shortPressActionOff READ getShortPressActionOff WRITE setShortPressActionOff NOTIFY shortPressActionOffChanged)
    QString getShortPressActionOff();
    void setShortPressActionOff(const QString &action);

    Q_PROPERTY(QString action1 READ getAction1 WRITE setAction1 NOTIFY action1Changed)
    QString getAction1();
    void setAction1(const QString &action);

    Q_PROPERTY(QString action2 READ getAction2 WRITE setAction2 NOTIFY action2Changed)
    QString getAction2();
    void setAction2(const QString &action);

    Q_PROPERTY(QString bannerPath READ bannerPath NOTIFY bannerPathChanged)
    QString bannerPath() const;

    Q_INVOKABLE void resetToDefaults();

    Q_INVOKABLE void checkActivation(const QString &code);

    Q_INVOKABLE QString readDesktopName(const QString &path) const;

private:
    QDBusInterface *iface;
    QNetworkAccessManager *nam;
    QString _bannerPath;

private slots:
    void onActivationReply();
    void createInterface();

signals:
    void bannerPathChanged();

    void longPressDelayChanged();
    void longPressActionOnChanged();
    void longPressActionOffChanged();
    void doublePressDelayChanged();
    void doublePressActionOnChanged();
    void doublePressActionOffChanged();
    void shortPressActionOnChanged();
    void shortPressActionOffChanged();
    void action1Changed();
    void action2Changed();
};

#endif // SHORTCUTSHELPER_H
