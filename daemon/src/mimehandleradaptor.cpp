#include "mimehandleradaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDBusReply>
#include <QMetaObject>
#include <QTimer>

MimeHandlerAdaptor::MimeHandlerAdaptor(QObject *parent)
    : QDBusVirtualObject(parent)
    , _watcher(new QFileSystemWatcher(this))
{
    _watchDir = QStringLiteral("/usr/share/applications/");
    _watcher->addPath(_watchDir);

    QObject::connect(_watcher, &QFileSystemWatcher::fileChanged, this, &MimeHandlerAdaptor::desktopChanged);
    QObject::connect(_watcher, &QFileSystemWatcher::directoryChanged, this, &MimeHandlerAdaptor::readApplications);
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
}

MimeHandlerAdaptor::~MimeHandlerAdaptor()
{
}

QString MimeHandlerAdaptor::introspect(const QString &) const
{
    QString xml;
    xml  = "  <interface name=\"org.coderus.aliendalvikcontrol\">\n";
    xml += "    <method name=\"sendKeyevent\">\n";
    xml += "      <arg name=\"code\" type=\"i\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"sendInput\">\n";
    xml += "      <arg name=\"text\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"broadcastIntent\">\n";
    xml += "      <arg name=\"intent\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"startIntent\">\n";
    xml += "      <arg name=\"intent\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"uriActivity\">\n";
    xml += "      <arg name=\"uri\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"uriActivitySelector\">\n";
    xml += "      <arg name=\"uri\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"hideNavBar\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"showNavBar\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"shareFile\">\n";
    xml += "      <arg name=\"filename\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"mimetype\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"shareText\">\n";
    xml += "      <arg name=\"text\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"getFocusedApp\">\n";
    xml += "      <arg name=\"package\" type=\"s\" direction=\"out\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"isTopmostAndroid\">\n";
    xml += "      <arg name=\"value\" type=\"b\" direction=\"out\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"openDownloads\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"openSettings\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"openContacts\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"openCamera\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"openGallery\">\n";
    xml += "    </method>\n";
    xml += "    <method name=\"getImeList\">\n";
    xml += "    </method>\n";
    xml += "    <signal name=\"imeAvailable\">";
    xml += "      <arg name=\"imeList\" type=\"a{sb}\" direction=\"out\"/>";
    xml += "    </signal>";
    xml += "    <method name=\"setImeMethod\">\n";
    xml += "      <arg name=\"ime\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"triggerImeMethod\">\n";
    xml += "      <arg name=\"ime\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"enable\" type=\"b\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"getSettings\">\n";
    xml += "      <arg name=\"namespace\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"key\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"value\" type=\"s\" direction=\"out\"/>";
    xml += "    </method>\n";
    xml += "    <method name=\"putSettings\">\n";
    xml += "      <arg name=\"namespace\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"key\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"value\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"getprop\">\n";
    xml += "      <arg name=\"key\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"value\" type=\"s\" direction=\"out\"/>";
    xml += "    </method>\n";
    xml += "    <method name=\"setprop\">\n";
    xml += "      <arg name=\"key\" type=\"s\" direction=\"in\"/>\n";
    xml += "      <arg name=\"value\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"quit\">\n";
    xml += "    </method>\n";
    xml += "  </interface>\n";
    return xml;
}

