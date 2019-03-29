#ifndef PACKAGEITEMINFO_H
#define PACKAGEITEMINFO_H

#include "../common/src/loggingclasswrapper.h"

#include <QString>

class Parcel;
class PackageItemInfo : public LoggingClassWrapper
{
public:
    PackageItemInfo(Parcel *parcel, const char *loggingCategoryName = LOGGING(PackageItemInfo)".parcel");

    QString name;
    QString packageName;

    int labelRes = -1;

    int icon = -1;
    int logo = -1;
    int banner = -1;
    int showUserIcon = -1;
};

#endif // PACKAGEITEMINFO_H
