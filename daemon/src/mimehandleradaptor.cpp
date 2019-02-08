#include "mimehandleradaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDBusReply>
#include <QMetaObject>
#include <QTimer>

#define BINDER_SERVICE "activity"
#define BINDER_IFACE "android.app.IActivityManager"

#define BUNDLE_MAGIC 0x4C444E42

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

    binderConnect();
}

MimeHandlerAdaptor::~MimeHandlerAdaptor()
{
    binderDisconnect();
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
    xml += "    <method name=\"openAppSettings\">\n";
    xml += "      <arg name=\"package\" type=\"s\" direction=\"in\"/>\n";
    xml += "    </method>\n";
    xml += "    <method name=\"launchApp\">\n";
    xml += "      <arg name=\"package\" type=\"s\" direction=\"in\"/>\n";
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
//            apkdIface->call(QStringLiteral("controlService"), QVariant::fromValue(true));
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
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.intent.action.VIEW" << "-d" << uri.toString());
    Intent intent;
    intent.action = QStringLiteral("androird.intent.action.VIEW");
    intent.data = uri.toString();
    sendIntent(intent);
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
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.documentsui/.LauncherActivity");
    launchPackage("com.android.documentsui");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openSettings(const QVariant &)
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.settings.SETTINGS");
    launchPackage("com.android.settings");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openContacts(const QVariant &)
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.contacts/com.android.contacts.activities.PeopleActivity");
    launchPackage("com.android.contacts");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openCamera(const QVariant &)
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-a" << "android.media.action.IMAGE_CAPTURE");
    launchPackage("com.android.camera2");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openGallery(const QVariant &)
{
//    appProcess("am.jar", QStringList() << "com.android.commands.am.Am" << "start" << "-n" << "com.android.gallery3d/com.android.gallery3d.app.GalleryActivity");
    launchPackage("com.android.gallery3d");
    return QVariant();
}

QVariant MimeHandlerAdaptor::openAppSettings(const QVariant &package)
{
    GBinderLocalRequest* req = gbinder_client_new_request(m_client);
    GBinderWriter writer;
    gbinder_local_request_init_writer(req, &writer);

    QString callingPackage = QStringLiteral("com.android.settings");

    QString data = QStringLiteral("package:%1").arg(package.toString());
    QString action = QStringLiteral("android.settings.APPLICATION_DETAILS_SETTINGS");
    QString appPackage = QStringLiteral("com.android.settings");
    QString componentPackage = QStringLiteral("com.android.settings");
    QString componentClass = QStringLiteral(".applications.InstalledAppDetails");

    QString type;

    int flags = 0x10008000;

    // IBinder b
    gbinder_writer_append_local_object(&writer, NULL);

    // String callingPackage
//    gbinder_writer_append_string16_utf16(&writer, callingPackage.utf16(), callingPackage.length());
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16(&writer, "");

//    gbinder_writer_append_int32(&writer, -1);
    gbinder_writer_append_int32(&writer, 1);

    // ** INTENT BEGIN **

    // String mAction
    gbinder_writer_append_string16_utf16(&writer, action.utf16(), action.length());

    // Uri.CREATOR.createFromParcel
    // int type UriString = 1
    gbinder_writer_append_int32(&writer, 1);
    // UriString
    gbinder_writer_append_string16_utf16(&writer, data.utf16(), data.length());
//    gbinder_writer_append_string16_len(&writer, data.toLatin1().constData(), data.length());

    // String mType
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16(&writer, "");
//    gbinder_writer_append_string16_utf16(&writer, type.utf16(), type.length());

    // int mFlags
    gbinder_writer_append_int32(&writer, flags);
//    gbinder_writer_append_int32(&writer, 0);

    // String mPackage
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16(&writer, "");
//    gbinder_writer_append_string16_utf16(&writer, appPackage.utf16(), appPackage.length());

    // ComponentName.readFromParcel
    // String mPackage
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16_len(&writer, "", 0);
//    gbinder_writer_append_string16_utf16(&writer, componentPackage.utf16(), componentPackage.length());
    // String mClass
//    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16_len(&writer, "", 0);
//    gbinder_writer_append_string16_utf16(&writer, componentClass.utf16(), componentClass.length());

    // mSourceBounds = Rect.CREATOR.createFromParcel
    // 0 = pass
    gbinder_writer_append_int32(&writer, 0);

    // int categories length
    // read String mCategory
    gbinder_writer_append_int32(&writer, 0);

    // mSelector = new Intent
    // 0 = pass
    gbinder_writer_append_int32(&writer, 0);

    // mClipData = new ClipData
    // 0 = pass
    gbinder_writer_append_int32(&writer, 0);

    // int mContentUserHint
    // -2 = current user
    gbinder_writer_append_int32(&writer, -2);

    // mExtras = in.readBundle
    // -1 = pass
    gbinder_writer_append_int32(&writer, -1);

    // ** INTENT END **

    // String resolvedType
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16(&writer, "");

    // IBinder resultTo
    gbinder_writer_append_local_object(&writer, NULL);

    // String resultWho
    gbinder_writer_append_int32(&writer, -1);
//    gbinder_writer_append_string16(&writer, "");

    // int requestCode
    gbinder_writer_append_int32(&writer, 0);

    // int startFlags
//    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, flags);

    // ProfilerInfo.CREATOR.createFromParcel
    // 0 = pass
    gbinder_writer_append_int32(&writer, 0);

    // Bundle.CREATOR.createFromParcel
    // 0 = pass
    gbinder_writer_append_int32(&writer, 0);

    int status = 0;
    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply(m_client,
                                                                   3,
                                                                   req,
                                                                   &status);
    qDebug() << Q_FUNC_INFO << "Call status:" << status;

    gbinder_local_request_unref(req);
    gbinder_remote_reply_unref(reply);



//    Intent intent;
//    intent.action = QStringLiteral("android.settings.APPLICATION_DETAILS_SETTINGS");
//    intent.data = QStringLiteral("package:%1").arg(package.toString());
////    intent.type = QStringLiteral("*/*");
////    intent.flags = 0x10000000;
//    intent.package = QStringLiteral("com.android.settings");
//    intent.classPackage = QStringLiteral("com.android.settings");
//    intent.className = QStringLiteral(".applications.InstalledAppDetails");
//    sendIntent(intent);
    return QVariant();
}

QVariant MimeHandlerAdaptor::launchApp(const QVariant &packageName)
{
    launchPackage(packageName.toString());
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
    QString containerPath = QStringLiteral("file:///storage/emulated/0/nemo/");
    if (filename.toString().startsWith(QStringLiteral("file:///home/nemo/"))) {
        containerPath.append(filename.toString().mid(18));
    } else {
        return QVariant();
    }

//    openDownloads();
//    QEventLoop loop;
//    QTimer timer;
//    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
//    timer.start(2000);
//    loop.exec();

//    QStringList params;
//    params << "com.android.commands.am.Am";
//    params << "start" << "-a" << "android.intent.action.SEND" << "-t";
//    params << mimetype.toString();
//    params << "--eu" << "android.intent.extra.STREAM";
//    params << containerPath;
//    appProcess("am.jar", QStringList() << params);


    QString action = QStringLiteral("android.intent.action.SEND");
    QString packageName;// = QStringLiteral("android");
    QString componentPackage;// = QStringLiteral("android");
    QString componentClass;// = QStringLiteral("com.android.internal.app.ResolverActivity");

    QStringList categories;
    int flags = 0x0;

    int contentUserHint = -2;

    if (!m_client) {
        qWarning() << Q_FUNC_INFO << "Can't get client!";
        return QVariant();
    }
    qDebug() << Q_FUNC_INFO << action << containerPath << mimetype.toString() << packageName << componentPackage << componentClass << categories << contentUserHint;

    GBinderLocalRequest* req = gbinder_client_new_request(m_client);
    GBinderWriter writer;
    int status = 0;

    gbinder_local_request_init_writer(req, &writer);

    gbinder_writer_append_local_object(&writer, NULL);
    gbinder_writer_append_string16(&writer, "");

//    gbinder_writer_append_int32(&writer, 1);

    // intent data begin
    gbinder_writer_append_string16_utf16(&writer, action.utf16(), action.length());
    gbinder_writer_append_int32(&writer, 1);
    gbinder_writer_append_string16_utf16(&writer, containerPath.utf16(), containerPath.length());
    gbinder_writer_append_string16_utf16(&writer, mimetype.toString().utf16(), mimetype.toString().length());
    gbinder_writer_append_int32(&writer, flags);
    gbinder_writer_append_string16_utf16(&writer, packageName.utf16(), packageName.length());
    gbinder_writer_append_string16_utf16(&writer, componentPackage.utf16(), componentPackage.length());
    gbinder_writer_append_string16_utf16(&writer, componentClass.utf16(), componentClass.length());
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, categories.length());
    for (const QString &category : categories) {
        gbinder_writer_append_string16_utf16(&writer, category.utf16(), category.length());
    }
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, contentUserHint);
    gbinder_writer_append_int32(&writer, -1); // extras todo
    // intent data end

    gbinder_writer_append_string16(&writer, "");
    gbinder_writer_append_local_object(&writer, NULL);
    gbinder_writer_append_string16(&writer, "");
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);

    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply(m_client,
                                                                   GBINDER_FIRST_CALL_TRANSACTION + 2, // START_ACTIVITY_TRANSACTION
                                                                   req,
                                                                   &status);
    qDebug() << Q_FUNC_INFO << "Call status:" << status;

    gbinder_local_request_unref(req);
    gbinder_remote_reply_unref(reply);

    return QVariant();
}

