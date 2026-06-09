#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H

#include <cw_common.h>

enum { CR = 13, LF = 10 };

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

typedef struct HashNameNode {
    struct HashNameNode *next;
    SInt32 id;
    SInt16 hashval;
    char name[];
} HashNameNode;

// TODO move me into CPrep.h or something?
struct CPrepFileInfo {
    FSSpec textfile;
    HashNameNode *nameNode;
    SInt32 fileModDate;
    char *textbuffer;
    SInt32 textlength;
    SInt32 linenumber;
    SInt32 pos;
    Boolean hasprepline;
    SInt16 fileID;
    Boolean recordbrowseinfo;
    Boolean unkfield123;
    Boolean unkfield124;
    Boolean unkfield125;
    Boolean unkfield126;
};

typedef struct CInt64 {
    SInt32 hi;
    UInt32 lo;
} CInt64;

typedef struct Float {
    double value;
} Float;

typedef union MWVector128 {
    UInt8 uc[16];
    UInt16 us[8];
    UInt32 ul[4];
    float f[4];
} MWVector128;

typedef enum AccessType {
    ACCESSPUBLIC,
    ACCESSPRIVATE,
    ACCESSPROTECTED,
    ACCESSNONE
} AccessType;

typedef enum Section {
    SECT_DEFAULT = 0,
    SECT_TEXT = 1,
    SECT_DATA = 2,
    SECT_UDATA = 3,
    SECT_DATA_FAR = 4,
    SECT_DATA_NEAR = 5,
    SECT_INIT = 6,
    SECT_OBJC_CAT_CLS_METH = 7,
    SECT_OBJC_CAT_INST_METH = 8,
    SECT_OBJC_STRING_OBJECT = 9,
    SECT_OBJC_CSTRING_OBJECT = 10,
    SECT_OBJC_MESSAGE_REFS = 11,
    SECT_OBJC_SEL_FIXUP = 12,
    SECT_OBJC_CLS_REFS = 13,
    SECT_OBJC_CLASS = 14,
    SECT_OBJC_META_CLASS = 15,
    SECT_OBJC_CLS_METH = 16,
    SECT_OBJC_INST_METH = 17,
    SECT_OBJC_PROTOCOL = 18,
    SECT_OBJC_CLASS_NAMES = 19,
    SECT_OBJC_METH_VAR_TYPES = 20,
    SECT_OBJC_METH_VAR_NAMES = 21,
    SECT_OBJC_CATEGORY = 22,
    SECT_OBJC_CLASS_VARS = 23,
    SECT_OBJC_INSTANCE_VARS = 24,
    SECT_OBJC_MODULE_INFO = 25,
    SECT_OBJC_MSYMBOLS = 26,
    SECT_TEXT_CSTRING = 27,
    SECT_BACKEND_SPECIFIC = 28,
    SECT_MW_EX_TABLE = 28,
    SECT_MW_EX_TABLE_INDEX = 29,
    SECT_MW_SWITCH = 30,
    SECT_8BYTE_LITERALS = 31,
    SECT_4BYTE_LITERALS = 32,
    SECT_MOD_INIT_FUNC = 33,
    SECT_CONST = 34,
    SECT_CONST_PTR = 35,
    SECT_NONLAZY_PTRS = 36,
    SECT_COMMON_VARS = 37,
    SECT_16BYTE_LITERALS = 38,
    SECT_TEXT_COALESCE = 39,
    SECT_DATA_COALESCE = 40,
    SECT_UDATA_COALESCE = 41,
    SECT_CONST_COALESCE = 42,
    SECT_CONST_PTR_COALESCE = 43,
    //SECT_CSTR_COALESCE = 44,
    N_SECTIONS = 44
} Section;

