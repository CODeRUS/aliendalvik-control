#include "mediatransfer.h"

#include <QDebug>

#include <TransferEngine-qt5/mediaitem.h>

#include <QMimeDatabase>
#include <QMimeType>
#include <QFile>
#include <QDateTime>
#include <QtDBus>
#include <QVariant>

MediaTransfer::MediaTransfer(QObject *parent) :
    MediaTransferInterface(parent)
{
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
    return QStringLiteral("Android");
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
    QDBusMessage shareFile = QDBusMessage::createMethodCall(QStringLiteral("org.coderus.aliendalvikcontrol"),
                                                            QStringLiteral("/"),
                                                            QStringLiteral("org.coderus.aliendalvikcontrol"),
                                                            QStringLiteral("shareFile"));
    shareFile.setArguments({filename, mimetype});
    QDBusConnection::sessionBus().send(shareFile);
}

void MediaTransfer::shareText(const QString &data)
{
    QDBusMessage shareText = QDBusMessage::createMethodCall(QStringLiteral("org.coderus.aliendalvikcontrol"),
                                                            QStringLiteral("/"),
                                                            QStringLiteral("org.coderus.aliendalvikcontrol"),
                                                            QStringLiteral("shareText"));
    shareText.setArguments({data});
    QDBusConnection::sessionBus().send(shareText);
}

void MediaTransfer::cancel()
{
    setStatus(MediaTransferInterface::TransferCanceled);
}

void MediaTransfer::start()
{
    const QUrl url = mediaItem()->value(MediaItem::Url).toUrl();
    if (!url.isValid()) {
        const QString mimeType = mediaItem()->value(MediaItem::MimeType).toString();
        const QString content = mediaItem()->value(MediaItem::ContentData).toString();

        if (mimeType == QLatin1String("text/vcard")) {
            QFile tmp(QStringLiteral("/home/nemo/.aliendalvik-control-share.vcf"));
            if (tmp.open(QFile::WriteOnly)) {
                tmp.write(content.toUtf8());

                shareFile(tmp.fileName(), mimeType);
            }
        } else if (mimeType == QLatin1String("text/x-url")) {
            const QVariantMap userData = mediaItem()->value(MediaItem::UserData).toMap();
            if (userData.isEmpty()) {
                return;
            }
            const QString data = userData.value(QStringLiteral("status"));
            if (data.isEmpty()) {
                return;
            }
            shareText(data);
        }
        else {
            shareText(content);
        }
    } else {
        const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(url.toString());

        shareFile(url.toString(QUrl::PreferLocalFile), mimeType.name());
    }
}
