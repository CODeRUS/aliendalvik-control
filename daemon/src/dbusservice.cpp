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

#include "../common/src/alienabstract.h"
#include "../common/src/systemdcontroller.h"

static const QString c_dbus_service = QStringLiteral("org.coderus.aliendalvikcontrol");
static const QString c_dbus_path = QStringLiteral("/");
static const QString s_sessionBusConnection = QStringLiteral("ad8connection");

static const QString s_helperApk = QStringLiteral("/usr/share/aliendalvik-control/apk/app-release.apk");

static const QString s_localSocketPart = QStringLiteral("%1/data/org.coderus.aliendalvikcontrol/.aliendalvik-control-socket");

typedef AlienAbstract *(*AlienLoader)(QObject *);

DBusService::DBusService(QObject *parent)
    : AliendalvikController(parent)
    , _watcher(new INotifyWatcher(this))
    , m_sbus(s_sessionBusConnection)
{
    QLibrary alienLib;
    if (guessApi() >= 27) {
        alienLib.setFileName(QStringLiteral("/usr/lib/libaliendalvikcontrolplugin-binder8.so"));
    } else {
        alienLib.setFileName(QStringLiteral("/usr/lib/libaliendalvikcontrolplugin-chroot.so"));
    }
    AlienLoader loader = reinterpret_cast<AlienLoader>(alienLib.resolve("instance"));
    if (!loader) {
        qWarning() << Q_FUNC_INFO << "Error loading library" << alienLib.fileName() << alienLib.errorString();
        return;
    }

    m_alien = loader(this);
    m_localSocket = s_localSocketPart.arg(m_alien->dataPath());

    qDebug() << Q_FUNC_INFO << "Loaded alien plugin instance:" << m_alien;

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
        bool listening = m_localServer->listen(m_localSocket);
        if (!listening // not listening
                && m_localServer->serverError() == QAbstractSocket::AddressInUseError // because of AddressInUseError
                && QFileInfo::exists(m_localSocket) // socket file already exists
                && QFile::remove(m_localSocket)) { // and successfully removed it
            qWarning() << Q_FUNC_INFO << "Removed old stuck socket";
            listening = m_localServer->listen(m_localSocket); // try to start lisening again
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

int DBusService::getApiVersion()
{
    return m_deviceProperties.value(QStringLiteral("api"), 19).toInt();
}

int DBusService::guessApiVersion()
{
    return guessApi();
}

void DBusService::sendKeyevent(int code)
{
    m_alien->sendKeyevent(code, 0);
}

void DBusService::sendInput(const QString &text)
{
    m_alien->sendInput(text);
}

void DBusService::sendTap(int posx, int posy)
{
    m_alien->sendTap(posx, posy, 0);
}

void DBusService::sendSwipe(int startx, int starty, int endx, int endy, int duration)
{
    m_alien->sendSwipe(startx, starty, endx, endy, duration);
}

void DBusService::uriActivity(const QString &uri)
{
    qDebug() << Q_FUNC_INFO << uri;
    if (!isServiceActive()) {
        return;
    }
    m_alien->uriActivity(uri);
}

void DBusService::uriActivitySelector(const QString &uri)
{
    qDebug() << Q_FUNC_INFO << uri;
    if (!isServiceActive()) {
        return;
    }

    m_alien->uriActivitySelector(uri);
}

void DBusService::hideNavBar()
{
    const int navbarHeight = m_deviceProperties.value(QStringLiteral("navbarHeight"), 96).toInt();
    const int api = m_deviceProperties.value(QStringLiteral("api"), guessApi()).toInt();

    m_alien->hideNavBar(navbarHeight, api);
}

void DBusService::showNavBar()
{
    const int api = m_deviceProperties.value(QStringLiteral("api"), guessApi()).toInt();

    m_alien->showNavBar(api);
}

void DBusService::hideStatusBar()
{
    m_alien->hideStatusBar();
}

void DBusService::showStatusBar()
{
    m_alien->showStatusBar();
}

void DBusService::openDownloads()
{
    m_alien->openDownloads();
}

void DBusService::openSettings()
{
    m_alien->openSettings();
}

void DBusService::openContacts()
{
    m_alien->openContacts();
}

void DBusService::openCamera()
{
    m_alien->openCamera();
}

void DBusService::openGallery()
{
    m_alien->openGallery();
}

void DBusService::openAppSettings(const QString &package)
{
    m_alien->openAppSettings(package);
}

void DBusService::launchApp(const QString &packageName)
{
    qDebug() << Q_FUNC_INFO << packageName;
    if (!isServiceActive()) {
        return;
    }
    m_alien->launchApp(packageName);
}

void DBusService::forceStop(const QString &packageName)
{
    m_alien->forceStop(packageName);
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
    if (!m_alien) {
        return;
    }

    checkHelperSocket();
    if (!checkFilePermissions(filename)) {
        qWarning() << Q_FUNC_INFO << "Failed to check file permissions!";
        return;
    }
    m_alien->shareFile(filename, mimetype);
}

void DBusService::shareText(const QString &text)
{
    qDebug() << Q_FUNC_INFO << text;
    if (!m_alien) {
        return;
    }

    checkHelperSocket();
    m_alien->shareText(text);
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

    componentActivity(packageName, launcherClass);
    waitForAndroidWindow();

    m_alien->doShare(mimetype, filename, data, packageName, className, launcherClass);
}

QString DBusService::getFocusedApp()
{
    QProcess *proc = new QProcess(this);
    proc->start(QStringLiteral("/system/bin/dumpsys"), {QStringLiteral("window"), QStringLiteral("windows")});
    proc->waitForFinished(5000);
    if (proc->state() == QProcess::Running) {
        proc->close();
        proc->deleteLater();
        return QString();
    }
    QString output = QString::fromUtf8(proc->readAll());
    int pos1 = output.indexOf(QLatin1String("mCurrentFocus"));
    int pos2 = output.indexOf(QChar(u'\n'), pos1);
    output = output.mid(pos1, pos2 - pos1);
    if (output.indexOf(QChar(u' ')) > 0) {
        output = output.split(QChar(u' '))[1];
        output = output.split(QChar(u'/'))[0];
        return output;
    }

    return QString();
}

bool DBusService::isTopmostAndroid()
{
    return _isTopmostAndroid;
}

void DBusService::getImeList()
{
    const QVariantList imeList = m_alien->getImeList();
    emit m_adaptor->imeAvailable(imeList);
}

void DBusService::triggerImeMethod(const QString &ime, bool enable)
{
    m_alien->triggerImeMethod(ime, enable);
}

void DBusService::setImeMethod(const QString &ime)
{
    m_alien->setImeMethod(ime);
}

QString DBusService::getSettings(const QString &nspace, const QString &key)
{
    return m_alien->getSettings(nspace, key);
}

void DBusService::putSettings(const QString &nspace, const QString &key, const QString &value)
{
    m_alien->putSettings(nspace, key, value);
}

QString DBusService::getprop(const QString &key)
{
    return m_alien->getprop(key);
}

void DBusService::setprop(const QString &key, const QString &value)
{
    m_alien->setprop(key, value);
}

void DBusService::quit()
{
    QTimer::singleShot(10, qApp, &QCoreApplication::quit);
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
    }, Qt::DirectConnection);
}

