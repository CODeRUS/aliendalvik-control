#include "alienbinder8.h"

#include <QDebug>

static const QString s_dataPath = QStringLiteral("/home/.android/data");


static AlienBinder8 *s_instance = nullptr;

extern "C" AlienBinder8 *instance(QObject *parent)
{
    if (!s_instance) {
        s_instance = new AlienBinder8(parent);
    }

    return s_instance;
}

AlienBinder8::AlienBinder8(QObject *parent)
    : AlienAbstract(parent)
{
}

void AlienBinder8::sendKeyevent(int code)
{

}

void AlienBinder8::sendInput(const QString &text)
{

}

void AlienBinder8::uriActivity(const QString &uri)
{

}

void AlienBinder8::uriActivitySelector(const QString &uri)
{

}

void AlienBinder8::hideNavBar(int height)
{

}

void AlienBinder8::showNavBar()
{

}

void AlienBinder8::openDownloads()
{

}

void AlienBinder8::openSettings()
{

}

void AlienBinder8::openContacts()
{

}

void AlienBinder8::openCamera()
{

}

void AlienBinder8::openGallery()
{

}

void AlienBinder8::openAppSettings(const QString &)
{

}

void AlienBinder8::launchApp(const QString &packageName)
{

}

void AlienBinder8::componentActivity(const QString &package, const QString &className, const QString &data)
{

}

void AlienBinder8::uriActivity(const QString &package, const QString &className, const QString &launcherClass, const QString &data)
{

}

void AlienBinder8::forceStop(const QString &packageName)
{

}

void AlienBinder8::shareFile(const QString &filename, const QString &mimetype)
{

}

void AlienBinder8::shareText(const QString &text)
{

}

void AlienBinder8::doShare(const QString &mimetype, const QString &filename, const QString &data, const QString &packageName, const QString &className, const QString &launcherClass)
{

}

QVariantList AlienBinder8::getImeList()
{
    return QVariantList();
}

void AlienBinder8::triggerImeMethod(const QString &ime, bool enable)
{

}

void AlienBinder8::setImeMethod(const QString &ime)
{

}

QString AlienBinder8::getSettings(const QString &nspace, const QString &key)
{
    return QString();
}

void AlienBinder8::putSettings(const QString &nspace, const QString &key, const QString &value)
{

}

QString AlienBinder8::getprop(const QString &key)
{
    return QString();
}

void AlienBinder8::setprop(const QString &key, const QString &value)
{

}

QString AlienBinder8::dataPath() const
{
    return s_dataPath;
}
