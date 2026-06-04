#ifndef COMPILER_COMPILERTOOLS_H
#define COMPILER_COMPILERTOOLS_H

#include <compiler/common.h>

#ifdef __LITTLE_ENDIAN__
#define ENDIAN_CONVERSION
#endif

extern void CompilerGetPString(short index, unsigned char *string);
extern void CompilerGetCString(short index, char *string);
extern unsigned char *CTool_CtoPstr(char *cstr);

typedef struct HeapBlock {
	struct HeapBlock *next;
	Handle blockhandle;
	SInt32 blocksize;
	SInt32 blockfree;
} HeapBlock;

typedef struct HeapMem {
	HeapBlock *blocks;
    /// The minimum size for a new block
	SInt32 allocsize;
	HeapBlock *curblock;
	char *curfreep;
	SInt32 curfree;
} HeapMem;

typedef struct _HeapInfo {
    /// The amount of blocks in this heap
    SInt32 blocks;
    /// The value of allocsize for the heap
    SInt32 allocsize;
    /// The total amount of bytes across all blocks
    SInt32 total_size;
    /// The amount of free bytes across all blocks
    SInt32 total_free;
    /// The average size of a block
    SInt32 average_block_size;
    /// The average amount of free space in a block
    SInt32 average_block_free;
    /// The amount of bytes in the largest contiguous section of free space
    SInt32 largest_free_block;
} HeapInfo;

struct GList {
	char **data;
	SInt32 size;
	SInt32 hndlsize;
	SInt32 growsize;
};

extern long hash_name_id;
extern HashNameNode **name_hash_nodes;

#ifdef __LP64__
#define CW_64_BIT_SUPPORT
#endif

// These don't exist in the original source, but are kind of necessary in the 2020s
#ifdef CW_64_BIT_SUPPORT
extern void *CTool_ResolveIndexToPointer(UInt32 index);
extern UInt32 CTool_CreateIndexFromPointer(void *ptr);
#else
#define CTool_ResolveIndexToPointer(index) ((void *) (index))
#define CTool_CreateIndexFromPointer(ptr) ((UInt32) (ptr))
#endif

extern void (*GListErrorProc)(void);

extern short InitGList(GList *gl, SInt32 size);
extern void FreeGList(GList *gl);
extern void LockGList(GList *gl);
extern void UnlockGList(GList *gl);
extern void ShrinkGList(GList *gl);
extern void AppendGListData(GList *gl, const void *data, SInt32 size);
extern void AppendGListNoData(GList *gl, SInt32 size);
extern void AppendGListByte(GList *gl, SInt8 thebyte);
extern void AppendGListWord(GList *gl, SInt16 theword);
extern void AppendGListTargetEndianWord(GList *gl, SInt16 theword);
extern void AppendGListLong(GList *gl, SInt32 theword);
extern void AppendGListTargetEndianLong(GList *gl, SInt32 theword);
extern void AppendGListID(GList *gl, const char *name);
extern void AppendGListName(GList *gl, const char *name);
extern void RemoveGListData(GList *gl, SInt32 size);
extern SInt16 GetGListByte(GList *gl);
extern SInt16 GetGListWord(GList *gl);
extern SInt32 GetGListLong(GList *gl);
extern short GetGListID(GList *gl, char *name);
extern void GetGListData(GList *gl, char *where, SInt32 size);

extern SInt16 CHash(const char *string);
extern HashNameNode *GetHashNameNode(const char *name);
extern HashNameNode *GetHashNameNodeHash(const char *name, SInt16 hashval);
extern HashNameNode *GetHashNameNodeHash2(const char *name, SInt16 hashval);
extern HashNameNode *GetHashNameNodeExport(const char *name);
extern SInt32 GetHashNameNodeExportID(HashNameNode *node);
extern HashNameNode *GetHashNameNodeByID(SInt32 id);
extern void NameHashExportReset(void);
extern void NameHashWriteNameTable(GList *glist);
extern void NameHashWriteTargetEndianNameTable(GList *glist);
extern void InitNameHash(void);

typedef void (*heaperror_t)(void);

extern SInt32 CTool_TotalHeapSize(void);
extern void CTool_GetHeapInfo(HeapInfo *result, unsigned char heapID);
extern short initheaps(heaperror_t heaperrorproc);
extern short initgheap(heaperror_t heaperrorproc);
extern heaperror_t getheaperror(void);
extern void setheaperror(heaperror_t heaperrorproc);
extern void releaseheaps(void);
extern void releasegheap(void);
extern void releaseoheap(void);
extern void *galloc(SInt32 s);
extern void *lalloc(SInt32 s);
extern void *aalloc(SInt32 s);
extern void *oalloc(SInt32 s);
extern void *balloc(SInt32 s);

extern void locklheap(void);
extern void unlocklheap(void);
extern void freelheap(void);
extern void freeaheap(void);
extern void freeoheap(void);
extern void freebheap(void);

extern char *ScanHex(char *string, UInt32 *result, Boolean *overflow);
extern char *ScanOct(char *string, UInt32 *result, Boolean *overflow);
extern char *ScanDec(char *string, UInt32 *result, Boolean *overflow);

extern void OldUnmangle(char *name, char *out, Boolean full);

extern short hash(char *a);
extern void memclr(void *ptr, SInt32 size);
extern void memclrw(void *ptr, SInt32 size);
extern void CToLowercase(char *a, char *b);
extern short getbit(SInt32 l);
#ifdef ENDIAN_CONVERSION
extern UInt16 CTool_EndianConvertWord16(UInt16 theword);
extern UInt32 CTool_EndianConvertWord32(UInt32 theword);
extern void CTool_EndianConvertMem(UInt8 *data, short len);
extern UInt32 CTool_EndianReadWord32(void *ptr);
#else
#define CTool_EndianConvertWord32(value) value
#define CTool_EndianConvertWord16(value) value
#define CTool_EndianConvertMem(data, len) ((void)0)
#define CTool_EndianReadWord32(ptr) (*((SInt32 *) (ptr)))
#endif
extern void CTool_EndianConvertWord64(CInt64 ci, char *result);
extern UInt16 CTool_EndianConvertInPlaceWord16Ptr(UInt16 *x);
extern UInt32 CTool_EndianConvertInPlaceWord32Ptr(UInt32 *x);
extern void CTool_EndianConvertVector128(); // not correct but idc
extern HashNameNode *CTool_GetPathName(const FSSpec *fss, SInt32 *moddateptr);
extern int strcat_safe(char *dest, const char *src, SInt32 len);

#endif
