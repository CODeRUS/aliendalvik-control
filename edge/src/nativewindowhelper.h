#ifndef NATIVEWINDOWHELPER_H
#define NATIVEWINDOWHELPER_H

#include <QObject>

class QQmlEngine;
class QJSEngine;
class QQuickItem;
class NativeWindowHelper : public QObject
{
    Q_OBJECT
public:
    static QObject *qmlSingleton(QQmlEngine *, QJSEngine *);

public slots:
    void setTouchRegion(QQuickItem *item, bool enabled);

protected:
    explicit NativeWindowHelper(QObject *parent = nullptr);
};

#endif // NATIVEWINDOWHELPER_H
