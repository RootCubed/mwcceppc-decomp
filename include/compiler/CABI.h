#ifndef COMPILER_CABI_H
#define COMPILER_CABI_H

#include <compiler/common.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

extern TypePointer vtable_ptr;

typedef enum CABIDestroyMode {
    CABIDestroy0 = 0,
    CABIDestroy1 = 1,
    CABIDestroy2 = 2,
    CABIDestroy3 = 3
} CABIDestroyMode;

typedef struct OffsetList {
    struct OffsetList *next;
    SInt32 offset;
    SInt8 m_08;
} OffsetList;

typedef struct VTableHeaderInfo {
    SInt32 size;
    UInt32 m_04;
    UInt32 m_08;
} VTableHeaderInfo;

extern short CABI_GetStructResultArgumentIndex(TypeFunc *tfunc);
extern Type *CABI_GetSizeTType(void);
extern Type *CABI_GetPtrDiffTType(void);
extern SInt16 CABI_StructSizeAlignValue(Type *type, UInt32 qual, SInt32 size);
extern void CABI_ReverseBitField(TypeBitfield *tbitfield);
extern void CABI_AddVTable(TypeClass *tclass);
extern SInt32 CABI_GetVTableOffset(TypeClass *tclass);
extern void CABI_GetVTableHeaderInfo(VTableHeaderInfo *info);
extern void CABI_LayoutClass(ClassLayout *layout, TypeClass *tclass);
extern void CABI_MakeDefaultArgConstructor(TypeClass *tclass, Object *func);
extern ENode *CABI_MakeThisExpr(TypeClass *tclass, SInt32 offset);
extern SInt32 CABI_GetCtorOffsetOffset(TypeClass *tclass, TypeClass *base);
extern Object *CABI_ConstructorCallsNew(TypeClass *tclass);
typedef Statement *(*TransConstructorCallback)(Statement *stmt, TypeClass *tclass, TypeClass *base, SInt32 offset, Boolean flag);
extern void CABI_TransConstructor(Object *obj, Statement *stmt, TypeClass *tclass, TransConstructorCallback callback, Boolean has_try);
extern void CABI_MakeDefaultConstructor(TypeClass *tclass, Object *func);
extern void CABI_MakeDefaultCopyConstructor(TypeClass *tclass, Object *func);
extern void CABI_MakeDefaultAssignmentOperator(TypeClass *tclass, Object *func);
extern void CABI_TransDestructor(Object *obj1, Object *obj2, Statement *stmt, TypeClass *tclass, CABIDestroyMode mode);
extern void CABI_MakeDefaultDestructor(TypeClass *tclass, Object *func);
extern void CABI_MakeLayeredDestructor(TypeClass *tclass, Object *func);
extern Object *CABI_GetDestructorObject(Object *obj, CABIDestroyMode mode);
extern void CABI_AddLayeredDestructors(TypeClass *tclass);
extern ENode *CABI_DestroyObject(Object *dtor, ENode *objexpr, CABIDestroyMode mode, Boolean flag1, Boolean flag2);
extern SInt32 CABI_ComputeNewArrayPadding(void);
extern ENode *CABI_ReleaseGuardVariable(Object *obj);
extern ENode *CABI_AcquireGuardVariable(Object *obj);
extern Object *CABI_NewGuardVariable(Object *expr);
extern Boolean CABI_PassedByReference(TypeClass *type);
extern ENode *CABI_AddPointerOffset(ENode *base, SInt32 offset);
extern Boolean CABI_ConstructorReturnsThis(TypeClass *tclass);
extern Boolean CABI_DestructorReturnsThis(TypeClass *tclass);
extern FuncArg *CABI_GetFirstRealArgument(TypeFunc *tfunc);
extern Object *CABI_DummyDefaultConstructor(TypeClass *tclass);

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
