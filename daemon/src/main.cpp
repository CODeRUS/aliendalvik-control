#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>

#include <sys/types.h>
#include <unistd.h>

#include "dbusservice.h"

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "restore") == 0) {
        QString appsDir = QStringLiteral("/usr/share/applications/");
        QDir appl(appsDir);
        for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
            QString path = appsDir + desktoppath;
            QFile desktop(path);
            if (desktop.open(QFile::ReadWrite | QFile::Text)) {
                QString data;
                QTextStream stream(&desktop);
                stream.setCodec("UTF-8");
                while (!stream.atEnd()) {
                    QString line = stream.readLine();
                    if (!line.startsWith(QLatin1String("MimeType")) &&
                            !line.startsWith(QLatin1String("X-Maemo-Method")) &&
                            !line.startsWith(QLatin1String("X-Maemo-Object")) &&
                            !line.startsWith(QLatin1String("X-Maemo-Service"))) {
                        data.append(line);
                        data.append("\n");
                    }
                }
                desktop.seek(0);
                desktop.resize(0);
                stream << data;
                desktop.close();
            }
        }
        return 0;
    }

    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/100000/dbus/user_bus_socket");

    QCoreApplication app(argc, argv);
    DBusService context;
    QTimer::singleShot(0, &context, &DBusService::start);

    return app.exec();
}

