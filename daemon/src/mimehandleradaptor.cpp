#include "mimehandleradaptor.h"

#include <QDebug>

MimeHandlerAdaptor::MimeHandlerAdaptor(QObject *parent)
    : QDBusVirtualObject(parent)
    , _watcher(new QFileSystemWatcher(this))
{
    _watchDir = QString("/home/nemo/.config/aliendalvikcontrol/mimehandlers/");
    _watcher->addPath(_watchDir);

    _xml = QString("") + headerXml() + footerXml();

    QObject::connect(_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(handlersChanged(QString)));
    handlersChanged(_watchDir);
}

MimeHandlerAdaptor::~MimeHandlerAdaptor()
{
}

QString MimeHandlerAdaptor::introspect(const QString &) const
{
    return _xml;
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
    if (member.startsWith("open")) {
        QString component = member.mid(4);
        if (_componentHash.contains(component)) {
            qDebug() << component << _componentHash[component];
            componentActivity(_componentHash[component], dbusArguments.first().toString());
        }
    }
    else {
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
    }

    QDBusMessage reply = message.createReply(QVariantList());
    connection.call(reply, QDBus::NoBlock);
    return true;
}

QString MimeHandlerAdaptor::headerXml() const
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
    return xml;
}

QString MimeHandlerAdaptor::footerXml() const
{
    QString xml;
    xml += "  </interface>\n";
    return xml;
}


void MimeHandlerAdaptor::sendKeyevent(int code)
{
    runCommand("input.jar", QStringList() << "com.android.commands.input.Input" << "keyevent" << QString::number(code));
}

void MimeHandlerAdaptor::sendInput(const QString &text)
{
    runCommand("input.jar", QStringList() << "com.android.commands.input.Input" << "text" << text);
}

void MimeHandlerAdaptor::broadcastIntent(const QString &intent)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "broadcast" << "-a" << intent);
}

void MimeHandlerAdaptor::startIntent(const QString &intent)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << intent.split(" "));
}

void MimeHandlerAdaptor::uriActivity(const QString &uri)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-d" << uri);
}

void MimeHandlerAdaptor::uriActivitySelector(const QString &uri)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "--selector" << "-d" << uri);
}

void MimeHandlerAdaptor::componentActivity(const QString &component, const QString &data)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-n" << component << "-d" << data);
}

void MimeHandlerAdaptor::runCommand(const QString &jar, const QStringList &params)
{
    qputenv("CLASSPATH", QString("/system/framework/%1").arg(jar).toUtf8());

    QString program = "/system/bin/app_process";
    QStringList arguments;
    arguments << "/system/bin" << params;

    qDebug() << "Executing" << program << arguments;

    QProcess::startDetached(program, arguments);
}

void MimeHandlerAdaptor::handlersChanged(const QString &)
{
    qDebug() << "working";
    _componentHash.clear();
    _xml = headerXml();
    QString applications("/usr/share/applications/");
    QString dprefix("url-handler-");
    QDir appl(applications);
    foreach (const QString &desktoppath, appl.entryList()) {
        QFile desktop(applications + desktoppath);
        if (desktoppath.startsWith(dprefix) && desktop.exists()) {
            desktop.remove();
        }
    }
    QDir dir(_watchDir);
    foreach (const QString &handlerpath, dir.entryList()) {
        QFile handler(_watchDir + handlerpath);
        if (handler.exists() && handler.open(QFile::ReadOnly)) {
            QTextStream in(&handler);
            if (!in.atEnd()) {
                QString component = in.readLine();
                qDebug() << "Adding" << handlerpath << component << "method to dbus";
                _xml += QString("    <method name=\"open%1\">\n").arg(handlerpath);
                _xml += "      <arg name=\"uri\" type=\"s\" direction=\"in\"/>\n";
                _xml += "    </method>\n";
                _componentHash[handlerpath] = component;
                QFile desktop(applications + dprefix + handlerpath + ".desktop");
                if (desktop.open(QFile::WriteOnly | QFile::Truncate)) {
                    qDebug() << "Creating" << desktop.fileName();
                    QTextStream out(&desktop);
                    out << "[Desktop Entry]\n";
                    out << "Type=Application\n";
                    out << "Name=Android " + handlerpath + "\n";
                    out << "Exec=\n";
                    out << "MimeType=text/html;x-maemo-urischeme/http;x-maemo-urischeme/https;\n";
                    out << "X-Maemo-Service=org.coderus.aliendalvikcontrol\n";
                    out << "X-Maemo-Object-Path=/\n";
                    out << "X-Maemo-Method=org.coderus.aliendalvikcontrol.open" + handlerpath + "\n";
                    if (!in.atEnd()) {
                        QString hidden = in.readLine();
                        out << "NoDisplay=" + hidden + "\n";
                    }
                    desktop.close();
                }
            }
            handler.close();
        }
    }
    _xml += footerXml();
}

