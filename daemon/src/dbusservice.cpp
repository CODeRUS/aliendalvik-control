#include "dbusservice.h"
#include "inotifywatcher.h"

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

static const QString s_helperApk = QStringLiteral("/usr/share/aliendalvik-control/apk/app-release.apk");
static const QString s_helperPath = QStringLiteral("/home/.android/data/app/aliendalvik-control.apk");

DBusService::DBusService(QObject *parent)
    : AliendalvikController(parent)
    , _watcher(new INotifyWatcher(this))
    , m_sbus(s_sessionBusConnection)
{
    if (QFileInfo::exists(s_helperPath)) {
        qWarning() << Q_FUNC_INFO << "Removing old helper:" <<
        QFile::remove(s_helperPath);
    }
    qWarning() << Q_FUNC_INFO << "Installing helper apk:" <<
    QFile::copy(s_helperApk, s_helperPath);

    m_alienEnvironment.insert(QStringLiteral("LD_LIBRARY_PATH"),
                              QStringLiteral("/system/vendor/lib:/system/lib:/vendor/lib:/system_jolla/lib:"));

    m_alienEnvironment.insert(QStringLiteral("PATH"),
                              QStringLiteral("/system/vendor/bin:/system/sbin:/system/bin:/system/xbin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"));

    QFile init(QStringLiteral("/opt/alien/system/script/start_alien.sh"));
    if (init.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&init);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(QLatin1String("export BOOTCLASSPATH="))) {
                line=line.mid(21).replace(QLatin1String("$FRAMEWORK"), QLatin1String("/system/framework"));
                m_alienEnvironment.insert(QStringLiteral("BOOTCLASSPATH"), line);
                break;
            }
        }
    }

    QFile envsetup(QStringLiteral("/opt/alien/system/script/platform_envsetup.sh"));
    if (envsetup.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&envsetup);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(QLatin1String("export ALIEN_ID="))) {
                line=line.mid(16);
                m_alienEnvironment.insert(QStringLiteral("ALIEN_ID"), line);
                break;
            }
        }
    }

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
    QDBusMessage pidMsg = QDBusMessage::createMethodCall(
                QStringLiteral("org.nemomobile.lipstick"),
                QStringLiteral("/"),
                QStringLiteral("org.nemomobile.compositor"),
                QStringLiteral("privateTopmostWindowProcessId"));
    QDBusPendingReply<int> pidMsgPending = QDBusConnection::systemBus().asyncCall(pidMsg);
    QDBusPendingCallWatcher *pidMsgWatcher =new QDBusPendingCallWatcher(pidMsgPending);
    connect(pidMsgWatcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher){
        watcher->deleteLater();
        QDBusPendingReply<int> reply = *watcher;
        if (reply.isError()) {
            return;
        }
        topmostIdChanged(reply.value());
    });

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

    m_localServer = new QLocalServer(nullptr); // controlled by separate thread
    m_localServer->setSocketOptions(QLocalServer::WorldAccessOption);
    m_localServer->setMaxPendingConnections(2147483647);

    m_serverThread = new QThread(this);
    connect(m_serverThread, &QThread::finished, this, [this](){
        qWarning() << Q_FUNC_INFO << "QThread::finished";
        m_localServer->close();
        m_serverThread->start();
    });
    connect(m_localServer, &QLocalServer::newConnection, this, &DBusService::startReadingLocalServer, Qt::DirectConnection);
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
}

DBusService::~DBusService()
{
}

void DBusService::start()
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

void DBusService::sendKeyevent(int code)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("keyevent"), QString::number(code)});
}

void DBusService::sendInput(const QString &text)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("text"), text});
}

void DBusService::uriActivity(const QString &uri)
{

}

void DBusService::hideNavBar()
{

}

void DBusService::showNavBar()
{

}

void DBusService::openDownloads()
{
    launchPackage(QStringLiteral("com.android.documentsui"));
}

void DBusService::openSettings()
{
    launchPackage(QStringLiteral("com.android.settings"));
}

void DBusService::openContacts()
{
    launchPackage(QStringLiteral("com.android.contacts"));
}

void DBusService::openCamera()
{
    launchPackage(QStringLiteral("com.android.camera2"));
}

void DBusService::openGallery()
{
    launchPackage(QStringLiteral("com.android.gallery3d"));
}

void DBusService::openAppSettings(const QString &package)
{
    qDebug() << Q_FUNC_INFO << package;
}

void DBusService::launchApp(const QString &packageName)
{
    launchPackage(packageName);
}

void DBusService::forceStop(const QString &packageName)
{

}

void DBusService::shareContent(const QVariantMap &content, const QString &source)
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

