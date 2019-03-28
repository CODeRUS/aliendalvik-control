#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>
#include <QScopedPointer>
#include <sailfishapp.h>

#include <qpa/qplatformnativeinterface.h>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    QObject::connect(view->engine(), &QQmlEngine::quit, app.data(), &QGuiApplication::quit);

    view->setColor(QColor(Qt::transparent));
    view->setClearBeforeRendering(true);
    view->create();

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QStringLiteral("CATEGORY"), QStringLiteral("notification"));

    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return app->exec();
}
