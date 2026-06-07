#ifndef COMPILER_CPARSER_H
#define COMPILER_CPARSER_H

#include <compiler/common.h>
#include <compiler/enode.h>
#include <compiler/tokens.h>
#include <compiler/types.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

enum {
    AlignMode0_Mac68k,
    AlignMode1_Mac68k4byte,
    AlignMode2_PPC,
    AlignMode3_1Byte,
    AlignMode4_2Byte,
    AlignMode5_4Byte,
    AlignMode6_8Byte,
    AlignMode7_16Byte,
    AlignMode8_Packed
};

enum {
    CPU_PPC401,
    CPU_PPC403,
    CPU_PPC505,
    CPU_PPC509,
    CPU_PPC555,
    CPU_PPC601,
    CPU_PPC602,
    CPU_PPC603,
    CPU_PPC603e,
    CPU_PPC604,
    CPU_PPC604e,
    CPU_PPC740,
    CPU_PPC750,
    CPU_PPC801,
    CPU_PPC821,
    CPU_PPC823,
    CPU_PPC850,
    CPU_PPC860,
    CPU_PPC8240,
    CPU_PPC8260,
    CPU_Generic,
    CPU_PPC7400,
    CPU_PPCGEKKO,
    CPU_PPCELF,
    CPU_PPC7450,
    CPU_PPC556,
    CPU_PPC565
};


