#ifndef SCREENSHOTCONTROL_H
#define SCREENSHOTCONTROL_H

#include <QObject>

#include <QtDBus>

class Q_DECL_EXPORT ScreenshotControl : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotControl(QObject *parent = 0);

    static ScreenshotControl *GetInstance(QObject *parent = 0);

    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    bool busy();

    Q_INVOKABLE void save(int delay = 0);

private slots:
    void doCapture();

    void onCaptureFinished(QDBusPendingCallWatcher *call);

private:
    QDBusInterface *iface;
    QHash<QDBusPendingCallWatcher*, QString> pendingScreenshots;

    bool _busy;

signals:
    void busyChanged();
};

#endif // SCREENSHOTCONTROL_H
