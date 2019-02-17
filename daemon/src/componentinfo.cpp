#include "applicationinfo.h"
#include "binderinterfaceabstract.h"
#include "componentinfo.h"

#include <QDebug>

ComponentInfo::ComponentInfo(Parcel *parcel)
    : PackageItemInfo(parcel)
{
    hasApplicationInfo = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "hasApplicationInfo:" << hasApplicationInfo;
    if (hasApplicationInfo) {
//        applicationInfo = ApplicationInfo.CREATOR.createFromParcel(source);
        applicationInfo = new ApplicationInfo(parcel);
    }
    processName = parcel->readString();
    qDebug() << Q_FUNC_INFO << "processName:" << processName;
    splitName = parcel->readString();
    qDebug() << Q_FUNC_INFO << "splitName:" << splitName;
    descriptionRes = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "descriptionRes:" << descriptionRes;
    enabled = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "enabled:" << enabled;
    exported = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "exported:" << exported;
    directBootAware = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "directBootAware:" << directBootAware;
    encryptionAware = directBootAware;

}

ComponentInfo::~ComponentInfo()
{
    if (applicationInfo) {
        delete applicationInfo;
        applicationInfo = nullptr;
    }
}
