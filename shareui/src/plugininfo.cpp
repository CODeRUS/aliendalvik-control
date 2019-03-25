#include "plugininfo.h"

PluginInfo::PluginInfo(): m_ready(false)
{
}

PluginInfo::~PluginInfo()
{

}

QList<TransferMethodInfo> PluginInfo::info() const
{
    return m_infoList;
}

void PluginInfo::query()
{
    TransferMethodInfo info;

    info.displayName     = QStringLiteral("Android");
    info.methodId        = QStringLiteral("AliendalvikSharePlugin");
    info.shareUIPath     = QStringLiteral("/usr/share/nemo-transferengine/plugins/AliendalvikShare.qml");
    info.capabilitities  = QStringList({QStringLiteral("*")});
    info.accountIcon     = QStringLiteral("image://theme/icon-m-share-aliendalvik");
    m_infoList.clear();
    m_infoList << info;

    m_ready = true;
    emit infoReady();
}

bool PluginInfo::ready() const
{
    return m_ready;
}
