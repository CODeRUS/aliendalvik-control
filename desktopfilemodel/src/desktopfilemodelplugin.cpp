#include "desktopfilemodelplugin.h"
#include "desktopfilesortmodel.h"

#include <qqml.h>

void DesktopFileModelPlugin::registerTypes(const char *uri)
{
    // @uri org.coderus.desktopfilemodel
    qmlRegisterType<DesktopFileSortModel>(uri, 1, 0, "DesktopFileSortModel");
}
