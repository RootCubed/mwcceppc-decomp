#ifndef CMDLINE_OSLIB_H
#define CMDLINE_OSLIB_H

#include <common.h>

#ifdef CW_HOST_MAC_CLASSIC
#define OS_PATHSEP ':'
#else
#define OS_PATHSEP '/'
#endif

/**
 * OS abstraction layer
 */

#define OPTION_ASSERT(cond) do { if (!!(cond) == 0) { printf("%s:%u: failed assertion\n", __FILE__, __LINE__); abort(); } } while(0)
#define OS_ASSERT(line, cond) do { if (!!(cond) == 0) { printf("%s:%u: failed assertion\n", __FILE__, line); abort(); } } while(0)

typedef struct uOSTypePair {
    int perm;
} uOSTypePair; // unknown name

typedef enum {
    OSReadOnly,
    OSWrite,
    OSReadWrite,
    OSAppend
} OSOpenMode; // assumed name

typedef enum {
    OSSeekRel,
    OSSeekAbs,
    OSSeekEnd
} OSSeekMode; // assumed name

typedef struct OSPathSpec {
    char s[256];
} OSPathSpec;

typedef struct OSNameSpec {
    char s[64];
} OSNameSpec;

typedef struct OSSpec {
    OSPathSpec path;
    OSNameSpec name;
} OSSpec;

typedef struct OSHandle {
    void *addr;
    UInt32 used;
    UInt32 size;
} OSHandle;

typedef struct {
    OSSpec spec;
    OSHandle hand;
    Boolean loaded;
    Boolean changed;
    Boolean writeable;
} OSFileHandle; // assumed name

typedef struct {
    void *dir;
    OSPathSpec spec;
} OSOpenedDir; // assumed name, might be something like OSDirRef though?

#ifdef	__MWERKS__
#pragma options align=2
#endif
typedef struct OSFileTypeMapping {
    OSType mactype;
    const char *magic;
    char length;
    char executable;
    const char *mimetype;
} OSFileTypeMapping;

typedef struct OSFileTypeMappingList {
    SInt16 numMappings;
    const OSFileTypeMapping *mappings;
} OSFileTypeMappingList;

typedef struct OSFileTypeMappings {
    const OSFileTypeMappingList *mappingList;
    struct OSFileTypeMappings *next;
} OSFileTypeMappings;
#ifdef	__MWERKS__
#pragma options align=reset
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************/
/* Generic */
extern int WildCardMatch(char *wild, char *name);
extern OSSpec *OS_MatchPath(const char *path);
extern char *OS_GetFileNamePtr(char *path);
extern char *OS_GetDirName(const OSPathSpec *spec, char *buf, int size);
extern int OS_MakeSpec2(const char *path, const char *filename, OSSpec *spec);
extern int OS_MakeSpecWithPath(OSPathSpec *path, const char *filename, Boolean noRelative, OSSpec *spec);
extern int OS_NameSpecChangeExtension(OSNameSpec *spec, const char *ext, Boolean append);
extern int OS_NameSpecSetExtension(OSNameSpec *spec, const char *ext);
extern char *OS_CompactPaths(char *buf, const char *p, const char *n, int size);
extern char *OS_SpecToStringRelative(const OSSpec *spec, const OSPathSpec *cwdspec, char *path, int size);
extern int OS_FindFileInPath(const char *filename, const char *plist, OSSpec *spec);
extern int OS_FindProgram(const char *filename, OSSpec *spec);
extern int OS_CopyHandle(OSHandle *hand, OSHandle *copy);
extern int OS_AppendHandle(OSHandle *hand, const void *data, UInt32 len);

