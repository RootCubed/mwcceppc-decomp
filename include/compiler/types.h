#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <compiler/common.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

typedef enum TypeType {
    TYPEVOID = 0,
    TYPEINT,
    TYPEFLOAT,
    TYPE_3, // From GC 3.0 onwards
    TYPE_4, // From Wii 1.1 onwards
    TYPEENUM,
    TYPESTRUCT,
    TYPECLASS,
    TYPEFUNC,
    TYPEBITFIELD,
    TYPELABEL,
    TYPETEMPLATE,
    TYPEMEMBERPOINTER,
    TYPEPOINTER,
    TYPEARRAY,
    TYPEOBJCID,
    TYPETEMPLDEPEXPR,
    TYPEILLEGAL = -1
} TypeType;

/// Common fields across all kinds of types
struct Type {
    TypeType type;
    SInt32 size;
};


typedef enum IntegralType {
    IT_BOOL = 0,
    IT_CHAR = 1,
    IT_SCHAR = 2,
    IT_UCHAR = 3,
    IT_WCHAR_T = 4,
    IT_SHORT = 5,
    IT_USHORT = 6,
    IT_INT = 7,
    IT_UINT = 8,
    IT_LONG = 9,
    IT_ULONG = 10,
    IT_LONGLONG = 11,
    IT_ULONGLONG = 12,
    IT_FLOAT = 13,
    IT_SHORTDOUBLE = 14,
    IT_DOUBLE = 15,
    IT_LONGDOUBLE = 16,
    IT_17 = 17,
    IT_18 = 18,
    IT_19 = 19,
    IT_20 = 20,
    IT_21 = 21,
    IT_22 = 22,
    IT_23 = 23,
    IT_24 = 24
} IntegralType;

// This is probably actually called AtomType / TypeAtom
struct TypeIntegral {
    TypeType type;
    SInt32 size;
    unsigned char integral;
};


struct TypeEnum {
    TypeType type;
    SInt32 size;
    NameSpace *nspace;
    ObjEnumConst *enumlist;
    Type *enumtype;
    HashNameNode *enumname;
};


struct TypeStruct {
    TypeType type;
    SInt32 size;
    HashNameNode *name;
    StructMember *members;
    char stype;
    SInt16 align;
};
struct StructMember {
    StructMember *next;
    Type *type;
    HashNameNode *name;
    SInt32 offset;
    UInt32 qual;
};

enum {
    STRUCT_TYPE_STRUCT = 0,
    STRUCT_TYPE_UNION = 1,
    STRUCT_TYPE_CLASS = 2,
    STRUCT_TYPE_MAX = 3,
    STRUCT_VECTOR_UCHAR = 4,
    STRUCT_VECTOR_SCHAR = 5,
    STRUCT_VECTOR_BCHAR = 6,
    STRUCT_VECTOR_USHORT = 7,
    STRUCT_VECTOR_SSHORT = 8,
    STRUCT_VECTOR_BSHORT = 9,
    STRUCT_VECTOR_UINT = 10,
    STRUCT_VECTOR_SINT = 11,
    STRUCT_VECTOR_BINT = 12,
    STRUCT_VECTOR_FLOAT = 13,
    STRUCT_VECTOR_PIXEL = 14
};


struct ClassList {
    ClassList *next;
    TypeClass *base;
    SInt32 offset;
    SInt32 voffset;
    AccessType access;
    Boolean is_virtual;
};

struct VClassList {
    VClassList *next;
    TypeClass *base;
    SInt32 offset; // offset within the class instance
    SInt32 voffset; // offset within the vtable
    Boolean has_override;
};

typedef struct ClassListList {
    struct ClassListList *next;
    ClassList *base;
    VClassList *vbase;
    Boolean m_0c;
    Boolean m_0d;
} ClassListList;

typedef struct ClassFriend {
    struct ClassFriend *next;
    union {
        TypeClass *theclass;
        Object *obj;
    } u;
    Boolean isclass;
} ClassFriend;

struct BClassList {
    struct BClassList *next;
    Type *type;
};

typedef struct VTable {
    Object *object;
    SInt32 offset;
    SInt32 size;
    TypeClass *owner;
} VTable;

struct TypeClass {
    TypeType type;
    SInt32 size;
    NameSpace *nspace;
    HashNameNode *classname;
    ClassList *bases;
    VClassList *vbases;
    ObjMemberVar *ivars;
    // ClassFriend *friends;
    VTable *vtable;
    // SOMInfo *sominfo;
    ObjCInfo *objcinfo;
    UInt32 flags;
    UInt32 othersize;
    SInt8 mode;
    SInt16 align;
    UInt16 eflags;
    UInt8 m_30;
    SInt8 action;
    SInt8 m_31;
};

