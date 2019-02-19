#include "applicationinfo.h"
#include "parcel.h"

ApplicationInfo::ApplicationInfo(Parcel *parcel, const char *loggingCategoryName)
    : PackageItemInfo(parcel, loggingCategoryName)
{
    taskAffinity = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "taskAffinity" << taskAffinity;
    permission = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "permission" << permission;
    processName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "processName" << processName;
    className = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "className" << className;
    theme = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "theme" << theme;
    flags = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "flags" << flags;
    privateFlags = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "privateFlags" << privateFlags;
    requiresSmallestWidthDp = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "requiresSmallestWidthDp" << requiresSmallestWidthDp;
    compatibleWidthLimitDp = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "compatibleWidthLimitDp" << compatibleWidthLimitDp;
    largestWidthLimitDp = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "largestWidthLimitDp" << largestWidthLimitDp;

    const int haveUuidInfo = parcel->readInt();
    if (haveUuidInfo != 0) {
        qCDebug(logging) << Q_FUNC_INFO << "Have UUID INFO!" << haveUuidInfo
                 << parcel->readLong() << parcel->readLong();

//        storageUuid = new UUID(parcel->readLong(), parcel->readLong());
//        volumeUuid = StorageManager.convert(storageUuid);
    }
    scanSourceDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "scanSourceDir" << scanSourceDir;
    scanPublicSourceDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "scanPublicSourceDir" << scanPublicSourceDir;
    sourceDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "sourceDir" << sourceDir;
    publicSourceDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "publicSourceDir" << publicSourceDir;
    splitNames = parcel->readStringList();
    qCDebug(logging) << Q_FUNC_INFO << "splitNames" << splitNames;
    splitSourceDirs = parcel->readStringList();
    qCDebug(logging) << Q_FUNC_INFO << "splitSourceDirs" << splitSourceDirs;
    splitPublicSourceDirs = parcel->readStringList();
    qCDebug(logging) << Q_FUNC_INFO << "splitPublicSourceDirs" << splitPublicSourceDirs;
    splitDependencies = parcel->readSparseArray();
    qCDebug(logging) << Q_FUNC_INFO << "splitDependencies" << splitDependencies;
    nativeLibraryDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "nativeLibraryDir" << nativeLibraryDir;
    secondaryNativeLibraryDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "secondaryNativeLibraryDir" << secondaryNativeLibraryDir;
    nativeLibraryRootDir = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "nativeLibraryRootDir" << nativeLibraryRootDir;
    const int i_nativeLibraryRootRequiresIsa = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "i_nativeLibraryRootRequiresIsa" << i_nativeLibraryRootRequiresIsa;
    if (i_nativeLibraryRootRequiresIsa != 0) {
        nativeLibraryRootRequiresIsa = enabled;

        primaryCpuAbi = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "primaryCpuAbi" << primaryCpuAbi;
        secondaryCpuAbi = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "secondaryCpuAbi" << secondaryCpuAbi;
        resourceDirs = parcel->readStringList();
        qCDebug(logging) << Q_FUNC_INFO << "resourceDirs" << resourceDirs;
        seInfo = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "seInfo" << seInfo;
        seInfoUser = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "seInfoUser" << seInfoUser;
        sharedLibraryFiles = parcel->readStringList();
        qCDebug(logging) << Q_FUNC_INFO << "sharedLibraryFiles" << sharedLibraryFiles;
        dataDir = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "dataDir" << dataDir;
        deviceProtectedDataDir = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "deviceProtectedDataDir" << deviceProtectedDataDir;
        credentialProtectedDataDir = parcel->readString();
        qCDebug(logging) << Q_FUNC_INFO << "credentialProtectedDataDir" << credentialProtectedDataDir;
        uid = parcel->readInt();
        qCDebug(logging) << Q_FUNC_INFO << "uid" << uid;
        minSdkVersion = parcel->readInt();
        qCDebug(logging) << Q_FUNC_INFO << "minSdkVersion" << minSdkVersion;
        targetSdkVersion = parcel->readInt();
        qCDebug(logging) << Q_FUNC_INFO << "targetSdkVersion" << targetSdkVersion;
        versionCode = parcel->readInt();
        qCDebug(logging) << Q_FUNC_INFO << "versionCode" << versionCode;
        const int finita = parcel->readInt();
        qCDebug(logging) << Q_FUNC_INFO << "finita" << finita;
        if (finita == 0) {
            return;
        }
    }

//    enabled = parcel->readBoolean();
    enabled = true;
    qCDebug(logging) << Q_FUNC_INFO << "enabled" << enabled;
    enabledSetting = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "enabledSetting" << enabledSetting;
    installLocation = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "installLocation" << installLocation;
    manageSpaceActivityName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "manageSpaceActivityName" << manageSpaceActivityName;
    backupAgentName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "backupAgentName" << backupAgentName;
    descriptionRes = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "descriptionRes" << descriptionRes;
    uiOptions = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "uiOptions" << uiOptions;
    fullBackupContent = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "fullBackupContent" << fullBackupContent;
    networkSecurityConfigRes = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "networkSecurityConfigRes" << networkSecurityConfigRes;
    category = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "category" << category;
    targetSandboxVersion = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "targetSandboxVersion" << targetSandboxVersion;

    classLoaderName = parcel->readString();
    qCDebug(logging) << Q_FUNC_INFO << "classLoaderName" << classLoaderName;

    splitClassLoaderNames = parcel->readStringList();
    qCDebug(logging) << Q_FUNC_INFO << "splitClassLoaderNames" << splitClassLoaderNames;
}
