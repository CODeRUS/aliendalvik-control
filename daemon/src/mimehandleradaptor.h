#ifndef MIMEHANDLERADAPTOR_H
#define MIMEHANDLERADAPTOR_H

#include "intentsender.h"

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusContext>

class DBusAdaptor;
class INotifyWatcher;
class QLocalServer;
class MimeHandlerAdaptor : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit MimeHandlerAdaptor(QObject *parent = nullptr);
    virtual ~MimeHandlerAdaptor();

public slots:
    void start();

private slots:
    void sendKeyevent(int code);
    void sendInput(const QString &text);
    void broadcastIntent(const QString &intent);
    void startIntent(const QString &intent);
    void uriActivity(const QString &uri);
    void uriActivitySelector(const QString &uri);
    void hideNavBar();
    void showNavBar();
    void openDownloads();
    void openSettings();
    void openContacts();
    void openCamera();
    void openGallery();
    void openAppSettings(const QString &package);
    void launchApp(const QString &packageName);
    void componentActivity(const QString &package, const QString &className, const QString &data);
    void forceStop(const QString &packageName);
    void getImeList();
    void triggerImeMethod(const QString &ime, bool enable);
    void setImeMethod(const QString &ime);
    void shareContent(const QVariantMap &content, const QString &source);
    void shareFile(const QString &filename, const QString &mimetype);
    void shareText(const QString &text);
    void doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className);
    QString getFocusedApp();
    bool isTopmostAndroid();
    QString getSettings(const QString &nspace, const QString &key);
    void putSettings(const QString &nspace, const QString &key, const QString &value);
    QString getprop(const QString &key);
    void setprop(const QString &key, const QString &value);
    void quit();

    void startReadingLocalServer();

private:
    friend class DBusAdaptor;
    void launchPackage(const QString &packageName);

    void checkSdcardMount();

    void appProcess(const QString &jar, const QStringList &params);
    QString appProcessOutput(const QString &jar, const QStringList &params);
    QString packageName(const QString &package);
    void runCommand(const QString &program, const QStringList &params);
    QString runCommandOutput(const QString &program, const QStringList &params);

    DBusAdaptor *m_adaptor = nullptr;

    QString _watchDir;
    INotifyWatcher *_watcher;

    bool _isTopmostAndroid;

    QDBusInterface *apkdIface;

    QThread *m_serverThread = nullptr;
    QLocalServer *m_localServer = nullptr;

private slots:
    void readApplications(const QString &);
    void desktopChanged(const QString &path);

    void topmostIdChanged(int pId);
    void aliendalvikChanged(const QString &, const QVariantMap &properties, const QStringList &);

};

#endif // MIMEHANDLERADAPTOR_H
