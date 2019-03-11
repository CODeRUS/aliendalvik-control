#include "intentsender.h"
#include "parcel.h"

IntentSender::IntentSender(GBinderRemoteObject *target, const char *loggingCategoryName)
    : Parcelable(QStringLiteral("android.content.IntentSender"))
    , LoggingClassWrapper(loggingCategoryName)
    , m_target(target)
{

}

IntentSender::IntentSender(const Parcel *parcel, const char *loggingCategoryName)
    : Parcelable(QStringLiteral("android.content.IntentSender"))
    , LoggingClassWrapper(loggingCategoryName)
{
    m_target = parcel->readStrongBinder();
}

IntentSender::IntentSender(const IntentSender &other)
    : Parcelable(other.creator)
    , LoggingClassWrapper(other.logging.categoryName())
{
    m_target = other.target();
}

IntentSender::~IntentSender()
{

}

GBinderRemoteObject *IntentSender::target() const
{
    return m_target;
}

void IntentSender::writeToParcel(Parcel *parcel) const
{
    parcel->writeStrongBinder(m_target);
}

QDebug operator<<(QDebug dbg, const IntentSender &intentSender)
{
    dbg << "IntentSender:" << intentSender.target();
    return dbg.maybeSpace();
}
