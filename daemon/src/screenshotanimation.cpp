#include "screenshotanimation.h"

ScreenshotAnimation::ScreenshotAnimation(QObject *parent) : QObject(parent)
{

}

void ScreenshotAnimation::deleteView(QQuickView *view)
{
    view->close();
    view->deleteLater();
}

void ScreenshotAnimation::showScreenshot(const QString &path)
{
    QQuickView *view = new QQuickView();
    view->setTitle("Screenshot");
    view->rootContext()->setContextProperty("view", view);
    view->rootContext()->setContextProperty("Screenshot", this);
    view->rootContext()->setContextProperty("screenshotPath", path);

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

void ScreenshotAnimation::onViewDestroyed()
{
    QQuickView *view = qobject_cast<QQuickView*>(sender());
    qDebug() << view;
    if (view) {
        QObject::disconnect(view, 0, 0, 0);
    }
}

void ScreenshotAnimation::onViewClosing(QQuickCloseEvent *)
{
    QQuickView *view = qobject_cast<QQuickView*>(sender());
    qDebug() << view;
    if (view) {
        view->destroy();
        view->deleteLater();
    }
}

