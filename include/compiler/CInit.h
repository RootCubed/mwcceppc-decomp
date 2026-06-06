#ifndef COMPILER_CINIT_H
#define COMPILER_CINIT_H

#include <compiler/common.h>

typedef void (*ExprCB)(Type *type, ENode *expr, Boolean flag);
typedef void (*InitExprRegisterCB)(ENode *expr);
typedef void (*InsertExprCB)(ENode *expr);
typedef ENode *(*RegisterObjectCB)(Type *type, Object *obj, SInt32 offset, Boolean flag);
typedef ENode *(*TempNodeCB)(Type *type, Boolean flag);

struct OLinkList {
    OLinkList *next;
    Object *obj; // the object containing a relocation
    SInt32 offset;
    SInt32 somevalue;
};

typedef struct PooledString {
    struct PooledString *next;
    Object *obj;
    int offset;
    char *data;
    SInt32 size;
    Boolean ispascal;
    Boolean iswide;
} PooledString;

typedef struct InitInfo {
    Object *obj;
    OLinkList *list;
    void *x8;
    SInt32 expr_offset;
    ExprCB expr_cb;
    Boolean expr_cb_called;
    Boolean x15;
    Boolean x16;
    InitExprRegisterCB init_expr_register_cb;
    Object *obj1C;
    InsertExprCB insert_expr_cb;
    RegisterObjectCB register_object_cb;
    char *buffer;
    SInt32 size;
    SInt32 bufferSize;
    struct InitInfo *next;
} InitInfo;

extern TempNodeCB cinit_tempnodefunc;
extern InitInfo *cinit_initinfo;

extern void CInit_Init(void);
extern Boolean CInit_RelocInitCheck(ENode *expr, Object **objptr, CInt64 *valptr, Boolean flag);
extern void CInit_ExportConst(Object *obj);
extern Statement *CInit_ConstructClassArray(Statement *stmt, TypeClass *tclass, Object *ctor, Object *dtor, ENode *firstarg, SInt32 count);
extern ENode *CInit_AutoObject(Object *obj, Type *type, UInt32 qual);
extern void CInit_InitializeAutoData(Object *obj, InsertExprCB insert_cb, RegisterObjectCB register_cb);
extern void CInit_InitializeStaticData(Object *obj, InitExprRegisterCB cb);
extern void CInit_InitializeData(Object *obj);
extern Object *CInit_DeclareString(char *data, SInt32 size, Boolean ispascal, Boolean iswide);
extern PooledString *CInit_DeclarePooledString(char *data, SInt32 size, Boolean ispascal);
extern PooledString *CInit_DeclarePooledWString(char *data, SInt32 size);
extern void CInit_RewriteString(ENode *expr, Boolean flag);
extern void CInit_DeclarePooledStrings(void);
extern void CInit_DeclareData(Object *obj, void *data, OLinkList *list, SInt32 size);
extern void CInit_DeclareReadOnlyData(Object *obj, void *data, OLinkList *list, SInt32 size);
extern void CInit_DefineTentativeData(void);

#endif
