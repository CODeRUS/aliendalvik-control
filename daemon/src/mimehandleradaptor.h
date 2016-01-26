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

class MimeHandlerAdaptor : public QDBusVirtualObject
{
    Q_OBJECT

public:
    MimeHandlerAdaptor(QObject *parent = 0);
    ~MimeHandlerAdaptor();

    QString introspect(const QString &) const;
    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

private:
    void sendKeyevent(int code);
    void sendInput(const QString &text);
    void broadcastIntent(const QString &intent);
    void startIntent(const QString &intent);
    void uriActivity(const QString &uri);
    void uriActivitySelector(const QString &uri);
    void hideNavBar();
    void showNavBar();
    void shareFile(const QVariantList &args);
    void shareText(const QString &text);

    void componentActivity(const QString &component, const QString &data);

    void appProcess(const QString &jar, const QStringList &params);
    void runCommand(const QString &program, const QStringList &params);

    QString _watchDir;
    QFileSystemWatcher *_watcher;

private slots:
    void readApplications(const QString &);
    void desktopChanged(const QString &path);

};

#endif // MIMEHANDLERADAPTOR_H