typedef enum {
    CLASS_MODE_STRUCT = 0,
    CLASS_MODE_UNION = 1,
    CLASS_MODE_CLASS = 2
} ClassMode;

enum {
    CLASS_HANDLEOBJECT = 1, // class is a HandleObject
    CLASS_COMPLETED = 2, // completed class?
    CLASS_ABSTRACT = 8,
    CLASS_SINGLE_OBJECT = 0x10, // inherits from SingleObject/SingleInheritance
    CLASS_HAS_VBASES = 0x20,
    CLASS_IS_CONVERTIBLE = 0x40, // defines at least one conversion function
    CLASS_IS_TEMPL = 0x100, // is TemplClass
    CLASS_IS_TEMPL_INST = 0x800, // is TemplClassInst
    CLASS_IS_TEMPL_ANY = CLASS_IS_TEMPL | CLASS_IS_TEMPL_INST,
    CLASS_EMPTY = 0x1000, // is 0 bytes (forced to 1 byte for ABI reasons)
    CLASS_COM_OBJECT = 0x2000, // inherits from __comobject
    CLASS_FLAGS_4000 = 0x4000,
    CLASS_FLAGS_8000 = 0x8000
};

/// maps to TypeClass::eflags
enum {
    CLASS_EFLAGS_INTERNAL = 1,
    CLASS_EFLAGS_IMPORT = 2,
    CLASS_EFLAGS_EXPORT = 4,
    CLASS_EFLAGS_F0 = 0xF0
};

enum {
    CLASS_ACTION_0 = 0,
    CLASS_ACTION_1 = 1,
    CLASS_ACTION_2 = 2,
    CLASS_ACTION_3 = 3 // __javaobject
};

struct ExceptSpecList {
    ExceptSpecList *next;
    Type *type;
    UInt32 qual;
};

struct FuncArg { // ok
    struct FuncArg *next;
    HashNameNode *name;
    ENode *dexpr;
    Type *type;
    UInt32 qual;
    SInt16 sclass;
    Boolean is_array;
    UInt8 pad[2];
    UInt32 m_1a;
};

struct TypeFunc {
    TypeType type;
    SInt32 size;
    FuncArg *args;
    ExceptSpecList *exspecs;
    Type *functype;
    UInt32 qual;
    UInt8 pad[0x4];
    UInt32 flags;
};

// TypeFunc::flags
enum {
    FUNC_PASCAL = 1,
    FUNC_DEFINED = 2, // does the function have a body?
    FUNC_FLAGS_4 = 4,
    FUNC_PURE = 8, // abstract
    FUNC_METHOD = 0x10,
    FUNC_FLAGS_20 = 0x20,
    FUNC_CONVERSION = 0x40, // func that's like "operator SomeOtherType()"
    FUNC_FLAGS_80 = 0x80,
    FUNC_AUTO_GENERATED = 0x100, // an auto-generated ctor, dtor or assignment operator
    FUNC_INTRINSIC = 0x200,
    FUNC_NOTHROW = 0x400,
    FUNC_FLAGS_800 = 0x800, // autoinlined?
    FUNC_IS_CTOR = 0x1000, // is ctor
    FUNC_IS_DTOR = 0x2000, // is dtor
    FUNC_OBJC_METHOD = 0x4000,
    FUNC_CONST = 0x8000,
    FUNC_VOLATILE = 0x10000,
    FUNC_FLAGS_CV = FUNC_CONST | FUNC_VOLATILE,
    FUNC_IS_TEMPL = 0x100000, // the definition of a template
    FUNC_FLAGS_200000 = 0x200000,
    FUNC_FLAGS_400000 = 0x400000, // covariant?
    FUNC_IS_TEMPL_INSTANCE = 0x800000, // a concrete instance of a template
    FUNC_IS_TEMPL_ANY = FUNC_IS_TEMPL | FUNC_IS_TEMPL_INSTANCE,
    FUNC_FLAGS_4000000 = 0x4000000,
    FUNC_FLAGS_10000000 = 0x10000000,
    FUNC_CALL_CONV_MASK = 0xF0000000 | FUNC_PASCAL
};

