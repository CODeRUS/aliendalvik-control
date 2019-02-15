#ifndef PACKAGEITEMINFO_H
#define PACKAGEITEMINFO_H

#include <qstring.h>

class Parcel;
class PackageItemInfo
{
public:
    PackageItemInfo(Parcel *parcel);

    QString name;
    QString packageName;

    int labelRes = -1;

    int icon = -1;
    int logo = -1;
    int banner = -1;
    int showUserIcon = -1;
};

#endif // PACKAGEITEMINFO_H
