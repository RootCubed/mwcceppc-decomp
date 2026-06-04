#pragma once

#include <common.h>

typedef UInt32 DWORD;

#define MAX_PATH 32
#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

#include "CWPlugins.h"
#include "CWPluginErrors.h"
#include "DropInCompilerLinker.h"

#ifdef	__MWERKS__
#pragma options align=2
#endif

#if CWPLUGIN_API == CWPLUGIN_API_MACOS
#define CWFileSpecNotEmpty(specPtr) ((specPtr)->name[0] != 0)
#elif CWPLUGIN_API == CWPLUGIN_API_WIN32
#define CWFileSpecNotEmpty(specPtr) ((specPtr)->path[0] != 0)
#elif CWPLUGIN_API == CWPLUGIN_API_UNIX
#define CWFileSpecNotEmpty(specPtr) ((specPtr)->path[0] != 0)
#endif

enum {
    CWDROPINPARSERTYPE = CWFOURCHAR('P','a','r','s'),
    CWDROPINDRIVERTYPE = CWFOURCHAR('c','l','d','r'),
    CWDROPINANYTYPE = CWFOURCHAR('*','*','*','*')
};

// Extra drop-in flags that aren't in the public plugin SDK headers
enum {
    dropInExecutableTool = 1
};

enum {
    Lang_Any = CWFOURCHAR('*','*','*','*')
};

typedef struct IDEAccessPath {
    FSSpec pathSpec;
    Boolean recursive;
    SInt32 subdirectoryCount;
    FSSpec *subdirectories;
} IDEAccessPath;

typedef struct IDEAccessPathList {
    SInt32 userPathCount;
    IDEAccessPath *userPaths;
    SInt32 systemPathCount;
    IDEAccessPath *systemPaths;
    Boolean alwaysSearchUserPaths;
    Boolean convertPaths;
} IDEAccessPathList;

enum {
    cwAccessPathTypeFlag1 = 1,
    cwAccessPathTypeFlag2 = 2
};
typedef struct CWNewAccessPathInfo {
    CWFileSpec pathSpec;
    SInt32 position;
    Boolean recursive;
    CWAccessPathType type;
} CWNewAccessPathInfo;

typedef struct CWTargetInfoV7 {
    CWFileSpec outfile;
    CWFileSpec symfile;
    short linkType;
    Boolean canRun;
    Boolean canDebug;
    Boolean useRunHelperApp;
    char reserved1;
    CWDataType debuggerCreator;
    CWDataType runHelperCreator;
    SInt32 reserved2[2];
} CWTargetInfoV7;

typedef struct CWEnvVarInfo {
    char *name;
    char *value;
} CWEnvVarInfo;

typedef struct CWCommandLineArgs {
    int argc;
    char **argv;
    char **envp;
} CWCommandLineArgs;

typedef struct ToolVersionInfo {
    char *company;
    char *product;
    char *tool;
    char *copyright;
    char *version;
} ToolVersionInfo;

typedef struct CWObjectFlags {
    SInt16 version;
    SInt32 flags;
    const char *objFileExt;
    const char *brsFileExt;
    const char *ppFileExt;
    const char *disFileExt;
    const char *depFileExt;
    const char *pchFileExt;
    OSType objFileCreator;
    OSType objFileType;
    OSType brsFileCreator;
    OSType brsFileType;
    OSType ppFileCreator;
    OSType ppFileType;
    OSType disFileCreator;
    OSType disFileType;
    OSType depFileCreator;
    OSType depFileType;
} CWObjectFlags;

typedef struct CLPluginInfo {
    OSType plugintype;
    OSType language;
    SInt32 dropinflags;
    char *version;
    Boolean storeCommandLine;
} CLPluginInfo;