QVariant MimeHandlerAdaptor::shareText(const QVariant &text)
{
    openDownloads();
    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(2000);
    loop.exec();

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

void MimeHandlerAdaptor::sendIntent(const Intent &intent)
{
    if (!m_client) {
        qWarning() << Q_FUNC_INFO << "Can't get client!";
        return;
    }

    qDebug() << Q_FUNC_INFO
             << intent.action
             << intent.data
             << intent.type
             << intent.package
             << intent.className
             << intent.categories
             << intent.contentUserHint;

    GBinderLocalRequest* req = gbinder_client_new_request(m_client);
    GBinderWriter writer;
    int status = 0;

    gbinder_local_request_init_writer(req, &writer);

    gbinder_writer_append_local_object(&writer, NULL);
    gbinder_writer_append_string16(&writer, "");

//    gbinder_writer_append_int32(&writer, 1);

    // intent data begin
    gbinder_writer_append_string16_utf16(&writer, intent.action.utf16(), intent.action.length());

    gbinder_writer_append_int32(&writer, 1);
    gbinder_writer_append_string16_utf16(&writer, intent.data.utf16(), intent.data.length());

    gbinder_writer_append_string16_utf16(&writer, intent.type.utf16(), intent.type.length());
    gbinder_writer_append_int32(&writer, intent.flags);
    gbinder_writer_append_string16_utf16(&writer, intent.package.utf16(), intent.package.length());
//    gbinder_writer_append_string16(&writer, "");
    gbinder_writer_append_string16_utf16(&writer, intent.classPackage.utf16(), intent.classPackage.length());
    gbinder_writer_append_string16_utf16(&writer, intent.className.utf16(), intent.className.length());
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
//    gbinder_writer_append_int32(&writer, intent.categories.length());
//    for (const QString &category : intent.categories) {
//        gbinder_writer_append_string16_utf16(&writer, category.utf16(), category.length());
//    }
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, -2);

    if (intent.extras.isEmpty()) {
        gbinder_writer_append_int32(&writer, -1);
    } else {
        gsize offset = gbinder_writer_bytes_written(&writer);
        gbinder_writer_append_int32(&writer, 1);
        gbinder_writer_append_int32(&writer, BUNDLE_MAGIC);
        gsize start = gbinder_writer_bytes_written(&writer);
        gbinder_writer_append_int32(&writer, intent.extras.count());
        QHash<QString, QVariant>::const_iterator it = intent.extras.begin();
        while (it != intent.extras.end()) {
          gbinder_writer_append_string16_utf16(&writer, it.key().utf16(), it.key().length());
//          if (it.value().isNull()) {
//            gbinder_writer_append_int32(out, (int)VAL_NULL);
//          }
//          else {
            switch (it.value().type()) {
            case QMetaType::QString:
              gbinder_writer_append_int32(&writer, 0);
              gbinder_writer_append_string16_utf16(&writer, it.value().toString().utf16(), it.value().toString().length());
              break;
            case QMetaType::Int:
            case QMetaType::UInt:
              gbinder_writer_append_int32(&writer, 1);
              gbinder_writer_append_int32(&writer, it.value().toInt());
              break;
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
              gbinder_writer_append_int32(&writer, 6);
              gbinder_writer_append_int64(&writer, it.value().toLongLong());
              break;
            case QMetaType::Bool:
              gbinder_writer_append_int32(&writer, 9);
              gbinder_writer_append_bool(&writer, it.value().toBool());
              break;
            case QMetaType::Double:
              gbinder_writer_append_int32(&writer, 8);
              gbinder_writer_append_double(&writer, it.value().toDouble());
              break;
            default:
              qCritical("Unsupported extra type! %s", QMetaType::typeName(it.value().type()));
            }
//          }
          ++it;
        }
        gsize end = gbinder_writer_bytes_written(&writer);
        gsize len = end - start;
        if (len > G_MAXINT32) {
          qCritical("Bundle too long for signed int32! %d", len);
          return;
        }
        gbinder_writer_overwrite_int32(&writer, offset, len);
    }
    // intent data end

    gbinder_writer_append_string16(&writer, "");
    gbinder_writer_append_local_object(&writer, NULL);
    gbinder_writer_append_string16(&writer, "");
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);
    gbinder_writer_append_int32(&writer, 0);

    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply(m_client,
                                                                   3, //GBINDER_FIRST_CALL_TRANSACTION + 13, // START_ACTIVITY_TRANSACTION
                                                                   req,
                                                                   &status);
    qDebug() << Q_FUNC_INFO << "Call status:" << status;

    gbinder_local_request_unref(req);
    gbinder_remote_reply_unref(reply);
}