typedef struct CompilerLinkerOptions {
    Boolean littleendian;
    Boolean codegen_dynamic;
    Boolean codegen_pic;
    Boolean no_common;
    Boolean no_implicit_templates;
    Boolean absolutepath; // determines the path written to debug info
    char x06; // linker/objgen related
    short processor;
    char scheduling;
    char schedule_factor;
    SInt32 debuglisting;
    Boolean profile;
    Boolean traceback;
    Boolean disable_registers;
    Boolean fp_contract;
    Boolean no_register_save_helpers;
    Boolean unroll_speculative;
    short unroll_instr_limit;
    short unroll_factor_limit;
    Boolean altivec_model;
    UInt8 altivec_vrsave;
    UInt8 function_align;
    UInt8 x1D;
    UInt8 gen_isel;
    UInt8 gen_fsel;
    Boolean opt_bcc_lr_ctr;
    Boolean use_lmw_stmw;
    char report_heap_info;
    Boolean misaligned_mem_access;
    Boolean switch_tables;
    Boolean prepare_compress;
    char min_struct_alignment; // used in CMach_AllocationAlignment
    Boolean asmsemicolcomment;
    Boolean asmpoundcomment;
    StringPtr oldprefixname;
    short inlinelevel;
    int inlinemaxsize;
    int inlinemaxtotalsize;
    Boolean inline_bottom_up;
    Boolean cplusplus;
    Boolean ecplusplus;
    Boolean objective_c;
    Boolean objc_strict;
    Boolean ARMconform;
    Boolean ARMscoping;
    Boolean checkprotos;
    Boolean trigraphs;
    Boolean onlystdkeywords;
    Boolean enumsalwaysint;
    Boolean ANSIstrict;
    Boolean mpwc_relax;
    Boolean mpwc_newline;
    Boolean ignore_oldstyle;
    Boolean cpp_extensions;
    Boolean pointercast_lvalue;
    Boolean RTTI;
    Boolean delete_exception;
    Boolean test_dummy;
    Boolean oldalignment;
    Boolean unsigned_char;
    Boolean multibyteaware;
    Boolean auto_inline;
    Boolean defer_codegen;
    Boolean direct_to_som;
    Boolean som_env_check;
    Boolean som_call_optimize;
    Boolean booltruefalse;
    Boolean old_enum_mangler;
    Boolean longlong;
    Boolean longlong_enums;
    Boolean no_tfuncinline;
    Boolean always_vdispatch;
    Boolean flat_include;
    Boolean syspath_once;
    Boolean always_import;
    Boolean simple_class_byval;
    Boolean wchar_type;
    Boolean vbase_ctor_offset;
    Boolean vbase_abi_v2;
    Boolean def_inherited;
    Boolean template_patch;
    Boolean template_friends;
    Boolean faster_pch_gen;
    Boolean array_new_delete;
    Boolean dollar_identifiers;
    Boolean def_inline_tfuncs;
    Boolean arg_dep_lookup;
    Boolean simple_prepdump;
    Boolean line_prepdump;
    Boolean fullpath_prepdump;
    Boolean old_mtemplparser;
    Boolean suppress_init_code;
    Boolean reverse_bitfields;
    Boolean c9x;
    Boolean float_constants;
    Boolean no_static_dtors;
    Boolean longlong_prepeval;
    Boolean const_strings;
    Boolean dumpir;
    Boolean experimental;
    Boolean gcc_extensions;
    Boolean stdc_fp_contract;
    Boolean stdc_fenv_access;
    Boolean stdc_cx_limitedr;
    Boolean old_argmatch;
    Boolean optEH;
    Boolean optEH2;
    Boolean new_mangler;
    Boolean microsoft_EH;
    Boolean warningerrors;
    Boolean pedantic;
    Boolean checkprecompflags;
    Boolean supress_warnings;
    Boolean warn_illpragma;
    Boolean warn_emptydecl;
    Boolean warn_possunwant;
    Boolean warn_unusedvar;
    Boolean warn_unusedarg;
    Boolean warn_extracomma;
    Boolean warn_hidevirtual;
    Boolean warn_largeargs;
    Boolean warn_implicitconv;
    Boolean warn_notinlined;
    Boolean warn_structclass;
    Boolean warn_padding;
    Boolean warn_no_side_effect;
    Boolean warn_resultnotused;
    Boolean warn_ptr_int_conv;
    char structalignment;
    Boolean alignarraymembers;
    Boolean dont_reuse_strings;
    Boolean poolstrings;
    Boolean explicit_zero_data;
    Boolean readonly_strings;
    Boolean exceptions;
    Boolean dddummy;
    Boolean dontinline;
    Boolean alwaysinline;
    Boolean peephole;
    Boolean globaloptimizer;
    Boolean sideeffects;
    Boolean cfm_internal;
    Boolean cfm_import;
    Boolean cfm_export;
    Boolean cfm_lib_export;
    Boolean nosyminline;
    Boolean force_active;
    char optimizationlevel;
    Boolean optimizesize;
    Boolean optimizewithasm;
    Boolean crippled;
    Boolean commonsubs;
    Boolean loopinvariants;
    Boolean propagation;
    Boolean deadstore;
    Boolean strengthreduction;
    Boolean strengthreductionstrict;
    Boolean deadcode;
    Boolean lifetimes;
    Boolean logopts;
    Boolean unrolling;
    Boolean vectorizeloops;
    Boolean multiplepasses;
    Boolean opt_pointer_analysis;
    UInt8 opt_pointer_analysis_mode;
    char unrollfactor;
    char unrollinstrfactor;
    Boolean filesyminfo;
    CWDataType appltype;
    CWDataType headtype;
    CWDataType texttype;
    HashNameNode *cursegmentname; // when set by #pragma segment
} CompilerLinkerOptions;

typedef struct ParserTryBlock {
    struct ParserTryBlock *next;
    jmp_buf jmpbuf;
    NameSpace *cscope_current;
    TypeClass *cscope_currentclass;
    Object *cscope_currentfunc;
    TemplStack *ctempl_curinstance;
    TStreamElement *cerror_locktoken;
    Boolean cscope_is_member_func;
} ParserTryBlock;

typedef struct CallbackAction {
    struct CallbackAction *next;
    Object *obj;
    TypeClass *tclass;
} CallbackAction;

