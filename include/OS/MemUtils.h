#ifndef OS_MEM_UTILS_H
#define OS_MEM_UTILS_H

#include <common.h>
#include <OS/OS.h>

extern void* __stdcall xmalloc(const char *what, int size);
extern void* __stdcall xcalloc(const char *what, int size);
extern void* __stdcall xmalloc_or_null(int size);
extern void* __stdcall xcalloc_or_null(int size);
extern void* __stdcall xrealloc(const char *what, void *old, int size);
extern char* __stdcall xstrdup(const char *str);
extern void __stdcall xfree(void *ptr);

#endif