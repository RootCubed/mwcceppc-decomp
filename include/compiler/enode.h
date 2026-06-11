#ifndef COMPILER_ENODE_H
#define COMPILER_ENODE_H

#include <compiler/common.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

typedef enum ENodeType {
    EPOSTINC,
    EPOSTDEC,
    EPREINC,
    EPREDEC,
    EINDIRECT,
    EMONMIN,
    EBINNOT,
    ELOGNOT,
    EFORCELOAD,
    EMUL,
    EMULV,
    EDIV,
    EMODULO,
    EADDV,
    ESUBV,
    EADD,
    ESUB,
    ESHL,
    ESHR,
    ELESS,
    EGREATER,
    ELESSEQU,
    EGREATEREQU,
    EEQU,
    ENOTEQU,
    EAND,
    EXOR,
    EOR,
    ELAND,
    ELOR,
    EASS,
    EMULASS,
    EDIVASS,
    EMODASS,
    EADDASS,
    ESUBASS,
    ESHLASS,
    ESHRASS,
    EANDASS,
    EXORASS,
    EORASS,
    ECOMMA,
    EMIN,
    EMAX,
    EPMODULO,
    EROTL,
    EROTR,
    EBCLR,
    EBTST,
    EBSET,
    ETYPCON,
    EBITFIELD,
    EINTCONST,
    EFLOATCONST,
    E_UNK_54,
    ESTRINGCONST,
    ECOND,
    EFUNCCALL,
    EFUNCCALLP,
    EOBJREF,
    ENULLCHECK,
    EPRECOMP,
    ELABEL,
    EGCCASM,
    ESCOPEBEGIN, ESCOPEEND,
    EINFO,
    EMFPOINTER,
    ETEMP,
    ELOCOBJ,
    EARGOBJ,
    ESETCONST,
    ENEWEXCEPTION,
    ENEWEXCEPTIONARRAY,
    EINITTRYCATCH,
    EOBJACCESS,
    ETEMPLDEP,
    EPOINTERSTAR,
    EDOTSTAR,
    ECTORINIT,
    ESTMT,
    E_UNK_81,
    E_UNK_82,
    EINSTRUCTION,
    EDEFINE,
    EREUSE,
    EASSBLK,
    EVECTOR128CONST,
    ECONDASS
} ENodeType;


struct EMemberInfo {
    BClassList *path;
    ENode *expr;
    NameSpaceObjectList *list;
    TemplArg *templargs;
    Boolean pr_1D;
    Boolean x11;
    Boolean isambig;
};


struct ENodeList {
    ENodeList *next;
    ENode *node;
};


typedef union ENodeUnion {
    CInt64 intval;
    Float floatval;
    SInt32 longval;
    ENode *monadic;
    Object *objref;
    CLabel *label;
    MWVector128 vector128val;
    struct {
        ENode *left;
        ENode *right;
    } diadic;
    struct {
        ENode *cond;
        ENode *expr1;
        ENode *expr2;
    } cond;
    struct {
        ENode *funcref;
        ENodeList *args;
        TypeFunc *functype;
    } funccall;
    //ObjAccess objaccess;
    struct {
        ENode *accessnode;
        ENode *mfpointer;
    } mfpointer;
    struct {
        ENode *nullcheckexpr;
        ENode *condexpr;
        SInt32 precompid;
    } nullcheck;
    SInt32 precompid;
    struct {
        Type *type;
        SInt32 uniqueid;
        Boolean needs_dtor;
    } temp;
    struct {
        SInt32 size;
        char *data;
        //SInt32 segnum; ??
        char ispascal;
        char ispacked;
    } string;
    struct {
        SInt32 size;
        char *data;
    } set;
    struct {
        ENode *initexpr;
        ENode *tryexpr;
        Object *pointertemp;
        Object *deletefunc;
    } newexception;
    struct {
        ENode *initexpr;
        ENode *tryexpr;
        ENode *catchexpr;
        ENode *result;
    } itc;
    struct {
        Object *objref;
        SInt32 offset;
    } addr;
    struct {
        NameSpaceObjectList *list;
        TemplArg *templargs;
        HashNameNode *name;
    } objlist;
    EMemberInfo *emember;
    void *inst;
    CtorChain *ctorinit;
    Statement *stmt;
    struct {
        union {
            TemplParamID pid;
            struct {
                Type *type;
            } typeexpr;
            struct {
                ENodeList *args;
                Type *type;
                UInt32 qual;
            } cast;
            struct {
                TypeTemplDep *type;
                HashNameNode *name;
            } qual;
            struct {
                ENode *expr;
                TStreamElement *token;
            } sourceref;
            ENode *monadic;
            Object *obj;
        } u;
        TemplDepSubType subtype;
    } templdep;
} ENodeUnion;

struct ENode {
    ENodeType type;
    UInt8 cost;
    Boolean ignored;
    Boolean hascall;
    Type *rtype;
    UInt32 flags;
    PointsToFunction *pointsTo;
    ENodeUnion data;
};