void MimeHandlerAdaptor::launchPackage(const QString &packageName)
{
//    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("com.jolla.apkd"),
//                                                      QStringLiteral("/com/jolla/apkd"),
//                                                      QStringLiteral("com.jolla.apkd"),
//                                                      QStringLiteral("launchApp"));
//    msg.setArguments({packageName});
//    QDBusConnection::systemBus().call(msg);

    if (!m_client) {
        qWarning() << Q_FUNC_INFO << "No client!";
        return;
    }

    GBinderLocalRequest* req = gbinder_client_new_request(m_client);

    GBinderWriter writer;
    gbinder_local_request_init_writer(req, &writer);

    gbinder_writer_append_string16_utf16(&writer, packageName.utf16(),
                    packageName.length());

    int status = 0;
    GBinderRemoteReply *reply = gbinder_client_transact_sync_reply(m_client,
                                                                   GBINDER_FIRST_CALL_TRANSACTION,
                                                                   req,
                                                                   &status);

    qDebug() << Q_FUNC_INFO << "Call status:" << status;

    gbinder_local_request_unref(req);
    gbinder_remote_reply_unref(reply);
}

void MimeHandlerAdaptor::componentActivity(const QString &component, const QString &data)
{
    QString package = component.left(component.indexOf("/"));
    launchPackage(package);

    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(1000);
    loop.exec();

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
    QString output = QString::fromUtf8(process->readAll());
    return output;
}

