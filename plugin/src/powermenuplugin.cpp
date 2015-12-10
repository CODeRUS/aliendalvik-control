#include "src/powermenuplugin.h"
#include "src/togglesmodel.h"
#include "src/fileutils.h"
#include "src/translator.h"
#include "../libpowermenutools/src/screenshotcontrol.h"
#include "../libpowermenutools/src/flashlightcontrol.h"

#include <qqml.h>

static QObject *screenshot_singleton_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return ScreenshotControl::GetInstance();
}

static QObject *flashlight_singleton_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return FlashlightControl::GetInstance();
}

void PowermenuPlugin::registerTypes(const char *uri)
{
    // @uri org.coderus.powermenu
    qmlRegisterType<TogglesModel>("org.coderus.powermenu", 1, 0, "TogglesModel");
    qmlRegisterType<FileUtils>("org.coderus.powermenu", 1, 0, "FileUtils");
    qmlRegisterSingletonType<ScreenshotControl>("org.coderus.powermenu", 1, 0, "Screenshot", screenshot_singleton_provider);
    qmlRegisterSingletonType<FlashlightControl>("org.coderus.powermenu", 1, 0, "Flashlight", flashlight_singleton_provider);
    qmlRegisterUncreatableType<Translator>("org.coderus.powermenu", 1, 0, "Translator", "Translator loaded by import");
}

void PowermenuPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    new Translator(engine);
}
