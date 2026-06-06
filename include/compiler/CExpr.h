#ifndef COMPILER_CEXPR_H
#define COMPILER_CEXPR_H

#include <compiler/common.h>
#include <compiler/enode.h>
#include <compiler/types.h>
#include <compiler/CScope.h>

typedef struct ConIteratorList {
    struct ConIteratorList *next;
    struct ConIterator *iter;
} ConIteratorList;

typedef struct ConIterator {
    struct ConIterator *parent;
    ConIteratorList *children;
    TypeClass *tclass;
} ConIterator;

typedef struct ConversionIterator {
    CScopeObjectIterator objiter;
    ConIterator myconiter;
    ConIterator *coniter;
} ConversionIterator;

enum {
    CheckResult0,
    CheckResult1,
    CheckResult2,
    CheckResult3,
    CheckResult4
};

// Rename me to StdMatch?
typedef struct Match5 {
    short x0;
    short x2;
    short x4;
    short x6;
    short x8;
} Match5;

typedef struct Match13 {
    Object *obj;
    Match5 anotherm5;
    short xE;
    Match5 match5;
    ObjectList *list;
} Match13;

typedef struct FuncMatchArgs {
    ENodeList *exprs;
    FuncArg *args;
} FuncMatchArgs;

typedef struct Conversion {
    ENode *x0;
    ENode *left;
    ENode *right;
} Conversion;

// CExpr.c
typedef Boolean (*NameObjCheckCB)(HashNameNode *, Object *);

extern NameObjCheckCB name_obj_check;
extern Boolean disallowgreaterthan;

// CExpr2.c
extern ENode *assign_node;
extern Boolean temp_reference_init;

// CExpr.c
extern ENode *CExpr_RewriteConst(ENode *expr);
extern void optimizecomm(ENode *expr);
extern short canadd(ENode *expr, SInt32 value);
extern ENode *checkreference(ENode *expr);
extern ENode *pointer_generation(ENode *expr);
extern ENode *CExpr_PointerGeneration(ENode *expr);
extern ENode *oldassignmentpromotion(ENode *expr, Type *type, short qual, Boolean flag);
extern ENode *argumentpromotion(ENode *expr, Type *type, short qual, Boolean flag);
extern ENode *classargument(ENode *expr);
extern ENodeList *CExpr_ScanExpressionList(Boolean is_parens);
extern ENode *CExpr_DoExplicitConversion(Type *type, UInt32 qual, ENodeList *list);
extern ENode *CExpr_MakeNameLookupResultExpr(NameResult *pr);
extern SInt32 scansizeof(void);
extern SInt32 scanalignof(void);
extern ENode *getnodeaddress(ENode *expr, Boolean flag);
extern ENode *getpointertomemberfunc(ENode *expr, Type *type, Boolean flag);
extern ENode *CExpr_New_ELOGNOT_Node(ENode *expr);
extern ENode *CExpr_New_EMONMIN_Node(ENode *expr);
extern ENode *CExpr_New_EBINNOT_Node(ENode *expr);
extern ENode *unary_expression(void);
extern ENode *do_castnullcheck(ENode *condexpr, ENode *nullcheckexpr);
extern ENode *CExpr_SafeClassPointerCast(ENode *expr, TypeClass *a, TypeClass *b, Boolean typconflag, Boolean pathcheckflag);
extern ENode *PointerToMemberCast(ENode *expr, TypeMemberPointer *tm1, TypeMemberPointer *tm2, Boolean flag);
extern ENode *CExpr_MemberPointerConversion(ENode *expr, TypeMemberPointer *type, Boolean flag1);
extern ENode *do_typecast(ENode *expr, Type *type, UInt32 qual);
extern ENode *cast_expression(void);
extern ENode *CExpr_New_EMUL_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EDIV_Node(ENode *left, ENode *right, Boolean no_warning);
extern ENode *CExpr_New_EMODULO_Node(ENode *left, ENode *right, Boolean no_warning);
extern ENode *CExpr_New_EADD_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ESUB_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ESHL_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ESHR_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ELESS_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ELESSEQU_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EGREATER_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EGREATEREQU_Node(ENode *left, ENode *right);
extern ENode *memberpointercompare(ENodeType t, ENode *left, ENode *right);
extern ENode *CExpr_New_EEQU_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ENOTEQU_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EAND_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EXOR_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_EOR_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ELAND_Node(ENode *left, ENode *right);
extern ENode *CExpr_New_ELOR_Node(ENode *left, ENode *right);
extern ENode *CExpr_NewDyadicNode(ENode *left, ENodeType nt, ENode *right);
extern ENode *CExpr_New_ECOND_Node(ENode *cond, ENode *expr1, ENode *expr2);
extern ENode *CExpr_New_EINDIRECT_Node(Object *obj);
extern ENode *assignment_expression(void);
extern ENode *conv_assignment_expression(void);
extern void CExpr_CheckUnusedExpression(ENode *expr);
extern ENode *s_expression(void);
extern ENode *expression(void);
extern CInt64 CExpr_IntegralConstExprType(Type **tint);
extern ENode *CExpr_IntegralConstOrDepExpr(void);
extern CInt64 CExpr_IntegralConstExpr(void);
extern void CExpr_CheckUnwantedAssignment(ENode *expr);
extern Boolean CExpr_ParseAsmExpr(Object **objptr, CInt64 *valptr);

