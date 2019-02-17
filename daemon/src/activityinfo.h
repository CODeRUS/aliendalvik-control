#ifndef ACTIVITYINFO_H
#define ACTIVITYINFO_H

#include "binderinterfaceabstract.h"
#include "componentinfo.h"

#include <QDebug>

class Parcel;
class WindowLayout;
class ActivityInfo : public ComponentInfo
{
public:
    ActivityInfo(Parcel *parcel);
    virtual ~ActivityInfo();

    int theme = -1;
    int launchMode = -1;
    int documentLaunchMode = -1;
    QString permission;
    QString taskAffinity;
    QString targetActivity;
    int flags = -1;
    int screenOrientation = -1;
    int configChanges = -1;
    int softInputMode = -1;
    int uiOptions = -1;
    QString parentActivityName;
    int persistableMode = -1;
    int maxRecents = -1;
    int lockTaskLaunchMode = -1;
    WindowLayout *windowLayout = nullptr;
    int resizeMode = -1;
    QString requestedVrComponent;
    int rotationAnimation = -1;
    int colorMode = -1;
    float maxAspectRatio = -1;
};

#endif // ACTIVITYINFO_H
