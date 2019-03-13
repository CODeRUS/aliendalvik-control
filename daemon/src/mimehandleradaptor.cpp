#include "mimehandleradaptor.h"
#include "inotifywatcher.h"

#include "activitymanager.h"
#include "packagemanager.h"
#include "intent.h"
#include "resolveinfo.h"
#include "componentinfo.h"
#include "appopsservice.h"
#include "intentsender.h"
#include "parcel.h"
#include "alienservice.h"

#include "aliendalvikcontrol_adaptor.h"

#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <QDebug>
#include <QCoreApplication>
#include <QDBusReply>
#include <QMetaObject>
#include <QTimer>
#include <QUrl>

static const QString c_dbus_service = QStringLiteral("org.coderus.aliendalvikcontrol");
static const QString c_dbus_path = QStringLiteral("/");

MimeHandlerAdaptor::MimeHandlerAdaptor(QObject *parent)
    : QObject(parent)
    , _watcher(new INotifyWatcher(this))
{
    _watchDir = QStringLiteral("/usr/share/applications/");
    _watcher->addPaths({ _watchDir });
    connect(_watcher, &INotifyWatcher::contentChanged, [this](const QString &path, bool created) {
        qDebug() << Q_FUNC_INFO << "contentChanged:" << path << "created:" << created;
        if (created) {
            desktopChanged(path);
        }
    });
    connect(_watcher, &INotifyWatcher::fileChanged, [this](const QString &path, bool removed) {
        qDebug() << Q_FUNC_INFO << "fileChanged:" << path << "removed:" << removed;
        if (!removed) {
            desktopChanged(path);
        }
    });
    readApplications(_watchDir);

    _isTopmostAndroid = false;
    QDBusConnection::systemBus().connect(QString(),
                                         QString(),
                                         QStringLiteral("org.nemomobile.compositor"),
                                         QStringLiteral("privateTopmostWindowProcessIdChanged"),
                                         this, SLOT(topmostIdChanged(int)));

    QDBusConnection::systemBus().connect(QString(),
                                         QStringLiteral("/org/freedesktop/systemd1/unit/aliendalvik_2eservice"),
                                         QStringLiteral("org.freedesktop.DBus.Properties"),
                                         QStringLiteral("PropertiesChanged"),
                                         this, SLOT(aliendalvikChanged(QString, QVariantMap, QStringList)));

    apkdIface = new QDBusInterface(QStringLiteral("com.jolla.apkd"),
                                   QStringLiteral("/com/jolla/apkd"),
                                   QStringLiteral("com.jolla.apkd"),
                                   QDBusConnection::systemBus(), this);

    ActivityManager::GetInstance();
    PackageManager::GetInstance();
    AppOpsService::GetInstance();
    AlienService::GetInstance();
}

MimeHandlerAdaptor::~MimeHandlerAdaptor()
{
}

void MimeHandlerAdaptor::start()
{
    QDBusConnection connection = QDBusConnection::systemBus();

    if (connection.interface()->isServiceRegistered(c_dbus_service)) {
        qWarning() << Q_FUNC_INFO << "Service already registered:" << c_dbus_service;
        return;
    }

    if (!connection.registerObject(c_dbus_path, this)) {
        qCritical() << Q_FUNC_INFO << "Cannot register object:" << c_dbus_path;
        QCoreApplication::quit();
        return;
    }

    qWarning() << Q_FUNC_INFO << "Object registered:" << c_dbus_path;

    if (!connection.registerService(c_dbus_service)) {
        qCritical() << Q_FUNC_INFO << "Cannot register D-Bus service:" << c_dbus_service;
        QCoreApplication::quit();
        return;
    }

    m_adaptor = new DBusAdaptor(this);
}

void MimeHandlerAdaptor::sendKeyevent(int code)
{
    appProcess("input.jar", QStringList() << "com.android.commands.input.Input" << "keyevent" << QString::number(code));
}

void MimeHandlerAdaptor::sendInput(const QString &text)
{
    appProcess("input.jar", QStringList() << "com.android.commands.input.Input" << "text" << text);
}

void MimeHandlerAdaptor::broadcastIntent(const QString &intent)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "broadcast" << "-a" << intent);
}