void DBusService::checkHelperSocket()
{
    qDebug() << Q_FUNC_INFO;

    const QFileInfo helperSocket(m_localSocket);
    if (!helperSocket.dir().exists()) {
        qWarning() << Q_FUNC_INFO << "Helper not installed!";

        installApkSync();
    }
    if (helperSocket.exists()) {
        QFile::remove(m_localSocket);
    }

    QEventLoop loop;
    QMetaObject::Connection conn =
    connect(m_serverThread, &QThread::started, &loop, &QEventLoop::quit);
    QTimer::singleShot(2000, this, [this](){
        if (m_serverThread->isRunning()) {
            m_serverThread->quit();
        } else {
            m_serverThread->start();
        }
    });
    loop.exec();
    disconnect(conn);
}

void DBusService::installApkSync()
{
    qDebug() << Q_FUNC_INFO;

    m_alien->installApk(s_helperApk);

    QEventLoop loop;
    QTimer timer;
    QFileSystemWatcher watcher({QStringLiteral("%1/data").arg(m_alien->dataPath())});
    connect(&watcher, &QFileSystemWatcher::directoryChanged, [this, &loop](const QString &) {
        if (!QFileInfo(m_localSocket).dir().exists()) {
            return;
        }
        loop.quit();
    });
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(5000);
    loop.exec();
    timer.stop();
}

