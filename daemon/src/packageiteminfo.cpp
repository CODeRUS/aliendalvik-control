#include "binderinterfaceabstract.h"
#include "packageiteminfo.h"
#include "parcel.h"

PackageItemInfo::PackageItemInfo(Parcel *parcel, const char *loggingCategoryName)
    : LoggingClassWrapper(loggingCategoryName)
{
    name = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "Name:" << loggingCategoryName;
    packageName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "Package:" << packageName;

    labelRes = parcel->readInt();

    // TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source)
    const int kind = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "kind:" << kind;
    const QString string = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "string:" << string;

    icon = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "icon:" << icon;
    logo = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "logo:" << logo;

    parcel->readBundle(); // metaData

    banner = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "banner:" << banner;
    showUserIcon = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "showUserIcon:" << showUserIcon;
}
