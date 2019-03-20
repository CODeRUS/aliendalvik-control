#include "mimehandleradaptor.h"
#include "inotifywatcher.h"

#include "activitymanager.h"
#include "packagemanager.h"
#include "intent.h"
#include "resolveinfo.h"
#include "componentinfo.h"
#include "applicationinfo.h"
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
#include <QLocalSocket>
#include <QLocalServer>

static const QString c_dbus_service = QStringLiteral("org.coderus.aliendalvikcontrol");
static const QString c_dbus_path = QStringLiteral("/");
static const QString s_sessionBusConnection = QStringLiteral("ad8connection");

static const QString s_localSocket = QStringLiteral("/home/.android/data/data/org.coderus.aliendalvikcontrol/.aliendalvik-control-socket");

MimeHandlerAdaptor::MimeHandlerAdaptor(QObject *parent)
    : AliendalvikController(parent)
    , _watcher(new INotifyWatcher(this))
    , m_sbus(s_sessionBusConnection)
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

    apkdIface = new QDBusInterface(QStringLiteral("com.jolla.apkd"),
                                   QStringLiteral("/com/jolla/apkd"),
                                   QStringLiteral("com.jolla.apkd"),
                                   QDBusConnection::systemBus(), this);

    m_sessionBusConnector = new QTimer(this);
    connect(m_sessionBusConnector, &QTimer::timeout, this, [this]() {
        QDBusConnection test = QDBusConnection::connectToBus(QDBusConnection::SessionBus, s_sessionBusConnection);
        if (!test.isConnected()) {
            QDBusConnection::disconnectFromBus(s_sessionBusConnection);
        } else {
            m_sbus = test;
            m_sessionBusConnector->stop();
            qDebug() << Q_FUNC_INFO << "Connected to session bus!";
        }
    });
    m_sessionBusConnector->setSingleShot(false);
    m_sessionBusConnector->setTimerType(Qt::VeryCoarseTimer);
    m_sessionBusConnector->setInterval(1000); // 1 second
    m_sessionBusConnector->start();

    ActivityManager::GetInstance();
    PackageManager::GetInstance();
    AppOpsService::GetInstance();
    AlienService::GetInstance();

    m_localServer = new QLocalServer(nullptr); // controlled by separate thread
    m_localServer->setSocketOptions(QLocalServer::WorldAccessOption);
    m_localServer->setMaxPendingConnections(2147483647);

    m_serverThread = new QThread(this);
    connect(m_serverThread, &QThread::finished, this, [this](){
        qWarning() << Q_FUNC_INFO << "QThread::finished";
        m_localServer->close();
        m_serverThread->start();
    });
    connect(m_localServer, &QLocalServer::newConnection, this, &MimeHandlerAdaptor::startReadingLocalServer, Qt::DirectConnection);
    connect(m_serverThread, &QThread::started, this, [this](){
        qWarning() << Q_FUNC_INFO << "QThread::started";
        bool listening = m_localServer->listen(s_localSocket);
        if (!listening // not listening
                && m_localServer->serverError() == QAbstractSocket::AddressInUseError // because of AddressInUseError
                && QFileInfo::exists(s_localSocket) // socket file already exists
                && QFile::remove(s_localSocket)) { // and successfully removed it
            qWarning() << Q_FUNC_INFO << "Removed old stuck socket";
            listening = m_localServer->listen(s_localSocket); // try to start lisening again
        }
        qDebug() << Q_FUNC_INFO << "Server listening:" << listening;
        if (!listening) {
            qWarning() << Q_FUNC_INFO << "Server error:" << m_localServer->serverError() << m_localServer->errorString();
        }
    }, Qt::DirectConnection);
    m_localServer->moveToThread(m_serverThread);

    qDebug() << Q_FUNC_INFO << "Java helper ready:"
             << checkHelperSocket(true);
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
    const QString navbarHeight = m_deviceProperties.value(QStringLiteral("navbarHeight"), QStringLiteral("144")).toString();
    appProcess("wm.jar", QStringList() << "com.android.commands.wm.Wm" << "overscan" << QStringLiteral("0,0,0,-%1").arg(navbarHeight));
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
    launchPackage(QStringLiteral("com.android.documentsui"));
}

