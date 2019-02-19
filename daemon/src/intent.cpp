#include "intent.h"
#include "parcel.h"

Intent::Intent(const char *loggingCategoryName)
    : Parcelable(QStringLiteral("android.content.Intent"))
    , LoggingClassWrapper(loggingCategoryName)
{

}

Intent::Intent(const Intent &other)
    : Parcelable(other.creator)
    , LoggingClassWrapper(other.logging.categoryName())
{
    action = other.action;
    data = other.data;
    type = other.type;

    flags = other.flags;

    package = other.package;
    classPackage = other.classPackage;
    className = other.className;

    contentUserHint = other.contentUserHint;

    extras = other.extras;
}

Intent::~Intent()
{

}

void Intent::writeToParcel(Parcel *parcel) const
{
    if (!parcel) {
        qCCritical(logging) << Q_FUNC_INFO << "Null Parcel!";
        return;
    }

    parcel->writeString(action);

    // Uri.writeToParcel
    if (data.isNull()) {
        parcel->writeInt(0); // NULL_TYPE_ID
    } else {
        parcel->writeInt(1); // TYPE_ID UriString = 1
        parcel->writeString(data);
    }

    parcel->writeString(type);

    parcel->writeInt(flags);

    parcel->writeString(package);

    // ComponentName.writeToParcel
    if (classPackage.isEmpty() && className.isEmpty()) {
        parcel->writeString(QString());
    } else {
        parcel->writeString(classPackage);
        if (className.startsWith(QChar(u'.'))) {
            parcel->writeString(classPackage + className);
        } else {
            parcel->writeString(className);
        }
    }

    parcel->writeInt(0); // mSourceBounds disable
    parcel->writeInt(0); // mCategories disable
    parcel->writeInt(0); // mSelector disable
    parcel->writeInt(0); // mClipData disable

    parcel->writeInt(contentUserHint);

    if (extras.isEmpty()) {
        parcel->writeInt(-1);
    } else {
        parcel->writeBundle(extras);
    }
}

QDebug operator<<(QDebug dbg, const Intent &intent)
{
    dbg << "Action:" << intent.action;
    return dbg.maybeSpace();
}
