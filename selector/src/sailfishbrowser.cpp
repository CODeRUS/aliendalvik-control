#include "sailfishbrowser.h"

#include <contentaction5/contentaction.h>

#include <QQmlEngine>

static SailfishBrowser *s_instance = nullptr;

SailfishBrowser *SailfishBrowser::GetInstance(QObject *parent)
{
    if (!s_instance) {
        s_instance = new SailfishBrowser(parent);
    }
    return s_instance;
}

QObject *SailfishBrowser::qmlSingleton(QQmlEngine *engine, QJSEngine *)
{
    return SailfishBrowser::GetInstance(engine);
}

SailfishBrowser::SailfishBrowser(QObject *parent)
    : QObject(parent)
{

}

void SailfishBrowser::openUrl(const QString &url)
{
    ContentAction::Action action = ContentAction::Action::launcherAction(QStringLiteral("open-url"), {url});
    action.triggerAndWait();
}