void MimeHandlerAdaptor::binderConnect()
{
    if (!m_serviceManager) {
        qDebug() << Q_FUNC_INFO << "Creating service manager";
        m_serviceManager = gbinder_servicemanager_new("/dev/puddlejumper");
    }

    if (!m_serviceManager) {
        qWarning() << Q_FUNC_INFO << "Can't create service manager!";
    }


    qDebug() << Q_FUNC_INFO << "Add registration handler";
    m_registrationHandler = gbinder_servicemanager_add_registration_handler(m_serviceManager,
                                                                            BINDER_SERVICE,
                                                                            &MimeHandlerAdaptor::registrationHandler,
                                                                            this);

}

void MimeHandlerAdaptor::binderDisconnect()
{
    qDebug() << Q_FUNC_INFO << "Binder disconnect";

    if (m_client) {
        qDebug() << Q_FUNC_INFO << "Removing client";
        gbinder_client_unref(m_client);
        m_client = nullptr;
    }

    if (m_deathHandler && m_remote) {
        gbinder_remote_object_remove_handler(m_remote, m_deathHandler);
        m_deathHandler = 0;
    }

    if (m_remote) {
        qDebug() << Q_FUNC_INFO << "Removing remote";
        gbinder_remote_object_unref(m_remote);
        m_remote = nullptr;
    }

    if (m_registrationHandler && m_serviceManager) {
        gbinder_servicemanager_remove_handler(m_serviceManager, m_registrationHandler);
        m_registrationHandler = 0;
    }

    if (m_serviceManager) {
        gbinder_servicemanager_unref(m_serviceManager);
        m_serviceManager = nullptr;
    }
}

void MimeHandlerAdaptor::registrationHandler(GBinderServiceManager *sm, const char *name, void *user_data)
{
    qDebug() << Q_FUNC_INFO << "Service registered" << sm << name << user_data;
    MimeHandlerAdaptor* instance = static_cast<MimeHandlerAdaptor *>(user_data);
    instance->registerManager();
}

void MimeHandlerAdaptor::registerManager()
{
    qDebug() << Q_FUNC_INFO << "Register manager";

    int status;
    m_remote = gbinder_remote_object_ref(
                gbinder_servicemanager_get_service_sync(m_serviceManager,
                                                        BINDER_SERVICE,
                                                        &status));

    qDebug() << Q_FUNC_INFO << "Service status:" << status;

    if (!m_remote) {
        qWarning() << Q_FUNC_INFO << "No remote!";
        binderDisconnect();
        return;
    }

    m_deathHandler = gbinder_remote_object_add_death_handler(m_remote, &MimeHandlerAdaptor::deathHandler, this);

    m_client = gbinder_client_new(m_remote, BINDER_IFACE);

    if (!m_client) {
        qWarning() << Q_FUNC_INFO << "No client!";
        binderDisconnect();
        return;
    }

    gbinder_servicemanager_remove_handler(m_serviceManager, m_registrationHandler);
    m_registrationHandler = 0;
}

void MimeHandlerAdaptor::deathHandler(GBinderRemoteObject *obj, void *user_data)
{
    qDebug() << Q_FUNC_INFO << "Binder died" << obj << user_data;
    MimeHandlerAdaptor* instance = static_cast<MimeHandlerAdaptor *>(user_data);
    instance->binderDisconnect();
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

