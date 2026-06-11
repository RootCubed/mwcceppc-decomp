#ifndef COMPILER_CTEMPLATETOOLS_H
#define COMPILER_CTEMPLATETOOLS_H

#include <compiler/common.h>
#include <compiler/enode.h>

extern short ctempl_instdepth;

extern TemplStack *CTemplTool_PushInstance(TypeClass *tmclass, Object *func);
extern void CTemplTool_PopInstance(TemplStack *stack);
extern ENode *CTempTool_GetPTMTemplArgExpr(ENode *expr, Type *type);
extern Boolean CTemplTool_InitDeduceInfo(DeduceInfo *info, TemplParam *params, TemplArg *args, Boolean flag);
extern void CTemplTool_InsertTemplateParameter(NameSpace *nspace, TemplParam *param);
extern TemplArg *CTemplTool_MakeTemplArgList(DeduceInfo *info);
extern Boolean CTemplTool_IsIdenticalTemplArgList(TemplArg *args, TemplParam *params);
extern Type *CTemplTool_GetSelfRefTemplate(Type *type);
extern TemplateFunction *CTemplTool_GetFuncTempl(Object *object);
extern Boolean CTemplTool_ParamHasDefaultArg(TemplParam *param);
extern void CTemplTool_MergeDefaultArgs(TemplParam *dest, TemplParam *src);
extern void CTemplTool_MergeArgNames(TypeFunc *src, TypeFunc *dest);
extern Boolean CTemplTool_EqualParams(TemplParam *a, TemplParam *b, Boolean copyNames);
extern NameSpace *CTemplTool_SetupTemplateArgumentNameSpace(TemplParam *params, TemplArg *args, Boolean is_global);
extern void CTemplTool_SetupOuterTemplateArgumentNameSpace(NameSpace *nspace);
extern NameSpace *CTemplTool_InsertTemplateArgumentNameSpace(TemplParam *params, TemplClassInst *inst, CScopeSave *save);
extern void CTemplTool_RemoveOuterTemplateArgumentNameSpace(NameSpace *nspace);
extern void CTemplTool_RemoveTemplateArgumentNameSpace(NameSpace *nspace, TemplClassInst *inst, CScopeSave *save);
extern Boolean CTemplTool_IsTemplateArgumentDependentType(Type *type);
extern Boolean CTemplTool_IsTemplateArgumentDependentExpression(ENode *expr);
extern Boolean CTemplTool_IsSameTemplate(TemplParam *params, TemplArg *args);
extern TemplClass *CTemplTool_IsTemplate(TypeTemplDep *ttd);
extern Type *CTemplTool_IsDependentTemplate(TemplClass *tmclass, TemplArg *args);
extern Boolean CTemplTool_EqualExprTypes(ENode *a, ENode *b);
extern ENode *CTempl_MakeTemplDepExpr(ENode *left, ENodeType nt, ENode *right);
extern void CTemplTool_CheckTemplArgType(Type *type);
extern Boolean CTemplTool_EqualArgs(TemplArg *a, TemplArg *b);
extern TemplArg *CTemplTool_MakeGlobalTemplArgCopy(TemplArg *args);
extern Boolean CTemplTool_TemplDepTypeCompare(TypeTemplDep *a, TypeTemplDep *b);
extern Type *CTemplTool_DeduceArgDepType(TemplArg *args, Type *type, UInt32 qual, UInt32 *resultQual);
extern ENode *CTemplTool_DeduceExpr(TypeDeduce *deduce, ENode *expr);
extern ENode *CTemplTool_DeduceDefaultArg(Object *func, ENode *expr);
extern FuncArg *CTemplTool_DeduceArgCopy(TypeDeduce *deduce, FuncArg *args);
extern Type *CTemplTool_DeduceTypeCopy(TypeDeduce *deduce, Type *type, UInt32 *resultQual);
extern Type *CTemplTool_ResolveMemberSelfRefs(TemplClass *templ, Type *type, UInt32 *resultQual);
extern Boolean CTemplTool_IsSameTemplateType(Type *a, Type *b);

#endif
