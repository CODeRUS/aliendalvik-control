#ifndef ALIENABSTRACT_H
#define ALIENABSTRACT_H

#include "aliendalvikcontroller.h"

#include <QObject>
#include <QVariantList>

class AlienAbstract : public AliendalvikController
{
    Q_OBJECT
public:
    explicit AlienAbstract(QObject *parent = nullptr);

public slots:
    virtual QString dataPath() const = 0;

    virtual void sendKeyevent(int code) = 0;
    virtual void sendInput(const QString &text) = 0;
    virtual void sendTap(int posx, int posy, quint64 uptime) = 0;
    virtual void sendSwipe(int startx, int starty, int endx, int endy, int duration) = 0;
    virtual void uriActivity(const QString &uri) = 0;
    virtual void uriActivitySelector(const QString &uri) = 0;
    virtual void hideNavBar(int height, int api = 0) = 0;
    virtual void showNavBar(int api = 0) = 0;
    virtual void openDownloads() = 0;
    virtual void openSettings() = 0;
    virtual void openContacts() = 0;
    virtual void openCamera() = 0;
    virtual void openGallery() = 0;
    virtual void openAppSettings(const QString &package) = 0;
    virtual void launchApp(const QString &packageName) = 0;
    virtual void componentActivity(const QString &package, const QString &className, const QString &data = QString()) = 0;
    virtual void forceStop(const QString &packageName) = 0;
    virtual void shareFile(const QString &filename, const QString &mimetype) = 0;
    virtual void shareText(const QString &text) = 0;
    virtual void doShare(const QString &mimetype,
                 const QString &filename,
                 const QString &data,
                 const QString &packageName,
                 const QString &className,
                 const QString &launcherClass) = 0;
    virtual QVariantList getImeList() = 0;
    virtual void triggerImeMethod(const QString &ime, bool enable) = 0;
    virtual void setImeMethod(const QString &ime) = 0;
    virtual QString getSettings(const QString &nspace, const QString &key) = 0;
    virtual void putSettings(const QString &nspace, const QString &key, const QString &value) = 0;
    virtual QString getprop(const QString &key) = 0;
    virtual void setprop(const QString &key, const QString &value) = 0;

    virtual void requestDeviceInfo() = 0;
    virtual void requestUptime() = 0;

    virtual QString checkShareFile(const QString &shareFilePath) = 0;

    virtual void installApk(const QString &fileName) = 0;
};

#endif // ALIENABSTRACT_H