bool DBusService::checkFilePermissions(const QString &filename)
{
    QFile file(filename);
    if (!file.exists()) {
        qWarning() << Q_FUNC_INFO << "File doesn't exists:" << filename;
        return false;
    }

    QFile::Permissions permissions = file.permissions();
    if (!permissions.testFlag(QFile::ReadOther)) {
        qDebug() << Q_FUNC_INFO << "Changing" << filename << "permissions to make it readable for all" <<
        file.setPermissions(permissions | QFile::ReadOther);
    }

    return true;
}

int DBusService::guessApi()
{
    if (QFileInfo::exists(QStringLiteral("/opt/alien/system.img"))) {
        return 27;
    }
    if (QFileInfo::exists(QStringLiteral("/opt/alien/system/bin/wm"))) {
        return 19;
    }
    return 16;
}

void DBusService::requestDeviceInfo()
{
    m_alien->requestDeviceInfo();
}

void DBusService::processHelperResult(const QByteArray &data)
{
    qDebug() << Q_FUNC_INFO;
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
        qDebug().noquote() << data;
        const int versionCode = m_deviceProperties.value(QStringLiteral("versionCode"), 0).toInt();
        if (versionCode <= 0) {
            requestDeviceInfo();
        } else if (versionCode < QStringLiteral(HELPER_VERSION).toInt()) {
            installApkSync();
            requestDeviceInfo();
        }
    } else if (command == QLatin1String("launchApp")) {
        const QJsonObject launchParametersJson = object.value(QStringLiteral("launchParameters")).toObject();
        const QString packageName = launchParametersJson.value(QStringLiteral("packageName")).toString();
        const QString className = launchParametersJson.value(QStringLiteral("className")).toString();
        componentActivity(packageName, className);
    } else if (command == QLatin1String("share")) {
        const QJsonObject shareIntentJson = object.value(QStringLiteral("shareIntent")).toObject();
        const QString shareFile = shareIntentJson.value(QStringLiteral("fileName")).toString();

        QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("org.coderus.aliendalvikshare"),
                    QStringLiteral("/"),
                    QStringLiteral("org.coderus.aliendalvikshare"),
                    QStringLiteral("share"));
        msg.setArguments({
                             shareIntentJson.value(QStringLiteral("mimeType")).toString(),
                             shareIntentJson.value(QStringLiteral("data")).toString(),
                             checkShareFile(shareFile),
                         });
        qDebug() << Q_FUNC_INFO << "Sending share to salifish:" <<
                    m_sbus.send(msg);
    } else if (command == QLatin1String("selector")) {
        const QVariantList selectorList = object.value(QStringLiteral("candidates")).toArray().toVariantList();
        const QString url = object.value(QStringLiteral("url")).toString();
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("org.coderus.aliendalvikselector"),
                    QStringLiteral("/"),
                    QStringLiteral("org.coderus.aliendalvikselector"),
                    QStringLiteral("openUrl"));
        msg.setArguments({url, selectorList});
        qDebug() << Q_FUNC_INFO << "Sending selector to salifish:" <<
                    m_sbus.send(msg);
    } else if (command == QLatin1String("uri")) {
        const QJsonObject defaultApplicationJson = object.value(QStringLiteral("default")).toObject();
        const QString packageName = defaultApplicationJson.value(QStringLiteral("packageName")).toString();
        const QString className = defaultApplicationJson.value(QStringLiteral("className")).toString();
        const QString data = defaultApplicationJson.value(QStringLiteral("data")).toString();
        componentActivity(packageName, className, data);
    } else if (command == QLatin1String("uptime")) {
        const quint64 uptime = object.value(QStringLiteral("value")).toVariant().toULongLong();
        qDebug() << Q_FUNC_INFO << "Received android uptime:" << uptime;
        const QString payloadString = object.value(QStringLiteral("payload")).toString();
        if (payloadString.isEmpty()) {
            return;
        }
        QJsonParseError error;
        QJsonDocument payloadDoc = QJsonDocument::fromJson(payloadString.toLatin1(), &error);
        if (error.error != QJsonParseError::NoError) {
            qWarning() << Q_FUNC_INFO << "Error parsing payload:" << error.errorString();
            return;
        }
        const QJsonObject payloadObject = payloadDoc.object();
        const QString payloadCommand = payloadObject.value(QStringLiteral("command")).toString();
        if (payloadCommand == QLatin1String("sendTap")) {
            const int posx = payloadObject.value(QStringLiteral("x")).toInt();
            const int posy = payloadObject.value(QStringLiteral("y")).toInt();
            m_alien->sendTap(posx, posy, uptime);
        }
        if (payloadCommand == QLatin1String("sendKeyevent")) {
            const int code = payloadObject.value(QStringLiteral("code")).toInt();
            m_alien->sendKeyevent(code, uptime);
        }
    }
}