typedef struct Alias Alias;
typedef struct BClassList BClassList;
typedef struct CI_FuncData CI_FuncData;
typedef struct CLabel CLabel;
typedef struct ClassLayout ClassLayout;
typedef struct ClassList ClassList;
typedef struct CompilerLinkerParamBlk CompilerLinkerParamBlk;
typedef struct CPrepFileInfo CPrepFileInfo;
typedef struct CScopeSave CScopeSave;
typedef struct DeclFucker DeclFucker;
typedef struct DeclInfo DeclInfo;
typedef struct DeclThing DeclThing; // rename me please
typedef struct DeduceInfo DeduceInfo;
typedef struct DefAction DefAction;
typedef struct DefArgCtorInfo DefArgCtorInfo;
typedef struct DepName DepName;
typedef struct EMemberInfo EMemberInfo;
typedef struct ENode ENode;
typedef struct ENodeList ENodeList;
typedef struct ExceptionAction ExceptionAction;
typedef struct ExceptSpecList ExceptSpecList;
typedef struct ExtendedParam ExtendedParam;
typedef struct FileOffsetInfo FileOffsetInfo;
typedef struct FuncArg FuncArg;
typedef struct GList GList;
typedef struct Initializer Initializer;
typedef struct InlineXRef InlineXRef;
typedef struct Macro Macro;
typedef struct MemInitializer MemInitializer;
typedef struct NameSpace NameSpace;
typedef struct NameSpaceName NameSpaceName;
typedef struct NameSpaceList NameSpaceList;
typedef struct NameSpaceLookupList NameSpaceLookupList;
typedef struct NameSpaceObjectList NameSpaceObjectList;
typedef struct ObjBase ObjBase;
typedef struct ObjEnumConst ObjEnumConst;
typedef struct ObjMemberVar ObjMemberVar;
typedef struct ObjMemberVarPath ObjMemberVarPath;
typedef struct ObjNameSpace ObjNameSpace;
typedef struct ObjType ObjType;
typedef struct ObjTypeTag ObjTypeTag;
typedef struct Object Object;
typedef struct ObjectList ObjectList;
typedef struct ObjectTemplated ObjectTemplated;
typedef struct ObjCCategory ObjCCategory;
typedef struct ObjCInfo ObjCInfo;
typedef struct ObjCMethod ObjCMethod;
typedef struct ObjCMethodArg ObjCMethodArg;
typedef struct ObjCMethodList ObjCMethodList;
typedef struct ObjCNamedArg ObjCNamedArg;
typedef struct ObjCProtocol ObjCProtocol;
typedef struct ObjCProtocolList ObjCProtocolList;
typedef struct ObjCSelector ObjCSelector;
typedef struct OLinkList OLinkList;
typedef struct Operand Operand;
typedef struct PackedDeclInfo PackedDeclInfo;
typedef struct PCode PCode;
typedef struct PCodeArg PCodeArg;
typedef struct PCodeBlock PCodeBlock;
typedef struct PCodeLabel PCodeLabel;
typedef struct PointsToFunction PointsToFunction;
typedef struct PTFList PTFList;
typedef struct SOMInfo SOMInfo;
typedef struct Statement Statement;
typedef struct StructMember StructMember;
typedef struct SwitchCase SwitchCase;
typedef struct SwitchInfo SwitchInfo;
typedef struct TemplateAction TemplateAction;
typedef struct TemplateFriend TemplateFriend;
typedef struct TemplateMember TemplateMember;
typedef struct TemplArg TemplArg;
typedef struct TemplClass TemplClass;
typedef struct TemplClassInst TemplClassInst;
typedef struct TemplateFunction TemplateFunction;
typedef struct TemplFuncInstance TemplFuncInstance;
typedef struct TemplParam TemplParam;
typedef struct TemplPartialSpec TemplPartialSpec;
typedef struct TemplStack TemplStack;
typedef struct TokenStream TokenStream;
typedef struct TStreamElement TStreamElement;
typedef struct Type Type;
typedef struct TypeBitfield TypeBitfield;
typedef struct TypeClass TypeClass;
typedef struct TypeDeduce TypeDeduce;
typedef struct TypeEnum TypeEnum;
typedef struct TypeFunc TypeFunc;
typedef struct TypeIntegral TypeIntegral;
typedef struct TypeList TypeList;
typedef struct TypeMemberPointer TypeMemberPointer;
typedef struct TypeMemberFunc TypeMemberFunc;
typedef struct TypeObjCID TypeObjCID;
typedef struct TypePointer TypePointer;
typedef struct TypeStruct TypeStruct;
typedef struct TypeTemplDep TypeTemplDep;
typedef struct VarInfo VarInfo;
typedef struct VarRecord VarRecord;
typedef struct VClassList VClassList;

// Common bits for templates
typedef enum TemplParamType {
    TPT_TYPE = 0,
    TPT_NONTYPE,
    TPT_TEMPLATE
} TemplParamType;

typedef struct TemplParamID {
    UInt16 index;
    UInt8 nindex;
    TemplParamType type;
} TemplParamID;

typedef enum TemplDepSubType {
    TDE_PARAM,
    TDE_SIZEOF,
    TDE_ALIGNOF,
    TDE_CAST,
    TDE_QUALNAME,
    TDE_OBJ,
    TDE_SOURCEREF,
    TDE_ADDRESS_OF
} TemplDepSubType;

