#ifndef TRANSFERIFACE_H
#define TRANSFERIFACE_H

#include <TransferEngine-qt5/transferplugininterface.h>
#include <TransferEngine-qt5/transferplugininfo.h>
#include <TransferEngine-qt5/transfermethodinfo.h>
#include <TransferEngine-qt5/mediatransferinterface.h>

class SharePlugin : public QObject, public TransferPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "aliendalvik.transfer.plugin")
    Q_INTERFACES(TransferPluginInterface)
public:
    SharePlugin();
    ~SharePlugin();

    QString pluginId() const;
    bool enabled() const;
    TransferPluginInfo *infoObject();
    MediaTransferInterface *transferObject();
};

#endif // TRANSFERIFACE_H
