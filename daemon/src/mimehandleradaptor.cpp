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
    xml += "  </interface>\n";
    return xml;
}

bool MimeHandlerAdaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    const QVariantList dbusArguments = message.arguments();

    QString member = message.member();
    QString interface = message.interface();

    qDebug() << interface << member << dbusArguments;

    if (interface == QLatin1String("org.freedesktop.DBus.Introspectable")) {
        return false;
    }
    else if (interface == "org.coderus.aliendalvikcontrol" || interface == "") {
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
        else if (member == "hideNavBar") {
            hideNavBar();
        }
        else if (member == "showNavBar") {
            showNavBar();
        }
    }
    else {
        QString activity = interface + "/" + QString::fromLatin1(QByteArray::fromPercentEncoding(member.toLatin1().replace("_", "%")));
        componentActivity(activity, dbusArguments.first().toString());
    }

    QDBusMessage reply = message.createReply(QVariantList());
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

void MimeHandlerAdaptor::componentActivity(const QString &component, const QString &data)
{
    QStringList params;
    params << "com.android.commands.am.Am" << "start" << "-n" << component;
    if (!data.isEmpty()) {
        params << "-a" << "android.intent.action.VIEW" << "-d" << data;
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

void MimeHandlerAdaptor::runCommand(const QString &program, const QStringList &params)
{
    qDebug() << "Executing" << program << params;

    QProcess::startDetached(program, params);
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

void MimeHandlerAdaptor::readApplications(const QString &)
{
    qDebug() << "working";
    QDir appl(_watchDir);
    foreach (const QString &desktoppath, appl.entryList(QStringList() << "apkd_launcher_*.desktop")) {
        desktopChanged(_watchDir + desktoppath);
    }
}

