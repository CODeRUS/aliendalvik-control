#ifndef INTENT_H
#define INTENT_H

#include <QString>
#include <QVariantHash>

enum {
    USER_NULL = -10000,
    USER_CURRENT_OR_SELF = -3,
    USER_CURRENT = -2,
    USER_ALL = -1,
    USER_OWNER = 0
};

class Parcel;
class Intent
{
public:
    void writeToParcel(Parcel *parcel);

    QString action;
    QString data;
    QString type;

    int flags = 0;

    QString package;
    QString classPackage;
    QString className;

    int contentUserHint = USER_CURRENT;

    QVariantHash extras;
};

#endif // INTENT_H
