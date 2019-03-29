#ifndef SAILFISHBROWSER_H
#define SAILFISHBROWSER_H

#include <QObject>

class QQmlEngine;
class QJSEngine;
class SailfishBrowser : public QObject
{
    Q_OBJECT
public:
    static SailfishBrowser *GetInstance(QObject *parent = nullptr);
    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *);

    explicit SailfishBrowser(QObject *parent = nullptr);

public slots:
    void openUrl(const QString &url);
};

#endif // SAILFISHBROWSER_H
