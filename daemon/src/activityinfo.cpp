#include "activityinfo.h"
#include "binderinterfaceabstract.h"
#include "windowlayout.h"

#include <QDebug>
ActivityInfo::ActivityInfo(Parcel *parcel)
    : ComponentInfo(parcel)
{
    theme = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "theme:" << theme;
    launchMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "launchMode:" << launchMode;
    documentLaunchMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "documentLaunchMode:" << documentLaunchMode;
    permission = parcel->readString();
    qDebug() << Q_FUNC_INFO << "permission:" << permission;
    taskAffinity = parcel->readString();
    qDebug() << Q_FUNC_INFO << "taskAffinity:" << taskAffinity;
    targetActivity = parcel->readString();
    qDebug() << Q_FUNC_INFO << "targetActivity:" << targetActivity;
    flags = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "flags:" << flags;
    screenOrientation = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "screenOrientation:" << screenOrientation;
    configChanges = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "configChanges:" << configChanges;
    softInputMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "softInputMode:" << softInputMode;
    uiOptions = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "uiOptions:" << uiOptions;
    parentActivityName = parcel->readString();
    qDebug() << Q_FUNC_INFO << "parentActivityName:" << parentActivityName;
    persistableMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "persistableMode:" << persistableMode;
    maxRecents = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "maxRecents:" << maxRecents;
    lockTaskLaunchMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "lockTaskLaunchMode:" << lockTaskLaunchMode;
    const int haveWindowLayout = parcel->readInt();
    if (haveWindowLayout == 1) {
        qDebug() << Q_FUNC_INFO << "Have window layout!" << haveWindowLayout;
        windowLayout = new WindowLayout(parcel);
    }
    resizeMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "resizeMode:" << resizeMode;
    requestedVrComponent = parcel->readString();
    qDebug() << Q_FUNC_INFO << "requestedVrComponent:" << requestedVrComponent;
    rotationAnimation = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "rotationAnimation:" << rotationAnimation;
    colorMode = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "colorMode:" << colorMode;
    maxAspectRatio = parcel->readFloat();
    qDebug() << Q_FUNC_INFO << "maxAspectRatio:" << maxAspectRatio;
}

ActivityInfo::~ActivityInfo()
{
    if (windowLayout) {
        delete windowLayout;
        windowLayout = nullptr;
    }
}
