#ifndef MIMEHANDLERADAPTOR_H
#define MIMEHANDLERADAPTOR_H

#include <QObject>
#include <QDBusVirtualObject>
#include <QFileSystemWatcher>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>

#include <gbinder.h>

class MimeHandlerAdaptor : public QDBusVirtualObject
{
    Q_OBJECT

public:
    MimeHandlerAdaptor(QObject *parent = nullptr);
    ~MimeHandlerAdaptor();

    QString introspect(const QString &) const;
    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

private slots:
    QVariant sendKeyevent(const QVariant &code);
    QVariant sendInput(const QVariant &text);
    QVariant broadcastIntent(const QVariant &intent);
    QVariant startIntent(const QVariant &intent);
    QVariant uriActivity(const QVariant &uri);
    QVariant uriActivitySelector(const QVariant &uri);
    QVariant hideNavBar();
    QVariant showNavBar();
    QVariant openDownloads(const QVariant & = QVariant());
    QVariant openSettings(const QVariant & = QVariant());
    QVariant openContacts(const QVariant & = QVariant());
    QVariant openCamera(const QVariant & = QVariant());
    QVariant openGallery(const QVariant & = QVariant());
    QVariant getImeList();
    QVariant triggerImeMethod(const QVariant &ime, const QVariant &enable);
    QVariant setImeMethod(const QVariant &ime);
    QVariant shareFile(const QVariant &filename, const QVariant &mimetype);
    QVariant shareText(const QVariant &text);
    QVariant getFocusedApp();
    QVariant isTopmostAndroid();
    QVariant getSettings(const QVariant &nspace, const QVariant &key);
    QVariant putSettings(const QVariant &nspace, const QVariant &key, const QVariant &value);
    QVariant getprop(const QVariant &key);
    QVariant setprop(const QVariant &key, const QVariant &value);
    QVariant quit();

private:
    void launchApp(const QString &packageName);

    void componentActivity(const QString &component, const QString &data);

    void appProcess(const QString &jar, const QStringList &params);
    QString appProcessOutput(const QString &jar, const QStringList &params);
    QString packageName(const QString &package);
    void runCommand(const QString &program, const QStringList &params);
    QString runCommandOutput(const QString &program, const QStringList &params);

    void emitSignal(const QString &name, const QList<QVariant> &arguments);

    void binderConnect();
    void binderDisconnect();

    static void registrationHandler(GBinderServiceManager* sm, const char* name, void* user_data);
    void registerManager();

    static void deathHandler(GBinderRemoteObject *obj, void *user_data);

    QString _watchDir;
    QFileSystemWatcher *_watcher;

    bool _isTopmostAndroid;

    QDBusInterface *apkdIface;

    GBinderRemoteObject *m_remote = nullptr;
    GBinderServiceManager *m_serviceManager = nullptr;
    GBinderClient *m_client = nullptr;

    int m_registrationHandler = 0;
    int m_deathHandler = 0;

private slots:
    void readApplications(const QString &);
    void desktopChanged(const QString &path);

    void topmostIdChanged(int pId);

};

#endif // MIMEHANDLERADAPTOR_H
