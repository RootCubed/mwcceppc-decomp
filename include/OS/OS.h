#ifndef OS_OS_H
#define OS_OS_H

#include <common.h>
#include <Windows.h>

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
    char s[MAX_PATH];
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

#endif