extern FileOffsetInfo cparser_fileoffset;
extern TStreamElement symdecltoken;
extern ParserTryBlock *trychain;
extern Boolean inassembler;
extern Boolean dont_set_references;
extern TypeStruct ptmstruct;
extern TypeStruct catchinfostruct;
extern Boolean in_assembler;
extern Boolean illegalimplicitconversion;
extern Boolean in_func_arglist;
extern NameSpaceName *newp_fobj;
extern NameSpaceName *newa_fobj;
extern NameSpaceName *delp_fobj;
extern NameSpaceName *dela_fobj;
extern Object *newh_func;
extern Object *delh_func;
extern Object *copy_func;
extern Object *clear_func;
extern Object *Rgtid_func;
extern Object *Rdync_func;
extern Object *rt_ptmf_cast;
extern Object *rt_ptmf_cmpr;
extern Object *rt_ptmf_test;
extern Object *rt_ptmf_call;
extern Object *rt_ptmf_scall;
extern Object *rt_ptmf_call4;
extern Object *rt_ptmf_scall4;
extern Object *rt_ptmf_null;
extern Object *rt_som_new;
extern Object *rt_som_newcheck;
extern Object *rt_som_check;
extern Object *rt_som_glue1;
extern Object *rt_som_glue2;
extern Object *rt_som_glue3;
extern Object *carr_func;
extern Object *cnar_func;
extern Object *darr_func;
extern Object *dnar_func;
extern Object *dnar3_func;
extern Object *Xgreg_func;
extern Object *Xthrw_func;
extern Object *Xicth_func;
extern Object *Xecth_func;
extern Object *Xunex_func;
extern CompilerLinkerOptions copts;
extern struct GList name_mangle_list;
extern HashNameNode *no_name_node;
extern HashNameNode *temp_argument_name;
extern HashNameNode *this_name_node;
extern HashNameNode *self_name_node;
extern HashNameNode *vptr_name_node;
extern CallbackAction *callbackactions;
extern Boolean fatalerrors;
extern Boolean anyerrors;
extern jmp_buf errorreturn;
extern char string[256];
extern SInt32 compilererrornum;
extern SInt32 compilererrfile;
extern SInt32 compilererrline;

extern Type sttemplexpr;
extern Type stillegal;
extern Type stvoid;
extern TypePointer void_ptr;
extern TypeFunc rt_func;

enum {
    ATOM_VOID = 1,
    ATOM_CHAR,
    ATOM_UCHAR,
    ATOM_WCHAR,
    ATOM_SHORT,
    ATOM_USHORT,
    ATOM_INT,
    ATOM_UINT,
    ATOM_LONG,
    ATOM_ULONG,
    ATOM_LONGLONG,
    ATOM_ULONGLONG,
    ATOM_FLOAT,
    ATOM_SHORTDOUBLE,
    ATOM_DOUBLE,
    ATOM_LONGDOUBLE
};