bool MimeHandlerAdaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    const QVariantList dbusArguments = message.arguments();

    QString member = message.member();
    QString interface = message.interface();

    qDebug() << interface << member << dbusArguments;

    QVariant output;

    if (interface == QLatin1String("org.freedesktop.DBus.Introspectable")) {
        return false;
    }
    else if (interface == QLatin1String("org.coderus.aliendalvikcontrol") || interface.isEmpty()) {
//        QDBusReply<bool> apkdReply = apkdIface->call(QStringLiteral("isRunning"));
//        if (apkdReply.isValid() && !apkdReply.value()) {
            apkdIface->call(QStringLiteral("controlService"), QVariant::fromValue(true));
//        }

        if (dbusArguments.size() == 0) {
            QMetaObject::invokeMethod(this, member.toLatin1().constData(), Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, output));
        } else if (dbusArguments.size() == 1) {
            QMetaObject::invokeMethod(this, member.toLatin1().constData(), Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, output),
                                      Q_ARG(QVariant, dbusArguments[0]));
        } else if (dbusArguments.size() == 2) {
            QMetaObject::invokeMethod(this, member.toLatin1().constData(), Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, output),
                                      Q_ARG(QVariant, dbusArguments[0]),
                                      Q_ARG(QVariant, dbusArguments[1]));
        } else if (dbusArguments.size() == 3) {
            QMetaObject::invokeMethod(this, member.toLatin1().constData(), Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, output),
                                      Q_ARG(QVariant, dbusArguments[0]),
                                      Q_ARG(QVariant, dbusArguments[1]),
                                      Q_ARG(QVariant, dbusArguments[2]));
        }
    }
    else if (dbusArguments.size() == 1) {
        QString activity = interface + "/" + QString::fromLatin1(QByteArray::fromPercentEncoding(member.toLatin1().replace("_", "%")));
        componentActivity(activity, dbusArguments.first().toString());
    }

    QVariantList dbusReply;
    if (!output.isNull()) {
        dbusReply << output;
    }
    QDBusMessage reply = message.createReply(dbusReply);
    connection.call(reply, QDBus::NoBlock);
    return true;
}

QVariant MimeHandlerAdaptor::sendKeyevent(const QVariant &code)
{
    appProcess("input.jar", QStringList() << "com.android.commands.input.Input" << "keyevent" << QString::number(code.toInt()));
    return QVariant();
}

QVariant MimeHandlerAdaptor::sendInput(const QVariant &text)
{
    appProcess("input.jar", QStringList() << "com.android.commands.input.Input" << "text" << text.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::broadcastIntent(const QVariant &intent)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "broadcast" << "-a" << intent.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::startIntent(const QVariant &intent)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << intent.toString().split(" "));
    return QVariant();
}

QVariant MimeHandlerAdaptor::uriActivity(const QVariant &uri)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-d" << uri.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::uriActivitySelector(const QVariant &uri)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "--selector" << "-d" << uri.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::hideNavBar()
{
    appProcess("wm.jar", QStringList() << "com.android.commands.wm.Wm" << "overscan" << "0,0,0,-144");
//    runCommand("/system/bin/service", QStringList() << "call" << "activity" << "42" << "s16" << "com.android.systemui");
    return QVariant();
}

QVariant MimeHandlerAdaptor::showNavBar()
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "startservice" << "-n" << "com.android.systemui/.SystemUIService");
    appProcess("wm.jar", QStringList() << "com.android.commands.wm.Wm" << "overscan" << "0,0,0,0");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openDownloads(const QVariant &)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW_DOWNLOADS" << "--activity-multiple-task" << "-f" << "268435456");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openSettings(const QVariant &)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.settings.SETTINGS");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openContacts(const QVariant &)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.contacts/com.android.contacts.activities.PeopleActivity");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openCamera(const QVariant &)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.media.action.IMAGE_CAPTURE");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openGallery(const QVariant &)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.gallery3d/com.android.gallery3d.app.GalleryActivity");
    return QVariant();
}

