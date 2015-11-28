#include "dbuslistener.h"

#include <QDesktopServices>
#include <QDebug>
#include <mlite5/MGConfItem>
#include "sailfishapp.h"
#include <qpa/qplatformnativeinterface.h>
#include <QQmlContext>
#include <QGuiApplication>
#include <QtQml>

#include "components/togglesmodel.h"
#include "components/fileutils.h"

#include <mlite5/MGConfItem>
#include <QDebug>

DBusListener::DBusListener(QObject *parent) :
    QObject(parent)
{
    view = NULL;
}

void DBusListener::startService()
{
    qDebug("startService");

    QDBusConnection::systemBus().connect("", MCE_SIGNAL_PATH, MCE_SIGNAL_IFACE,
                                         "power_button_trigger", this, SLOT(powerButtonTrigger(QString)));

    mce = new QDBusInterface(MCE_SERVICE,
                             MCE_REQUEST_PATH,
                             MCE_REQUEST_IFACE,
                             QDBusConnection::systemBus(), this);

    qDebug() << "DBus service" << (QDBusConnection::sessionBus().registerService("org.coderus.powermenu") ? "registered" : "error!");
    qDebug() << "DBus object" << (QDBusConnection::sessionBus().registerObject("/", this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties)
                ? "registered" : "error!");

    qDebug() << "listener started";

    qmlRegisterType<TogglesModel>("org.coderus.powermenu.controls", 1, 0, "TogglesModel");
    qmlRegisterType<FileUtils>("org.coderus.powermenu.controls", 1, 0, "FileUtils");

    flashlight = new FlashlightControl(this);
    screenshot = new ScreenshotControl(this);

    MGConfItem verConf("/apps/powermenu/version");
    int version = verConf.value(1).toInt();
    if (version < 2) {
        resetToDefaults();
        verConf.set(2);
    }
}

QString DBusListener::getAction1()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION1).toString();
}

void DBusListener::setAction1(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION1, action);
}

QString DBusListener::getAction2()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION2).toString();
}

void DBusListener::setAction2(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION2, action);
}

QString DBusListener::getAction3()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION3).toString();
}

void DBusListener::setAction3(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION3, action);
}

QString DBusListener::getAction4()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION4).toString();
}

void DBusListener::setAction4(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION4, action);
}

QString DBusListener::getAction5()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION5).toString();
}

void DBusListener::setAction5(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION5, action);
}

QString DBusListener::getAction6()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION6).toString();
}

void DBusListener::setAction6(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION6, action);
}

int DBusListener::getLongPressDelay()
{
    return getMceValue(MCE_GCONF_POWERKEY_LONG_PRESS_DELAY).toInt();
}

void DBusListener::setLongPressDelay(int msecs)
{
    setMceValue(MCE_GCONF_POWERKEY_LONG_PRESS_DELAY, msecs);
}

QString DBusListener::getLongPressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_ON).toString();
}

QString DBusListener::getLongPressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_OFF).toString();
}

void DBusListener::setLongPressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_ON, action);
}

void DBusListener::setLongPressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_OFF, action);
}

int DBusListener::getDoublePressDelay()
{
    return getMceValue(MCE_GCONF_POWERKEY_DOUBLE_PRESS_DELAY).toInt();
}

void DBusListener::setDoublePressDelay(int msecs)
{
    setMceValue(MCE_GCONF_POWERKEY_DOUBLE_PRESS_DELAY, msecs);
}

QString DBusListener::getDoublePressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_ON).toString();
}

QString DBusListener::getDoublePressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_OFF).toString();
}

void DBusListener::setDoublePressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_ON, action);
}

void DBusListener::setDoublePressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_OFF, action);
}

QString DBusListener::getShortPressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_ON).toString();
}

QString DBusListener::getShortPressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_OFF).toString();
}

void DBusListener::setShortPressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_ON, action);
}

void DBusListener::setShortPressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_OFF, action);
}

void DBusListener::resetToDefaults()
{
    qDebug("resetToDefaults");

    setAction1("power-key-menu");
    setAction2("double-power-key");
    setAction3("event3");
    setAction4("event4");
    setAction5("event5");
    setAction6("event6");

    setShortPressActionOn("blank,tklock");
    setDoublePressActionOn("blank,tklock,devlock");
    setLongPressActionOn("dbus1");
    setShortPressActionOff("unblank");
    setDoublePressActionOff("unblank,tkunlock,dbus2");
    setLongPressActionOff("");

    setLongPressDelay(1500);
    setDoublePressDelay(400);
}

void DBusListener::openDesktop(const QString &desktop)
{
    if (desktop.endsWith(".desktop")) {
        if (QFile(desktop).exists()) {
            qDebug() << "starting desktop:" << desktop;
            QDesktopServices::openUrl(QUrl::fromLocalFile(desktop));
        }
        else {
            qWarning() << "file not exists:" << desktop;
        }
    }
    else {
        qWarning() << "not a desktop file:" << desktop;
    }
}

QVariant DBusListener::getMceValue(const QString &key)
{
    QDBusReply<QVariant> reply = mce->call(QDBus::AutoDetect, "get_config", key);
    if (reply.isValid()) {
        return reply.value();
    }
    else {
        return QVariant();
    }
}

void DBusListener::setMceValue(const QString &key, const QVariant &value)
{
    mce->call(QDBus::AutoDetect, "set_config", key, QVariant::fromValue(QDBusVariant(value)));
}

void DBusListener::openPowerMenu()
{
    if (!view) {
        view = SailfishApp::createView();
        QObject::connect(view, SIGNAL(destroyed()), this, SLOT(viewDestroyed()));
        view->setTitle("Powermenu 2");

        QColor color;
        color.setRedF(0.0);
        color.setGreenF(0.0);
        color.setBlueF(0.0);
        color.setAlphaF(0.0);
        view->setColor(color);

        view->setClearBeforeRendering(true);

        view->rootContext()->setContextProperty("view", view);
        view->rootContext()->setContextProperty("Flashlight", flashlight);
        view->rootContext()->setContextProperty("Screenshot", screenshot);

        view->setSource(QUrl::fromLocalFile("/usr/share/powermenu2/qml/dialog.qml"));
    }
    view->create();

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(view->handle(), QLatin1String("CATEGORY"), "notification");

    view->showFullScreen();
}

void DBusListener::powerButtonTrigger(const QString &triggerName)
{
    qDebug() << "powerButtonTrigger" << triggerName;
    if (triggerName == "powermenu2") {
        openPowerMenu();
    }
    else if (triggerName == "screenshot") {
        screenshot->save();
    }
    else if (triggerName == "flashlight") {
        flashlight->toggle();
    }
    else if (triggerName.startsWith("event")) {
        MGConfItem shortcut(QString("/apps/powermenu/applicationShortcut%1").arg(triggerName.mid(5)));
        if (!shortcut.value().isNull()) {
            openDesktop(shortcut.value().toString());
        }
    }
}

void DBusListener::viewDestroyed()
{
    qDebug() << "viewDestroyed";
    view = NULL;
}
