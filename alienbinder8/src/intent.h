#ifndef INTENT_H
#define INTENT_H

#include "../common/src/loggingclasswrapper.h"
#include "parcelable.h"

#include <QMetaType>
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
class Intent : public Parcelable, public LoggingClassWrapper
{
public:
    Intent(const char *loggingCategoryName = LOGGING(Intent)".parcel");
    Intent(const Parcel *parcel, const char *loggingCategoryName = LOGGING(Intent)".parcel");
    Intent(const Intent &other);
    ~Intent();

    void writeToParcel(Parcel *parcel) const override;

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

Q_DECLARE_METATYPE(Intent)

QDebug operator<<(QDebug dbg, const Intent &intent);

#endif // INTENT_H
