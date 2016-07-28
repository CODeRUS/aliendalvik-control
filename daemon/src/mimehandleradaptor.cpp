#include "mimehandleradaptor.h"

#include <QDebug>

MimeHandlerAdaptor::MimeHandlerAdaptor(QObject *parent)
    : QDBusVirtualObject(parent)
    , _watcher(new QFileSystemWatcher(this))
{
    _watchDir = QString("/usr/share/applications/");
    _watcher->addPath(_watchDir);

    QObject::connect(_watcher, SIGNAL(fileChanged(QString)), this, SLOT(desktopChanged(QString)));
    QObject::connect(_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(readApplications(QString)));
    readApplications(_watchDir);

    _isTopmostAndroid = false;
    QDBusConnection::systemBus().connect("", "", "org.nemomobile.compositor", "privateTopmostWindowProcessIdChanged", this, SLOT(topmostIdChanged(int)));
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
    xml += "    <signal name=\"imeAvailable\">";
    xml += "      <arg type=\"as\" name=\"imeList\" direction=\"out\"/>";
    xml += "    </signal>";
    xml += "  </interface>\n";
    return xml;
}

bool MimeHandlerAdaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    const QVariantList dbusArguments = message.arguments();

    QString member = message.member();
    QString interface = message.interface();

    qDebug() << interface << member << dbusArguments;

    QVariantList output;

    if (interface == QLatin1String("org.freedesktop.DBus.Introspectable")) {
        return false;
    }
    else if (interface == "org.coderus.aliendalvikcontrol" || interface == "") {
        if (dbusArguments.size() == 0) {
            if (member == "hideNavBar") {
                hideNavBar();
            }
            else if (member == "showNavBar") {
                showNavBar();
            }
            else if (member == "openDownloads") {
                openDownloads();
            }
            else if (member == "getImeList") {
                getImeList();
            }
            else if (member == "getFocusedApp") {
                output << getFocusedApp();
            }
            else if (member == "isTopmostAndroid") {
                output << _isTopmostAndroid;
            }
        }
        else if (dbusArguments.size() == 1) {
            if (member == "sendKeyevent") {
                sendKeyevent(dbusArguments.first().toInt());
            }
            else if (member == "sendInput") {
                sendInput(dbusArguments.first().toString());
            }
            else if (member == "broadcastIntent") {
                broadcastIntent(dbusArguments.first().toString());
            }
            else if (member == "startIntent") {
                startIntent(dbusArguments.first().toString());
            }
            else if (member == "uriActivity") {
                uriActivity(dbusArguments.first().toString());
            }
            else if (member == "uriActivitySelector") {
                uriActivitySelector(dbusArguments.first().toString());
            }
            else if (member == "shareText") {
                shareText(dbusArguments.first().toString());
            }
            else if (member == "setImeMethod") {
                setImeMethod(dbusArguments.first().toString());
            }
        }
        else if (dbusArguments.size() == 2) {
            if (member == "shareFile") {
                shareFile(dbusArguments);
            }
        }
    }
    else if (dbusArguments.size() == 1) {
        QString activity = interface + "/" + QString::fromLatin1(QByteArray::fromPercentEncoding(member.toLatin1().replace("_", "%")));
        componentActivity(activity, dbusArguments.first().toString());
    }

    QDBusMessage reply = message.createReply(output);
    connection.call(reply, QDBus::NoBlock);
    return true;
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
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-d" << uri);
}

void MimeHandlerAdaptor::uriActivitySelector(const QString &uri)
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "--selector" << "-d" << uri);
}

void MimeHandlerAdaptor::hideNavBar()
{
    runCommand("/system/bin/service", QStringList() << "call" << "activity" << "42" << "s16" << "com.android.systemui");
}

void MimeHandlerAdaptor::showNavBar()
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "startservice" << "-n" << "com.android.systemui/.SystemUIService");
}

void MimeHandlerAdaptor::openDownloads()
{
    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW_DOWNLOADS");
}

void MimeHandlerAdaptor::getImeList()
{
    QString output = appProcessOutput("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "list");

    QStringList outputLines = output.split("\n");
    QStringList imeList;
    foreach (const QString &line, outputLines) {
        if (!line.startsWith(" ") && line.length() > 0) {
            QString package = line.split(":").first();
            imeList << package;
        }
    }
    emitSignal("imeAvailable", QVariantList() << imeList);
}

void MimeHandlerAdaptor::setImeMethod(const QString &ime)
{
    appProcess("ime.jar", QStringList() << "com.android.commands.ime.Ime" << "set" << ime);
}

void MimeHandlerAdaptor::shareFile(const QVariantList &args)
{
    QStringList params;
    params << "com.android.commands.am.Am";
    params << "start" << "-a" << "android.intent.action.SEND" << "-t";
    params << args[1].toString();
    params << "--eu" << "android.intent.extra.STREAM";
    params << args[0].toString();
    appProcess("am.jar", QStringList() << params);
}

void MimeHandlerAdaptor::shareText(const QString &text)
{
    QStringList params;
    params << "com.android.commands.am.Am";
    params << "start" << "-a" << "android.intent.action.SEND" << "-t";
    params << "text/*";
    params << "--es" << "android.intent.extra.TEXT";
    params << text;
    appProcess("am.jar", QStringList() << params);
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

    QString program = "/system/bin/app_process";
    QStringList arguments;
    arguments << "/system/bin" << params;

    qDebug() << "Executing" << program << arguments;

    QProcess::startDetached(program, arguments);
}

QString MimeHandlerAdaptor::appProcessOutput(const QString &jar, const QStringList &params)
{
    qputenv("CLASSPATH", QString("/system/framework/%1").arg(jar).toUtf8());

    QString program = "/system/bin/app_process";
    QStringList arguments;
    arguments << "/system/bin" << params;

    qDebug() << "Executing" << program << arguments;

    QProcess *process = new QProcess(this);
    process->start(program, arguments);
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

    QProcess::startDetached(program, params);
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
            if (!content.contains("MimeType")) {
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
                    out << "MimeType=text/html;x-maemo-urischeme/http;x-maemo-urischeme/https;\n";
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

