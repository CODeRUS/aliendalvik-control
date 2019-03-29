#ifndef INTENTSENDER_H
#define INTENTSENDER_H

#include "../common/src/loggingclasswrapper.h"
#include "parcelable.h"

#include <gbinder.h>

class Parcel;
class IntentSender : public Parcelable, public LoggingClassWrapper
{
public:
    IntentSender(GBinderRemoteObject *target = nullptr, const char *loggingCategoryName = LOGGING(Intent)".parcel");
    IntentSender(const Parcel *parcel, const char *loggingCategoryName = LOGGING(Intent)".parcel");
    IntentSender(const IntentSender &other);
    virtual ~IntentSender();

    GBinderRemoteObject *target() const;
    void writeToParcel(Parcel *parcel) const override;

private:
    GBinderRemoteObject *m_target = nullptr;
};

Q_DECLARE_METATYPE(IntentSender)

QDebug operator<<(QDebug dbg, const IntentSender &intentSender);

#endif // INTENTSENDER_H
