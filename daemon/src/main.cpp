#include <QtQuick>
#include <QGuiApplication>

#include <sailfishapp.h>

#include "dbuslistener.h"

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

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setQuitOnLastWindowClosed(false);
    app->setApplicationDisplayName("Powermenu 2");
    app->setApplicationName("Powermenu 2");
    new DBusListener(app.data());

    return app->exec();
}

