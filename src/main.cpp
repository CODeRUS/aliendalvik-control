#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>

#include "dbusmain.h"

int main(int argc, char *argv[])
{
    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/100000/dbus/user_bus_socket");

    QByteArray ANDROID_ROOT("/system");
    if (argc > 1) {
        ANDROID_ROOT = argv[1];
    }
    QByteArray ANDROID_DATA("/data");

    QByteArray LD_LIBRARY_PATH(qgetenv("LD_LIBRARY_PATH"));
    LD_LIBRARY_PATH.prepend("/system_jolla/lib:");
    LD_LIBRARY_PATH.prepend("/vendor/lib:");
    LD_LIBRARY_PATH.prepend("/system/lib:");
    LD_LIBRARY_PATH.prepend("/system/vendor/lib:");
    qputenv("LD_LIBRARY_PATH", LD_LIBRARY_PATH);

    qputenv("SYSTEM_USER_LANG", "C");

    qputenv("ANDROID_ROOT", ANDROID_ROOT);
    qputenv("ANDROID_DATA", ANDROID_DATA);

    QByteArray BOOTCLASSPATH(qgetenv("BOOTCLASSPATH"));
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/apache-xml.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/services.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/android.policy.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/framework_ext.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/framework.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/ext.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/bouncycastle.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/core-junit.jar:");
    BOOTCLASSPATH.prepend(ANDROID_ROOT + "/framework/core.jar:");
    qputenv("BOOTCLASSPATH", BOOTCLASSPATH);

    QScopedPointer<QCoreApplication> app(new QCoreApplication(argc, argv));
    QScopedPointer<DBusMain> dbus(new DBusMain(app.data()));
    QTimer::singleShot(100, dbus.data(), SLOT(start()));

    return app->exec();
}

