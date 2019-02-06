#include "mediatransfer.h"

#include <QDebug>

MediaTransfer::MediaTransfer(QObject *parent) :
    MediaTransferInterface(parent)
{
    _iface = new QDBusInterface("org.coderus.aliendalvikcontrol", "/", "org.coderus.aliendalvikcontrol",
                                QDBusConnection::sessionBus(), this);
}

MediaTransfer::~MediaTransfer()
{

}

bool MediaTransfer::cancelEnabled() const
{
    return false;
}

QString MediaTransfer::displayName() const
{
    return QString("Android");
}

bool MediaTransfer::restartEnabled() const
{
    return false;
}

QUrl MediaTransfer::serviceIcon() const
{
    return QUrl();
}

void MediaTransfer::shareFile(const QString &filename, const QString mimetype)
{
    if (_iface) {
        _iface->call(QDBus::NoBlock, "shareFile", filename, mimetype);
    }
}

void MediaTransfer::shareText(const QString &data)
{
    if (_iface) {
        _iface->call(QDBus::NoBlock, "shareText", data);
    }
}

void MediaTransfer::cancel()
{
    setStatus(MediaTransferInterface::TransferCanceled);
}

void MediaTransfer::start()
{
    QUrl url = mediaItem()->value(MediaItem::Url).toUrl();
    if (!url.isValid()) {
        QString mimeType = mediaItem()->value(MediaItem::MimeType).toString();
        QString content = mediaItem()->value(MediaItem::ContentData).toString();

        if (mimeType == "text/vcard") {
            QFile tmp(QString("/home/nemo/%1.vcf").arg(QDateTime::currentMSecsSinceEpoch()));
            if (tmp.open(QFile::WriteOnly)) {
                tmp.write(content.toUtf8());
                tmp.close();

                shareFile("file://" + tmp.fileName(), mimeType);
            }
        }
        else if (mimeType == "text/x-url") {
            shareText(mediaItem()->value(MediaItem::UserData).toMap().value("status").toString());
        }
        else {
            shareText(content);
        }
    }
    else {
        QMimeDatabase db;
        QMimeType mimeType = db.mimeTypeForFile(url.toString());

        shareFile(url.toString(), mimeType.name());
    }
}