struct CW_BasePluginCallbacks {
    CWResult (*cbGetFileInfo)(CWPluginContext, SInt32, Boolean, CWProjectFileInfo *);
    CWResult (*cbFindAndLoadFile)(CWPluginContext, const char *, CWFileInfo *);
    CWResult (*cbGetFileText)(CWPluginContext, const CWFileSpec *, const char **, SInt32 *, short *);
    CWResult (*cbReleaseFileText)(CWPluginContext, const char *);
    CWResult (*cbGetSegmentInfo)(CWPluginContext, SInt32, CWProjectSegmentInfo *);
    CWResult (*cbGetOverlay1GroupInfo)(CWPluginContext, SInt32, CWOverlay1GroupInfo *);
    CWResult (*cbGetOverlay1Info)(CWPluginContext, SInt32, SInt32, CWOverlay1Info *);
    CWResult (*cbGetOverlay1FileInfo)(CWPluginContext, SInt32, SInt32, SInt32, CWOverlay1FileInfo *);
    CWResult (*cbReportMessage)(CWPluginContext, const CWMessageRef *, const char *, const char *, short, SInt32);
    CWResult (*cbAlert)(CWPluginContext, const char *, const char *, const char *, const char *);
    CWResult (*cbShowStatus)(CWPluginContext, const char *, const char *);
    CWResult (*cbUserBreak)(CWPluginContext);
    CWResult (*cbGetNamedPreferences)(CWPluginContext, const char *, CWMemHandle *);
    CWResult (*cbStorePluginData)(CWPluginContext, SInt32, CWDataType, CWMemHandle);
    CWResult (*cbGetPluginData)(CWPluginContext, SInt32, CWDataType, CWMemHandle *);
    CWResult (*cbSetModDate)(CWPluginContext, const CWFileSpec *, CWFileTime *, Boolean);
    CWResult (*cbAddProjectEntry)(CWPluginContext, const CWFileSpec *, Boolean, const CWNewProjectEntryInfo *, SInt32 *);
    CWResult (*cbCreateNewTextDocument)(CWPluginContext, const CWNewTextDocumentInfo *);
    CWResult (*cbAllocateMemory)(CWPluginContext, SInt32, Boolean, void **);
    CWResult (*cbFreeMemory)(CWPluginContext, void *, Boolean);
    CWResult (*cbAllocMemHandle)(CWPluginContext, SInt32, Boolean, CWMemHandle *);
    CWResult (*cbFreeMemHandle)(CWPluginContext, CWMemHandle);
    CWResult (*cbGetMemHandleSize)(CWPluginContext, CWMemHandle, SInt32 *);
    CWResult (*cbResizeMemHandle)(CWPluginContext, CWMemHandle, SInt32);
    CWResult (*cbLockMemHandle)(CWPluginContext, CWMemHandle, Boolean, void **);
    CWResult (*cbUnlockMemHandle)(CWPluginContext, CWMemHandle);
    void *cbInternal[8];
    CWResult (*cbGetTargetName)(CWPluginContext, char *, short);
    CWResult (*cbCacheAccessPathList)(CWPluginContext);
    CWResult (*cbPreDialog)(CWPluginContext);
    CWResult (*cbPostDialog)(CWPluginContext);
    CWResult (*cbPreFileAction)(CWPluginContext, const CWFileSpec *);
    CWResult (*cbPostFileAction)(CWPluginContext, const CWFileSpec *);
    CWResult (*cbCheckoutLicense)(CWPluginContext, const char *, const char *, SInt32, void *, SInt32 *);
    CWResult (*cbCheckinLicense)(CWPluginContext, SInt32);
    CWResult (*cbResolveRelativePath)(CWPluginContext, const CWRelativePath *, CWFileSpec *, Boolean);
};

