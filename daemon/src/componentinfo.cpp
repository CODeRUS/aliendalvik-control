#include "applicationinfo.h"
#include "binderinterfaceabstract.h"
#include "componentinfo.h"
#include "parcel.h"

ComponentInfo::ComponentInfo(Parcel *parcel, const char *loggingCategoryName)
    : PackageItemInfo(parcel, loggingCategoryName)
{
    hasApplicationInfo = parcel->readBoolean();
    qCDebug(logging) << Q_FUNC_INFO << "hasApplicationInfo:" << hasApplicationInfo;
    if (hasApplicationInfo) {
//        applicationInfo = ApplicationInfo.CREATOR.createFromParcel(source);
        applicationInfo = new ApplicationInfo(parcel);
    }
    processName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "processName:" << processName;
    splitName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "splitName:" << splitName;
    descriptionRes = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "descriptionRes:" << descriptionRes;
    enabled = parcel->readBoolean();
    qCDebug(logging) << Q_FUNC_INFO << "enabled:" << enabled;
    exported = parcel->readBoolean();
    qCDebug(logging) << Q_FUNC_INFO << "exported:" << exported;
    directBootAware = parcel->readBoolean();
    qCDebug(logging) << Q_FUNC_INFO << "directBootAware:" << directBootAware;
    encryptionAware = directBootAware;

}

ComponentInfo::~ComponentInfo()
{
    if (applicationInfo) {
        delete applicationInfo;
        applicationInfo = nullptr;
    }
}
