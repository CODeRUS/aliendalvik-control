#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <TransferEngine-qt5/transferplugininfo.h>
#include <TransferEngine-qt5/transfermethodinfo.h>

class PluginInfo : public TransferPluginInfo
{
    Q_OBJECT
public:
    PluginInfo();
    ~PluginInfo();
    QList<TransferMethodInfo> info() const;
    void query();
    bool ready() const;

private:
    QList<TransferMethodInfo> m_infoList;
    bool m_ready;

};

#endif // PLUGININFO_H
