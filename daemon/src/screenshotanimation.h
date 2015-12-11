#ifndef SCREENSHOTANIMATION_H
#define SCREENSHOTANIMATION_H

#include <QObject>
#include <QQuickView>
#include <QtQml>
#include <qpa/qplatformnativeinterface.h>
#include <QGuiApplication>

class ScreenshotAnimation : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotAnimation(QObject *parent = 0);

    Q_INVOKABLE void deleteView(QQuickView *view);

public slots:
    void showScreenshot(const QString &path);

private slots:
    void onViewDestroyed();
    void onViewClosing(QQuickCloseEvent *);

};

#endif // SCREENSHOTANIMATION_H