/********************************/
/* Platform-Specific */
extern uOSTypePair OS_TEXTTYPE;
extern const char *OS_GetErrText(int err);
extern int OS_InitProgram(int *pArgc, char ***pArgv);
extern int OS_TermProgram(void);
extern int OS_Create(const OSSpec *spec, const uOSTypePair *type);
extern int OS_Status(const OSSpec *spec);
extern int OS_GetFileType(const OSSpec *spec, uOSTypePair *type);
extern int OS_SetFileType(const OSSpec *spec, const uOSTypePair *type);
extern int OS_GetFileTime(const OSSpec *spec, time_t *crtm, time_t *chtm);
extern int OS_SetFileTime(const OSSpec *spec, const time_t *crtm, const time_t *chtm);
extern int OS_Open(const OSSpec *spec, OSOpenMode mode, int *ref);
extern int OS_Write(int ref, const void *buffer, UInt32 *length);
extern int OS_Read(int ref, void *buffer, UInt32 *length);
extern int OS_Seek(int ref, OSSeekMode how, SInt32 offset);
extern int OS_Tell(int ref, SInt32 *offset);
extern int OS_Close(int ref);
extern int OS_GetSize(int ref, UInt32 *length);
extern int OS_SetSize(int ref, UInt32 size);
extern int OS_Delete(const OSSpec *spec);
extern int OS_Rename(const OSSpec *oldspec, const OSSpec *newspec);
extern int OS_Mkdir(const OSSpec *spec);
extern int OS_Rmdir(const OSPathSpec *spec);
extern int OS_Chdir(const OSPathSpec *spec);
extern int OS_GetCWD(OSPathSpec *spec);
extern int OS_Execute(OSSpec *spec, char **argv, char **envp, const char *stdoutfile, const char *stderrfile, int *exitcode);
extern int OS_IsLegalPath(const char *path);
extern int OS_IsFullPath(const char *path);
extern char *OS_GetDirPtr(char *path);
extern int OS_EqualPath(const char *a, const char *b);
extern int OS_CanonPath(const char *src, char *dst);
extern int OS_MakeSpec(const char *path, OSSpec *spec, Boolean *isfile);
extern int OS_MakeFileSpec(const char *path, OSSpec *spec);
extern int OS_MakePathSpec(const char *vol, const char *dir, OSPathSpec *spec);
extern int OS_MakeNameSpec(const char *name, OSNameSpec *spec);
extern int OS_GetRootSpec(OSPathSpec *spec);
extern char *OS_SpecToString(const OSSpec *spec, char *path, int size);
extern char *OS_PathSpecToString(const OSPathSpec *pspec, char *path, int size);
extern char *OS_NameSpecToString(const OSNameSpec *nspec, char *name, int size);
extern int OS_SizeOfPathSpec(const OSPathSpec *spec);
extern int OS_SizeOfNameSpec(const OSNameSpec *spec);
extern int OS_EqualSpec(const OSSpec *a, const OSSpec *b);
extern int OS_EqualPathSpec(const OSPathSpec *a, const OSPathSpec *b);
extern int OS_EqualNameSpec(const OSNameSpec *a, const OSNameSpec *b);
extern int OS_IsDir(const OSSpec *spec);
extern int OS_IsFile(const OSSpec *spec);
extern int OS_IsLink(const OSSpec *spec);
extern int OS_ResolveLink(const OSSpec *link, OSSpec *target);
extern int OS_OpenDir(const OSPathSpec *spec, OSOpenedDir *ref);
extern int OS_ReadDir(OSOpenedDir *ref, OSSpec *spec, char *filename, Boolean *isfile);
extern int OS_CloseDir(OSOpenedDir *ref);
extern UInt32 OS_GetMilliseconds(void);
extern void OS_GetTime(time_t *p);
extern int OS_NewHandle(UInt32 size, OSHandle *hand);
extern int OS_ResizeHandle(OSHandle *hand, UInt32 size);
extern void *OS_LockHandle(OSHandle *hand);
extern void OS_UnlockHandle(OSHandle *hand);
extern int OS_FreeHandle(OSHandle *hand);
extern int OS_GetHandleSize(OSHandle *hand, UInt32 *size);
extern void OS_InvalidateHandle(OSHandle *hand);
extern Boolean OS_ValidHandle(OSHandle *hand);
extern OSErr OS_MacError(int err);
extern void OS_TimeToMac(time_t sectm, UInt32 *secs);
extern void OS_MacToTime(UInt32 secs, time_t *sectm);
extern SInt16 OS_RefToMac(int ref);
extern int OS_MacToRef(SInt16 refnum);
extern int OS_OpenLibrary(const char *a, void **lib);
extern int OS_GetLibrarySymbol(void *a, void *b, void **sym);
extern int OS_CloseLibrary(void *a);
extern int OS_LoadMacResourceFork(const OSSpec *spec, void **file_data, SInt32 *file_len);
extern Boolean OS_IsMultiByte(const char *str1, const char *str2);

