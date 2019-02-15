#include "resolveinfo.h"
#include "activityinfo.h"
#include "intentfilter.h"

#include <QDebug>

ResolveInfo::ResolveInfo(Parcel *parcel)
{
    typeComponentInfo = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "Info source:" << typeComponentInfo;
    switch (typeComponentInfo) {
    case ComponentInfoActivityInfo:
        activityInfo = new ActivityInfo(parcel);
        break;
    case ComponentInfoServiceInfo:
        break;
    case ComponentInfoProviderInfo:
        break;
    default:
        qCritical() << Q_FUNC_INFO << "Missing ComponentInfo!";
        return;
    }

    const int haveIntentFilter = parcel->readInt();
    if (haveIntentFilter != 0) {
        qDebug() << Q_FUNC_INFO << "have intent filter!" << haveIntentFilter;
        intentFilter = new IntentFilter(parcel);
    }
    priority = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "priority:" << priority;
    preferredOrder = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "preferredOrder:" << preferredOrder;
    match = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "match:" << match;
    specificIndex = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "specificIndex:" << specificIndex;
    labelRes = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "labelRes:" << labelRes;

//    nonLocalizedLabel
//            = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source);
//    icon = parcel->readInt();
//    resolvePackageName = parcel->readString();
//    targetUserId = parcel->readInt();
//    system = parcel->readInt() != 0;
//    noResourceId = parcel->readInt() != 0;
//    iconResourceId = parcel->readInt();
//    handleAllWebDataURI = parcel->readInt() != 0;
//    instantAppAvailable = isInstantAppAvailable = parcel->readInt() != 0;
}

ResolveInfo::~ResolveInfo()
{
    if (activityInfo) {
        delete activityInfo;
    }
}

ComponentInfo *ResolveInfo::getComponentInfo()
{
    if (activityInfo) {
        return activityInfo;
    }
    qCritical() << Q_FUNC_INFO << "Missing ComponentInfo!";
    return nullptr;
}
