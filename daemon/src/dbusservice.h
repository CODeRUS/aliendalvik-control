#ifndef DBUSSERVICE_H
#define DBUSSERVICE_H

#include "../common/src/aliendalvikcontroller.h"

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
class QLocalSocket;
class QTimer;
class AlienAbstract;
class DBusService : public AliendalvikController, public QDBusContext
{
    Q_OBJECT

public:
    explicit DBusService(QObject *parent = nullptr);
    virtual ~DBusService();

public slots:
    void start();

signals:
    void isTopmostAndroidChanged(bool isAndroid);

private slots:
    int getApiVersion();

    void sendKeyevent(int code);
    void sendInput(const QString &text);
    void sendTap(int posx, int posy);
    void sendSwipe(int startx, int starty, int endx, int endy, int duration);
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
    void launcherActivity(const QString &package, const QString &className, const QString &data = QString());
    void componentActivity(const QString &package, const QString &className, const QString &data = QString());
    void uriLaunchActivity(const QString &package, const QString &className, const QString &launcherClass, const QString &data = QString());
    void forceStop(const QString &packageName);
    void shareContent(const QVariantMap &content, const QString &source);
    void shareFile(const QString &filename, const QString &mimetype);
    void shareText(const QString &text);
    void doShare(const QString &mimetype,
                 const QString &filename,
                 const QString &data,
                 const QString &packageName,
                 const QString &className,
                 const QString &launcherClass);
    QString getFocusedApp();
    bool isTopmostAndroid();
    void getImeList();
    void triggerImeMethod(const QString &ime, bool enable);
    void setImeMethod(const QString &ime);
    QString getSettings(const QString &nspace, const QString &key);
    void putSettings(const QString &nspace, const QString &key, const QString &value);
    QString getprop(const QString &key);
    void setprop(const QString &key, const QString &value);
    void quit();

    void startReadingLocalServer();
    void processHelperResult(const QByteArray &data);

private:
    friend class DBusAdaptor;

    void checkHelperSocket();
    void installApkSync();

    bool checkFilePermissions(const QString &filename);

    int guessApi();

    bool activateApp(const QString &packageName, const QString &launcherClass);
    void waitForAndroidWindow();

    QString checkShareFile(const QString &shareFilePath);

    DBusAdaptor *m_adaptor = nullptr;

    QString _watchDir;
    INotifyWatcher *_watcher;

    bool _isTopmostAndroid;

    QThread *m_serverThread = nullptr;
    QLocalServer *m_localServer = nullptr;

    QHash<QLocalSocket*, QByteArray> m_pendingRequests;

    QVariantHash m_deviceProperties;

    QTimer *m_sessionBusConnector = nullptr;
    QDBusConnection m_sbus;

    AlienAbstract *m_alien = nullptr;
    QString m_localSocket;

    QProcessEnvironment m_alienEnvironment;

private slots:
    void requestDeviceInfo();

    void readApplications(const QString &);
    void desktopChanged(const QString &path);

    void topmostIdChanged(int pId);

    void serviceStopped() override;
    void serviceStarted() override;

};

#endif // DBUSSERVICE_H
