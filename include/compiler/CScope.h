#ifndef COMPILER_CSCOPE_H
#define COMPILER_CSCOPE_H

#include "compiler/common.h"

extern Boolean cscope_is_member_func;
extern Object *cscope_currentfunc;
extern TypeClass *cscope_currentclass;
extern NameSpace *cscope_current;
extern NameSpace *cscope_root;

struct CScopeSave {
    NameSpace *current;
    TypeClass *currentclass;
    Object *currentfunc;
    Boolean is_member_func;
}; // assumed name

typedef struct NameResult {
    NameSpace *nspace_0;
    HashNameNode *name_4;
    Type *type;
    UInt32 qual;
    ObjBase *obj_10;
    NameSpaceObjectList *nsol_14;
    BClassList *bcl_18;
    Boolean x1C;
    Boolean x1D;
    Boolean isambig;
    Boolean x1F;
    Boolean x20;
    Boolean x21;
} NameResult;

typedef struct CScopeNSIterator {
    NameSpace *nspace;
    NameSpaceLookupList *lookup;
    NameResult *result;
} CScopeNSIterator;

typedef struct CScopeObjectIterator {
    NameSpace *nspace;
    NameSpaceName *nextname;
    NameSpaceObjectList *currlist;
    int hashindex;
} CScopeObjectIterator;

extern void CScope_Setup(void);
extern void CScope_Cleanup(void);
extern void CScope_GetScope(CScopeSave *save);
extern void CScope_SetNameSpaceScope(NameSpace *nspace, CScopeSave *save);
extern void CScope_SetClassScope(TypeClass *cls, CScopeSave *save);
extern void CScope_SetClassDefScope(TypeClass *cls, CScopeSave *save);
extern void CScope_SetFunctionScope(Object *function, CScopeSave *save);
extern void CScope_SetMethodScope(Object *function, TypeClass *cls, Boolean is_static, CScopeSave *save);
extern void CScope_RestoreScope(CScopeSave *saved);
extern Boolean CScope_IsEmptySymTable(void);
extern Boolean CScope_IsInLocalNameSpace(NameSpace *nspace);
extern NameSpaceObjectList *CScope_FindName(NameSpace *nspace, HashNameNode *name);
extern NameSpaceName *CScope_FindNameSpaceName(NameSpace *nspace, HashNameNode *name);
extern NameSpaceObjectList *CScope_InsertName(NameSpace *nspace, HashNameNode *name);
extern NameSpaceObjectList *CScope_ArgumentDependentNameLookup(NameSpaceObjectList *list, HashNameNode *name, ENodeList *argNodes, Boolean flag);
extern NameSpace *CScope_NewHashNameSpace(HashNameNode *name);
extern NameSpace *CScope_NewListNameSpace(HashNameNode *name, Boolean is_global);
extern NameSpace *CScope_FindNonClassNonFunctionNS(NameSpace *nspace);
extern NameSpace *CScope_FindGlobalNS(NameSpace *nspoce);
extern Boolean CScope_IsStdNameSpace(NameSpace *nspace);
extern Boolean CScope_IsEmptyNameSpace(NameSpace *nspace);
extern void CScope_MergeNameSpace(NameSpace *dst, NameSpace *src);
extern void CScope_AddObject(NameSpace *nspace, HashNameNode *name, ObjBase *obj);
extern void CScope_AddGlobalObject(Object *obj);
extern NameSpaceLookupList *CScope_BuildNameSpaceLookupList(NameSpace *nspace);
extern Boolean CScope_FindQualifiedClassMember(NameResult *result, TypeClass *tclass, HashNameNode *name);
extern Type *CScope_GetType(NameSpace *nspace, HashNameNode *name, UInt32 *qual);
extern Type *CScope_GetTagType(NameSpace *nspace, HashNameNode *name);
extern Boolean CScope_ParseExprName(NameResult *result);
extern Boolean CScope_ParseDeclName(NameResult *result);
extern Boolean CScope_ParseQualifiedNameSpace(NameResult *result, Boolean flag1, Boolean flag2);
extern Boolean CScope_ParseElaborateName(NameResult *result);
extern Boolean CScope_FindObject(NameSpace *nspace, NameResult *result, HashNameNode *name);
extern Boolean CScope_FindNonClassObject(NameSpace *nspace, NameResult *result, HashNameNode *name);
extern NameSpaceObjectList *CScope_FindObjectList(NameResult *result, HashNameNode *name);
extern Boolean CScope_PossibleTypeName(HashNameNode *name);
extern Boolean CScope_FindClassMemberObject(TypeClass *tclass, NameResult *result, HashNameNode *name);
extern void CScope_InitObjectIterator(CScopeObjectIterator *iterator, NameSpace *nspace);
extern ObjBase *CScope_NextObjectIteratorObject(CScopeObjectIterator *iterator);
extern NameSpaceObjectList *CScope_NextObjectIteratorObjectList(CScopeObjectIterator *iterator);
extern void CScope_DefineTypeTag(NameSpace *nspace, HashNameNode *name, Type *type);
extern Type *CScope_GetLocalTagType(NameSpace *nspace, HashNameNode *name);
extern Boolean CScope_FindTypeName(NameSpace *nspace, HashNameNode *name, NameResult *result);
extern NameSpaceObjectList *CScope_GetLocalObject(NameSpace *nspace, HashNameNode *name);
extern BClassList *CScope_GetClassAccessPath(BClassList *list, TypeClass *tclass);
extern Boolean CScope_ParseMemberName(TypeClass *tclass, NameResult *result, Boolean flag);
extern void CScope_AddClassUsingDeclaration(TypeClass *tclass, TypeClass *tclass2, HashNameNode *name, AccessType access);
extern void CScope_ParseUsingDeclaration(NameSpace *nspace, AccessType access, Boolean flag);
extern void CScope_ParseNameSpaceAlias(HashNameNode *name);
extern void CScope_ParseUsingDirective(NameSpace *nspace);

#endif
