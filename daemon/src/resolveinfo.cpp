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
        qDebug() << Q_FUNC_INFO << "activityInfo:" << activityInfo;
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

    // TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source)
    const int kind = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "kind:" << kind;
    const QString string = parcel->readString();
    qDebug() << Q_FUNC_INFO << "string:" << string;

    icon = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "icon:" << icon;
    resolvePackageName = parcel->readString();
    qDebug() << Q_FUNC_INFO << "resolvePackageName:" << resolvePackageName;
    targetUserId = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "targetUserId:" << targetUserId;
    system = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "system:" << system;
    noResourceId = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "noResourceId:" << noResourceId;
    iconResourceId = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "iconResourceId:" << iconResourceId;
    handleAllWebDataURI = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "handleAllWebDataURI:" << handleAllWebDataURI;
    instantAppAvailable = parcel->readBoolean();
    qDebug() << Q_FUNC_INFO << "instantAppAvailable:" << instantAppAvailable;
    isInstantAppAvailable = instantAppAvailable;
    qDebug() << Q_FUNC_INFO << "isInstantAppAvailable:" << isInstantAppAvailable;
}

ResolveInfo::~ResolveInfo()
{
    if (activityInfo) {
        delete activityInfo;
        activityInfo = nullptr;
    }

    if (intentFilter) {
        delete intentFilter;
        intentFilter = nullptr;
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