enum {
    ENODE_FLAG_CONST = Q_CONST,
    ENODE_FLAG_VOLATILE = Q_VOLATILE,
    ENODE_FLAG_QUALS = Q_CONST | Q_VOLATILE,
    ENODE_FLAG_8 = 0x8,
    ENODE_FLAG_10 = 0x10,
    ENODE_FLAG_80 = 0x80
};

#define ENODE_IS(_enode, _etype) ( (_enode)->type == (_etype) )
#define ENODE_IS2(_enode, _etype1, _etype2) ( ENODE_IS((_enode), (_etype1)) || ENODE_IS((_enode), (_etype2)) )
#define ENODE_IS3(_enode, _etype1, _etype2, _etype3) ( ENODE_IS((_enode), (_etype1)) || ENODE_IS((_enode), (_etype2)) || ENODE_IS((_enode), (_etype3)) )
#define ENODE_IS4(_enode, _etype1, _etype2, _etype3, _etype4) ( ENODE_IS((_enode), (_etype1)) || ENODE_IS((_enode), (_etype2)) || ENODE_IS((_enode), (_etype3)) || ENODE_IS((_enode), (_etype4)) )
#define ENODE_IS5(_enode, _etype1, _etype2, _etype3, _etype4, _etype5) ( ENODE_IS((_enode), (_etype1)) || ENODE_IS((_enode), (_etype2)) || ENODE_IS((_enode), (_etype3)) || ENODE_IS((_enode), (_etype4)) || ENODE_IS((_enode), (_etype5)) )
#define ENODE_IS_RANGE(_enode, _lo, _hi) ( ((_enode)->type >= (_lo)) && ((_enode)->type <= (_hi)) )
#define ENODE_QUALS(_enode) ( (UInt32) ( (_enode)->flags & ENODE_FLAG_QUALS ) )
#define ENODE_IS_INDIRECT_TO(_enode, _etype) ( ENODE_IS((_enode), EINDIRECT) && ENODE_IS((_enode)->data.monadic, (_etype)) )
#define ENODE_IS_ASSIGN(_enode) ( ENODE_IS_RANGE((_enode), EASS, EORASS) )
#define ENODE_IS_ASSIGN_TO(_enode, _etype) ( ENODE_IS_RANGE((_enode), EASS, EORASS) && ENODE_IS((_enode)->data.diadic.left->data.monadic, (_etype)) )

// 0 to 8, 0x30 to 0x31
#define ENODE_CASE_MONADIC \
        case EPOSTINC: \
        case EPOSTDEC: \
        case EPREINC: \
        case EPREDEC: \
        case EINDIRECT: \
        case EMONMIN: \
        case EBINNOT: \
        case ELOGNOT: \
        case EFORCELOAD: \
        case ETYPCON: \
        case EBITFIELD

// 9 to 0x1B
#define ENODE_CASE_DIADIC_1 \
        case EMUL: \
        case EMULV: \
        case EDIV: \
        case EMODULO: \
        case EADDV: \
        case ESUBV: \
        case EADD: \
        case ESUB: \
        case ESHL: \
        case ESHR: \
        case ELESS: \
        case EGREATER: \
        case ELESSEQU: \
        case EGREATEREQU: \
        case EEQU: \
        case ENOTEQU: \
        case EAND: \
        case EXOR: \
        case EOR

// 0x1E to 0x28, 0x2D, 0x2F
#define ENODE_CASE_ASSIGN \
        case EASS: \
        case EMULASS: \
        case EDIVASS: \
        case EMODASS: \
        case EADDASS: \
        case ESUBASS: \
        case ESHLASS: \
        case ESHRASS: \
        case EANDASS: \
        case EXORASS: \
        case EORASS: \
        case EBCLR: \
        case EBSET

// 9 to 0x2F
#define ENODE_CASE_DIADIC_ALL \
        case EMUL: \
        case EMULV: \
        case EDIV: \
        case EMODULO: \
        case EADDV: \
        case ESUBV: \
        case EADD: \
        case ESUB: \
        case ESHL: \
        case ESHR: \
        case ELESS: \
        case EGREATER: \
        case ELESSEQU: \
        case EGREATEREQU: \
        case EEQU: \
        case ENOTEQU: \
        case EAND: \
        case EXOR: \
        case EOR: \
        case ELAND: \
        case ELOR: \
        case EASS: \
        case EMULASS: \
        case EDIVASS: \
        case EMODASS: \
        case EADDASS: \
        case ESUBASS: \
        case ESHLASS: \
        case ESHRASS: \
        case EANDASS: \
        case EXORASS: \
        case EORASS: \
        case ECOMMA: \
        case EPMODULO: \
        case EROTL: \
        case EROTR: \
        case EBCLR: \
        case EBTST: \
        case EBSET

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
