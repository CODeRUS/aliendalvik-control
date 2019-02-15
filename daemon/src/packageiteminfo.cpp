#include "binderinterfaceabstract.h"
#include "packageiteminfo.h"

#include <QDebug>

PackageItemInfo::PackageItemInfo(Parcel *parcel)
{
    name = parcel->readString();
    qDebug() << Q_FUNC_INFO << "Name:" << name;
    packageName = parcel->readString();
    qDebug() << Q_FUNC_INFO << "Package:" << packageName;

    labelRes = parcel->readInt();

    // TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source)
    const int kind = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "kind:" << kind;
    const QString string = parcel->readString();
    qDebug() << Q_FUNC_INFO << "string:" << string;

    icon = parcel->readInt();
    logo = parcel->readInt();

    parcel->readBundle(); // metaData

    banner = parcel->readInt();
    showUserIcon = parcel->readInt();
}
