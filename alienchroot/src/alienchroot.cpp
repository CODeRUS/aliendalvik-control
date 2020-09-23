#include "alienchroot.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

static const QString s_dataPath = QStringLiteral("/opt/alien/data");

static const QString s_chrootExecutable = QStringLiteral("/usr/sbin/chroot");
static const QString s_chrootPath = QStringLiteral("/opt/alien");

static const QString s_helperPath = QStringLiteral("/opt/alien/data/app/aliendalvik-control.apk");

static AlienAbstract *s_instance = nullptr;

extern "C" AlienAbstract *instance(QObject *parent)
{
    if (!s_instance) {
        s_instance = new AlienChroot(parent);
    }

    return s_instance;
}

AlienChroot::AlienChroot(QObject *parent)
    : AlienAbstract(parent)
{
    m_alienEnvironment.insert(QStringLiteral("SYSTEM_USER_LANG"), QStringLiteral("C"));
    m_alienEnvironment.insert(QStringLiteral("ANDROID_ROOT"), QStringLiteral("/system"));
    m_alienEnvironment.insert(QStringLiteral("ANDROID_DATA"), QStringLiteral("/data"));

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
                qDebug() << "BOOTCLASSPATH:" << line;
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
                qDebug() << "ALIEN_ID:" << line;
                m_alienEnvironment.insert(QStringLiteral("ALIEN_ID"), line);
                break;
            }
        }
    }
}

QString AlienChroot::dataPath() const
{
    return s_dataPath;
}

void AlienChroot::sendKeyevent(int code, quint64)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("keyevent"), QString::number(code)});
}

void AlienChroot::sendInput(const QString &text)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("text"), text});
}

void AlienChroot::sendTap(int posx, int posy, quint64)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("tap"), QString::number(posx), QString::number(posy)});
}

void AlienChroot::sendSwipe(int startx, int starty, int endx, int endy, int duration)
{
    runCommand(QStringLiteral("input"), {QStringLiteral("swipe"), QString::number(startx), QString::number(starty),
                                         QString::number(endx), QString::number(endy), QString::number(duration)});
}

void AlienChroot::uriActivity(const QString &uri)
{
    qDebug() << Q_FUNC_INFO << uri;
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("uri"),
                   QStringLiteral("-d"),
                   uri,
               });
}

void AlienChroot::uriActivitySelector(const QString &uri)
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("selector"),
                   QStringLiteral("-d"),
                   uri,
               });
}

void AlienChroot::hideNavBar(int height, int api)
{
    if (api >= 19) {
        runCommand(QStringLiteral("wm"), {
                       QStringLiteral("overscan"),
                       QStringLiteral("0,0,0,-%1").arg(QString::number(height))
                   });
    } else {
        runCommand(QStringLiteral("/system/bin/service"), {
                       QStringLiteral("call"),
                       QStringLiteral("activity"),
                       QStringLiteral("42"),
                       QStringLiteral("s16"),
                       QStringLiteral("com.android.systemui")
                   });
    }
}

void AlienChroot::showNavBar(int api)
{
    if (api >= 19) {
        runCommand(QStringLiteral("wm"), {
                       QStringLiteral("overscan"),
                       QStringLiteral("0,0,0,0")
                   });
    } else {
        runCommand(QStringLiteral("am"), {
                       QStringLiteral("startservice"),
                       QStringLiteral("-n"),
                       QStringLiteral("com.android.systemui/.SystemUIService")
                   });
    }
}

void AlienChroot::hideStatusBar()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::showStatusBar()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::openDownloads()
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-a"),
                   QStringLiteral("android.intent.action.VIEW_DOWNLOADS")
               });
}

void AlienChroot::openSettings()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::openContacts()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::openCamera()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::openGallery()
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::openAppSettings(const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented!";
}

void AlienChroot::launchApp(const QString &packageName)
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("launchApp"),
                   QStringLiteral("--es"),
                   QStringLiteral("android.intent.extra.TEXT"),
                   packageName,
               });
}

void AlienChroot::componentActivity(const QString &package, const QString &className, const QString &data)
{
    QStringList options = {
        QStringLiteral("start"),
        QStringLiteral("-n"),
        QStringLiteral("%1/%2").arg(package, className)
    };
    if (data.isEmpty()) {
        options.append({
                           QStringLiteral("-a"),
                           QStringLiteral("android.intent.action.MAIN")
                       });
    } else {
        options.append({
                           QStringLiteral("-a"),
                           QStringLiteral("android.intent.action.VIEW"),
                           QStringLiteral("-d"),
                           data
                       });
    }

    runCommand(QStringLiteral("am"), options);
}

void AlienChroot::forceStop(const QString &packageName)
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("force-stop"),
                   packageName
               });
}

void AlienChroot::shareFile(const QString &filename, const QString &mimetype)
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("sharing"),
                   QStringLiteral("-a"),
                   QStringLiteral("android.intent.action.SEND"),
                   QStringLiteral("-t"),
                   mimetype,
                   QStringLiteral("--eu"),
                   QStringLiteral("android.intent.extra.STREAM"),
                   filename
               });
}

void AlienChroot::shareText(const QString &text)
{
    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("sharing"),
                   QStringLiteral("-a"),
                   QStringLiteral("android.intent.action.SEND"),
                   QStringLiteral("-t"),
                   QStringLiteral("text/*"),
                   QStringLiteral("--es"),
                   QStringLiteral("android.intent.extra.TEXT"),
                   text
               });
}

