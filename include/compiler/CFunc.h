#ifndef COMPILER_CFUNC_H
#define COMPILER_CFUNC_H

#include <compiler/common.h>
#include <compiler/CScope.h>

typedef struct DeclBlock {
    struct DeclBlock *next;
    ExceptionAction *dobjstack; // is type right?
    NameSpace *parent_nspace;
    short index;
} DeclBlock;

struct CLabel {
    CLabel *next;
    Statement *stmt;
    HashNameNode *uniquename;
    HashNameNode *name;
    PCodeLabel *pclabel;
    //void *sicg_label;
};

typedef enum StatementType {
    ST_NOP = 1,
    ST_LABEL,
    ST_GOTO,
    ST_EXPRESSION,
    ST_SWITCH,
    ST_IFGOTO,
    ST_IFNGOTO,
    ST_RETURN,
    ST_OVF,
    ST_EXIT,
    ST_ENTRY,
    ST_BEGINCATCH,
    ST_ENDCATCH,
    ST_ENDCATCHDTOR,
    ST_GOTOEXPR,
    ST_ASM,
    ST_BEGINLOOP,
    ST_ENDLOOP,
    ST_ILLEGAL
} StatementType;

enum {
    StmtFlag_1 = 1,
    StmtFlag_2 = 2,
    StmtFlag_4 = 4,
    StmtFlag_8 = 8,
    StmtFlag_10 = 0x10
};

// TODO: move me - ST_BEGINLOOP related?
typedef struct LoopInfo {
    int x0;
    int x4;
    short x8;
    Statement *stmt;
} LoopInfo;

struct Statement {
    Statement *next;
    StatementType type;
    char marked;
    UInt8 flags;
    UInt16 value;
    ENode *expr;
    CLabel *label;
    ExceptionAction *dobjstack;
    SInt32 sourceoffset;
    HashNameNode *sourcefilepath;
};

typedef struct InitExpr {
    struct InitExpr *next;
    ENode *expr;
    Object *object;
} InitExpr;

typedef enum CtorChainType {
    CtorChain_Base = 0,
    CtorChain_VBase = 1,
    CtorChain_MemberVar = 2
} CtorChainType;

typedef struct CtorChain {
    struct CtorChain *next;
    CtorChainType what;
    ENode *objexpr;
    union {
        ClassList *base; // 0
        VClassList *vbase; // 1
        ObjMemberVar *membervar; // 2
    } u;
} CtorChain;

typedef struct CFuncSave CFuncSave;

struct DeclThing {
    Type *thetype;
    UInt32 qual;
    SwitchInfo *switchinfo;
    CLabel *loopContinue;
    CLabel *loopBreak;
};

struct DefArgCtorInfo {
    Object *default_func;
    ENode *default_arg;
};

extern FuncArg elipsis;
extern FuncArg oldstyle;
extern ObjectList *arguments;
extern ObjectList *locals;
extern short localcount;
extern SInt32 curstmtvalue;
extern SInt32 sourceoffset;
extern HashNameNode *sourcefilepath;
extern SInt32 functionbodyoffset;
extern HashNameNode *functionbodypath;
extern InitExpr *init_expressions;
extern CLabel *Labels;
extern CtorChain *ctor_chain;
extern Statement *curstmt;

extern DeclBlock *CFunc_NewDeclBlock(void);
extern void CFunc_RestoreBlock(DeclBlock *block);
extern void CFunc_SetupLocalVarInfo(Object *obj);
extern ENode *CFunc_DefaultArg(Type *type, UInt32 qual, FuncArg *args);
extern Boolean CFunc_ParseFakeArgList(Boolean flag);
extern FuncArg *parameter_type_list(DeclInfo *declinfo);
extern CLabel *findlabel(void);
extern CLabel *newlabel(void);
extern Statement *CFunc_AppendStatement(StatementType sttype);
extern Statement *CFunc_InsertStatement(StatementType sttype, Statement *after);
extern Statement *CFunc_InsertBeforeStatement(StatementType sttype, Statement *before);
extern void CheckCLabels(void);
extern Object *create_temp_object(Type *type);
extern ENode *create_temp_node(Type *type);
extern ENode *create_temp_node2(Type *type);
extern void CFunc_WarnUnused(void);
extern void CFunc_CodeCleanup(Statement *stmt);
extern void CFunc_DestructorCleanup(Statement *stmt);
extern Statement *CFunc_GenerateLoop(Statement *stmt, Type *type, ENode *lowerBound, ENode *upperBound, ENode *increment1, ENode *increment2, ENode *(*callback)(ENode *, ENode *));
extern void CFunc_CompoundStatement(DeclThing *thing);
extern void CFunc_SetupNewFuncArgs(Object *func, FuncArg *args);
extern NameSpace *CFunc_FuncGenSetup(Statement *stmt, Object *func);
extern CFuncSave *CFunc_GetGlobalCompilerState(void);
extern void CFunc_SetGlobalCompilerState(CFuncSave *state);
extern void CFunc_Gen(Statement *stmt, Object *func, UInt8 unk);
extern void CFunc_CheckClassCtors(TypeClass *tclass);
extern void CFunc_ParseFuncDef(Object *func, DeclInfo *di, TypeClass *tclass, Boolean is_method, Boolean is_static, NameSpace *nspace);
extern void InitExpr_Register(ENode *expr, Object *object);
extern void CFunc_GenerateDummyFunction(Object *func);
extern void CFunc_GenerateSingleExprFunc(Object *func, ENode *expr);
extern void CFunc_GenerateDummyCtorFunc(Object *func, Object *real_ctor);

#endif
