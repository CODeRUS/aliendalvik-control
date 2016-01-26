#ifndef MEDIATRANSFER_H
#define MEDIATRANSFER_H

#include <TransferEngine-qt5/mediatransferinterface.h>
#include <TransferEngine-qt5/mediaitem.h>

#include <QMimeDatabase>
#include <QMimeType>
#include <QFile>
#include <QDateTime>
#include <QtDBus>
#include <QVariant>

class MediaTransfer : public MediaTransferInterface
{
    Q_OBJECT
public:
    MediaTransfer(QObject * parent = 0);
    ~MediaTransfer();

    bool	cancelEnabled() const;
    QString	displayName() const;
    bool	restartEnabled() const;
    QUrl	serviceIcon() const;

private:
    void shareFile(const QString &filename, const QString mimetype);
    void shareText(const QString &data);

    QDBusInterface *_iface;

public slots:
    void	cancel();
    void	start();

};

#endif // MEDIATRANSFER_H
