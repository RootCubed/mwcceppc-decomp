#ifndef OS_WIN32_H
#define OS_WIN32_H

#include <OS/OS.h>
#include <Windows.h>

uOSTypePair OS_TEXTTYPE;
const char* __stdcall OS_GetErrText(int err);
int __stdcall OS_InitProgram(int *pArgc, char ***pArgv);
int __stdcall OS_TermProgram(void);
int __stdcall OS_Create(const OSSpec *spec, const uOSTypePair *type);
int __stdcall OS_Status(const OSSpec *spec);
int __stdcall OS_GetFileType(const OSSpec *spec, uOSTypePair *type);
int __stdcall OS_SetFileType(const OSSpec *spec, const uOSTypePair *type);
int __stdcall OS_GetFileTime(const OSSpec *spec, MacTime *crtm, MacTime *chtm);
int __stdcall OS_SetFileTime(const OSSpec *spec, const MacTime *crtm, const MacTime *chtm);
int __stdcall OS_Open(const OSSpec *spec, OSOpenMode mode, HANDLE *ref);
int __stdcall OS_Write(HANDLE ref, const void *buffer, UInt32 *length);
int __stdcall OS_Read(HANDLE ref, void *buffer, UInt32 *length);
int __stdcall OS_Seek(HANDLE ref, OSSeekMode how, SInt32 offset);
int __stdcall OS_Tell(HANDLE ref, SInt32 *offset);
int __stdcall OS_Close(HANDLE ref);
int __stdcall OS_GetSize(HANDLE ref, UInt32 *length);
int __stdcall OS_SetSize(HANDLE ref, UInt32 size);
int __stdcall OS_Delete(const OSSpec *spec);
int __stdcall OS_Rename(const OSSpec *oldspec, const OSSpec *newspec);
int __stdcall OS_Mkdir(const OSSpec *spec);
int __stdcall OS_Rmdir(const OSPathSpec *spec);
int __stdcall OS_Chdir(const OSPathSpec *spec);
int __stdcall OS_GetCWD(OSPathSpec *spec);
int __stdcall OS_Execute(OSSpec *spec, char **argv, char **envp, const char *stdoutfile, const char *stderrfile, int *exitcode);
int __stdcall OS_IsLegalPath(const char *path);
int __stdcall OS_IsFullPath(const char *path);
char* __stdcall OS_GetDirPtr(char *path);
int __stdcall OS_EqualPath(const char *a, const char *b);
int __stdcall OS_CanonPath(const char *src, char *dst);
int __stdcall OS_MakeSpecEx(const char *path, Boolean bool1, Boolean bool2, OSSpec *spec, Boolean *isfile);
int __stdcall OS_MakeSpec(const char *path, OSSpec *spec, Boolean *isfile);
int __stdcall OS_MakeFileSpecEx(const char *path, Boolean bool1, Boolean bool2, OSSpec *spec);
int __stdcall OS_MakeFileSpec(const char *path, OSSpec *spec);
int __stdcall OS_MakePathSpecEx(const char *vol, const char *dir, Boolean bool1, Boolean bool2, OSPathSpec *spec);
int __stdcall OS_MakePathSpec(const char *vol, const char *dir, OSPathSpec *spec);
int __stdcall OS_MakeNameSpec(const char *name, OSNameSpec *spec);
int __stdcall OS_GetRootSpec(OSPathSpec *spec);
char* __stdcall OS_SpecToString(const OSSpec *spec, char *path, int size);
char* __stdcall OS_PathSpecToString(const OSPathSpec *pspec, char *path, int size);
char* __stdcall OS_NameSpecToString(const OSNameSpec *nspec, char *name, int size);
int __stdcall OS_SizeOfPathSpec(const OSPathSpec *spec);
int __stdcall OS_SizeOfNameSpec(const OSNameSpec *spec);
int __stdcall OS_EqualSpec(const OSSpec *a, const OSSpec *b);
int __stdcall OS_EqualPathSpec(const OSPathSpec *a, const OSPathSpec *b);
int __stdcall OS_EqualNameSpec(const OSNameSpec *a, const OSNameSpec *b);
int __stdcall OS_IsDir(const OSSpec *spec);
int __stdcall OS_IsFile(const OSSpec *spec);
int __stdcall OS_IsLink(const OSSpec *spec);
int __stdcall OS_ResolveLink(const OSSpec *link, OSSpec *target);
int __stdcall OS_OpenDir(const OSPathSpec *spec, OSOpenedDir *ref);
int __stdcall OS_ReadDir(OSOpenedDir *ref, OSSpec *spec, char *filename, Boolean *isfile);
int __stdcall OS_CloseDir(OSOpenedDir *ref);
UInt32 __stdcall OS_GetMilliseconds(void);
void __stdcall OS_GetTime(MacTime *p);
int __stdcall OS_NewHandle(UInt32 size, OSHandle *hand);
int __stdcall OS_ResizeHandle(OSHandle *hand, UInt32 size);
void* __stdcall OS_LockHandle(OSHandle *hand);
void __stdcall OS_UnlockHandle(OSHandle *hand);
int __stdcall OS_FreeHandle(OSHandle *hand);
int __stdcall OS_GetHandleSize(OSHandle *hand, UInt32 *size);
void __stdcall OS_InvalidateHandle(OSHandle *hand);
Boolean __stdcall OS_ValidHandle(OSHandle *hand);
int __stdcall OS_OSErrorToMacError(int err);
void __stdcall OS_TimeToMac(MacTime sectm, UInt32 *secs);
void __stdcall OS_MacToTime(UInt32 secs, MacTime *sectm);
SInt16 __stdcall OS_RefToMac(HANDLE ref);
int __stdcall OS_MacToRef(SInt16 refnum);
int __stdcall OS_OpenLibrary(const char *a, void **lib);
int __stdcall OS_GetLibrarySymbol(void *a, void *b, void **sym);
int __stdcall OS_CloseLibrary(void *a);
int __stdcall OS_LoadMacResourceFork(const OSSpec *spec, void **file_data, SInt32 *file_len);
int __stdcall OS_CreateMutex(OSMutex *mutex);
//int __stdcall OS_MapFile(HANDLE *ref, void **mapping, HANDLE file, DWORD size, Boolean readonly, Boolean executable);
Boolean __stdcall OS_IsMultiByte(const char *str1, const char *str2);

#endif