void MimeHandlerAdaptor::startIntent(const QString &intent)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << intent.split(" "));
}

void MimeHandlerAdaptor::uriActivity(const QString &uri)
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-d" << uri.toString());
    Intent intent;
    intent.action = QStringLiteral("android.intent.action.VIEW");
    intent.data = uri;
    ActivityManager::startActivity(intent);
}

void MimeHandlerAdaptor::uriActivitySelector(const QString &uri)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "--selector" << "-d" << uri);
}

void MimeHandlerAdaptor::hideNavBar()
{
    appProcess("wm.jar", QStringList() << "com.android.commands.wm.Wm" << "overscan" << "0,0,0,-144");
//    runCommand("/system/bin/service", QStringList() << "call" << "activity" << "42" << "s16" << "com.android.systemui");
}

void MimeHandlerAdaptor::showNavBar()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "startservice" << "-n" << "com.android.systemui/.SystemUIService");
    appProcess("wm.jar", QStringList() << "com.android.commands.wm.Wm" << "overscan" << "0,0,0,0");
}

void MimeHandlerAdaptor::openDownloads()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.documentsui/.LauncherActivity");
    launchPackage("com.android.documentsui");
}

void MimeHandlerAdaptor::openSettings()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.settings.SETTINGS");
    launchPackage("com.android.settings");
}

void MimeHandlerAdaptor::openContacts()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.contacts/com.android.contacts.activities.PeopleActivity");
    launchPackage("com.android.contacts");
}

void MimeHandlerAdaptor::openCamera()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.media.action.IMAGE_CAPTURE");
    launchPackage("com.android.camera2");
}

void MimeHandlerAdaptor::openGallery()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.gallery3d/com.android.gallery3d.app.GalleryActivity");
    launchPackage("com.android.gallery3d");
}

void MimeHandlerAdaptor::openAppSettings(const QString &package)
{
    qDebug() << Q_FUNC_INFO << package;

    Intent intent;
    intent.action = QStringLiteral("android.settings.APPLICATION_DETAILS_SETTINGS");
    intent.data = QStringLiteral("package:%1").arg(package);

    qDebug() << Q_FUNC_INFO << "startActivity" << ActivityManager::GetInstance();

    ActivityManager::startActivity(intent);
}

void MimeHandlerAdaptor::launchApp(const QString &packageName)
{
    launchPackage(packageName);
}

void MimeHandlerAdaptor::forceStop(const QString &packageName)
{
    ActivityManager::forceStopPackage(packageName);
}

