#include "activitymanager.h"
#include "alienbinder8.h"
#include "intent.h"
#include "windowmanager.h"
#include "inputmanager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QUrl>

#include <unistd.h>
#include <pwd.h>
#include <grp.h>

static const QString s_dataPath = QStringLiteral("/home/.android/data");

static const QString s_sessionBusConnection = QStringLiteral("ad8connection");

static const QString aliendalvikControlClassName = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
static const QString aliendalvikControlClassPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

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

void AlienBinder8::sendTap(int posx, int posy, quint64 uptime)
{
    qDebug() << Q_FUNC_INFO << posx << posy << uptime;

    if (uptime == 0) {
        requestUptimePayload({{QStringLiteral("command"), QStringLiteral("sendTap")},
                              {QStringLiteral("x"), posx},
                              {QStringLiteral("y"), posy}});
    } else {
        InputManager::sendTap(posx, posy, uptime);
    }
}

void AlienBinder8::sendSwipe(int, int, int, int, int)
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
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

    ActivityManager::startActivity(intent);
}

void AlienBinder8::uriActivitySelector(const QString &uri)
{
    Intent intent;
    intent.data = uri;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("selector")},
    };
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

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
        containerPath.append(QStringLiteral("/sd"));
        containerPath.append(filename.mid(15));
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
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

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
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

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
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

    ActivityManager::startActivity(intent);
}

void AlienBinder8::requestUptime()
{
    Intent intent;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("uptime")},
    };
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

    ActivityManager::startActivity(intent);
}

QString AlienBinder8::checkShareFile(const QString &shareFilePath)
{
    qDebug() << Q_FUNC_INFO << shareFilePath;

    if (shareFilePath.startsWith(QLatin1String("/data/user/0/org.coderus.aliendalvikcontrol"))) {
        const QString fileName = shareFilePath.section(QChar(u'/'), -1);
        QFile shareFile(QStringLiteral("%1/data/org.coderus.aliendalvikcontrol/files/%2").arg(s_dataPath, fileName));
        const QString tempFilePath = QStringLiteral("/tmp/%1").arg(fileName);
        if (QFile::exists(tempFilePath)) {
            QFile::remove(tempFilePath);
        }
        if (shareFile.copy(tempFilePath)) {
            shareFile.remove();
            qWarning() << Q_FUNC_INFO << "Moved share file" << shareFile.fileName() << "to temp location" << tempFilePath;

            return tempFilePath;
        } else {
            qWarning() << Q_FUNC_INFO << "Error moving to temp location" << shareFile.errorString();
        }
    }

    return shareFilePath;
}

void AlienBinder8::installApk(const QString &fileName)
{
    qDebug() << Q_FUNC_INFO << fileName;

    QProcess apkdInstall;
    apkdInstall.start(QStringLiteral("/usr/bin/apkd-install"), {fileName});
    apkdInstall.waitForFinished(5000);
}

void AlienBinder8::requestUptimePayload(const QVariantMap &payload)
{
    Intent intent;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("uptime")},
        {QStringLiteral("payload"), QString::fromLatin1(QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact))}
    };
    intent.className = aliendalvikControlClassName;
    intent.classPackage = aliendalvikControlClassPackage;

    ActivityManager::startActivity(intent);
}

void AlienBinder8::serviceStopped()
{
    qDebug() << Q_FUNC_INFO;
}

void AlienBinder8::serviceStarted()
{

}

QString AlienBinder8::dataPath() const
{
    return s_dataPath;
}