void MimeHandlerAdaptor::openSettings()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.settings.SETTINGS");
    launchPackage(QStringLiteral("com.android.settings"));
}

void MimeHandlerAdaptor::openContacts()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.contacts/com.android.contacts.activities.PeopleActivity");
    launchPackage(QStringLiteral("com.android.contacts"));
}

void MimeHandlerAdaptor::openCamera()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.media.action.IMAGE_CAPTURE");
    launchPackage(QStringLiteral("com.android.camera2"));
}

void MimeHandlerAdaptor::openGallery()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.gallery3d/com.android.gallery3d.app.GalleryActivity");
    launchPackage(QStringLiteral("com.android.gallery3d"));
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

void MimeHandlerAdaptor::shareContent(const QVariantMap &content, const QString &source)
{
    qDebug() << Q_FUNC_INFO << content << source;

    if (source.isEmpty()) {
        const QString mimeType = content.value(QStringLiteral("type")).toString();
        if (mimeType == QLatin1String("text/vcard")) {
            const QString data = content.value(QStringLiteral("data")).toString();
            QFile tmp(QStringLiteral("/home/nemo/.aliendalvik-control-share.vcf"));
            if (tmp.open(QFile::WriteOnly)) {
                tmp.write(data.toUtf8());
                shareFile(tmp.fileName(), mimeType);
            }
        } else if (mimeType == QLatin1String("text/x-vcalendar")) {
            const QString data = content.value(QStringLiteral("data")).toString();
            QFile tmp(QStringLiteral("/home/nemo/.aliendalvik-control-share.vcs"));
            if (tmp.open(QFile::WriteOnly)) {
                tmp.write(data.toUtf8());
                shareFile(tmp.fileName(), mimeType);
            }
        } else {
            const QString data = content.value(QStringLiteral("content")).toString();
            if (!data.isEmpty()) {
                shareText(data);
            }
            const QString status = content.value(QStringLiteral("status")).toString();
            if (!status.isEmpty()) {
                shareText(status);
            }
        }
    } else {
        const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(source);
        const QUrl url = QUrl(source);

        shareFile(url.toString(QUrl::PreferLocalFile), mimeType.name());
    }
}

