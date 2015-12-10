#include "screenshotcontrol.h"

#include <QGuiApplication>
#include <QTimer>
#include <QDebug>

ScreenshotControl::ScreenshotControl(QObject *parent) : QObject(parent)
{
    _busy = false;

    iface = new QDBusInterface("org.nemomobile.lipstick",
                               "/org/nemomobile/lipstick/screenshot",
                               "org.nemomobile.lipstick",
                               QDBusConnection::sessionBus(), this);
}

ScreenshotControl *ScreenshotControl::GetInstance(QObject *parent)
{
    static ScreenshotControl* lsSingleton = NULL;
    if (!lsSingleton) {
        lsSingleton = new ScreenshotControl(parent);
    }
    return lsSingleton;
}

bool ScreenshotControl::busy()
{
    return _busy;
}

void ScreenshotControl::save(int delay)
{
    if (_busy) {
        return;
    }
    else {
        _busy = true;
        Q_EMIT busyChanged();
    }

    QTimer::singleShot(delay, this, SLOT(doCapture()));
}

void ScreenshotControl::deleteView(QQuickView *view)
{
    view->close();
    view->deleteLater();
}

void ScreenshotControl::doCapture()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    folder.append("/Screenshots");

    QDir test;
    if (!test.exists(folder)) {
        test.mkpath(folder);
    }

    QString pendingScreenshot = QString("%1/Screenshot-%2.png")
            .arg(folder)
            .arg(QDateTime::currentDateTime().toString("yy-MM-dd-hh-mm-ss"));

    QDBusPendingCall async = iface->asyncCall("saveScreenshot", pendingScreenshot);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
    pendingScreenshots.insert(watcher, pendingScreenshot);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onCaptureFinished(QDBusPendingCallWatcher*)));
}

void ScreenshotControl::onCaptureFinished(QDBusPendingCallWatcher *call)
{
    if (call->error().type() == QDBusError::NoError) {
        QString screenshot = pendingScreenshots.take(call);
        if (screenshot.isEmpty()) {
            return;
        }

        QQuickView *view = new QQuickView();
        view->setTitle("Screenshot");
        view->rootContext()->setContextProperty("view", view);
        view->rootContext()->setContextProperty("Screenshot", this);
        view->rootContext()->setContextProperty("screenshotPath", screenshot);

        QColor color;
        color.setRedF(0.0);
        color.setGreenF(0.0);
        color.setBlueF(0.0);
        color.setAlphaF(0.0);
        view->setColor(color);
        view->setClearBeforeRendering(true);

        view->setSource(QUrl::fromLocalFile("/usr/share/powermenu2/qml/screenshot.qml"));
        view->create();
        QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
        native->setWindowProperty(view->handle(), QLatin1String("CATEGORY"), "notification");
        native->setWindowProperty(view->handle(), QLatin1String("MOUSE_REGION"), QRegion(0,0,0,0));
        view->show();

        QObject::connect(view, SIGNAL(destroyed()), this, SLOT(onViewDestroyed()));
        QObject::connect(view, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onViewClosing(QQuickCloseEvent*)));
    }
}

void ScreenshotControl::onViewDestroyed()
{
    QQuickView *view = qobject_cast<QQuickView*>(sender());
    qDebug() << view;
    if (view) {
        QObject::disconnect(view, 0, 0, 0);
    }

    _busy = false;
    Q_EMIT busyChanged();
}

void ScreenshotControl::onViewClosing(QQuickCloseEvent *)
{
    QQuickView *view = qobject_cast<QQuickView*>(sender());
    qDebug() << view;
    if (view) {
        view->destroy();
        view->deleteLater();
    }
}

