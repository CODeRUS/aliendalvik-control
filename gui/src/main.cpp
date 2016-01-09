#include <sailfishapp.h>
#include <QGuiApplication>
#include <QQuickView>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QString appname("aliendalvikk-control");
    QString displayname("Aliendalvik Controls");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationName(appname);
    app->setOrganizationName(appname);
    app->setApplicationDisplayName(displayname);

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/main.qml"));

    return app->exec();
}

