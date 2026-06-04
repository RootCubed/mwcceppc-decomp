#ifndef ROOT_CW_COMMON_H
#define ROOT_CW_COMMON_H

/*
 * Things that seem to be shared across different CodeWarrior modules
 */

#include <common.h>
#include <oslib.h>
#include <plugin.h>

// typedef struct VersionInfo {
//     UInt16 major;
//     UInt16 minor;
//     UInt16 patch;
//     UInt16 build;
// } VersionInfo;

// typedef struct BasePluginCallbacks {
//     CWPLUGIN_ENTRY (*main)(CWPluginContext context);
//     CWPLUGIN_ENTRY (*GetDropInFlags)(const DropInFlags **flags, SInt32 *flagsSize);
//     CWPLUGIN_ENTRY (*GetDisplayName)(const char **displayName);
//     CWPLUGIN_ENTRY (*GetDropInName)(const char **dropInName);
//     CWPLUGIN_ENTRY (*GetPanelList)(const CWPanelList **panelList);
//     CWPLUGIN_ENTRY (*GetFamilyList)(const CWFamilyList **familyList);
//     CWPLUGIN_ENTRY (*GetHelpInfo)(const CWHelpInfo **helpInfo);
//     CWPLUGIN_ENTRY (*GetVersionInfo)(const VersionInfo **versionInfo);
//     CWPLUGIN_ENTRY (*GetFileTypeMappings)(const OSFileTypeMappingList **mappingList);
// } BasePluginCallbacks;

// typedef struct CompilerLinkerPluginCallbacks {
//     CWPLUGIN_ENTRY (*GetTargetList)(const struct CWTargetList **targetList);
//     CWPLUGIN_ENTRY (*GetDefaultMappingList)(const CWExtMapList **mappings);
//     CWPLUGIN_ENTRY (*Unmangle)(CWUnmangleInfo *unmangleInfo);
//     CWPLUGIN_ENTRY (*BrSymbolEntryPoint)(CWCompilerBrSymbolInfo *symbolInfo);
//     CWPLUGIN_ENTRY (*GetObjectFlags)(const CWObjectFlags **objectFlags);
//     CWPLUGIN_ENTRY (*WriteObjectFile)(const FSSpec *srcfss, const FSSpec *outfss, OSType creator, OSType type, Handle data);
// } CompilerLinkerPluginCallbacks;

// typedef struct ParserPluginCallbacks {
//     CWPLUGIN_ENTRY (*SupportsPlugin)(const CLPluginInfo *pluginfo, OSType cpu, OSType os, Boolean *isSupported);
//     CWPLUGIN_ENTRY (*SupportsPanels)(int numPanels, const char **panelNames, Boolean *isSupported);
// } ParserPluginCallbacks;

#endif
