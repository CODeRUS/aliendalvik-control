#include "nativewindowhelper.h"
#include <QGuiApplication>
#include <QQuickItem>
#include <QQuickWindow>

#include <QDebug>

#include <qpa/qplatformnativeinterface.h>

NativeWindowHelper::NativeWindowHelper(QObject *parent)
    : QObject(parent)
{

}

QObject *NativeWindowHelper::qmlSingleton(QQmlEngine *, QJSEngine *)
{
    return new NativeWindowHelper(qGuiApp);
}

void NativeWindowHelper::setTouchRegion(QQuickItem *item, bool enabled)
{
    QQuickWindow *quickWindow = item->window();
    qDebug() << quickWindow->handle();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    QRegion region;
    if (enabled) {
        const QPointF position = item->position();
        const QRectF rect = item->childrenRect();
        region = QRegion(position.x(), position.y(), rect.width(), rect.height());
        qDebug() << Q_FUNC_INFO << position << rect << region;
    }
    native->setWindowProperty(quickWindow->handle(), QStringLiteral("MOUSE_REGION"), region);
}
