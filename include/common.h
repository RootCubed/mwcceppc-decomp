#ifndef ROOT_COMMON_H
#define ROOT_COMMON_H

/*
 * Common imports and Mac OS types
 */
#include <ctype.h>
#include <float.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __MWERKS__
#define macintosh
#define CW_PASCAL pascal
#undef toupper
#undef tolower

#ifdef CW_CLT
// What the fuck is this haha
// typedef char * va_list;
#define va_start(a,b) (a = ((va_list) __builtin_next_arg(b)))
#define va_arg(a,b) (*(b *) (void *) ((a = (char *) (((((unsigned long)(a)) + ((__alignof__ (b) == 16) ? 15 : 3)) & ~((__alignof__ (b) == 16) ? 15 : 3)) + ((sizeof (b) + 3) & ~3))) - ((sizeof (b) + 3) & ~3)))
#define va_end(a) ((void)0)
#define alloca(x) __alloca(x)
#else
#include <stdarg.h>
#endif

#define CW_INLINE inline

// format string specifiers
#define PRIxPTR "lx"

#define PRIx32 "lx"
#define PRIX32 "lX"
#define PRId32 "ld"

#else
#include <inttypes.h>
#include <stdarg.h>
// expand this to nothing
#define CW_PASCAL

#define CW_INLINE static inline
#endif

// ----------
// MacTypes.h
#ifdef __MWERKS__
typedef unsigned char UInt8;
typedef char SInt8;
typedef unsigned short UInt16;
typedef short SInt16;
typedef unsigned long UInt32;
typedef long SInt32;
typedef unsigned long long UInt64;
typedef long long SInt64;

typedef long Size;

typedef unsigned char Boolean;
#else
typedef uint8_t UInt8;
typedef int8_t SInt8;
typedef uint16_t UInt16;
typedef int16_t SInt16;
typedef uint32_t UInt32;
typedef int32_t SInt32;
typedef uint64_t UInt64;
typedef int64_t SInt64;

typedef int32_t Size;

typedef uint8_t Boolean;
#endif

typedef char *Ptr;
typedef Ptr *Handle;

typedef SInt16 OSErr;
typedef SInt32 OSStatus;
typedef SInt16 ScriptCode;
typedef UInt32 FourCharCode;
typedef FourCharCode OSType;
typedef FourCharCode ResType;
typedef OSType *OSTypePtr;
typedef ResType *ResTypePtr;

enum {
    noErr = 0,
    abortErr = -27,
    nsvErr = -35,
    ioErr = -36,
    bdNamErr = -37,
    eofErr = -39,
    tmfoErr = -42,
    fnfErr = -43, // file not found
    dupFNErr = -48, // duplicate filename
    paramErr = -50,
    rfNumErr = -51,
    permErr = -54,
    memFullErr = -108, // not enough memory
    nilHandleErr = -109, // pointer was nil
    memLockedErr = -117,
    dirNFErr = -120,
    userCanceledErr = -128,
    inputOutOfBounds = -190,
    resNotFound = -192, // resource not found
    resFNotFound = -193, // resource file not found
    addResFailed = -194, // AddResource failed
    rmvResFailed = -196, // RmveResource failed
    resAttrErr = -198, // inconsistent attribute
    mapReadErr = -199, // map inconsistent with operation
    notAFileErr = -1302,
    fsDataTooBigErr = -1310
};
enum { kNilOptions = 0 };
enum { kUnknownType = 0x3F3F3F3F };

// Pascal strings
typedef unsigned char Str255[256];
typedef unsigned char Str63[64];
typedef unsigned char *StringPtr;
typedef const unsigned char *ConstStringPtr;
typedef const unsigned char *ConstStr63Param;
typedef const unsigned char *ConstStr255Param;
// XXX: MacTypes.h defines StrLength() which is an inline for C++ and a macro for C
// This might be useful?

typedef struct Point {
    SInt16 v, h;
} Point;

typedef struct Rect {
    SInt16 top, left, bottom, right;
} Rect;

// -------
// Files.h
enum {
    fsCurPerm = 0x00,
    fsRdPerm = 0x01,
    fsWrPerm = 0x02,
    fsRdWrPerm = 0x03,
    fsRdWrShPerm = 0x04,
    fsRdDenyPerm = 0x10,
    fsWrDenyPerm = 0x20
};
enum {
    fsAtMark = 0,
    fsFromStart = 1,
    fsFromLEOF = 2,
    fsFromMark = 3
};
enum {
    fsRtParID = 1,
    fsRtDirID = 2
};
enum {
    ioDirMask = 0x10
};
enum {
    kIsAlias = 0x8000
};

