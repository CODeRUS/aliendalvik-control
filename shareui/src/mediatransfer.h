#ifndef MEDIATRANSFER_H
#define MEDIATRANSFER_H

#include <TransferEngine-qt5/mediatransferinterface.h>

class MediaTransfer : public MediaTransferInterface
{
    Q_OBJECT
public:
    explicit MediaTransfer(QObject * parent = nullptr);
    virtual ~MediaTransfer();

    bool cancelEnabled() const;
    QString displayName() const;
    bool restartEnabled() const;
    QUrl serviceIcon() const;

private:
    void shareFile(const QString &filename, const QString mimetype);
    void shareText(const QString &data);

public slots:
    void cancel();
    void start();

};

#endif // MEDIATRANSFER_H
