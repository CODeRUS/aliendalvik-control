#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>

#include <sys/types.h>
#include <unistd.h>

#include "dbusmain.h"

int main(int argc, char *argv[])
{
    ::setgid(0);
    ::setuid(0);

//    if (argc > 1 && strcmp(argv[1], "restore") == 0) {
//        QString appsDir = QStringLiteral("/usr/share/applications/");
//        QDir appl(appsDir);
//        for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
//            QString path = appsDir + desktoppath;
//            QFile desktop(path);
//            if (desktop.open(QFile::ReadWrite | QFile::Text)) {
//                QString data;
//                QTextStream stream(&desktop);
//                stream.setCodec("UTF-8");
//                while (!stream.atEnd()) {
//                    QString line = stream.readLine();
//                    if (!line.startsWith(QLatin1String("MimeType")) &&
//                            !line.startsWith(QLatin1String("X-Maemo-Method")) &&
//                            !line.startsWith(QLatin1String("X-Maemo-Object")) &&
//                            !line.startsWith(QLatin1String("X-Maemo-Service"))) {
//                        data.append(line);
//                        data.append("\n");
//                    }
//                }
//                desktop.seek(0);
//                desktop.resize(0);
//                stream << data;
//                desktop.close();
//            }
//        }
//        return 0;
//    }

//    ::chroot("/opt/alien");
//    ::chdir("/");

    // Hacka hacking hacky-hacky hacked hacku hacka hack.
    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/100000/dbus/user_bus_socket");

//    qputenv("LD_LIBRARY_PATH", "/system/vendor/lib:/system/lib:/vendor/lib:/system_jolla/lib:");
//    qputenv("SYSTEM_USER_LANG", "C");

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

//    QFile init("/system/script/start_alien.sh");
//    if (init.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&init);
//        while (!in.atEnd()) {
//            QString line = in.readLine();
//            if (line.startsWith("export BOOTCLASSPATH=")) {
//                line=line.mid(21).replace("$FRAMEWORK", "/system/framework");
//                qputenv("BOOTCLASSPATH", line.toUtf8());
//                break;
//            }
//        }
//    }
//    else {
//        return 0;
//    }

//    QFile envsetup("/system/script/platform_envsetup.sh");
//    if (envsetup.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&envsetup);
//        while (!in.atEnd()) {
//            QString line = in.readLine();
//            if (line.startsWith("export ALIEN_ID=")) {
//                line=line.mid(16);
//                qputenv("ALIEN_ID", line.toUtf8());
//                break;
//            }
//        }
//    }
//    else {
//        return 0;
//    }

    QScopedPointer<QCoreApplication> app(new QCoreApplication(argc, argv));
    QScopedPointer<DBusMain> dbus(new DBusMain(app.data()));
    QTimer::singleShot(100, dbus.data(), SLOT(start()));

    return app->exec();
}

