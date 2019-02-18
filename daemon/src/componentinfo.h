#ifndef COMPONENTINFO_H
#define COMPONENTINFO_H

#include "packageiteminfo.h"

class Parcel;
class ApplicationInfo;
class ComponentInfo : public PackageItemInfo
{
public:
    ComponentInfo(Parcel *parcel, const char *loggingCategoryName = LOGGING(ComponentInfo)".parcel");
    virtual ~ComponentInfo();

    bool hasApplicationInfo = false;

    ApplicationInfo *applicationInfo = nullptr;

    QString processName;
    QString splitName;
    int descriptionRes = -1;
    bool enabled = false;
    bool exported = false;
    bool encryptionAware = false;
    bool directBootAware = false;
};

#endif // COMPONENTINFO_H
