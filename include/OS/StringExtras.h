#ifndef OS_STRING_EXTRAS_H
#define OS_STRING_EXTRAS_H

#include <common.h>
#include <OS/OS.h>

extern char* __stdcall strcatn(char *d, const char *s, SInt32 max);
extern char* __stdcall strcpyn(char *d, const char *s, SInt32 len, SInt32 max);
extern int __stdcall ustrcmp(const char *src, const char *dst);
extern int __stdcall ustrncmp(const char *src, const char *dst, UInt32 len);

#endif