struct CWCompilerLinkerCallbacks {
    CWResult (*cbCachePrecompiledHeader)(CWPluginContext, const CWFileSpec *, CWMemHandle);
    CWResult (*cbLoadObjectData)(CWPluginContext, SInt32, CWMemHandle *);
    CWResult (*cbStoreObjectData)(CWPluginContext, SInt32, CWObjectData *);
    CWResult (*cbFreeObjectData)(CWPluginContext, SInt32, CWMemHandle);
    CWResult (*cbDisplayLines)(CWPluginContext, SInt32);
    CWResult (*cbBeginSubCompile)(CWPluginContext, SInt32, CWPluginContext *);
    CWResult (*cbEndSubCompile)(CWPluginContext);
    CWResult (*cbGetPrecompiledHeaderSpec)(CWPluginContext, CWFileSpec *, const char *);
    CWResult (*cbGetResourceFile)(CWPluginContext, CWFileSpec *);
    CWResult (*cbPutResourceFile)(CWPluginContext, const char *, const char *, CWFileSpec *);
    CWResult (*cbLookUpUnit)(CWPluginContext, const char *, Boolean, const void **, SInt32 *);
    CWResult (*cbSBMfiles)(CWPluginContext, short);
    CWResult (*cbStoreUnit)(CWPluginContext, const char *, CWMemHandle, CWDependencyTag);
    CWResult (*cbReleaseUnit)(CWPluginContext, void *);
    CWResult (*cbUnitNameToFileName)(CWPluginContext, const char *, char *);
    CWResult (*cbOSErrorMessage)(CWPluginContext, const char *, OSErr);
    CWResult (*cbOSAlert)(CWPluginContext, const char *, OSErr);
    CWResult (*cbGetModifiedFiles)(CWPluginContext, SInt32 *, const SInt32 **);
    CWResult (*cbGetSuggestedObjectFileSpec)(CWPluginContext, SInt32, CWFileSpec *);
    CWResult (*cbGetStoredObjectFileSpec)(CWPluginContext, SInt32, CWFileSpec *);
    CWResult (*cbGetRuntimeSettings)(CWPluginContext);
    CWResult (*cbGetFrameworkCount)(CWPluginContext, SInt32 *);
    CWResult (*cbGetFrameworkInfo)(CWPluginContext, SInt32, CWFrameworkInfo *);
    CWResult (*cbGetFrameworkSharedLibrary)(CWPluginContext);
};

struct CWParserCallbacks {
    CWResult (*cbParserAddAccessPath)(CWPluginContext, const CWNewAccessPathInfo *);
    CWResult (*cbParserSwapAccessPaths)(CWPluginContext);
    CWResult (*cbParserSetNamedPreferences)(CWPluginContext, const char *, Handle);
    CWResult (*cbParserSetFileOutputName)(CWPluginContext, SInt32, short, const char *);
    CWResult (*cbParserSetOutputFileDirectory)(CWPluginContext, const CWFileSpec *);
    CWResult (*cbParserAddOverlay1Group)(CWPluginContext, const char *, const CWAddr64 *, SInt32 *);
    CWResult (*cbParserAddOverlay1)(CWPluginContext, const char *, SInt32, SInt32 *);
    CWResult (*cbParserAddSegment)(CWPluginContext, const char *, short, SInt32 *);
    CWResult (*cbParserSetSegment)(CWPluginContext, SInt32, const char *, short);
};

#ifdef	__MWERKS__
#pragma options align=reset
#endif

