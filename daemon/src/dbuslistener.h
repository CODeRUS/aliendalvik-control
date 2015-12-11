#ifndef DBUSLISTENER_H
#define DBUSLISTENER_H

#include <QObject>

#include <QtDBus>
#include <QQuickView>

#include "../libpowermenutools/src/flashlightcontrol.h"
#include "../libpowermenutools/src/screenshotcontrol.h"

#include "screenshotanimation.h"

#define MCE_SERVICE "com.nokia.mce"

# define MCE_GCONF_POWERKEY_PATH       "/system/osso/dsm/powerkey"

# define MCE_GCONF_POWERKEY_MODE                 MCE_GCONF_POWERKEY_PATH "/mode"
# define MCE_GCONF_POWERKEY_BLANKING_MODE        MCE_GCONF_POWERKEY_PATH "/blanking_mode"
# define MCE_GCONF_POWERKEY_PS_OVERRIDE_COUNT    MCE_GCONF_POWERKEY_PATH "/ps_override_count"
# define MCE_GCONF_POWERKEY_PS_OVERRIDE_TIMEOUT  MCE_GCONF_POWERKEY_PATH "/ps_override_timeout"
# define MCE_GCONF_POWERKEY_LONG_PRESS_DELAY     MCE_GCONF_POWERKEY_PATH "/long_press_delay"
# define MCE_GCONF_POWERKEY_DOUBLE_PRESS_DELAY   MCE_GCONF_POWERKEY_PATH "/double_press_delay"
# define MCE_GCONF_POWERKEY_ACTIONS_SINGLE_ON    MCE_GCONF_POWERKEY_PATH "/actions_single_on"
# define MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_ON    MCE_GCONF_POWERKEY_PATH "/actions_double_on"
# define MCE_GCONF_POWERKEY_ACTIONS_LONG_ON      MCE_GCONF_POWERKEY_PATH "/actions_long_on"
# define MCE_GCONF_POWERKEY_ACTIONS_SINGLE_OFF   MCE_GCONF_POWERKEY_PATH "/actions_single_off"
# define MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_OFF   MCE_GCONF_POWERKEY_PATH "/actions_double_off"
# define MCE_GCONF_POWERKEY_ACTIONS_LONG_OFF     MCE_GCONF_POWERKEY_PATH "/actions_long_off"
# define MCE_GCONF_POWERKEY_DBUS_ACTION1         MCE_GCONF_POWERKEY_PATH "/dbus_action1"
# define MCE_GCONF_POWERKEY_DBUS_ACTION2         MCE_GCONF_POWERKEY_PATH "/dbus_action2"
# define MCE_GCONF_POWERKEY_DBUS_ACTION3         MCE_GCONF_POWERKEY_PATH "/dbus_action3"
# define MCE_GCONF_POWERKEY_DBUS_ACTION4         MCE_GCONF_POWERKEY_PATH "/dbus_action4"
# define MCE_GCONF_POWERKEY_DBUS_ACTION5         MCE_GCONF_POWERKEY_PATH "/dbus_action5"
# define MCE_GCONF_POWERKEY_DBUS_ACTION6         MCE_GCONF_POWERKEY_PATH "/dbus_action6"

#define MCE_REQUEST_PATH "/com/nokia/mce/request"
#define MCE_REQUEST_IFACE "com.nokia.mce.request"

#define MCE_SIGNAL_PATH "/com/nokia/mce/signal"
#define MCE_SIGNAL_IFACE "com.nokia.mce.signal"

class DBusListener : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.powermenu")
public:
    explicit DBusListener(QObject *parent = 0);

    Q_PROPERTY(QString action1 READ getAction1 WRITE setAction1 FINAL)
    Q_SCRIPTABLE QString getAction1();
    Q_SCRIPTABLE void setAction1(const QString &action);

    Q_PROPERTY(QString action2 READ getAction2 WRITE setAction2 FINAL)
    Q_SCRIPTABLE QString getAction2();
    Q_SCRIPTABLE void setAction2(const QString &action);

    Q_PROPERTY(QString action3 READ getAction3 WRITE setAction3 FINAL)
    Q_SCRIPTABLE QString getAction3();
    Q_SCRIPTABLE void setAction3(const QString &action);

    Q_PROPERTY(QString action4 READ getAction4 WRITE setAction4 FINAL)
    Q_SCRIPTABLE QString getAction4();
    Q_SCRIPTABLE void setAction4(const QString &action);

    Q_PROPERTY(QString action5 READ getAction5 WRITE setAction5 FINAL)
    Q_SCRIPTABLE QString getAction5();
    Q_SCRIPTABLE void setAction5(const QString &action);

    Q_PROPERTY(QString action6 READ getAction6 WRITE setAction6 FINAL)
    Q_SCRIPTABLE QString getAction6();
    Q_SCRIPTABLE void setAction6(const QString &action);

    Q_PROPERTY(int longPressDelay READ getLongPressDelay WRITE setLongPressDelay FINAL)
    Q_SCRIPTABLE int getLongPressDelay();
    Q_SCRIPTABLE void setLongPressDelay(int msecs);

    Q_PROPERTY(QString longPressActionOn READ getLongPressActionOn WRITE setLongPressActionOn FINAL)
    Q_SCRIPTABLE QString getLongPressActionOn();
    Q_SCRIPTABLE void setLongPressActionOn(const QString &action);

    Q_PROPERTY(QString longPressActionOff READ getLongPressActionOff WRITE setLongPressActionOff FINAL)
    Q_SCRIPTABLE QString getLongPressActionOff();
    Q_SCRIPTABLE void setLongPressActionOff(const QString &action);

    Q_PROPERTY(int doublePressDelay READ getDoublePressDelay WRITE setDoublePressDelay FINAL)
    Q_SCRIPTABLE int getDoublePressDelay();
    Q_SCRIPTABLE void setDoublePressDelay(int msecs);

    Q_PROPERTY(QString doublePressActionOn READ getDoublePressActionOn WRITE setDoublePressActionOn FINAL)
    Q_SCRIPTABLE QString getDoublePressActionOn();
    Q_SCRIPTABLE void setDoublePressActionOn(const QString &action);

    Q_PROPERTY(QString doublePressActionOff READ getDoublePressActionOff WRITE setDoublePressActionOff FINAL)
    Q_SCRIPTABLE QString getDoublePressActionOff();
    Q_SCRIPTABLE void setDoublePressActionOff(const QString &action);

    Q_PROPERTY(QString shortPressActionOn READ getShortPressActionOn WRITE setShortPressActionOn FINAL)
    Q_SCRIPTABLE QString getShortPressActionOn();
    Q_SCRIPTABLE void setShortPressActionOn(const QString &action);

    Q_PROPERTY(QString shortPressActionOff READ getShortPressActionOff WRITE setShortPressActionOff FINAL)
    Q_SCRIPTABLE QString getShortPressActionOff();
    Q_SCRIPTABLE void setShortPressActionOff(const QString &action);

    Q_SCRIPTABLE void resetToDefaults();

    Q_SCRIPTABLE void openDesktop(const QString &desktop);

    Q_SCRIPTABLE void openPowerMenu();

    Q_SCRIPTABLE void showScreenshot(const QString &path);

public slots:
    void startService();

private:
    QVariant getMceValue(const QString &key);
    void setMceValue(const QString &key, const QVariant &value);

    QQuickView *view;
    QDBusInterface *mce;

    ScreenshotAnimation *anim;

private slots:
    void powerButtonTrigger(const QString &triggerName);
    void viewDestroyed();
};

#endif // DBUSLISTENER_H
