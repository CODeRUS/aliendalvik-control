#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include "packageiteminfo.h"

#include <QHash>
#include <QVariant>

class Parcel;
class ApplicationInfo : public PackageItemInfo
{
public:
    ApplicationInfo(Parcel *parcel);

    QString taskAffinity;
    QString permission;
    QString processName;
    QString className;
    int theme = -1;
    int flags = -1;
    int privateFlags = -1;
    int requiresSmallestWidthDp = -1;
    int compatibleWidthLimitDp = -1;
    int largestWidthLimitDp = -1;

//    if (source.readInt() != 0) {
//        storageUuid = new UUID(source.readLong(), source.readLong());
//        volumeUuid = StorageManager.convert(storageUuid);
//    }

    QString scanSourceDir;
    QString scanPublicSourceDir;
    QString sourceDir;
    QString publicSourceDir;
    QStringList splitNames;
    QStringList splitSourceDirs;
    QStringList splitPublicSourceDirs;
    QHash<int, QVariant> splitDependencies;
    QString nativeLibraryDir;
    QString secondaryNativeLibraryDir;
    QString nativeLibraryRootDir;
    bool nativeLibraryRootRequiresIsa;
    QString primaryCpuAbi;
    QString secondaryCpuAbi;
    QStringList resourceDirs;
    QString seInfo;
    QString seInfoUser;
    QStringList sharedLibraryFiles;
    QString dataDir;
    QString deviceProtectedDataDir;
    QString credentialProtectedDataDir;
    int uid = -1;
    int minSdkVersion = -1;
    int targetSdkVersion = -1;
    int versionCode = -1;
    bool enabled = false;
    int enabledSetting = -1;
    int installLocation = -1;
    QString manageSpaceActivityName;
    QString backupAgentName;
    int descriptionRes = -1;
    int uiOptions = -1;
    int fullBackupContent = -1;
    int networkSecurityConfigRes = -1;
    int category = -1;
    int targetSandboxVersion = -1;
    QString classLoaderName;
    QStringList splitClassLoaderNames;
};

#endif // APPLICATIONINFO_H