#ifdef __cplusplus
extern "C" {
#endif
CW_CALLBACK CWCheckoutLicense(CWPluginContext context, const char *featureName, const char *licenseVersion, SInt32 flags, void *reserved, SInt32 *cookie);
CW_CALLBACK CWCheckinLicense(CWPluginContext context, SInt32 cookie);

CW_CALLBACK	CWOSErrorMessage(CWPluginContext context, const char *msg, OSErr errorcode);
CW_CALLBACK	CWOSAlert(CWPluginContext context, const char* message, OSErr errorcode);

CW_CALLBACK CWSecretAttachHandle(CWPluginContext context, Handle handle, CWMemHandle *memHandle);
CW_CALLBACK CWSecretDetachHandle(CWPluginContext context, CWMemHandle memHandle, Handle *handle);
CW_CALLBACK CWSecretPeekHandle(CWPluginContext context, CWMemHandle memHandle, Handle *handle);
CW_CALLBACK CWSecretGetNamedPreferences(CWPluginContext context, const char *prefsname, Handle *prefsdata);

CW_CALLBACK CWParserGetBuildDate(CWPluginContext context, const char **bdate, const char **btime);
CW_CALLBACK CWParserGetCommandLine(CWPluginContext context, CWCommandLineArgs **args);
CW_CALLBACK CWParserGetTargetInfo(CWPluginContext context, CWDataType *cpu, CWDataType *os);
CW_CALLBACK CWParserGetToolInfo(CWPluginContext context, const ToolVersionInfo **toolVersionInfo);
CW_CALLBACK CWParserGetPlugins(CWPluginContext context, int *numPlugins, const CLPluginInfo **pluginInfo);
CW_CALLBACK CWParserGetPanels(CWPluginContext context, int *numPanels, const char ***panelNames);
CW_CALLBACK CWParserStoreCommandLineForPanel(CWPluginContext context, int index, const CWCommandLineArgs *args);
CW_CALLBACK CWParserStoreCommandLineForPlugin(CWPluginContext context, int index, const CWCommandLineArgs *args);
CW_CALLBACK CWParserAddAccessPath(CWPluginContext context, const CWNewAccessPathInfo *api);
CW_CALLBACK CWParserSwapAccessPaths(CWPluginContext context);
CW_CALLBACK CWParserSetNamedPreferences(CWPluginContext context, const char *panelName, Handle paneldata);
CW_CALLBACK CWParserSetFileOutputName(CWPluginContext context, SInt32 position, short which, const char *outfilename);
CW_CALLBACK CWParserSetOutputFileDirectory(CWPluginContext context, const CWFileSpec *idefss);
CW_CALLBACK CWParserAddOverlay1Group(CWPluginContext context, const char *name, const CWAddr64 *addr, SInt32 *newGroupNumber);
CW_CALLBACK CWParserAddOverlay1(CWPluginContext context, const char *name, SInt32 groupNumber, SInt32 *newOverlayNumber);
CW_CALLBACK CWParserAddSegment(CWPluginContext context, const char *name, short attrs, SInt32 *newSegmentNumber);
CW_CALLBACK CWParserSetSegment(CWPluginContext context, SInt32 segmentNumber, const char *name, short attrs);
CW_CALLBACK CWParserCreateVirtualFile(CWPluginContext context, const char *name, CWMemHandle text);
CW_CALLBACK CWParserDisplayTextHandle(CWPluginContext context, const char *name, CWMemHandle text);

// CLDropinCallbacks
extern CWResult UCBGetFileInfo(CWPluginContext context, SInt32 whichfile, Boolean checkFileLocation, CWProjectFileInfo *fileinfo);
extern CWResult UCBFindAndLoadFile(CWPluginContext context, const char *filename, CWFileInfo *fileinfo);
extern CWResult UCBGetFileText(CWPluginContext context, const CWFileSpec *filespec, const char **text, SInt32 *textLength, short *filedatatype);
extern CWResult UCBReleaseFileText(CWPluginContext context, const char *text);
extern CWResult UCBGetSegmentInfo(CWPluginContext context, SInt32 whichsegment, CWProjectSegmentInfo *segmentinfo);
extern CWResult UCBGetOverlay1GroupInfo(CWPluginContext context, SInt32 whichgroup, CWOverlay1GroupInfo *groupinfo);
extern CWResult UCBGetOverlay1FileInfo(CWPluginContext context, SInt32 whichgroup, SInt32 whichoverlay, SInt32 whichoverlayfile, CWOverlay1FileInfo *fileinfo);
extern CWResult UCBGetOverlay1Info(CWPluginContext context, SInt32 whichgroup, SInt32 whichoverlay, CWOverlay1Info *overlayinfo);
extern CWResult UCBReportMessage(CWPluginContext context, const CWMessageRef *msgRef, const char *line1, const char *line2, short errorlevel, SInt32 errorNumber);
extern CWResult UCBAlert(CWPluginContext context, const char *msg1, const char *msg2, const char *msg3, const char *msg4);
extern CWResult UCBShowStatus(CWPluginContext context, const char *line1, const char *line2);
extern CWResult UCBUserBreak(CWPluginContext context);
extern CWResult UCBGetNamedPreferences(CWPluginContext context, const char *prefsname, CWMemHandle *prefsdata);
extern CWResult UCBStorePluginData(CWPluginContext context, SInt32 whichfile, CWDataType type, CWMemHandle prefsdata);
extern CWResult UCBGetPluginData(CWPluginContext context, SInt32 whichfile, CWDataType type, CWMemHandle *prefsdata);
extern CWResult UCBSetModDate(CWPluginContext context, const CWFileSpec *filespec, CWFileTime *moddate, Boolean isGenerated);
extern CWResult UCBAddProjectEntry(CWPluginContext context, const CWFileSpec *fileSpec, Boolean isGenerated, const CWNewProjectEntryInfo *projectEntryInfo, SInt32 *whichfile);
extern CWResult UCBCreateNewTextDocument(CWPluginContext context, const CWNewTextDocumentInfo *docinfo);
extern CWResult UCBAllocateMemory(CWPluginContext context, SInt32 size, Boolean isPermanent, void **ptr);
extern CWResult UCBFreeMemory(CWPluginContext context, void *ptr, Boolean isPermanent);
extern CWResult UCBAllocMemHandle(CWPluginContext context, SInt32 size, Boolean useTempMemory, CWMemHandle *handle);
extern CWResult UCBFreeMemHandle(CWPluginContext context, CWMemHandle handle);
extern CWResult UCBGetMemHandleSize(CWPluginContext context, CWMemHandle handle, SInt32 *size);
extern CWResult UCBResizeMemHandle(CWPluginContext context, CWMemHandle handle, SInt32 newSize);
extern CWResult UCBLockMemHandle(CWPluginContext context, CWMemHandle handle, Boolean moveHi, void **ptr);
extern CWResult UCBUnlockMemHandle(CWPluginContext context, CWMemHandle handle);
extern CWResult UCBGetTargetName(CWPluginContext context, char *name, short maxLength);
extern CWResult UCBPreDialog(CWPluginContext context);
extern CWResult UCBPostDialog(CWPluginContext context);
extern CWResult UCBPreFileAction(CWPluginContext context, const CWFileSpec *theFile);
extern CWResult UCBPostFileAction(CWPluginContext context, const CWFileSpec *theFile);
extern CWResult UCBCacheAccessPathList(CWPluginContext context);
extern CWResult UCBSecretAttachHandle(CWPluginContext context, Handle handle, CWMemHandle *memHandle);
extern CWResult UCBSecretDetachHandle(CWPluginContext context, CWMemHandle memHandle, Handle *handle);
extern CWResult UCBSecretPeekHandle(CWPluginContext context, CWMemHandle memHandle, Handle *handle);
extern CWResult UCBCheckoutLicense(CWPluginContext context, const char *featureName, const char *licenseVersion, SInt32 flags, void *reserved, SInt32 *cookie);
extern CWResult UCBCheckinLicense(CWPluginContext context, SInt32 cookie);
extern CWResult UCBResolveRelativePath(CWPluginContext context, const CWRelativePath *relativePath, CWFileSpec *fileSpec, Boolean create);
extern CWResult UCBMacOSErrToCWResult(CWPluginContext context, OSErr err);

// CLCompilerLinkerDropin
extern CWResult UCBCachePrecompiledHeader(CWPluginContext context, const CWFileSpec *filespec, CWMemHandle pchhandle);
extern CWResult UCBLoadObjectData(CWPluginContext context, SInt32 whichfile, CWMemHandle *objectdata);
extern CWResult UCBStoreObjectData(CWPluginContext context, SInt32 whichfile, CWObjectData *object);
extern CWResult UCBFreeObjectData(CWPluginContext context, SInt32 whichfile, CWMemHandle objectdata);
extern CWResult UCBDisplayLines(CWPluginContext context, SInt32 nlines);
extern CWResult UCBBeginSubCompile(CWPluginContext context, SInt32 whichfile, CWPluginContext *subContext);
extern CWResult UCBEndSubCompile(CWPluginContext subContext);
extern CWResult UCBGetPrecompiledHeaderSpec(CWPluginContext context, CWFileSpec *pchspec, const char *target);
extern CWResult UCBGetResourceFile(CWPluginContext context, CWFileSpec *filespec);
extern CWResult UCBPutResourceFile(CWPluginContext context, const char *prompt, const char *name, CWFileSpec *filespec);
extern CWResult UCBLookUpUnit(CWPluginContext context, const char *name, Boolean isdependency, const void **unitdata, SInt32 *unitdatalength);
extern CWResult UCBSBMfiles(CWPluginContext context, short libref);
extern CWResult UCBStoreUnit(CWPluginContext context, const char *unitname, CWMemHandle unitdata, CWDependencyTag dependencytag);
extern CWResult UCBReleaseUnit(CWPluginContext context, void *unitdata);
extern CWResult UCBUnitNameToFileName(CWPluginContext context, const char *unitname, char *filename);
extern CWResult UCBOSAlert(CWPluginContext context, const char *message, OSErr errorcode);
extern CWResult UCBOSErrorMessage(CWPluginContext context, const char *msg, OSErr errorcode);
extern CWResult UCBGetModifiedFiles(CWPluginContext context, SInt32 *modifiedFileCount, const SInt32 **modifiedFiles);
extern CWResult UCBGetSuggestedObjectFileSpec(CWPluginContext context, SInt32 whichfile, CWFileSpec *fileSpec);
extern CWResult UCBGetStoredObjectFileSpec(CWPluginContext context, SInt32 whichfile, CWFileSpec *fileSpec);
extern CWResult UCBGetFrameworkCount(CWPluginContext context, SInt32 *frameworkCount);
extern CWResult UCBGetFrameworkInfo(CWPluginContext context, SInt32 whichFramework, CWFrameworkInfo *frameworkInfo);

// CLParserCallbacks
extern CWResult UCBParserAddAccessPath(CWPluginContext context, const CWNewAccessPathInfo *api);
extern CWResult UCBParserSwapAccessPaths(CWPluginContext context);
extern CWResult UCBParserSetNamedPreferences(CWPluginContext context, const char *panelName, Handle paneldata);
extern CWResult UCBParserSetFileOutputName(CWPluginContext context, SInt32 position, short which, const char *outfilename);
extern CWResult UCBParserSetOutputFileDirectory(CWPluginContext context, const CWFileSpec *idefss);
extern CWResult UCBParserAddOverlay1Group(CWPluginContext context, const char *name, const CWAddr64 *addr, SInt32 *newGroupNumber);
extern CWResult UCBParserAddOverlay1(CWPluginContext context, const char *name, SInt32 groupNumber, SInt32 *newOverlayNumber);
extern CWResult UCBParserAddSegment(CWPluginContext context, const char *name, short attrs, SInt32 *newSegmentNumber);
extern CWResult UCBParserSetSegment(CWPluginContext context, SInt32 segmentNumber, const char *name, short attrs);

#ifdef __cplusplus
}
#endif

// This one is intentionally outwith the extern "C" block as it's mangled
extern CWResult OSErrtoCWResult(OSErr err);
