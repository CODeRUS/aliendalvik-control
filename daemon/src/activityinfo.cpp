#include "activityinfo.h"
#include "binderinterfaceabstract.h"
#include "parcel.h"
#include "windowlayout.h"

ActivityInfo::ActivityInfo(Parcel *parcel, const char *loggingCategoryName)
    : ComponentInfo(parcel, loggingCategoryName)
{
    theme = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "theme:" << theme;
    launchMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "launchMode:" << launchMode;
    documentLaunchMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "documentLaunchMode:" << documentLaunchMode;
    permission = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "permission:" << permission;
    taskAffinity = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "taskAffinity:" << taskAffinity;
    targetActivity = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "targetActivity:" << targetActivity;
    flags = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "flags:" << flags;
    screenOrientation = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "screenOrientation:" << screenOrientation;
    configChanges = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "configChanges:" << configChanges;
    softInputMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "softInputMode:" << softInputMode;
    uiOptions = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "uiOptions:" << uiOptions;
    parentActivityName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "parentActivityName:" << parentActivityName;
    persistableMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "persistableMode:" << persistableMode;
    maxRecents = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "maxRecents:" << maxRecents;
    lockTaskLaunchMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "lockTaskLaunchMode:" << lockTaskLaunchMode;
    const int haveWindowLayout = parcel->readInt();
    if (haveWindowLayout == 1) {
        qCDebug(logging) << Q_FUNC_INFO << "Have window layout!" << haveWindowLayout;
        windowLayout = new WindowLayout(parcel);
    }
    resizeMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "resizeMode:" << resizeMode;
    requestedVrComponent = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "requestedVrComponent:" << requestedVrComponent;
    rotationAnimation = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "rotationAnimation:" << rotationAnimation;
    colorMode = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "colorMode:" << colorMode;
    maxAspectRatio = parcel->readFloat();
    qCDebug(logging) << Q_FUNC_INFO << "maxAspectRatio:" << maxAspectRatio;
}

ActivityInfo::~ActivityInfo()
{
    if (windowLayout) {
        delete windowLayout;
        windowLayout = nullptr;
    }
}
