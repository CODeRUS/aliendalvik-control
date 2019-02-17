#ifndef PARCELABLE_H
#define PARCELABLE_H

#include <QString>

class Parcel;
class Parcelable
{
public:
    Parcelable(const QString &creator);

    QString creator;

    virtual void writeToParcel(Parcel *parcel) const = 0;
};

#endif // PARCELABLE_H
