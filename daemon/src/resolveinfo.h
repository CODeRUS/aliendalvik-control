#ifndef RESOLVEINFO_H
#define RESOLVEINFO_H

#include "binderinterfaceabstract.h"

enum {
    ComponentInfoActivityInfo = 1,
    ComponentInfoServiceInfo,
    ComponentInfoProviderInfo,
};

class ComponentInfo;
class ActivityInfo;
class IntentFilter;
class ResolveInfo
{
public:
    ResolveInfo(Parcel *parcel);
    ~ResolveInfo();

    int typeComponentInfo = 0;

    ComponentInfo *getComponentInfo();
    ActivityInfo *activityInfo = nullptr;
    IntentFilter *intentFilter = nullptr;

    int priority = -1;
    int preferredOrder = -1;
    int match = -1;
    int specificIndex = -1;
    int labelRes = -1;

//    nonLocalizedLabel
//            = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source);
//    icon = -1;
//    resolvePackageName = parcel->readString();
//    targetUserId = -1;
//    system = -1 != 0;
//    noResourceId = -1 != 0;
//    iconResourceId = -1;
//    handleAllWebDataURI = -1 != 0;
//    instantAppAvailable = isInstantAppAvailable = -1 != 0;
};

#endif // RESOLVEINFO_H