typedef struct FSSpec {
    SInt16 vRefNum;
    SInt32 parID;
#ifdef CW_CLT
    Str255 name;
#else
    Str63 name;
#endif
} FSSpec;

typedef struct FInfo {
    OSType fdType;
    OSType fdCreator;
    UInt16 fdFlags;
    Point fdLocation;
    SInt16 fdFldr;
} FInfo;

typedef struct FXInfo {
    SInt16 fdIconID;
    SInt16 fdUnused[3];
    SInt8 fdScript;
    SInt8 fdXFlags;
    SInt16 fdComment;
    SInt32 fdPutAway;
} FXInfo;

typedef struct DInfo {
    Rect frRect;
    SInt16 frFlags;
    Point frLocation;
    SInt16 frView;
} DInfo;

typedef struct DXInfo {
    Point frScroll;
    SInt32 frOpenChain;
    SInt16 frUnused;
    SInt16 frComment;
    SInt32 frPutAway;
} DXInfo;

typedef struct IOParam {
    OSErr ioResult;
    SInt16 ioRefNum;
    char *ioBuffer;
    SInt32 ioReqCount;
    SInt32 ioActCount;
    SInt16 ioPosMode;
    SInt32 ioPosOffset;
} IOParam;

typedef union ParamBlockRec {
    IOParam ioParam;
} ParamBlockRec;
typedef ParamBlockRec *ParmBlkPtr;

typedef struct HFileParam {
    void *qLink;
    SInt16 qType;
    SInt16 ioTrap;
    Ptr ioCmdAddr;
    void *ioCompletion;
    SInt16 ioResult;
    StringPtr ioNamePtr;
    SInt16 ioVRefNum;
    SInt16 ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    SInt16 ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioFlVersNum;
    FInfo ioFlFndrInfo;
    SInt32 ioDirID;
    UInt16 ioFlStBlk;
    SInt32 ioFlLgLen;
    SInt32 ioFlPyLen;
    UInt16 ioFlRStBlk;
    SInt32 ioFlRLgLen;
    SInt32 ioFlRPyLen;
    UInt32 ioFlCrDat;
    UInt32 ioFlMdDat;
} HFileParam;

typedef union HParamBlockRec {
    HFileParam fileParam;
} HParamBlockRec;
typedef HParamBlockRec *HParmBlkPtr;

typedef struct HFileInfo {
    void *qLink;
    SInt16 qType;
    SInt16 ioTrap;
    Ptr ioCmdAddr;
    void *ioCompletion;
    SInt16 ioResult;
    StringPtr ioNamePtr;
    SInt16 ioVRefNum;
    SInt16 ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    SInt16 ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioACUser;
    FInfo ioFlFndrInfo;
    SInt32 ioDirID;
    UInt16 ioFlStBlk;
    SInt32 ioFlLgLen;
    SInt32 ioFlPyLen;
    UInt16 ioFlRStBlk;
    SInt32 ioFlRLgLen;
    SInt32 ioFlRPyLen;
    UInt32 ioFlCrDat;
    UInt32 ioFlMdDat;
    UInt32 ioFlBkDat;
    FXInfo ioFlXFndrInfo;
    SInt32 ioFlParID;
    SInt32 ioFlClpSiz;
} HFileInfo;
typedef struct DirInfo {
    void *qLink;
    SInt16 qType;
    SInt16 ioTrap;
    Ptr ioCmdAddr;
    void *ioCompletion;
    SInt16 ioResult;
    StringPtr ioNamePtr;
    SInt16 ioVRefNum;
    SInt16 ioFRefNum;
    SInt8 ioFVersNum;
    SInt8 filler1;
    SInt16 ioFDirIndex;
    SInt8 ioFlAttrib;
    SInt8 ioACUser;
    DInfo ioDrUsrWds;
    SInt32 ioDrDirID;
    UInt16 ioDrNmFls;
    SInt16 filler3[9];
    UInt32 ioDrCrDat;
    UInt32 ioDrMdDat;
    UInt32 ioDrBkDat;
    DXInfo ioDrFndrInfo;
    SInt32 ioDrParID;
} DirInfo;

typedef union CInfoPBRec {
    HFileInfo hFileInfo;
    DirInfo hDirInfo;
} CInfoPBRec;
typedef CInfoPBRec *CInfoPBPtr;


#endif