struct TypeMemberFunc {
    TypeType type;
    SInt32 size;
    FuncArg *args;
    ExceptSpecList *exspecs;
    Type *functype;
    UInt32 qual;
    UInt8 pad[0x4];
    UInt32 flags;
    UInt8 pad2[0x4];
    TypeClass *theclass;
    SInt32 vtbl_index; // index of this method within the vtable
    SInt32 funcid; // ID used for tracking this method within browse data
    Boolean is_static; // is static
};


struct TypeBitfield {
    TypeType type;
    SInt32 size;
    Type *bitfieldtype;
    char offset; // offset in bits
    char bitlength; // size in bits
};


typedef enum TypeTemplDepType {
    TEMPLDEP_ARGUMENT,
    TEMPLDEP_QUALNAME,
    TEMPLDEP_TEMPLATE,
    TEMPLDEP_ARRAY,
    TEMPLDEP_QUALTEMPL,
    TEMPLDEP_BITFIELD
} TypeTemplDepType;

struct TypeTemplDep {
    TypeType type;
    SInt32 size;
    TypeTemplDepType dtype;
    union {
        TemplParamID pid;
        struct {
            TypeTemplDep *type;
            HashNameNode *name;
        } qual;
        struct {
            TemplClass *templ;
            TemplArg *args;
        } templ;
        struct {
            Type *type;
            ENode *index;
        } array;
        struct {
            TypeTemplDep *type;
            TemplArg *args;
        } qualtempl;
        struct {
            Type *type;
            ENode *size;
        } bitfield;
    } u;
};


struct TypeMemberPointer {
    TypeType type;
    SInt32 size;
    Type *ty1;
    Type *ty2;
    UInt32 qual;
};


/// Used for TYPEPOINTER and TYPEARRAY
struct TypePointer {
    TypeType type;
    SInt32 size;
    Type *target;
    UInt32 qual;
};


/// Always has Q_IS_OBJC_ID set
struct TypeObjCID {
    TypePointer pointer;
    ObjCProtocolList *protocols;
    short x12;
};


struct TypeList {
    TypeList *next;
    Type *type;
};


// Not sure if these existed originally, but they'll help
#define TYPE(ty) ((Type *) (ty))
#define TYPE_INTEGRAL(ty) ((TypeIntegral *) (ty))
#define TYPE_ENUM(ty) ((TypeEnum *) (ty))
#define TYPE_STRUCT(ty) ((TypeStruct *) (ty))
#define TYPE_CLASS(ty) ((TypeClass *) (ty))
#define TYPE_FUNC(ty) ((TypeFunc *) (ty))
#define TYPE_METHOD(ty) ((TypeMemberFunc *) (ty))
#define TYPE_BITFIELD(ty) ((TypeBitfield *) (ty))
#define TYPE_TEMPLATE(ty) ((TypeTemplDep *) (ty))
#define TYPE_MEMBER_POINTER(ty) ((TypeMemberPointer *) (ty))
#define TYPE_POINTER(ty) ((TypePointer *) (ty))
#define TPTR_TARGET(ty) (TYPE_POINTER(ty)->target)
#define TPTR_QUAL(ty) (TYPE_POINTER(ty)->qual)
#define TYPE_OBJC_ID(ty) ((TypeObjCID *) (ty))

