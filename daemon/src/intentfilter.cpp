#include "binderinterfaceabstract.h"
#include "intentfilter.h"

#include <QDebug>

IntentFilter::IntentFilter(Parcel *parcel)
{
    const int length = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "mActions length:" << length;
//    mActions = parcel->readStringList();
//    qDebug() << Q_FUNC_INFO << "mActions:" << mActions;
//    if (parcel->readInt() != 0) {
//        mCategories = parcel->readStringList();
//        qDebug() << Q_FUNC_INFO << "mCategories:" << mActions;
//    }
//    if (parcel->readInt() != 0) {
//        mDataSchemes = parcel->readStringList();
//        qDebug() << Q_FUNC_INFO << "mDataSchemes:" << mActions;
//    }
//    if (parcel->readInt() != 0) {
//        mDataTypes = parcel->readStringList();
//        qDebug() << Q_FUNC_INFO << "mDataTypes:" << mActions;
//    }
//    const int N = parcel->readInt();
//    qDebug() << Q_FUNC_INFO << "have mDataSchemeSpecificParts!";
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
}
