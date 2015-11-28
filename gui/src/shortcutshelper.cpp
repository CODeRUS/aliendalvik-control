#include "shortcutshelper.h"

#include <QTimer>
#include <QDebug>
#include <mlite5/MDesktopEntry>
#include <QSettings>

ShortcutsHelper::ShortcutsHelper(QObject *parent) :
    QObject(parent)
{
    QTimer::singleShot(1, this, SLOT(createInterface()));

    nam = new QNetworkAccessManager(this);

    QSettings settings;
    settings.sync();
    QString code = settings.value("code", "demo").toString();
    checkActivation(code);
}

int ShortcutsHelper::getLongPressDelay()
{
    if (iface) {
        QDBusReply<int> reply = iface->call("getLongPressDelay");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return 1500;
}

void ShortcutsHelper::setLongPressDelay(int msecs)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressDelay", msecs);
        Q_EMIT longPressDelayChanged();
    }
}

QString ShortcutsHelper::getLongPressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getLongPressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "shutdown";
}

QString ShortcutsHelper::getLongPressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getLongPressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "";
}

void ShortcutsHelper::setLongPressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressActionOn", action);
        Q_EMIT longPressActionOnChanged();
    }
}

void ShortcutsHelper::setLongPressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressActionOff", action);
        Q_EMIT longPressActionOffChanged();
    }
}

int ShortcutsHelper::getDoublePressDelay()
{
    if (iface) {
        QDBusReply<int> reply = iface->call("getDoublePressDelay");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return 500;
}

void ShortcutsHelper::setDoublePressDelay(int msecs)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressDelay", msecs);
        Q_EMIT doublePressDelayChanged();
    }
}

QString ShortcutsHelper::getDoublePressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getDoublePressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "blank,tklock,devlock";
}

QString ShortcutsHelper::getDoublePressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getDoublePressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "unblank,tkunlock";
}

void ShortcutsHelper::setDoublePressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressActionOn", action);
        Q_EMIT doublePressActionOnChanged();
    }
}

void ShortcutsHelper::setDoublePressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressActionOff", action);
        Q_EMIT doublePressActionOffChanged();
    }
}

QString ShortcutsHelper::getShortPressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getShortPressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "blank,tklock";
}

QString ShortcutsHelper::getShortPressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getShortPressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "unblank";
}

void ShortcutsHelper::setShortPressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setShortPressActionOn", action);
        Q_EMIT shortPressActionOnChanged();
    }
}

void ShortcutsHelper::setShortPressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setShortPressActionOff", action);
        Q_EMIT shortPressActionOffChanged();
    }
}

QString ShortcutsHelper::getAction1()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction1");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "power-key-menu";
}

void ShortcutsHelper::setAction1(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction1", action);
        Q_EMIT action1Changed();
    }
}

QString ShortcutsHelper::getAction2()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction2");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "double-power-key";
}

void ShortcutsHelper::setAction2(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction2", action);
        Q_EMIT action2Changed();
    }
}

QString ShortcutsHelper::getAction3()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction3");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "event3";
}

void ShortcutsHelper::setAction3(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction3", action);
        Q_EMIT action3Changed();
    }
}

QString ShortcutsHelper::getAction4()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction4");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "event4";
}

void ShortcutsHelper::setAction4(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction4", action);
        Q_EMIT action4Changed();
    }
}

QString ShortcutsHelper::getAction5()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction5");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "event5";
}

void ShortcutsHelper::setAction5(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction5", action);
        Q_EMIT action5Changed();
    }
}

QString ShortcutsHelper::getAction6()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getAction6");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "event6";
}

void ShortcutsHelper::setAction6(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setAction6", action);
        Q_EMIT action6Changed();
    }
}

QString ShortcutsHelper::bannerPath() const
{
    return _bannerPath;
}

void ShortcutsHelper::resetToDefaults()
{
    if (iface) {
        iface->call(QDBus::NoBlock, "resetToDefaults");
    }
}

void ShortcutsHelper::checkActivation(const QString &code)
{
    QSettings settings;
    settings.setValue("code", code);
    settings.sync();

    QString url(QByteArray::fromBase64("aHR0cHM6Ly9jb2RlcnVzLm9wZW5yZXBvcy5uZXQvd2hpdGVzb2Z0L2FjdGl2YXRpb24vJTE="));
    QObject::connect(nam->get(QNetworkRequest(QUrl(url.arg(code)))), SIGNAL(finished()), this, SLOT(onActivationReply()));
}

QString ShortcutsHelper::readDesktopName(const QString &path) const
{
    MDesktopEntry desktop(path);
    if (desktop.isValid()) {
        return desktop.name();
    }
    return path;
}

void ShortcutsHelper::onActivationReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        _bannerPath = QString::fromUtf8(reply->readAll());
        Q_EMIT bannerPathChanged();
    }
}

void ShortcutsHelper::createInterface()
{
    iface = new QDBusInterface("org.coderus.powermenu", "/", "org.coderus.powermenu", QDBusConnection::sessionBus(), this);
}
