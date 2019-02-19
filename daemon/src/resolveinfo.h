#ifndef RESOLVEINFO_H
#define RESOLVEINFO_H

#include "loggingclasswrapper.h"

enum {
    ComponentInfoActivityInfo = 1,
    ComponentInfoServiceInfo,
    ComponentInfoProviderInfo,
};

class ComponentInfo;
class ActivityInfo;
class IntentFilter;
class Parcel;
class ResolveInfo : public LoggingClassWrapper
{
public:
    ResolveInfo(Parcel *parcel, const char *loggingCategoryName = LOGGING(ResolveInfo)".parcel");
    virtual ~ResolveInfo();

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

    int icon = -1;
    QString resolvePackageName;
    int targetUserId = -1;
    bool system = false;
    bool noResourceId = false;
    int iconResourceId = -1;
    bool handleAllWebDataURI = false;
    bool instantAppAvailable = false;
    bool isInstantAppAvailable = false;
};

#endif // RESOLVEINFO_H
