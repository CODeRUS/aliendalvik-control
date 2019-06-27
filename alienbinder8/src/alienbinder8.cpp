#include "activitymanager.h"
#include "alienbinder8.h"
#include "intent.h"
#include "windowmanager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include <unistd.h>
#include <pwd.h>
#include <grp.h>

static const QString s_dataPath = QStringLiteral("/home/.android/data");
static const QString s_sdcardMountPath = QStringLiteral("/home/.android/data/media/0/sdcard_external");

static const QString s_sessionBusConnection = QStringLiteral("ad8connection");

static AlienBinder8 *s_instance = nullptr;

extern "C" AlienAbstract *instance(QObject *parent)
{
    if (!s_instance) {
        s_instance = new AlienBinder8(parent);
    }

    return s_instance;
}

AlienBinder8::AlienBinder8(QObject *parent)
    : AlienAbstract(parent)
{
}

void AlienBinder8::sendKeyevent(int)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienBinder8::sendInput(const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienBinder8::uriActivity(const QString &uri)
{
    Intent intent;
    intent.data = uri;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("uri")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

    ActivityManager::startActivity(intent);
}

void AlienBinder8::uriActivitySelector(const QString &uri)
{
    Intent intent;
    intent.data = uri;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("selector")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

    ActivityManager::startActivity(intent);
}

void AlienBinder8::hideNavBar(int height, int)
{
    WindowManager::setOverscan(0, 0, 0, 0, -height);
}

void AlienBinder8::showNavBar(int)
{
    WindowManager::setOverscan(0, 0, 0, 0, 0);
}

void AlienBinder8::openDownloads()
{
    launchApp(QStringLiteral("com.android.documentsui"));
}

void AlienBinder8::openSettings()
{
    launchApp(QStringLiteral("com.android.settings"));
}

void AlienBinder8::openContacts()
{
    launchApp(QStringLiteral("com.android.contacts"));
}

void AlienBinder8::openCamera()
{
    launchApp(QStringLiteral("com.android.camera2"));
}

void AlienBinder8::openGallery()
{
    launchApp(QStringLiteral("com.android.gallery3d"));
}

void AlienBinder8::openAppSettings(const QString &packageName)
{
    qDebug() << Q_FUNC_INFO << packageName;

    Intent intent;
    intent.action = QStringLiteral("android.settings.APPLICATION_DETAILS_SETTINGS");
    intent.data = QStringLiteral("package:%1").arg(packageName);

    ActivityManager::startActivity(intent);
}

void AlienBinder8::launchApp(const QString &packageName)
{
    QDBusMessage launchApp = QDBusMessage::createMethodCall(
                QStringLiteral("com.jolla.apkd"),
                QStringLiteral("/com/jolla/apkd"),
                QStringLiteral("com.jolla.apkd"),
                QStringLiteral("launchApp"));
    launchApp.setArguments({packageName});

    QDBusConnection::systemBus().send(launchApp);
}

void AlienBinder8::componentActivity(const QString &package, const QString &className, const QString &data)
{
    Q_UNUSED(className)

    launchApp(package);

    if (data.isEmpty()) {
        return;
    }

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.VIEW");
    intent.package = package;
    intent.data = data;
    ActivityManager::startActivity(intent);
}

void AlienBinder8::forceStop(const QString &packageName)
{
    ActivityManager::forceStopPackage(packageName);
}

void AlienBinder8::shareFile(const QString &filename, const QString &mimetype)
{
    qDebug() << Q_FUNC_INFO << filename << mimetype;

    QString containerPath = QStringLiteral("/storage/emulated/0");
    if (filename.startsWith(QLatin1String("/home/nemo/"))) {
        containerPath.append(filename.mid(5));
    } else if (filename.startsWith(QLatin1String("/run/media/nemo"))) {
        const QString sdcardPath = filename.section(QChar(u'/'), 0, 4);
        const QString filePath = filename.section(QChar(u'/'), 5, -1);

        if (!QFileInfo::exists(QStringLiteral("%1/%2").arg(s_sdcardMountPath, filePath))) {
            mountSdcard(sdcardPath);
        }

        containerPath = filePath;
        containerPath.prepend(QStringLiteral("/storage/emulated/0/nemo/android_storage/sdcard_external/"));
    } else {
        containerPath = filename;
    }

    qDebug() << Q_FUNC_INFO << containerPath;

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = mimetype;
    intent.extras = {
        {QStringLiteral("android.intent.extra.STREAM"), QUrl::fromLocalFile(containerPath)},
        {QStringLiteral("command"), QStringLiteral("sharing")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

ActivityManager::startActivity(intent);
}

void AlienBinder8::shareText(const QString &text)
{
    qDebug() << Q_FUNC_INFO << text;

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = QStringLiteral("text/*");
    intent.extras = {
        {QStringLiteral("android.intent.extra.TEXT"), text},
        {QStringLiteral("command"), QStringLiteral("sharing")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

    ActivityManager::startActivity(intent);
}

void AlienBinder8::doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className, const QString &launcherClass)
{
    Q_UNUSED(launcherClass)

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = mimetype;
    intent.classPackage = packageName;
    intent.className = className;
    if (filename.isEmpty()) {
        intent.extras = {
            {QStringLiteral("android.intent.extra.TEXT"), data}
        };
    } else {
        intent.extras = {
            {QStringLiteral("android.intent.extra.STREAM"), QUrl(filename)}
        };
    }
    ActivityManager::startActivity(intent);
}

QVariantList AlienBinder8::getImeList()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
    return QVariantList();
}

void AlienBinder8::triggerImeMethod(const QString &, bool)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienBinder8::setImeMethod(const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

QString AlienBinder8::getSettings(const QString &, const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
    return QString();
}

void AlienBinder8::putSettings(const QString &, const QString &, const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

QString AlienBinder8::getprop(const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
    return QString();
}

void AlienBinder8::setprop(const QString &, const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienBinder8::requestDeviceInfo()
{
    Intent intent;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("deviceInfo")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

    ActivityManager::startActivity(intent);
}

void AlienBinder8::installApk(const QString &fileName)
{
    qDebug() << Q_FUNC_INFO << fileName;

    QProcess apkdInstall;
    apkdInstall.start(QStringLiteral("/usr/bin/apkd-install"), {fileName});
    apkdInstall.waitForFinished(5000);
}

void AlienBinder8::mountSdcard(const QString mountPath)
{
    qDebug() << Q_FUNC_INFO << mountPath;

    QDir sdcardMount(s_sdcardMountPath);
    if (!sdcardMount.exists()) {
        sdcardMount.mkpath(QStringLiteral("."));

        const struct passwd *nemo_pwd = getpwnam("nemo");
        if (!nemo_pwd) {
            qWarning() << Q_FUNC_INFO << "Can't get nemo uid";
            return;
        }

        const struct group *nemo_grp = getgrnam("nemo");
        if (!nemo_grp) {
            qWarning() << Q_FUNC_INFO << "Can't get nemo gid";
            return;
        }

        const int status = chown(s_sdcardMountPath.toLatin1().constData(), nemo_pwd->pw_uid, nemo_grp->gr_gid);
        qDebug() << Q_FUNC_INFO << "Sdcard mount chown to nemo status:" << status;
    }

    const int status = QProcess::execute(QStringLiteral("/bin/mount"), {QStringLiteral("--bind"), mountPath, sdcardMount.absolutePath()});

    qDebug() << Q_FUNC_INFO << "Mounting sdcard:"
             << QString::number(status);
}

void AlienBinder8::umountSdcard()
{
    qDebug() << Q_FUNC_INFO;

    QDir sdcardMount(s_sdcardMountPath);
    if (!sdcardMount.exists()) {
        return;
    }

    const int status = QProcess::execute(QStringLiteral("/bin/umount"), {sdcardMount.absolutePath()});

    qDebug() << Q_FUNC_INFO << "Mounting sdcard:"
             << QString::number(status);
}

void AlienBinder8::serviceStopped()
{
    qDebug() << Q_FUNC_INFO;

    umountSdcard();
}

void AlienBinder8::serviceStarted()
{

}

QString AlienBinder8::dataPath() const
{
    return s_dataPath;
}
