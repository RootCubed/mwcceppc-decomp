#ifndef COMPILER_OBJECTS_H
#define COMPILER_OBJECTS_H

#include <compiler/common.h>
#include <compiler/tokens.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif


typedef enum ObjectType {
    OT_ENUMCONST,
    OT_TYPE,
    OT_TYPETAG,
    OT_NAMESPACE,
    OT_MEMBERVAR,
    OT_OBJECT,
    OT_ILLEGAL
} ObjectType;


struct ObjectList {
    ObjectList *next;
    Object *object;
};


/// General structure with all shared fields for all kinds of objects
struct ObjBase {
    ObjectType otype;
    AccessType access;
};


/// Type 0 (OT_ENUMCONST)
struct ObjEnumConst {
    ObjectType otype;
    AccessType access;
    ObjEnumConst *next;
    HashNameNode *name;
    Type *type;
    CInt64 val;
};


/// Type 1 (OT_TYPE)
struct ObjType {
    ObjectType otype;
    AccessType access;
    Type *type;
    UInt32 qual;
};


/// Type 2 (OT_TYPETAG)
struct ObjTypeTag {
    ObjectType otype;
    AccessType access;
    Type *type;
};


/// Type 3 (OT_NAMESPACE)
struct ObjNameSpace {
    ObjectType otype;
    AccessType access;
    NameSpace *nspace;
};


/// Type 4 (OT_MEMBERVAR)
struct ObjMemberVar {
    ObjectType otype;
    AccessType access;
    Boolean anonunion;
    Boolean has_path;
    struct ObjMemberVar *next;
    HashNameNode *name;
    UInt32 m_0c; // From Wii 1.0 onwards
    Type *type;
    UInt32 qual;
    UInt32 offset;
};
struct ObjMemberVarPath {
    ObjectType otype;
    AccessType access;
    Boolean anonunion;
    Boolean has_path;
    struct ObjMemberVar *next;
    HashNameNode *name;
    Type *type;
    UInt32 qual;
    UInt32 offset;
    BClassList *path;
};


typedef enum DataType {
    DDATA,
    DLOCAL,
    DABSOLUTE,
    DFUNC,
    DVFUNC,
    DINLINEFUNC,
    DALIAS,
    DEXPR,
    DNONLAZYPTR,
    DLABEL,
    DUNUSED
} DataType;

/// Type 5 (OT_OBJECT)
struct Object {
    ObjectType otype;
    AccessType access;
    DataType datatype;
    Section section;
    UInt8 pad[4];
    NameSpace *nspace;
    HashNameNode *name;
    Type *type;
    UInt32 qual;
    UInt8 pad2[2];
    SInt16 sclass;
    UInt8 flags;
    ExtendedParam *extParam;
    Object *toc;
    VarRecord *varptr;
    union {
        struct {
            union {
                CInt64 intconst;
                Float *floatconst;
                MWVector128 *vector128const;
                char *string;
                struct {
                    char *data;
                    SInt32 size;
                } switchtable;
            } u;
            VarInfo *info;
            HashNameNode *linkname;
            Boolean islocalstatic;
        } data;
        UInt32 address;
        struct {
            VarInfo *info;
            HashNameNode *linkname;
            Object *over_load;
        } toc;
        struct {
            union {
                TemplateFunction *templ;
                CI_FuncData *ifuncdata;
                SInt32 intrinsicid;
            } u;
            DefArgCtorInfo *defargdata;
            HashNameNode *linkname;
            TemplFuncInstance *inst;
            PTFList *ptfList;
            ObjectList *argList;
        } func; // Used with DFUNC and DVFUNC
        struct {
            char *data;
            SInt32 size;
            InlineXRef *xrefs;
        } ifunc; // Used with DINLINEFUNC
        struct {
            VarInfo *info;
            SInt32 uid;
            // SInt32 offset; // ???
            Object *realObj;
        } var;
        struct {
            Object *object;
            //TypeClass *member;
            BClassList *member; // ???
            SInt32 offset;
        } alias;
        struct {
            Object *function;
            HashNameNode *labelname;
        } label;
        ENode *expr;
    } u;
};

struct ObjectTemplated {
    Object object;
    Object *parent;
};

enum {
    OBJECT_USED = 1,
    OBJECT_FLAGS_2 = 2, // this object has been used in certain contexts (figure me out?)
    OBJECT_DEFINED = 4,
    OBJECT_LAZY = 8, // this object will be created by a CParser callback action only if referenced
    OBJECT_INTERNAL = 0x10,
    OBJECT_IMPORT = 0x20,
    OBJECT_EXPORT = 0x40,
    OBJECT_LIB_EXPORT = 0x60
};

#define OBJ_BASE(obj) ((ObjBase *) (obj))
#define OBJ_ENUM_CONST(obj) ((ObjEnumConst *) (obj))
#define OBJ_TYPE(obj) ((ObjType *) (obj))
#define OBJ_TYPE_TAG(obj) ((ObjTypeTag *) (obj))
#define OBJ_NAMESPACE(obj) ((ObjNameSpace *) (obj))
#define OBJ_MEMBER_VAR(obj) ((ObjMemberVar *) (obj))
#define OBJ_MEMBER_VAR_PATH(obj) ((ObjMemberVarPath *) (obj))
#define OBJECT(obj) ((Object *) (obj))
#define OBJECT_TEMPL(obj) ((ObjectTemplated *) (obj))


struct VarInfo {
    Object *func;
    SInt32 usage;
    TStreamElement deftoken;
    SInt16 varnumber;
    Boolean noregister;
    Boolean used;
    UInt8 flags;
    UInt8 rclass;
    SInt16 reg;
    SInt16 regHi;
};

enum {
    VarInfoFlag1 = 1, // is parameter?
    VarInfoFlag2 = 2, // spill related?
    VarInfoFlag4 = 4, // 64-bit spill related?
    VarInfoFlag40 = 0x40,
    VarInfoFlag80 = 0x80
};

#define OBJECT_OR_ALIAS(_object) ( ((_object)->datatype == DALIAS) ? ((_object)->u.alias.object) : (_object) )

// placing these here until further notice

// unknown name, mwcppc.exe 7.0: 484870
CW_INLINE Boolean Inline_IsObjectData(Object *object) {
    return object->datatype == DDATA;
}

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
