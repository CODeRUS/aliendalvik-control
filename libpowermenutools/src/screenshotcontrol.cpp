#include "screenshotcontrol.h"

#include <QTimer>
#include <QDBusConnection>
#include <QDBusInterface>
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
        if (!screenshot.isEmpty()) {
            QDBusInterface iface("org.coderus.powermenu", "/", "org.coderus.powermenu");
            iface.call(QDBus::NoBlock, "showScreenshot", screenshot);
        }

        _busy = false;
        Q_EMIT busyChanged();
    }
}