void MimeHandlerAdaptor::getImeList()
{
    QString fullOutput = appProcessOutput("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "list" << "-s" << "-a");
    QStringList fullOutputLines = fullOutput.trimmed().split("\n");
    qDebug() << fullOutput.trimmed();

    QString enabledOutput = appProcessOutput("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "list" << "-s");
    QStringList enabledOutputLines = enabledOutput.trimmed().split("\n");
    qDebug() << enabledOutput.trimmed();

    QVariantList imeList;
    for (const QString &imeName : fullOutputLines) {
        QVariantMap imeMethod;
        imeMethod["name"] = imeName;
        imeMethod["enabled"] = enabledOutputLines.contains(imeName);
        imeList.append(imeMethod);
    }

    emit m_adaptor->imeAvailable(imeList);
}

void MimeHandlerAdaptor::triggerImeMethod(const QString &ime, bool enable)
{
    appProcess("ime.jar", QStringList() << "com.android.commands.ime.Ime" << (enable ? "enable" : "disable") << ime);
}

void MimeHandlerAdaptor::setImeMethod(const QString &ime)
{
    appProcess("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "set" << ime);
}

void MimeHandlerAdaptor::shareFile(const QString &filename, const QString &mimetype)
{
    qDebug() << Q_FUNC_INFO << filename << mimetype;

    QString containerPath = QStringLiteral("/storage/emulated/0");
    if (filename.startsWith(QStringLiteral("/home/nemo/"))) {
        containerPath.append(filename.mid(5));
    } else if (filename.startsWith(QStringLiteral("/run/media/nemo"))) {
        checkSdcardMount();

        containerPath = filename;
        for (int i = 0; i < 5; i++) {
            const int index = containerPath.indexOf(QChar(u'/'));
            containerPath = containerPath.mid(index + 1);
        }
        containerPath.prepend(QStringLiteral("/storage/emulated/0/nemo/android_storage/sdcard_external/"));
    } else {
        containerPath = filename;
    }

    qDebug() << Q_FUNC_INFO << containerPath;

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = mimetype;
    intent.extras = {
        {"android.intent.extra.STREAM", QUrl::fromLocalFile(containerPath)}
    };

    QList<QSharedPointer<ResolveInfo>> resolveInfo = PackageManager::queryIntentActivities(intent);
    qDebug() << Q_FUNC_INFO << "resolveInfo size:" << resolveInfo.size();

    QVariantList sharingList;

    for (QSharedPointer<ResolveInfo> resolved : resolveInfo) {
        ComponentInfo *info = resolved->getComponentInfo();
        if (!info) {
            continue;
        }
        const QString packageName = info->packageName;
        if ((packageName == QLatin1String("com.android.bluetooth"))
                || (packageName == QLatin1String("com.myriadgroup.nativeapp.email"))
                || (packageName == QLatin1String("com.myriadgroup.nativeapp.messages"))) {
            continue;
        }
        const QString className = resolved->getComponentInfo()->name;

        const int uid = PackageManager::getPackageUid(packageName);
        qDebug() << packageName << uid;

        const QString prettyName = AlienService::getPrettyName(uid);
        qDebug() << prettyName;

        QVariantMap sharing;
        sharing.insert(QStringLiteral("mimetype"), intent.type);
        sharing.insert(QStringLiteral("filename"), containerPath);
        sharing.insert(QStringLiteral("data"), QString());
        sharing.insert(QStringLiteral("packageName"), packageName);
        sharing.insert(QStringLiteral("className"), className);
        sharing.insert(QStringLiteral("uid"), uid);
        sharing.insert(QStringLiteral("prettyName"), prettyName);
        sharingList.append(sharing);
    }

    emit m_adaptor->sharingListReady(sharingList);
}

void MimeHandlerAdaptor::shareText(const QString &text)
{
    qDebug() << Q_FUNC_INFO << text;

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = QStringLiteral("text/*");
    intent.extras = {
        {"android.intent.extra.TEXT", text}
    };

    QList<QSharedPointer<ResolveInfo>> resolveInfo = PackageManager::queryIntentActivities(intent);
    qDebug() << Q_FUNC_INFO << "resolveInfo size:" << resolveInfo.size();

    QVariantList sharingList;

    for (QSharedPointer<ResolveInfo> resolved : resolveInfo) {
        const QString packageName = resolved->getComponentInfo()->packageName;
        if ((packageName == QLatin1String("com.android.bluetooth"))
                || (packageName == QLatin1String("com.myriadgroup.nativeapp.email"))
                || (packageName == QLatin1String("com.myriadgroup.nativeapp.messages"))) {
            continue;
        }
        const QString className = resolved->getComponentInfo()->name;

        const int uid = PackageManager::getPackageUid(packageName);
        qDebug() << packageName << uid;

        const QString prettyName = AlienService::getPrettyName(uid);
        qDebug() << prettyName;

        QVariantMap sharing;
        sharing.insert(QStringLiteral("mimetype"), intent.type);
        sharing.insert(QStringLiteral("filename"), QString());
        sharing.insert(QStringLiteral("data"), text);
        sharing.insert(QStringLiteral("packageName"), packageName);
        sharing.insert(QStringLiteral("className"), className);
        sharing.insert(QStringLiteral("uid"), uid);
        sharing.insert(QStringLiteral("prettyName"), prettyName);
        sharingList.append(sharing);
    }

    emit m_adaptor->sharingListReady(sharingList);
}

void MimeHandlerAdaptor::doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className)
{
    qDebug() << Q_FUNC_INFO << mimetype << filename << data << packageName << className;

    forceStop(packageName);
    launchPackage(packageName);

    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(1000);
    loop.exec();

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.SEND");
    intent.type = mimetype;
    intent.classPackage = packageName;
    intent.className = className;
    if (filename.isEmpty()) {
        intent.extras = {
            {"android.intent.extra.TEXT", data}
        };
    } else {
        intent.extras = {
            {"android.intent.extra.STREAM", QUrl::fromLocalFile(filename)}
        };
    }
    ActivityManager::startActivity(intent);
}

QString MimeHandlerAdaptor::getFocusedApp()
{
    QProcess *proc = new QProcess(this);
    proc->start("/system/bin/dumpsys", QStringList() << "window" << "windows");
    proc->waitForFinished(5000);
    if (proc->state() == QProcess::Running) {
        proc->close();
        proc->deleteLater();
        return QString();
    }
    QString output = QString::fromUtf8(proc->readAll());
    int pos1 = output.indexOf("mCurrentFocus");
    int pos2 = output.indexOf("\n", pos1);
    output = output.mid(pos1, pos2 - pos1);
    if (output.indexOf(" ") > 0) {
        output = output.split(" ")[1];
        output = output.split("/")[0];
        return output;
    }

    return QString();
}

bool MimeHandlerAdaptor::isTopmostAndroid()
{
    return _isTopmostAndroid;
}

QString MimeHandlerAdaptor::getSettings(const QString &nspace, const QString &key)
{
    QString value = appProcessOutput("settings.jar", QStringList() << "com.android.commands.settings.SettingsCmd" << "get" << nspace << key);
    return value.trimmed();
}

void MimeHandlerAdaptor::putSettings(const QString &nspace, const QString &key, const QString &value)
{
    appProcess("settings.jar", QStringList() << "com.android.commands.settings.SettingsCmd" << "put" << nspace << key << value);
}

QString MimeHandlerAdaptor::getprop(const QString &key)
{
    QString value = runCommandOutput("/system/bin/getprop", QStringList() << key);
    return value.trimmed();
}

void MimeHandlerAdaptor::setprop(const QString &key, const QString &value)
{
    runCommand("/system/bin/setprop", QStringList() << key << value);
}

void MimeHandlerAdaptor::quit()
{
    QTimer::singleShot(10, qApp, SLOT(quit()));
}

void MimeHandlerAdaptor::launchPackage(const QString &packageName)
{
    apkdIface->call(QStringLiteral("launchApp"), packageName);
}

void MimeHandlerAdaptor::checkSdcardMount()
{
    const QString sdcardDev = QStringLiteral("/dev/mmcblk1p1");
    if (!QFileInfo::exists(sdcardDev)) {
        qWarning() << Q_FUNC_INFO << "No sdcard device found!";
        return;
    }

    const QString sdcardMountPath = QStringLiteral("/home/nemo/android_storage/sdcard_external");
    QDir sdcardMount(sdcardMountPath);
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

        const int status = chown(sdcardMountPath.toLatin1().constData(), nemo_pwd->pw_uid, nemo_grp->gr_gid);
        qDebug() << Q_FUNC_INFO << "Sdcard mount chown to nemo status:" << status;
    }

    const int status = QProcess::execute(QStringLiteral("/bin/mount"), {sdcardDev, sdcardMount.absolutePath()});

    qDebug() << Q_FUNC_INFO << "Mounting sdcard:"
             << QString::number(status);
}

void MimeHandlerAdaptor::componentActivity(const QString &package, const QString &className, const QString &data)
{
    qDebug() << Q_FUNC_INFO << package << className << data;

    launchPackage(package);

    if (data.isEmpty()) {
        return;
    }

    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(1000);
    loop.exec();

    Intent intent;
    intent.action = QStringLiteral("android.intent.action.VIEW");
    intent.package = package;
    intent.data = data;
    ActivityManager::startActivity(intent);
}

void MimeHandlerAdaptor::appProcess(const QString &jar, const QStringList &params)
{
    qputenv("CLASSPATH", QString("/system/framework/%1").arg(jar).toUtf8());

    QString lxc = "/usr/bin/lxc-attach";
    QString program = "/system/bin/app_process";
    QStringList arguments;
    arguments << "-n" << "aliendalvik" << "--" << program << "/system/bin" << params;

    qDebug() << "Executing" << lxc << arguments;

    QProcess::startDetached(lxc, arguments);
}

QString MimeHandlerAdaptor::appProcessOutput(const QString &jar, const QStringList &params)
{
    qputenv("CLASSPATH", QString("/system/framework/%1").arg(jar).toUtf8());

    QString lxc = "/usr/bin/lxc-attach";
    QString program = "/system/bin/app_process";
    QStringList arguments;
    arguments << "-n" << "aliendalvik" << "--" << program << "/system/bin" << params;

    qDebug() << "Executing" << lxc << arguments;

    QProcess *process = new QProcess(this);
    process->start(lxc, arguments);
    process->waitForFinished(5000);
    if (process->state() == QProcess::Running) {
        process->close();
        process->deleteLater();
        return QString();
    }
    QString output = QString::fromUtf8(process->readAll());
    return output;
}

QString MimeHandlerAdaptor::packageName(const QString &package)
{
    QProcess *proc = new QProcess(this);
    proc->start("/system/bin/dumpsys", QStringList() << "package" << package);
    proc->waitForFinished(5000);
    if (proc->state() == QProcess::Running) {
        proc->close();
        proc->deleteLater();
        return QString();
    }
    QString output = QString::fromUtf8(proc->readAll());
    return output;
}

void MimeHandlerAdaptor::runCommand(const QString &program, const QStringList &params)
{
    qDebug() << "Executing" << program << params;

    QString lxc = "/usr/bin/lxc-attach";
    QStringList arguments;
    arguments << "-n" << "aliendalvik" << "--" << program << params;
    QProcess::startDetached(lxc, arguments);
}

QString MimeHandlerAdaptor::runCommandOutput(const QString &program, const QStringList &params)
{
    QProcess *process = new QProcess(this);
    QString lxc = "/usr/bin/lxc-attach";
    QStringList arguments;
    arguments << "-n" << "aliendalvik" << "--" << program << params;
    process->start(lxc, arguments);
    process->waitForFinished(5000);
    if (process->state() == QProcess::Running) {
        process->close();
        process->deleteLater();
        return QString();
    }
    const QString output = QString::fromUtf8(process->readAll());
    return output;
}

void MimeHandlerAdaptor::desktopChanged(const QString &path)
{
    QFile desktop(path);
    if (!desktop.exists()) {
        return;
    }
    if (!desktop.open(QFile::ReadWrite | QFile::Text)) {
        return;
    }
    QString content = QString::fromUtf8(desktop.readAll());
    if (content.contains("X-Maemo-Service")) {
        return;
    }
    int off0 = content.indexOf("Exec=apkd-launcher ");
    if (off0 == -1) {
        return;
    }
    int off1 = content.indexOf(" ", off0 + 20) + 1;
    int off2 = content.indexOf("\n", off1) - 1;
    QStringList data = content.mid(off1, off2 - off1 + 1).split("/");
    QString package = data[0];
    QString activity = QStringLiteral("empty");
    if (data.length() >= 2) {
        activity = data[1];
    }
    activity = QString::fromLatin1(activity.toLatin1().toPercentEncoding(QByteArray(), QByteArray("-._~")).replace("%", "_"));
    qDebug() << path << package << activity;
    QTextStream out(&desktop);
    desktop.seek(desktop.size() - 1);
    QString sym = out.read(1);
    if (sym != "\n") {
        out << "\n";
    }
    out << "X-Maemo-Service=org.coderus.aliendalvikcontrol\n";
    out << "X-Maemo-Object-Path=/\n";
    out << "X-Maemo-Method=" + package + "." + activity + "\n";
}

void MimeHandlerAdaptor::topmostIdChanged(int pId)
{
    QProcess *proc = new QProcess(this);
    proc->start("/bin/ps", QStringList() << "-p" << QString::number(pId) << "-o" << "comm=");
    proc->waitForFinished(1000);
    if (proc->state() != QProcess::NotRunning) {
        _isTopmostAndroid = false;
        return;
    }
    QByteArray out = proc->readAll().trimmed();
    _isTopmostAndroid = (out == "system_server");
}

void MimeHandlerAdaptor::aliendalvikChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    if (properties.value(QStringLiteral("ActiveState")).toString() == QLatin1String("active")) {
        ActivityManager::GetInstance()->reconnect();
        PackageManager::GetInstance()->reconnect();
        AppOpsService::GetInstance()->reconnect();
        AlienService::GetInstance()->reconnect();
    }
}

void MimeHandlerAdaptor::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
        desktopChanged(_watchDir + desktoppath);
    }
}

