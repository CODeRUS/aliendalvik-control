#include <QtQuick>
#include <QGuiApplication>
#include <QQuickView>

#include <sailfishapp.h>

#include "shortcutshelper.h"
#include "desktopfilesortmodel.h"

const char* msgTypeToString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return "D";
    case QtWarningMsg:
        return "W";
    case QtCriticalMsg:
        return "C";
    case QtFatalMsg:
        return "F";
        //abort();
    default:
        return "D";
    }
}

void printLog(const QString &message)
{
    QTextStream(stdout) << message;
}

QString simpleLog(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    return QString("%1 %2 [%3:%4] %5\n").arg(msgTypeToString(type))
                                        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                                        .arg(context.function)
                                        .arg(context.line)
                                        .arg(message);
}

void stdoutHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    printLog(simpleLog(type, context, msg));
    if (type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(stdoutHandler);

    qmlRegisterType<DesktopFileSortModel>("org.coderus.powermenu.desktopfilemodel", 1, 0, "DesktopFileSortModel");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationDisplayName("Powermenu2 settings");
    app->setApplicationName("Powermenu2 settings");
    app->setApplicationVersion(QString(APP_VERSION));

    QTranslator translator;
    if (translator.load("powermenu_en", "/usr/share/powermenu2/translations", QString(), ".qm")) {
        app->installTranslator(&translator);
    }

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setTitle("Powermenu2 settings");
    view->engine()->addImportPath("/usr/share/powermenu2/qml");

    view->rootContext()->setContextProperty("helper", new ShortcutsHelper(view.data()));

    view->setSource(QUrl::fromLocalFile("/usr/share/powermenu2/qml/main.qml"));
    view->showFullScreen();

    return app->exec();
}


