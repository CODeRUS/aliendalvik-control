#include "transferiface.h"
#include "plugininfo.h"
#include "mediatransfer.h"

#include <QtPlugin>

SharePlugin::SharePlugin()
{

}

SharePlugin::~SharePlugin()
{

}

QString SharePlugin::pluginId() const
{
    return QLatin1String("AliendalvikSharePlugin");
}

bool SharePlugin::enabled() const
{
    return true;
}

TransferPluginInfo *SharePlugin::infoObject()
{
    return new PluginInfo;
}

MediaTransferInterface *SharePlugin::transferObject()
{
    return new MediaTransfer;
}