QVariant MimeHandlerAdaptor::getImeList()
{
    QString fullOutput = appProcessOutput("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "list" << "-s" << "-a");
    QStringList fullOutputLines = fullOutput.trimmed().split("\n");
    qDebug() << fullOutput.trimmed();

    QString enabledOutput = appProcessOutput("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "list" << "-s");
    QStringList enabledOutputLines = enabledOutput.trimmed().split("\n");
    qDebug() << enabledOutput.trimmed();

    QVariantList imeList;
    foreach (const QString & imeName, fullOutputLines) {
        QVariantMap imeMethod;
        imeMethod["name"] = imeName;
        imeMethod["enabled"] = enabledOutputLines.contains(imeName);
        imeList.append(imeMethod);
    }
    QVariantList args;
    args.append(QVariant::fromValue(imeList));

    emitSignal("imeAvailable", args);
    return QVariant();
}

QVariant MimeHandlerAdaptor::triggerImeMethod(const QVariant &ime, const QVariant &enable)
{
    appProcess("ime.jar", QStringList() << "com.android.commands.ime.Ime" << (enable.toBool() ? "enable" : "disable") << ime.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::setImeMethod(const QVariant &ime)
{
    appProcess("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "set" << ime.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::shareFile(const QVariant &filename, const QVariant &mimetype)
{
    QStringList params;
    params << "com.android.commands.am.Am";
    params << "start" << "-a" << "android.intent.action.SEND" << "-t";
    params << mimetype.toString();
    params << "--eu" << "android.intent.extra.STREAM";
    params << filename.toString();
    appProcess("am.jar", QStringList() << params);
    return QVariant();
}

QVariant MimeHandlerAdaptor::shareText(const QVariant &text)
{
    QStringList params;
    params << "com.android.commands.am.Am";
    params << "start" << "-a" << "android.intent.action.SEND" << "-t";
    params << "text/*";
    params << "--es" << "android.intent.extra.TEXT";
    params << text.toString();
    appProcess("am.jar", QStringList() << params);
    return QVariant();
}

QVariant MimeHandlerAdaptor::getFocusedApp()
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

QVariant MimeHandlerAdaptor::isTopmostAndroid()
{
    return _isTopmostAndroid;
}

QVariant MimeHandlerAdaptor::getSettings(const QVariant &nspace, const QVariant &key)
{
    QString value = appProcessOutput("settings.jar", QStringList() << "com.android.commands.settings.SettingsCmd" << "get" << nspace.toString() << key.toString());
    return value.trimmed();
}

QVariant MimeHandlerAdaptor::putSettings(const QVariant &nspace, const QVariant &key, const QVariant &value)
{
    appProcess("settings.jar", QStringList() << "com.android.commands.settings.SettingsCmd" << "put" << nspace.toString() << key.toString() << value.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::getprop(const QVariant &key)
{
    QString value = runCommandOutput("/system/bin/getprop", QStringList() << key.toString());
    return value.trimmed();
}

QVariant MimeHandlerAdaptor::setprop(const QVariant &key, const QVariant &value)
{
    runCommand("/system/bin/setprop", QStringList() << key.toString() << value.toString());
    return QVariant();
}

QVariant MimeHandlerAdaptor::quit()
{
    QTimer::singleShot(10, qApp, SLOT(quit()));
    return QVariant();
}

void MimeHandlerAdaptor::componentActivity(const QString &component, const QString &data)
{
    QStringList params;
    params << "com.android.commands.am.Am" << "start" << "-n" << component << "-a";
    if (data.isEmpty()) {
        params << "android.intent.action.MAIN";
    }
    else {
        params << "android.intent.action.VIEW";
        params << "-d" << data;
    }
    appProcess("am.jar", params);
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
    QString lxc = "/usr/bin/lxc-attach";QStringList arguments;
    arguments << "-n" << "aliendalvik" << "--" << program << params;
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

void MimeHandlerAdaptor::emitSignal(const QString &name, const QVariantList &arguments)
{
    QDBusMessage signal = QDBusMessage::createSignal("/", "org.coderus.aliendalvikcontrol", name);
    QDBusConnection conn = QDBusConnection::sessionBus();

    if (!arguments.isEmpty()) {
        signal.setArguments(arguments);
    }

    if (!conn.send(signal)) {
        qWarning() << conn.lastError().message();
    }
}

void MimeHandlerAdaptor::desktopChanged(const QString &path)
{
    qDebug() << path;

    QFile desktop(path);
    if (desktop.exists()) {
        if (!_watcher->files().contains(path)) {
            qDebug() << "new desktop" << path;
            _watcher->addPath(path);
        }

        if (desktop.open(QFile::ReadWrite | QFile::Text)) {
            QString content = QString::fromUtf8(desktop.readAll());
            if (!content.contains("X-Maemo-Service")) {
                int off0 = content.indexOf("Exec=apkd-launcher ");
                if (off0 > 0) {
                    int off1 = content.indexOf(" ", off0 + 20) + 1;
                    int off2 = content.indexOf("\n", off1) - 1;
                    QStringList data = content.mid(off1, off2 - off1 + 1).split("/");
                    QString package = data[0];
                    QString activity = data[1];
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
            }
            desktop.close();
        }
    }
    else {
        if (_watcher->files().contains(path)) {
            qDebug() << "deleted desktop" << path;
            _watcher->removePath(path);
        }
    }
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

void MimeHandlerAdaptor::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    foreach (const QString &desktoppath, appl.entryList(QStringList() << "apkd_launcher_*.desktop")) {
        desktopChanged(_watchDir + desktoppath);
    }
}

