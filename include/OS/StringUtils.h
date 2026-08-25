#ifndef OS_STRING_UTILS_H
#define OS_STRING_UTILS_H

#include <common.h>
#include <OS/OS.h>

extern StringPtr _pstrcpy(StringPtr dst, ConstStringPtr src);
extern void _pstrcat(StringPtr dst, ConstStringPtr src);
extern void _pstrcharcat(StringPtr to, char ch);
extern void pstrncpy(StringPtr to, ConstStringPtr from, int max);
extern void pstrncat(StringPtr to, ConstStringPtr append, int max);
extern int pstrcmp(ConstStringPtr a, ConstStringPtr b);
extern int pstrchr(ConstStringPtr str, char find);
extern void c2pstrcpy(StringPtr dst, const char *src);
extern void p2cstrcpy(char *dst, ConstStringPtr src);
extern char* mvprintf(char *mybuf, unsigned int len, const char *format, va_list va);
extern char* mprintf(char *mybuf, unsigned int len, const char *format, ...);
extern int HPrintF(Handle text, const char *format, ...);

#endif