void DBusService::launcherActivity(const QString &package, const QString &className, const QString &data)
{
    qDebug() << Q_FUNC_INFO << package << className << data << isServiceActive() << m_serverThread->isRunning();
    QString classNameReplaced = className;
    if (className == QLatin1String("org.mozilla.gecko.LauncherActivity")) {
        classNameReplaced = QStringLiteral("org.mozilla.firefox.App");
        qDebug() << Q_FUNC_INFO << "Replacing" << className << "with" << classNameReplaced;
    }

    if (!isServiceActive() || !m_serverThread->isRunning()) {
        qDebug() << Q_FUNC_INFO << "Calling apkd-launcher";
        QProcess apkdLauncher;
        apkdLauncher.start(QStringLiteral("/usr/bin/apkd-launcher"), {
                               package,
                               QStringLiteral("%1/%2").arg(package, classNameReplaced)
                           });
        apkdLauncher.waitForFinished(-1);
    } else {
        launchApp(package);
    }

    if (data.isEmpty()) {
        return;
    }

    if (!isServiceActive()) {
        QEventLoop loop;
        connect(controller(), &SystemdController::serviceStarted, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (!m_serverThread->isRunning()) {
        QEventLoop loop;
        connect(m_serverThread, &QThread::started, &loop, &QEventLoop::quit);
        loop.exec();
    }

    waitForAndroidWindow();

    componentActivity(package, className, data);
}

void DBusService::componentActivity(const QString &package, const QString &className, const QString &data)
{
    qDebug() << Q_FUNC_INFO << package << className << data;

    m_alien->componentActivity(package, className, data);
}

void DBusService::uriLaunchActivity(const QString &package, const QString &className, const QString &launcherClass, const QString &data)
{
    qDebug() << Q_FUNC_INFO << package << className << launcherClass << data;

    if (package == QLatin1String("com.myriadgroup.nativeapp.browser")) {
        QProcess lca;
        lca.start(QStringLiteral("/usr/bin/lca-tool"), {
                      QStringLiteral("--triggerdesktop"),
                      QStringLiteral("open-url.desktop"),
                      data
                  });
        lca.waitForFinished(5000);
        return;
    }

    if (!isServiceActive()) {
        QProcess apkdLauncher;
        apkdLauncher.start(QStringLiteral("/usr/bin/apkd-launcher"), {
                               QString(),
                               QStringLiteral("%1/%2").arg(package, launcherClass)
                           });
        apkdLauncher.waitForFinished(-1);
    }

    if (!activateApp(package, launcherClass)) {
        qDebug() << Q_FUNC_INFO << "Force stopping" << package;
        forceStop(package);
    }

    componentActivity(package, className, data);
}

bool DBusService::activateApp(const QString &packageName, const QString &launcherClass)
{
    const QString packageNameReplaced = QString(packageName).replace(QChar(u'.'), QChar(u'_'));
    const QString launcherClassReplaced = QString(launcherClass).replace(QChar(u'.'), QChar(u'_'));
    QString desktopFile = QStringLiteral("/usr/share/applications/apkd_launcher_%1-%2.desktop").arg(packageNameReplaced, launcherClassReplaced);

    if (!QFileInfo::exists(desktopFile)) {
        if (packageName != QLatin1String("org.mozilla.firefox")) {
            qWarning() << Q_FUNC_INFO << "Desktop does not exists:" << desktopFile;
            return false;
        }

        desktopFile = QStringLiteral("/usr/share/applications/apkd_launcher_org_mozilla_firefox-org_mozilla_gecko_LauncherActivity.desktop");
        if (!QFileInfo::exists(desktopFile)) {
            return false;
        }
    }

    qDebug() << Q_FUNC_INFO << "Activating desktop" << desktopFile;
    QDBusMessage activateDesktop = QDBusMessage::createMethodCall(
            QStringLiteral("com.jolla.lipstick"),
            QStringLiteral("/LauncherModel"),
            QStringLiteral("org.nemomobile.lipstick.LauncherModel"),
            QStringLiteral("notifyLaunching"));
    activateDesktop.setArguments({desktopFile});
    const bool result = m_sbus.send(activateDesktop);
    if (!result) {
        qWarning() << Q_FUNC_INFO << "Failed to send dbus message:" << m_sbus.lastError().message();
    }
    return result;
}

void DBusService::waitForAndroidWindow()
{
    if (_isTopmostAndroid) {
        return;
    }

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
    disconnect(connection);
    if (!timer.isActive()) {
        qDebug() << Q_FUNC_INFO << "Timer expired. Cancel waiting!";
        return;
    }
}

QString DBusService::checkShareFile(const QString &shareFilePath)
{
    const QString shareFileChecked = m_alien->checkShareFile(shareFilePath);
    QFile shareFile(shareFileChecked);
    QFile::Permissions permissions = shareFile.permissions();
    if (!permissions.testFlag(QFile::ReadOther)) {
        qDebug() << Q_FUNC_INFO << "Changing" << shareFileChecked << "permissions to make it readable for all" <<
        shareFile.setPermissions(permissions | QFile::ReadOther);
    }

    return shareFileChecked;
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
    const QStringList data = content.mid(off1, off2 - off1 + 1).split(QChar(u'/'));
    const QString package = data.first();
    QString activity = QStringLiteral("empty");
    if (data.length() >= 2) {
        activity = data.at(1);
    }
    activity = QString::fromLatin1(activity.toLatin1().toPercentEncoding(QByteArray(), QByteArrayLiteral("-._~")).replace("%", "_"));
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
    const bool value = (out == QByteArrayLiteral("system_server"));
    if (_isTopmostAndroid == value) {
        return;
    }
    _isTopmostAndroid = value;
    emit isTopmostAndroidChanged(_isTopmostAndroid);
}

void DBusService::serviceStopped()
{
    qDebug() << Q_FUNC_INFO;
}

void DBusService::serviceStarted()
{
    if (!m_alien) {
        return;
    }

    checkHelperSocket();
    QTimer::singleShot(5000, this, &DBusService::requestDeviceInfo);
}

void DBusService::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    for (const QString &desktoppath : appl.entryList({QStringLiteral("apkd_launcher_*.desktop")})) {
        desktopChanged(_watchDir + desktoppath);
    }
}

