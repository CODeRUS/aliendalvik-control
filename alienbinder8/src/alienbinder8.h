#ifndef ALIENCHROOT_H
#define ALIENCHROOT_H

#include "../common/src/alienabstract.h"

#include <QProcessEnvironment>

class AlienBinder8 : public AlienAbstract
{
    Q_OBJECT
public:
    explicit AlienBinder8(QObject *parent = nullptr);

public slots:
    QString dataPath() const override;

    void sendKeyevent(int) override;
    void sendInput(const QString &) override;
    void uriActivity(const QString &uri) override;
    void uriActivitySelector(const QString &uri) override;
    void hideNavBar(int height, int) override;
    void showNavBar(int) override;
    void openDownloads() override;
    void openSettings() override;
    void openContacts() override;
    void openCamera() override;
    void openGallery() override;
    void openAppSettings(const QString &packageName) override;
    void launchApp(const QString &packageName) override;
    void componentActivity(const QString &package, const QString &className, const QString &data) override;
    void forceStop(const QString &packageName) override;
    void shareFile(const QString &filename, const QString &mimetype) override;
    void shareText(const QString &text) override;
    void doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className, const QString &launcherClass) override;
    QVariantList getImeList() override;
    void triggerImeMethod(const QString &ime, bool enable) override;
    void setImeMethod(const QString &ime) override;
    QString getSettings(const QString &nspace, const QString &key) override;
    void putSettings(const QString &nspace, const QString &key, const QString &value) override;
    QString getprop(const QString &key) override;
    void setprop(const QString &key, const QString &value) override;

    void requestDeviceInfo() override;

    void installApk(const QString &fileName) override;

private:
    void mountSdcard(const QString mountPath);
    void umountSdcard();

private slots:
    void serviceStopped() override;
    void serviceStarted() override;
};

#endif // ALIENCHROOT_H