#define IS_TYPE_VOID(ty) ( (ty)->type == TYPEVOID )
#define IS_TYPE_INT(ty) ( (ty)->type == TYPEINT )
#define IS_TYPE_ENUM(ty) ( (ty)->type == TYPEENUM )
#define IS_TYPE_INT_OR_ENUM(ty) ( IS_TYPE_INT(ty) || IS_TYPE_ENUM(ty) )
#define IS_TYPE_FLOAT(ty) ( (ty)->type == TYPEFLOAT )
#define IS_TYPE_INT_OR_FLOAT(ty) ( IS_TYPE_INT(ty) || IS_TYPE_FLOAT(ty) )
#define IS_TYPE_STRUCT(ty) ( (ty)->type == TYPESTRUCT )
#define IS_TYPE_CLASS(ty) ( (ty)->type == TYPECLASS )
#define IS_TYPE_SOM_CLASS(ty) ( (ty)->type == TYPECLASS && TYPE_CLASS((ty))->sominfo )
#define IS_TYPE_OBJC_CLASS(ty) ( (ty)->type == TYPECLASS && TYPE_CLASS((ty))->objcinfo )
#define IS_TYPE_FUNC(ty) ( (ty)->type == TYPEFUNC )
#define IS_TYPEFUNC_METHOD(ty) ( (ty)->flags & FUNC_METHOD )
#define IS_TYPE_METHOD(ty) ( IS_TYPE_FUNC(ty) && IS_TYPEFUNC_METHOD(TYPE_FUNC(ty)) )
#define IS_TYPE_NONMETHOD(ty) ( IS_TYPE_FUNC(ty) && !IS_TYPEFUNC_METHOD(TYPE_FUNC(ty)) )
#define IS_TYPE_NONSTATIC_METHOD(ty) ( IS_TYPE_FUNC(ty) && IS_TYPEFUNC_METHOD(TYPE_FUNC(ty)) && !TYPE_METHOD(ty)->is_static )
#define IS_TYPE_STATIC_METHOD(ty) ( IS_TYPE_FUNC(ty) && IS_TYPEFUNC_METHOD(TYPE_FUNC(ty)) && TYPE_METHOD(ty)->is_static )
#define IS_TYPEFUNC_NONSTATIC_METHOD(ty) ( IS_TYPEFUNC_METHOD(ty) && !TYPE_METHOD(ty)->is_static )
#define IS_TYPEFUNC_STATIC_METHOD(ty) ( IS_TYPEFUNC_METHOD(ty) && TYPE_METHOD(ty)->is_static )
#define IS_TYPE_TEMPLATE(ty) ( (ty)->type == TYPETEMPLATE )
#define IS_TYPE_POINTER(ty) ( (ty)->type == TYPEPOINTER || (ty)->type == TYPEARRAY )
#define IS_TYPE_POINTER_ONLY(ty) ( (ty)->type == TYPEPOINTER )
#define IS_TYPE_REFERENCE(ty) ( (ty)->type == TYPEPOINTER && (TYPE_POINTER(ty)->qual & Q_REFERENCE) )
#define IS_TYPEPOINTER_REFERENCE(ty) ( (ty)->qual & Q_REFERENCE )
#define IS_TYPE_ARRAY(ty) ( (ty)->type == TYPEARRAY )
#define IS_TYPE_BITFIELD(ty) ( (ty)->type == TYPEBITFIELD )
#define IS_TYPE_TEMPLDEPEXPR(ty) ( (ty)->type == TYPETEMPLDEPEXPR )
#define IS_TYPESTRUCT_VECTOR(ty) ( (ty)->stype >= STRUCT_VECTOR_UCHAR && (ty)->stype <= STRUCT_VECTOR_PIXEL )
#define IS_TYPE_VECTOR(ty) ( (ty)->type == TYPESTRUCT && IS_TYPESTRUCT_VECTOR(TYPE_STRUCT(ty)) )
#define IS_TYPE_NONVECTOR_STRUCT(ty) ( (ty)->type == TYPESTRUCT && !IS_TYPESTRUCT_VECTOR(TYPE_STRUCT(ty)) )
#define IS_TYPE_MEMBERPOINTER(ty) ( (ty)->type == TYPEMEMBERPOINTER )
#define IS_TYPE_4BYTES_MEMBERPOINTER(ty) ( ((ty)->type == TYPEMEMBERPOINTER) && ((ty)->size == 4u) )
#define IS_TYPE_12BYTES_MEMBERPOINTER(ty) ( ((ty)->type == TYPEMEMBERPOINTER) && ((ty)->size == 12u) )
#define TYPE_FITS_IN_REGISTER(ty) ( ((ty)->type == TYPEINT) || ((ty)->type == TYPEENUM) || (IS_TYPE_POINTER(ty) && ((ty)->type != TYPEARRAY)) || IS_TYPE_4BYTES_MEMBERPOINTER(ty) )
#define TYPE_IS_8BYTES(ty) ( (((ty)->type == TYPEINT) || ((ty)->type == TYPEENUM)) && ((ty)->size == 8) )

#define TYPE_FITS_IN_REGISTER_2(ty) ( ((ty)->type == TYPEINT) || ((ty)->type == TYPEENUM) || IS_TYPE_POINTER(ty) || IS_TYPE_4BYTES_MEMBERPOINTER(ty) )

#define OBJ_GET_TARGET_VOLATILE(obj) ( IS_TYPE_POINTER((obj)->type) ? (TYPE_POINTER((obj)->type)->qual & Q_VOLATILE) : ((obj)->qual & Q_VOLATILE) )
#define OBJ_GET_TARGET_CONST(obj) ( IS_TYPE_POINTER((obj)->type) ? (TYPE_POINTER((obj)->type)->qual & Q_CONST) : ((obj)->qual & Q_CONST) )

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
