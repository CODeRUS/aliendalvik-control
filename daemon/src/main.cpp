#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>

#include "mimehandleradaptor.h"

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

    // Hacka hacking hacky-hacky hacked hacku hacka hack.
    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/100000/dbus/user_bus_socket");

    qputenv("ANDROID_ROOT", "/system");
    qputenv("ANDROID_DATA", "/data");

    qputenv("BOOTCLASSPATH", "/system/framework/core-oj.jar:"
                             "/system/framework/core-libart.jar:"
                             "/system/framework/conscrypt.jar:"
                             "/system/framework/okhttp.jar:"
                             "/system/framework/bouncycastle.jar:"
                             "/system/framework/apache-xml.jar:"
                             "/system/framework/legacy-test.jar:"
                             "/system/framework/ext.jar:"
                             "/system/framework/framework.jar:"
                             "/system/framework/telephony-common.jar:"
                             "/system/framework/voip-common.jar:"
                             "/system/framework/ims-common.jar:"
                             "/system/framework/org.apache.http.legacy.boot.jar:"
                             "/system/framework/android.hidl.base-V1.0-java.jar:"
                             "/system/framework/android.hidl.manager-V1.0-java.jar");


    QCoreApplication app(argc, argv);
    MimeHandlerAdaptor context;
    QTimer::singleShot(0, &context, &MimeHandlerAdaptor::start);

    return app.exec();
}