void AlienChroot::doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className, const QString &launcherClass)
{
    Q_UNUSED(launcherClass)

    QStringList options = {
        QStringLiteral("start"),
        QStringLiteral("-a"),
        QStringLiteral("android.intent.action.SEND"),
        QStringLiteral("-n"),
        QStringLiteral("%1/%2").arg(packageName, className),
        QStringLiteral("-t"),
        mimetype
    };
    if (filename.isEmpty()) {
        options.append({
                           QStringLiteral("--es"),
                           QStringLiteral("android.intent.extra.TEXT"),
                           data
                       });
    } else {
        options.append({
                           QStringLiteral("--eu"),
                           QStringLiteral("android.intent.extra.STREAM"),
                           QStringLiteral("file://%1").arg(filename)
                       });
    }

    runCommand(QStringLiteral("am"), options);
}

QVariantList AlienChroot::getImeList()
{
    const QString fullOutput = runCommandOutput(QStringLiteral("ime"), {
                                                   QStringLiteral("list"),
                                                   QStringLiteral("-s"),
                                                   QStringLiteral("-a")
                                               });
    const QStringList fullOutputLines = fullOutput.trimmed().split(QChar(u'\n'));
    qDebug() << fullOutput.trimmed();

    const QString enabledOutput = runCommandOutput(QStringLiteral("ime"), {
                                                 QStringLiteral("list"),
                                                 QStringLiteral("-s")
                                             });
    const QStringList enabledOutputLines = enabledOutput.trimmed().split(QChar(u'\n'));
    qDebug() << enabledOutput.trimmed();

    QVariantList imeList;
    for (const QString &imeName : fullOutputLines) {
        QVariantMap imeMethod;
        imeMethod.insert(QStringLiteral("name"), imeName);
        imeMethod.insert(QStringLiteral("enabled"), enabledOutputLines.contains(imeName));
        imeList.append(imeMethod);
    }

    return imeList;
}

void AlienChroot::triggerImeMethod(const QString &ime, bool enable)
{
    runCommand(QStringLiteral("ime"), {
                   enable ? QStringLiteral("enable") : QStringLiteral("disable"),
                   ime
               });
}

void AlienChroot::setImeMethod(const QString &ime)
{
    runCommand(QStringLiteral("ime"), {
                   QStringLiteral("set"),
                   ime
               });
}

QString AlienChroot::getSettings(const QString &nspace, const QString &key)
{
    const QString value = runCommandOutput(QStringLiteral("settings"), {
                                               QStringLiteral("get"),
                                               nspace,
                                               key
                                           });
    return value.trimmed();
}

void AlienChroot::putSettings(const QString &nspace, const QString &key, const QString &value)
{
    runCommand(QStringLiteral("settings"), {
                   QStringLiteral("put"),
                   nspace,
                   key,
                   value
               });
}

QString AlienChroot::getprop(const QString &key)
{
    const QString value = runCommandOutput(QStringLiteral("/system/bin/getprop"), {key});
    return value.trimmed();
}

void AlienChroot::setprop(const QString &key, const QString &value)
{
    runCommand(QStringLiteral("/system/bin/setprop"), {
                   key,
                   value
               });
}

void AlienChroot::requestDeviceInfo()
{
    qDebug() << Q_FUNC_INFO;

    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("deviceInfo")
               });
}

void AlienChroot::requestUptime()
{
    qDebug() << Q_FUNC_INFO;

    runCommand(QStringLiteral("am"), {
                   QStringLiteral("start"),
                   QStringLiteral("-n"),
                   QStringLiteral("org.coderus.aliendalvikcontrol/.MainActivity"),
                   QStringLiteral("--es"),
                   QStringLiteral("command"),
                   QStringLiteral("uptime")
               });
}

QString AlienChroot::checkShareFile(const QString &shareFilePath)
{
    qDebug() << Q_FUNC_INFO << shareFilePath;

    if (shareFilePath.startsWith(QLatin1String("/data/data/org.coderus.aliendalvikcontrol"))) {
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

void AlienChroot::installApk(const QString &fileName)
{
    if (QFileInfo::exists(s_helperPath)) {
        qWarning() << Q_FUNC_INFO << "Removing old helper:" <<
        QFile::remove(s_helperPath);
    }
    qWarning() << Q_FUNC_INFO << "Installing helper apk:" <<
    QFile::copy(fileName, s_helperPath);
}

void AlienChroot::runCommand(const QString &program, const QStringList &params)
{
    qDebug() << "Executing" << program << params;

    QStringList arguments = {
        s_chrootPath,
        program
    };
    arguments.append(params);

    QProcess process;
    process.setProcessEnvironment(m_alienEnvironment);
    process.start(s_chrootExecutable, arguments);
    process.waitForFinished(5000);
}

QString AlienChroot::runCommandOutput(const QString &program, const QStringList &params)
{
    QStringList arguments = {
        s_chrootPath,
        program
    };
    arguments.append(params);

    QProcess process;
    process.setProcessEnvironment(m_alienEnvironment);
    process.start(s_chrootExecutable, arguments);
    process.waitForFinished(5000);
    if (process.state() == QProcess::Running) {
        process.close();
        return QString();
    }
    return QString::fromUtf8(process.readAll());
}

void AlienChroot::serviceStopped()
{

}

void AlienChroot::serviceStarted()
{

}