void DBusService::shareFile(const QString &filename, const QString &mimetype)
{
    qDebug() << Q_FUNC_INFO << filename << mimetype;
    if (!checkHelperSocket()) {
        return;
    }

    runCommand("am", {
                   "start",
                   "-n",
                   "org.coderus.aliendalvikcontrol/.MainActivity",
                   "--es",
                   "command",
                   "sharing",
                   "-a",
                   "android.intent.action.SEND",
                   "-t",
                   mimetype,
                   "--eu",
                   "android.intent.extra.STREAM",
                   filename
               });
}

void DBusService::shareText(const QString &text)
{
    qDebug() << Q_FUNC_INFO << text;
}

void DBusService::doShare(
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
        componentActivity(packageName, launcherClass);
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
        componentActivity(packageName, launcherClass);
    }

    if (!_isTopmostAndroid) {
        QEventLoop loop;
        QTimer timer;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QMetaObject::Connection connection = connect(this, &DBusService::isTopmostAndroidChanged, [&loop](bool isTopmostAndroid){
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

    QStringList options = {
        "start",
        "-a",
        "android.intent.action.SEND",
        "-n",
        QStringLiteral("%1/%2").arg(packageName, className),
        "-t",
        mimetype
    };
    if (filename.isEmpty()) {
        options.append({
                           "--es",
                           "android.intent.extra.TEXT",
                           data
                       });
    } else {
        options.append({
                           "--eu",
                           "android.intent.extra.STREAM",
                           filename
                       });
    }

    runCommand("am", options);
}

QString DBusService::getFocusedApp()
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

bool DBusService::isTopmostAndroid()
{
    return _isTopmostAndroid;
}

QString DBusService::getprop(const QString &key)
{
    QString value = runCommandOutput("/system/bin/getprop", QStringList() << key);
    return value.trimmed();
}

void DBusService::setprop(const QString &key, const QString &value)
{
    runCommand("/system/bin/setprop", QStringList() << key << value);
}

void DBusService::quit()
{
    QTimer::singleShot(10, qApp, SLOT(quit()));
}

void DBusService::startReadingLocalServer()
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

void DBusService::launchPackage(const QString &packageName)
{
//    apkdIface->call(QStringLiteral("launchApp"), packageName);
}

void DBusService::mountSdcard(const QString mountPath)
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

void DBusService::umountSdcard()
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

bool DBusService::checkHelperSocket(bool remove)
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

void DBusService::requestDeviceInfo()
{
    qDebug() << Q_FUNC_INFO;

    runCommand(QStringLiteral("/system/bin/am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("deviceInfo")
               });
}

void DBusService::processHelperResult(const QByteArray &data)
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

void DBusService::componentActivity(const QString &package, const QString &className, const QString &data)
{
    qDebug() << Q_FUNC_INFO << package << className << data;

    QStringList options = {
        "start",
        "-n",
        QStringLiteral("%1/%2").arg(package, className)
    };
    if (data.isEmpty()) {
        options.append({
                           "-a",
                           "android.intent.action.MAIN"
                       });
    } else {
        options.append({
                           "-a",
                           "android.intent.action.VIEW",
                           "-d",
                           data
                       });
    }

    runCommand("am", options);
}

QString DBusService::packageName(const QString &package)
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

void DBusService::runCommand(const QString &program, const QStringList &params)
{
    qDebug() << "Executing" << program << params;

    const QString application = QStringLiteral("/usr/sbin/chroot");
    QStringList arguments = {
        QStringLiteral("/opt/alien"),
        program
    };
    arguments.append(params);

    QProcess process;
    process.setProcessEnvironment(m_alienEnvironment);
    process.start(application, arguments);
    process.waitForFinished(5000);
}

QString DBusService::runCommandOutput(const QString &program, const QStringList &params)
{
    const QString application = QStringLiteral("/usr/sbin/chroot");
    QStringList arguments = {
        QStringLiteral("/opt/alien"),
        program
    };
    arguments.append(params);

    QProcess process;
    process.setProcessEnvironment(m_alienEnvironment);
    process.start(application, arguments);
    process.waitForFinished(5000);
    if (process.state() == QProcess::Running) {
        process.close();
        return QString();
    }
    const QString output = QString::fromUtf8(process.readAll());
    return output;
}

void DBusService::desktopChanged(const QString &path)
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

void DBusService::topmostIdChanged(int pId)
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

void DBusService::serviceStopped()
{
    qDebug() << Q_FUNC_INFO;

    umountSdcard();
}

void DBusService::serviceStarted()
{
    checkHelperSocket(true);

    requestDeviceInfo();
}

void DBusService::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
        desktopChanged(_watchDir + desktoppath);
    }
}