// CExpr2.c
typedef void (*CExprSearchCB)(ENode *expr);
typedef ENode *(*CExprReplaceCB)(ENode *expr);

extern void CExpr_SearchExprTree(ENode *expr, CExprSearchCB callback, int count, ...);
extern ENode *CExpr_SearchExprTreeReplace(ENode *expr, CExprReplaceCB callback, int count, ...);
extern Boolean CExpr_HasFuncCall(ENode *expr);
extern void CExpr_AliasTransform(ENode *expr);
extern ENode *CExpr_UnaryFloatExpression(ENode *expr);
extern ENode *CExpr_BinaryFloatExpression(ENode *expr);
extern ENode *CExpr_NewENode(ENodeType ty);
extern ENode *CExpr_NewTemplDepENode(TemplDepSubType t);
extern ENode *nullnode(void);
extern ENode *intconstnode(Type *type, SInt32 value);
extern ENode *stringconstnode(char *str);
extern ENode *forceintegral(ENode *expr);
extern ENode *makemonadicnode(ENode *inner, ENodeType ty);
extern ENode *makediadicnode(ENode *left, ENode *right, ENodeType ty);
extern ENode *makecommaexpression(ENode *left, ENode *right);
extern short iszero(ENode *expr);
extern short isnotzero(ENode *expr);
extern Boolean CExpr_IsOne(ENode *expr);
extern Boolean CExpr_AllBitsSet(ENode *expr);
extern ENode *CExpr_NewETEMPNode(Type *type, Boolean assign_id);
extern ENode *CExpr_GetETEMPCopy(ENode *expr);
extern ENode *integralpromote(ENode *expr);
extern CInt64 CExpr_IntConstConvert(Type *a, Type *b, CInt64 val);
extern ENode *promote(ENode *expr, Type *type);
extern void CExpr_ArithmeticConversion(ENode **left, ENode **right);
extern ENode *CExpr_TempModifyExpr(ENode *expr);
extern Boolean CExpr_IsLValue(ENode *expr);
extern ENode *CExpr_LValue(ENode *expr, Boolean flag1, Boolean flag2);
extern ENode *CExpr_MakeObjRefNode(Object *obj, Boolean flag);
extern ENode *create_objectrefnode(Object *obj);
extern ENode *create_objectnode2(Object *obj);
extern ENode *create_objectnode(Object *obj);
extern ENode *CExpr_IsTempConstruction(ENode *expr, Type *type, ENode **resultexpr);
extern ENode *CExpr_AdjustFunctionCall(ENode *expr);
extern ENode *funccallexpr(Object *func, ENode *arg1, ENode *arg2, ENode *arg3, ENode *arg4);
extern ENode *CExpr_FuncCallSix(Object *func, ENode *arg1, ENode *arg2, ENode *arg3, ENode *arg4, ENode *arg5, ENode *arg6);
extern void CExpr_MatchCV(Type *t1, UInt32 q1, Type *t2, UInt32 q2, Match13 *match);
extern Boolean CExpr_MatchAssign(Type *type, UInt32 qual, ENode *expr, Match13 *match);
extern ENode *CExpr_ClassPointerCast(BClassList *cls, ENode *expr, Boolean nullcheckflag);
extern ENode *CExpr_GetClassAccessNode(BClassList *a, BClassList *b, ENode *expr, Object *obj, AccessType access, Boolean flag);
extern ENode *CExpr_ConvertToBool(ENode *expr, Boolean isExplicit);
extern void CExpr_ConversionIteratorInit(ConversionIterator *iter, TypeClass *tclass);
extern Object *CExpr_ConversionIteratorNext(ConversionIterator *iter);
extern short user_assign_check(ENode *expr, Type *type, UInt32 qual, Boolean flag1, Boolean flag2, Boolean flag3);
extern ENode *CExpr_ConvertToCondition(ENode *expr);
extern ENode *CExpr_ConvertToIntegral(ENode *expr);
extern void CExpr_CheckArithmConversion(ENode *expr, Type *type);
extern ENode *get_address_of_temp_copy(ENode *expr, Boolean flag);
extern short assign_check(ENode *expr, Type *type, UInt32 qual, Boolean flag1, Boolean flag2, Boolean flag3);
extern Boolean CExpr_MatchCompare(Object *obj, Match13 *a, Match13 *b);
extern Boolean CExpr_GetFuncMatchArgs(Object *obj, ENodeList *argexprs, ENode *expr, FuncMatchArgs *result);
extern ENode *CExpr_GetDefaultArgument(ENode *funcexpr, FuncArg *arg);
extern ENode *CExpr_VarArgPromotion(ENode *expr, Boolean flag);
extern ENode *CExpr_GenericFuncCall(BClassList *path, ENode *funcexpr, Boolean flag1, Object *obj, NameSpaceObjectList *nsol, TemplArg *templargs, ENodeList *nodes, Boolean flag2, Boolean flag3, Boolean flag4);
extern ENode *CExpr_GenericPtmfCall(Object *obj, TypeFunc *tfunc, ENodeList *arg_exprs);
extern ENode *CExpr_MakeFunctionCall(ENode *funcexpr, ENodeList *arg_exprs);
extern Boolean CExpr_CheckOperatorConversion(short token, ENode *left, ENode *right, ENodeList *list, Conversion *conv);
extern Boolean CExpr_CheckOperator(short token, ENode *left, ENode *right, Conversion *conv);
extern ENode *CExpr_ConstructObject(TypeClass *tclass, ENode *addr_expr, ENodeList *args, Boolean flag1, Boolean flag2, Boolean flag3, Boolean flag4, Boolean flag5);
extern ENode *scannew(Boolean flag);
extern ENode *scandelete(Boolean flag);

// CExprConvMatch.c
extern Boolean CExpr_CanImplicitlyConvert(ENode *expr, Type *type2, UInt32 qual2);
extern ENode *CExpr_Convert(ENode *expr, Type *type, UInt32 qual, Boolean isExplicit, Boolean flag2);
extern ENode *CExpr_AssignmentPromotion(ENode *expr, Type *type2, UInt32 qual2, Boolean flag);
extern void CExpr_FuncArgMatch(NameSpaceObjectList *list, TemplArg *templargs, ENodeList *argexprs, Match13 *match13, ENode *expr, Boolean flag);
extern Boolean CExpr_CondOperatorMatch(ENode *left, ENode *right, Conversion *conv);
extern Boolean CExpr_OperatorMatch(short token, ENode *left, ENode *right, Conversion *conv);

#endif
