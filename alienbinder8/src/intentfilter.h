#ifndef INTENTFILTER_H
#define INTENTFILTER_H

#include "../common/src/loggingclasswrapper.h"

#include <QStringList>


class Parcel;
class IntentFilter : public LoggingClassWrapper
{
public:
    IntentFilter(Parcel *parcel, const char *loggingCategoryName = LOGGING(IntentFilter)".parcel");

    QStringList mActions;
    QStringList mCategories;
    QStringList mDataSchemes;
    QStringList mDataTypes;

//    int N = parcel->readInt();
//    if (N > 0) {
//        mDataSchemeSpecificParts = new ArrayList<PatternMatcher>(N);
//        for (int i=0; i<N; i++) {
//            mDataSchemeSpecificParts.add(new PatternMatcher(source));
//        }
//    }
//    N = parcel->readInt();
//    if (N > 0) {
//        mDataAuthorities = new ArrayList<AuthorityEntry>(N);
//        for (int i=0; i<N; i++) {
//            mDataAuthorities.add(new AuthorityEntry(source));
//        }
//    }
//    N = parcel->readInt();
//    if (N > 0) {
//        mDataPaths = new ArrayList<PatternMatcher>(N);
//        for (int i=0; i<N; i++) {
//            mDataPaths.add(new PatternMatcher(source));
//        }
//    }
//    mPriority = parcel->readInt();
//    mHasPartialTypes = parcel->readInt() > 0;
//    setAutoVerify(parcel->readInt() > 0);
//    setVisibilityToInstantApp(parcel->readInt());
};

#endif // INTENTFILTER_H