extern Object *CParser_NewRTFunc(Type *rettype, HashNameNode *name, Boolean flag, int argcount, ...);
extern Boolean CParser_IsPublicRuntimeObject(Object *obj);
extern Object *CParser_FindPublicRuntimeObject(HashNameNode *name);
extern Boolean CParser_ReInitRuntimeObjects(Boolean is_precompiler);
extern void CParser_Setup(void);
extern void CParser_Cleanup(void);
extern short GetPrec(short token);
extern Boolean CParser_ParseOperatorName(short *token, Boolean flag1, Boolean flag2);
extern SInt32 CParser_GetUniqueID(void);
extern void CParser_PrintUniqueID(char *buf);
extern void CParser_SetUniqueID(SInt32 id);
extern HashNameNode *CParser_GetUniqueName(void);
extern HashNameNode *CParser_NameConcat(const char *a, const char *b);
extern HashNameNode *CParser_AppendUniqueName(char *prefix);
extern HashNameNode *CParser_AppendUniqueNameFile(char *prefix);
extern Boolean IsTempName(HashNameNode *name);
extern void CParser_UpdateObject(Object *object, DeclInfo *declinfo);
extern Object *CParser_NewObject(DeclInfo *declinfo);
extern Object *CParser_NewLocalDataObject(DeclInfo *declinfo, Boolean add_to_locals);
extern Object *CParser_NewGlobalDataObject(DeclInfo *declinfo);
extern Object *CParser_NewCompilerDefDataObject(void);
extern Object *CParser_NewFunctionObject(DeclInfo *declinfo);
extern Object *CParser_NewCompilerDefFunctionObject(void);
extern Object *CParser_NewAliasObject(Object *object, SInt32 offset);
extern FuncArg *CParser_NewFuncArg(void);
extern Type *atomtype(void);
extern Object *CParser_FindDeallocationObject(Type *type, FuncArg *args, Boolean flag1, Boolean flag2, Boolean *outflag);
extern short is_memberpointerequal(Type *a, Type *b);
extern short is_typeequal(Type *a, Type *b);
extern short iscpp_typeequal(Type *a, Type *b);
extern short CParser_CompareArgLists(FuncArg *a, FuncArg *b);
extern Boolean is_arglistsame(FuncArg *a, FuncArg *b);
extern short is_typesame(Type *a, Type *b);
extern Type *CParser_GetBoolType(void);
extern Type *CParser_GetWCharType(void);
extern short CParser_GetOperator(ENodeType t);
extern Boolean CParser_IsMoreCVQualified(UInt32 a, UInt32 b);
extern Boolean CParser_IsSameOrMoreCVQualified(UInt32 a, UInt32 b);
extern Boolean is_unsigned(Type *type);
extern StructMember *ismember(TypeStruct *tstruct, HashNameNode *name);
extern void appendmember(TypeStruct *tstruct, StructMember *member);
extern Boolean isdeclaration(UInt8 flag1, UInt8 flag2, UInt8 flag3, short token);
extern Boolean islookaheaddeclaration(void);
extern Type *CParser_ParseTypeID(UInt32 *qual, Boolean *flag);
extern Boolean CParser_TryFuncDecl(void);
extern Boolean CParser_TryParamList(Boolean flag);
extern Type *CParser_RemoveTopMostQualifiers(Type *type, UInt32 *qual);
extern UInt32 CParser_GetTypeQualifiers(Type *type, UInt32 qual);
extern UInt32 CParser_GetCVTypeQualifiers(Type *type, UInt32 qual);
extern Boolean CParser_IsConst(Type *type, UInt32 qual);
extern Boolean CParser_IsVolatile(Type *type, UInt32 qual);
extern Boolean is_const_object(Object *obj);
extern Boolean is_volatile_object(Object *obj);
extern Boolean CParserIsConstExpr(ENode *expr);
extern Boolean CParserIsVolatileExpr(ENode *expr);
extern Boolean CParser_HasInternalLinkage(const Object *obj);
extern Boolean CParser_HasInternalLinkage2(const Object *obj);
extern Boolean CParser_IsVirtualFunction(Object *obj, TypeClass **tclass, SInt32 *index);
extern Boolean is_pascal_object(Object *obj);
extern Boolean is_cfm_type(Type *type);
extern Boolean CParser_IsVTableObject(Object *obj);
extern void TypedefDeclInfo(DeclInfo *declinfo, Type *type, UInt32 qual);
extern void CParser_ParseAttribute(Type *type, DeclInfo *declinfo);
extern void CParser_ParseDeclSpec(DeclInfo *declinfo, Boolean flag);
extern Boolean CParser_CheckTemplateClassUsage(TemplClass *tmclass, Boolean flag);
extern void CParser_GetDeclSpecs(DeclInfo *di, Boolean flag);
extern void CParser_RegisterNonGlobalClass(TypeClass *tclass);
extern void CParser_RegisterSingleExprFunction(Object *func, ENode *expr);
extern void CParser_RegisterDummyCtorFunction(Object *func, Object *obj);
extern Boolean CParser_IsAnonymousUnion(DeclInfo *di, Boolean flag);
extern void CParser_CheckAnonymousUnion(DeclInfo *di, Boolean flag);
extern void CParser_NewCallBackAction(Object *obj, TypeClass *tclass);
extern void CParser_NewClassAction(TypeClass *tclass);
extern void CParser_CallBackAction(Object *obj);
extern Object *CParser_ParseObject(void);
extern void CParser_ParseGlobalDeclaration(void);
extern void cparser(void);

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