/// Qualifiers
enum {
    Q_CONST = 1,
    Q_VOLATILE = 2,
    Q_CV = Q_CONST | Q_VOLATILE,
    Q_ASM = 4,
    Q_PASCAL = 8,
    Q_INLINE = 0x10,
    Q_REFERENCE = 0x20,
    Q_EXPLICIT = 0x40,
    Q_MUTABLE = 0x80,
    Q_VIRTUAL = 0x100,
    Q_FRIEND = 0x200,
    Q_IN = 0x400,
    Q_OUT = 0x800,
    Q_INOUT = 0x1000,
    Q_BYCOPY = 0x2000,
    Q_BYREF = 0x4000,
    Q_ONEWAY = 0x8000,
    Q_INLINE_DATA = 0x10000, // this object contains constant data in its union
    Q_20000 = 0x20000,
    Q_WEAK = 0x40000, // weak
    Q_MANGLE_NAME = 0x80000, // adds C++ mangling info to a name
    Q_IS_OBJC_ID = 0x100000, // Objective-C ID
    Q_RESTRICT = 0x200000,
    Q_IS_TEMPLATED = 0x400000, // is templated from another object?
    Q_1000000 = 0x1000000,
    Q_ALIGNED_1 = 0x2000000,
    Q_ALIGNED_2 = 0x4000000,
    Q_ALIGNED_4 = 0x6000000,
    Q_ALIGNED_8 = 0x8000000,
    Q_ALIGNED_16 = 0xA000000,
    Q_ALIGNED_32 = 0xC000000,
    Q_ALIGNED_64 = 0x10000000,
    Q_ALIGNED_128 = 0x12000000,
    Q_ALIGNED_256 = 0x14000000,
    Q_ALIGNED_512 = 0x16000000,
    Q_ALIGNED_1024 = 0x18000000,
    Q_ALIGNED_2048 = 0x1A000000,
    Q_ALIGNED_4096 = 0x1C000000,
    Q_ALIGNED_8192 = 0x1E000000,
    Q_ALIGNED_MASK = 0x1E000000
};

enum {
    EXPORT_FLAGS_INTERNAL = 0x10,
    EXPORT_FLAGS_IMPORT = 0x20,
    EXPORT_FLAGS_EXPORT = 0x40
};

typedef enum RegClass {
    RegClass_Invalid = -1,
    RegClass_SPR = 0,
    RegClass_CRFIELD = 1,
    RegClass_VR = 2,
    RegClass_FPR = 3,
    RegClass_GPR = 4,
    RegClassMax = 5,
    RegClass_6 = 6,
    RegClass_DCR = 7
} RegClass;

struct CompilerLinkerParamBlk {
    CWPluginContext context;
    CWObjectData object;
    Handle objectdata;
    Handle browsedata;
    SInt32 request;
    SInt32 version;
    FSSpec targetfile;
    SInt32 numfiles;
    SInt32 whichfile;
    FSSpec sourcefile;
    const char *sourcetext;
    SInt32 sourcetextsize;
    Boolean precompile;
    Boolean autoprecompile;
    Boolean preprocess;
    Boolean SYMinfo;
    Boolean caching_includes;
    CWBrowseOptions browseoptions;
    Boolean recordbrowseinfo;
    SInt16 browserfileID;
    CWDataType targetOS;
    CWDataType targetCPU;
    char *idetargetname;
};

#define FITS_IN_SHORT(value) ( (value) == ((short) (value)) )
#define FITS_IN_USHORT(value) ( (value) == ((unsigned short) (value)) )
#define FITS_IN_HI_SHORT(value) ( (value) == (value & 0xFFFF0000) )
#define FITS_IN_SHORT2(value) ( ((short) (value)) == (value) )
#define HIGH_PART(value) ( (short) (((value) >> 16) + (((value) >> 15) & 1)) )
#define LOW_PART(value) ( (short) (value) )

// The PowerPC code generator has a bug which breaks the PowerPC code generator,
// causing 32-bit value loads to emit "addi x, y, 0" even if the lower 16 bits are
// zero. This define simulates that behaviour on other platforms
#ifdef CW_REPLICATE_PPC_CODEGEN_BUG
#define LOW_PART_BUGGY(value) (value)
#else
#define LOW_PART_BUGGY(value) ( (short) (value) )
#endif


#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
