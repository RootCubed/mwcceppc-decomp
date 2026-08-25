#ifndef OS_GENERIC_H
#define OS_GENERIC_H

#include <common.h>
#include <OS/OS.h>

int __stdcall WildCardMatch(char *wild, char *name);
OSSpec* __stdcall OS_MatchPath(const char *path);
char* __stdcall OS_GetFileNamePtr(char *path);
char* __stdcall OS_GetDirName(const OSPathSpec *spec, char *buf, int size);
int __stdcall OS_MakeSpec2(const char *path, const char *filename, OSSpec *spec);
int __stdcall OS_MakeSpecWithPath(OSPathSpec *path, const char *filename, Boolean noRelative, OSSpec *spec);
int __stdcall OS_NameSpecChangeExtension(OSNameSpec *spec, const char *ext, Boolean append);
int __stdcall OS_NameSpecSetExtension(OSNameSpec *spec, const char *ext);
char* __stdcall OS_CompactPaths(char *buf, const char *p, const char *n, int size);
char* __stdcall OS_SpecToStringRelative(const OSSpec *spec, const OSPathSpec *cwdspec, char *path, int size);
int __stdcall OS_FindFileInPath(const char *filename, const char *plist, OSSpec *spec);
int __stdcall OS_FindProgram(const char *filename, OSSpec *spec);
int __stdcall OS_CopyHandle(OSHandle *hand, OSHandle *copy);
int __stdcall OS_AppendHandle(OSHandle *hand, const void *data, UInt32 len);

#endif