/********************************/
/* FileHandles */
extern int OS_NewFileHandle(const OSSpec *spec, OSHandle *src, Boolean writeable, OSFileHandle *hand);
extern int OS_LockFileHandle(OSFileHandle *hand, Ptr *ptr, UInt32 *size);
extern int OS_UnlockFileHandle(OSFileHandle *hand);
extern int OS_FreeFileHandle(OSFileHandle *hand);
extern void OS_GetFileHandleSpec(const OSFileHandle *hand, OSSpec *spec);

/********************************/
/* MacFileTypes */
extern void OS_AddFileTypeMappingList(OSFileTypeMappings **list, const OSFileTypeMappingList *entry);
extern void OS_UseFileTypeMappings(OSFileTypeMappings *list);
extern void OS_MacType_To_OSType(OSType mactype, uOSTypePair *type);
extern int OS_SetMacFileType(const OSSpec *spec, OSType mactype);
extern Boolean OS_GetMacFileTypeMagic(const char *buffer, int count, OSType *mactype);
extern int OS_GetMacFileType(const OSSpec *spec, OSType *mactype);
extern int OS_SetMacFileCreatorAndType(const OSSpec *spec, OSType creator, OSType mactype);

/********************************/
/* MacSpecs */
extern int OS_OSPathSpec_To_VolDir(const OSPathSpec *spec, SInt16 *vRefNum, SInt32 *dirID);
extern int OS_OSSpec_To_FSSpec(const OSSpec *spec, FSSpec *fss);
extern int OS_VolDir_To_OSNameSpec(SInt16 vRefNum, SInt32 dirID, OSNameSpec *spec, SInt32 *parID);
extern int OS_VolDir_To_OSPathSpec(SInt16 vRefNum, SInt32 dirID, OSPathSpec *spec);
extern int OS_FSSpec_To_OSSpec(const FSSpec *fss, OSSpec *spec);
extern int OS_GetRsrcOSSpec(const OSSpec *spec, OSSpec *rspec, Boolean create);

/********************************/
/* MemUtils */
extern void *xmalloc(const char *what, int size);
extern void *xcalloc(const char *what, int size);
extern void *xrealloc(const char *what, void *old, int size);
extern char *xstrdup(const char *str);
extern void xfree(void *ptr);

/********************************/
/* StringExtras.c */
extern char *strcatn(char *d, const char *s, SInt32 max);
extern char *strcpyn(char *d, const char *s, SInt32 len, SInt32 max);
extern int ustrcmp(const char *src, const char *dst);
extern int ustrncmp(const char *src, const char *dst, UInt32 len);

/********************************/
/* StringUtils.c */
extern StringPtr _pstrcpy(StringPtr dst, ConstStringPtr src);
extern void _pstrcat(StringPtr dst, ConstStringPtr src);
extern void _pstrcharcat(StringPtr to, char ch);
extern void pstrncpy(StringPtr to, ConstStringPtr from, int max);
extern void pstrncat(StringPtr to, ConstStringPtr append, int max);
extern int pstrcmp(ConstStringPtr a, ConstStringPtr b);
extern int pstrchr(ConstStringPtr str, char find);
extern void c2pstrcpy(StringPtr dst, const char *src);
extern void p2cstrcpy(char *dst, ConstStringPtr src);
extern char *mvprintf(char *mybuf, unsigned int len, const char *format, va_list va);
extern char *mprintf(char *mybuf, unsigned int len, const char *format, ...);
extern int HPrintF(Handle text, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