void MimeHandlerAdaptor::shareFile(const QString &filename, const QString &mimetype)
{
    qDebug() << Q_FUNC_INFO << filename << mimetype;
    if (!checkHelperSocket()) {
        return;
    }

    QString containerPath = QStringLiteral("/storage/emulated/0");
    if (filename.startsWith(QLatin1String("/home/nemo/"))) {
        containerPath.append(filename.mid(5));
    } else if (filename.startsWith(QLatin1String("/run/media/nemo"))) {
        const QString sdcardPath = filename.section(QChar(u'/'), 0, 4);
        const QString filePath = filename.section(QChar(u'/'), 5, -1);

        if (!QFileInfo::exists(QStringLiteral("/home/.android/data/media/0/sdcard_external/%1").arg(filePath))) {
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

void MimeHandlerAdaptor::shareText(const QString &text)
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

void MimeHandlerAdaptor::doShare(
        const QString &mimetype,
        const QString &filename,
        const QString &data,
        const QString &packageName,
        const QString &className,
        const QString &launcherClass)
{
    qDebug() << Q_FUNC_INFO << mimetype << filename << data << packageName << className << launcherClass;

    const QString packageNameReplaced = QString(packageName).replace(QChar(u'.'), QChar(u'_'));
    const QString launcherClassReplaced = QString(launcherClass).replace(QChar(u'.'), QChar(u'_'));
    const QString desktopFile = QStringLiteral("/usr/share/applications/apkd_launcher_%1-%2.desktop").arg(packageNameReplaced, launcherClassReplaced);

    if (QFileInfo::exists(desktopFile)) {
        qDebug() << Q_FUNC_INFO << "Activating desktop" << desktopFile;
        launchPackage(packageName);
        QDBusMessage activateDesktop = QDBusMessage::createMethodCall(
                QStringLiteral("com.jolla.lipstick"),
                QStringLiteral("/LauncherModel"),
                QStringLiteral("org.nemomobile.lipstick.LauncherModel"),
                QStringLiteral("notifyLaunching"));
        activateDesktop.setArguments({desktopFile});
        m_sbus.send(activateDesktop);
    } else {
        qDebug() << Q_FUNC_INFO << "Force stopping" << packageName;
        forceStop(packageName);
        launchPackage(packageName);
    }

    if (!_isTopmostAndroid) {
        QEventLoop loop;
        QTimer timer;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QMetaObject::Connection connection = connect(this, &MimeHandlerAdaptor::isTopmostAndroidChanged, [&loop](bool isTopmostAndroid){
            if (isTopmostAndroid) {
                loop.quit();
            }
        });
        timer.start(5000);
        loop.exec();
        if (!timer.isActive()) {
            qDebug() << Q_FUNC_INFO << "Timer expired. Cancel share!";
            return;
        }
        disconnect(connection);
    }

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

void MimeHandlerAdaptor::startReadingLocalServer()
{
    QLocalSocket *clientConnection = m_localServer->nextPendingConnection();
    if (!clientConnection) {
        qWarning() << Q_FUNC_INFO << "Got empty connection!";
        return;
    }
    if (clientConnection->state() != QLocalSocket::ConnectedState) {
        clientConnection->waitForConnected();
    }
    connect(clientConnection, &QLocalSocket::disconnected, this, [this, clientConnection](){
        if (m_pendingRequests.contains(clientConnection)) {
            const QByteArray data = m_pendingRequests.take(clientConnection);
            QTimer::singleShot(0, this, [this, data](){
                processHelperResult(data);
            });
        }
        clientConnection->deleteLater();
    }, Qt::DirectConnection);
    connect(clientConnection, &QLocalSocket::readyRead, this, [this, clientConnection](){
        if (!clientConnection) {
            qWarning() << Q_FUNC_INFO << "Can not get socket!";
            return;
        }
        const qint64 bytes = clientConnection->bytesAvailable();
        if (bytes <= 0) {
            clientConnection->disconnectFromServer();
            return;
        }
        const QByteArray request = clientConnection->readAll();
        if (!m_pendingRequests.contains(clientConnection)) {
            m_pendingRequests.insert(clientConnection, request);
        } else {
            m_pendingRequests[clientConnection] = m_pendingRequests[clientConnection] + request;
        }
        qWarning() << Q_FUNC_INFO << "Available:" << bytes << "Read:" << request.size();
        qDebug().noquote() << request;
    }, Qt::DirectConnection);
}

void MimeHandlerAdaptor::launchPackage(const QString &packageName)
{
    apkdIface->call(QStringLiteral("launchApp"), packageName);
}

void MimeHandlerAdaptor::mountSdcard(const QString mountPath)
{
    qDebug() << Q_FUNC_INFO << mountPath;

    const QString sdcardMountPath = QStringLiteral("/home/.android/data/media/0/sdcard_external");
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

    const int status = QProcess::execute(QStringLiteral("/bin/mount"), {QStringLiteral("--bind"), mountPath, sdcardMount.absolutePath()});

    qDebug() << Q_FUNC_INFO << "Mounting sdcard:"
             << QString::number(status);
}

void MimeHandlerAdaptor::umountSdcard()
{
    qDebug() << Q_FUNC_INFO;

    const QString sdcardMountPath = QStringLiteral("/home/.android/data/media/0/sdcard_external");
    QDir sdcardMount(sdcardMountPath);
    if (!sdcardMount.exists()) {
        return;
    }

    const int status = QProcess::execute(QStringLiteral("/bin/umount"), {sdcardMount.absolutePath()});

    qDebug() << Q_FUNC_INFO << "Mounting sdcard:"
             << QString::number(status);
}

bool MimeHandlerAdaptor::checkHelperSocket(bool remove)
{
    qDebug() << Q_FUNC_INFO;

    const QFileInfo helperSocket(s_localSocket);
    if (!helperSocket.dir().exists()) {
        qWarning() << Q_FUNC_INFO << "Helper not installed!";
        return false;
    }
    if (helperSocket.exists()) {
        if (remove) {
            QFile::remove(s_localSocket);
        } else {
            return true;
        }
    }

    QEventLoop loop;
    connect(m_serverThread, &QThread::started, &loop, &QEventLoop::quit);
    if (m_serverThread->isRunning()) {
        m_serverThread->exit(0);
    } else {
        m_serverThread->start();
    }
    loop.exec();

    return true;
}

void MimeHandlerAdaptor::requestDeviceInfo()
{
    Intent intent;
    intent.extras = {
        {QStringLiteral("command"), QStringLiteral("deviceInfo")},
    };
    intent.className = QStringLiteral("org.coderus.aliendalvikcontrol.MainActivity");
    intent.classPackage = QStringLiteral("org.coderus.aliendalvikcontrol");

    ActivityManager::startActivity(intent);
}

void MimeHandlerAdaptor::processHelperResult(const QByteArray &data)
{
    qDebug() << Q_FUNC_INFO << data;
    QJsonParseError error;
    error.error = QJsonParseError::NoError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << Q_FUNC_INFO << error.errorString();
        return;
    }
    const QJsonObject object = doc.object();
    const QString command = object.value(QStringLiteral("command")).toString();
    if (command == QLatin1String("sharing")) {
        const QVariantList sharingList = object.value(QStringLiteral("candidates")).toArray().toVariantList();
        emit m_adaptor->sharingListReady(sharingList);
    } else if (command == QLatin1String("deviceInfo")) {
        const QJsonObject devicePropertiesJson = object.value(QStringLiteral("deviceProperties")).toObject();
        m_deviceProperties = devicePropertiesJson.toVariantHash();
    }
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
    const QString content = QString::fromUtf8(desktop.readAll());
    if (content.contains(QLatin1String("X-Maemo-Service"))) {
        return;
    }
    const int off0 = content.indexOf(QLatin1String("Exec=apkd-launcher "));
    if (off0 == -1) {
        return;
    }
    const int off1 = content.indexOf(QChar(u' '), off0 + 20) + 1;
    const int off2 = content.indexOf(QChar(u'\n'), off1) - 1;
    const QStringList data = content.mid(off1, off2 - off1 + 1).split("/");
    const QString package = data.first();
    QString activity = QStringLiteral("empty");
    if (data.length() >= 2) {
        activity = data.at(1);
    }
    activity = QString::fromLatin1(activity.toLatin1().toPercentEncoding(QByteArray(), QByteArray("-._~")).replace("%", "_"));
    qDebug() << path << package << activity;
    QTextStream out(&desktop);
    desktop.seek(desktop.size() - 1);
    const QString sym = out.read(1);
    if (sym != QLatin1String("\n")) {
        out << QChar(u'\n');
    }
    out << QLatin1String("X-Maemo-Service=org.coderus.aliendalvikcontrol\n");
    out << QLatin1String("X-Maemo-Object-Path=/\n");
    out << QLatin1String("X-Maemo-Method=") << package << QChar(u'.') << activity << QChar(u'\n');
}

void MimeHandlerAdaptor::topmostIdChanged(int pId)
{
    QProcess proc;
    proc.start(QStringLiteral("/bin/ps"), {
                    QStringLiteral("-p"),
                    QString::number(pId),
                    QStringLiteral("-o"),
                    QStringLiteral("comm=")});
    proc.waitForFinished(1000);
    if (proc.state() != QProcess::NotRunning) {
        _isTopmostAndroid = false;
        return;
    }
    const QByteArray out = proc.readAll().trimmed();
    _isTopmostAndroid = (out == QByteArrayLiteral("system_server"));
    emit isTopmostAndroidChanged(_isTopmostAndroid);
}

void MimeHandlerAdaptor::serviceStopped()
{
    qDebug() << Q_FUNC_INFO;

    umountSdcard();
}

void MimeHandlerAdaptor::serviceStarted()
{
    qDebug() << Q_FUNC_INFO;

    requestDeviceInfo();
}

void MimeHandlerAdaptor::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
        desktopChanged(_watchDir + desktoppath);
    }
}

