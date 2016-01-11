#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QDebug>

#include <sys/types.h>
#include <unistd.h>

#include "dbusmain.h"

int main(int argc, char *argv[])
{
    ::setgid(0);
    ::setuid(0);

    if (argc > 1 && strcmp(argv[1], "restore") == 0) {
        QString appsDir("/usr/share/applications/");
        QDir appl(appsDir);
        foreach (const QString &desktoppath, appl.entryList(QStringList() << "apkd_launcher_*.desktop")) {
            QString path = appsDir + desktoppath;
            QFile desktop(path);
            if (desktop.open(QFile::ReadWrite | QFile::Text)) {
                QString content = QString::fromUtf8(desktop.readAll());
                if (content.contains("MimeType")) {
                    int off0 = content.indexOf("MimeType");
                    desktop.resize(off0);
                }
                desktop.close();
            }
        }
        return 0;
    }

    ::chroot("/opt/alien");
    ::chdir("/");

    // Hacka hacking hacky-hacky hacked hacku hacka hack.
    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/100000/dbus/user_bus_socket");

    qputenv("LD_LIBRARY_PATH", "/system/vendor/lib:/system/lib:/vendor/lib:/system_jolla/lib:");
    qputenv("SYSTEM_USER_LANG", "C");

    qputenv("ANDROID_ROOT", "/system");
    qputenv("ANDROID_DATA", "/data");

    QFile init("/system/script/start_alien.sh");
    if (init.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&init);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("export BOOTCLASSPATH=")) {
                line=line.mid(21).replace("$FRAMEWORK", "/system/framework");
                qputenv("BOOTCLASSPATH", line.toUtf8());
                break;
            }
        }
    }
    else {
        return 0;
    }

    QFile envsetup("/system/script/platform_envsetup.sh");
    if (envsetup.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&envsetup);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("export ALIEN_ID=")) {
                line=line.mid(16);
                qputenv("ALIEN_ID", line.toUtf8());
                break;
            }
        }
    }
    else {
        return 0;
    }

    QScopedPointer<QCoreApplication> app(new QCoreApplication(argc, argv));
    QScopedPointer<DBusMain> dbus(new DBusMain(app.data()));
    QTimer::singleShot(100, dbus.data(), SLOT(start()));

    return app->exec();
}

