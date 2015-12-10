#ifndef SCREENSHOTCONTROL_H
#define SCREENSHOTCONTROL_H

#include <QObject>
#include <QQuickView>
#include <QtQml>
#include <qpa/qplatformnativeinterface.h>

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
    Q_INVOKABLE void deleteView(QQuickView *view);

private slots:
    void doCapture();

    void onCaptureFinished(QDBusPendingCallWatcher *call);

    void onViewDestroyed();
    void onViewClosing(QQuickCloseEvent *);

private:
    QDBusInterface *iface;
    QHash<QDBusPendingCallWatcher*, QString> pendingScreenshots;

    bool _busy;

signals:
    void busyChanged();
};

#endif // SCREENSHOTCONTROL_H
