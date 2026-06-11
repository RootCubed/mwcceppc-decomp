#ifndef COMPILER_CPARSER_H
#define COMPILER_CPARSER_H

#include <compiler/common.h>
#include <compiler/enode.h>
#include <compiler/tokens.h>
#include <compiler/types.h>

#ifdef __MWERKS__
// #pragma options align=mac68k
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

struct elfsectioninfo; // TODO: fill out
typedef struct elfsectioninfo elfsectioninfo;

// (Known names from CodeView debug data)
typedef struct CompilerLinkerOptions {
    Boolean littleendian;
    Boolean prologue_helper_functions;
    Boolean epilogue_helper_functions;
    Boolean opt_tail_call;
    short processor;
    char scheduling;
    char schedule_factor;
    Boolean profile;
    Boolean traceback;
    Boolean disable_registers;
    Boolean fp_contract;
    Boolean no_register_save_helpers;
    Boolean unroll_speculative;
    short unroll_instr_limit;
    short unroll_factor_limit;
    Boolean altivec_model;
    Boolean altivec_vrsave;
    Boolean power_of_2_func_align;
    Boolean catssupport;
    Boolean forcecatssupport;
    long nonconstsmalldatathreshold;
    long constsmalldatathreshold;
    Boolean usedatapool;
    Boolean commonblocks;
    Boolean floatingpoint;
    Boolean sfp_emulation;
    Boolean pic;
    Boolean SDS_compatible;
    Boolean disable_extensions;
    char rel109_offset;
    elfsectioninfo *curtext;
    elfsectioninfo *curdata;
    elfsectioninfo *curconst;
    elfsectioninfo *cursdata;
    elfsectioninfo *cursconst;
    long num_sections;
    void *addressing_modes;
    void *interrupt_stack;
    UInt16 *interrupt_top;
    Boolean incompatible_return_small_structs;
    Boolean create_file_object;
    Boolean incompatible_sfpe_double_params;
    Boolean ibm_stackframe;
    Boolean pool_fp_consts;
    Boolean gen_isel;
    Boolean gen_fsel;
    Boolean gecko_float_typecons;
    Boolean disable_vector_frame;
    Boolean opt_bcc_lr_ctr;
    Boolean use_lmw_stmw;
    Boolean misaligned_mem_access;
    Boolean switch_tables;
    Boolean read_only_switch_tables;
    Boolean prepare_compress;
    char min_struct_alignment;
    UInt32 bc_branch_limit;
    UInt32 b_branch_limit;
    Boolean ppc_dont_balance_ir;
    Boolean volatileasm;
    Boolean strictfp;
    Boolean orderedfpcmp;
    Boolean ppc_lvxl_stvxl_errata;
    Boolean ppc_warn_on_gcc_vect_init;
    char ppc_iro_level;
    Boolean abi_frame_align_16;
    Boolean abi_frame_align_32;
    char frame_align_power_2;
    Boolean alignment_metrowerks;
    Boolean old_style_bitfields;
    Boolean fp_contract_aggressive;
    Boolean ppc_dont_balance_exp;
    Boolean ppc_no_fp_blockmove;
    Boolean useDWARF2;
    Boolean sda_pic_pid;
    char code_model;
    Boolean spe;
    Boolean spe_addl;
    Boolean spe2;
    Boolean remove_frsp_aggressive;
    Boolean full_loop_align;
    Boolean aggressive_loop_align;
    Boolean unrolled_loop_align;
    Boolean unswitchloops;
    Boolean dwarf2typedefchains;
    Boolean dwarf2lexblockcodeaddrcalc;
    Boolean dwarfnamespaces;
    Boolean aggressive_ls_scheduling;
    Boolean legacy_struct_alignment;
    Boolean keepcomments;
    Boolean trigraphs;
    Boolean multibyteaware;
    Boolean dollar_identifiers;
    Boolean srcrelincludes;
    Boolean flat_include;
    Boolean syspath_once;
    Boolean always_import;
    Boolean warn_filenamecaps;
    Boolean warn_filenamecaps_as_error;
    Boolean warn_filenamecaps_system;
    Boolean warn_filenamecaps_system_as_error;
    Boolean fullpath_file;
    Boolean simple_prepdump;
    Boolean line_prepdump;
    Boolean fullpath_prepdump;
    Boolean space_prepdump;
    Boolean macro_prepdump;
    Boolean longlong_prepeval;
    Boolean asmsemicolcomment;
    Boolean asmpoundcomment;
    Boolean warn_undefmacro;
    Boolean warn_undefmacro_as_error;
    Boolean multibyteaware_preserve_literals;
    Boolean msg_show_lineref;
    Boolean msg_show_realref;
    Boolean pragma_prepdump;
    Boolean warn_illtokenpasting;
    Boolean new_textloading;
    Boolean filestack_show_fullpath;
    Boolean allow_macro_redefs;
    Boolean asm_accept_unterm_quote;
    Boolean showmessagenumber;
    Boolean warningerrors;
    long maxerrorcount;
    Boolean suppress_warnings;
    Boolean uclcregister;
    char prefixfile[256];
    long inlineautomax;
    long ipainlineautomax;
    long inlinemaxsize;
    long inlinemaxtotalsize;
    short inlinelevel;
    short min_enum_size;
    Boolean ipa_not_complete;
    Boolean inline_bottom_up;
    Boolean inlinebottomuponce;
    Boolean cplusplus;
    Boolean ecplusplus;
    Boolean cpp1x;
    Boolean c9x;
    Boolean c9x_alias_by_type;
    Boolean gcc_extensions;
    Boolean cpp_extensions;
    Boolean iso_templates;
    Boolean ARMscoping;
    Boolean checkprotos;
    Boolean error_on_missingprotos;
    Boolean onlystdkeywords;
    Boolean enumsalwaysint;
    Boolean enums_signed;
    Boolean ANSIstrict;
    Boolean mpwc_relax;
    Boolean mpwc_newline;
    Boolean vast_relax;
    Boolean ignore_oldstyle;
    Boolean pointercast_lvalue;
    Boolean RTTI;
    Boolean delete_exception;
    Boolean test_dummy;
    Boolean oldalignment;
    Boolean unsigned_char;
    Boolean auto_inline;
    Boolean defer_codegen;
    Boolean booltruefalse;
    Boolean longlong;
    Boolean longlong_enums;
    Boolean always_vdispatch;
    Boolean simple_class_byval;
    Boolean wchar_type;
    Boolean vbase_ctor_offset;
    Boolean def_inherited;
    Boolean faster_pch_gen;
    Boolean array_new_delete;
    Boolean arg_dep_lookup;
    Boolean suppress_init_code;
    Boolean reverse_bitfields;
    Boolean float_constants;
    Boolean no_static_dtors;
    Boolean const_strings;
    Boolean dumpir;
    Boolean dumpir_postiro;
    Boolean dump_file;
    Boolean dump_files;
    Boolean dump_source;
    Boolean dump_options;
    Boolean experimental;
    Boolean late_fixes;
    Boolean stdc_fp_contract;
    Boolean stdc_fenv_access;
    Boolean stdc_cx_limitedr;
    Boolean always_fold_floats;
    Boolean optEH;
    Boolean optEHC;
    Boolean new_mangler;
    Boolean show_error_filestack;
    Boolean defer_defarg_parsing;
    Boolean reg_class_allocs;
    Boolean opt_classresults;
    Boolean thread_safe_init;
    Boolean old_class_export;
    Boolean use_simple_stdpnew;
    Boolean use_simple_stdnew;
    Boolean old_tail_padding;
    Boolean old_pods;
    Boolean old_friend_lookup;
    Boolean reject_floats;
    Boolean cobj_extensions;
    Boolean microsoft_EH;
    Boolean no_conststringconv;
    Boolean auto_restrict;
    Boolean auto_restrict_locals;
    Boolean relax_restrict;
    Boolean auto_align8;
    Boolean instance_manager;
    Boolean store_object_files;
    Boolean emit_templ_stack;
    Boolean continue_on_error;
    Boolean access_errors;
    Boolean pedantic;
    Boolean pedantic_as_error;
    Boolean checkprecompflags;
    Boolean warn_illpragma;
    Boolean warn_illpragma_as_error;
    Boolean warn_emptydecl;
    Boolean warn_emptydecl_as_error;
    Boolean warn_possunwant;
    Boolean warn_possunwant_as_error;
    Boolean warn_unusedvar;
    Boolean warn_unusedvar_as_error;
    Boolean warn_unusedarg;
    Boolean warn_unusedarg_as_error;
    Boolean warn_extracomma;
    Boolean warn_extracomma_as_error;
    Boolean warn_hidevirtual;
    Boolean warn_hidevirtual_as_error;
    Boolean warn_largeargs;
    Boolean warn_largeargs_as_error;
    Boolean warn_implicitconv;
    Boolean warn_implicitconv_as_error;
    Boolean warn_impl_i2f_conv;
    Boolean warn_impl_i2f_conv_as_error;
    Boolean warn_impl_f2i_conv;
    Boolean warn_impl_f2i_conv_as_error;
    Boolean warn_impl_s2u_conv;
    Boolean warn_impl_s2u_conv_as_error;
    Boolean relax_i2i_conv;
    Boolean warn_notinlined;
    Boolean warn_notinlined_as_error;
    Boolean warn_structclass;
    Boolean warn_structclass_as_error;
    Boolean warn_padding;
    Boolean warn_padding_as_error;
    Boolean warn_no_side_effect;
    Boolean warn_no_side_effect_as_error;
    Boolean warn_resultnotused;
    Boolean warn_resultnotused_as_error;
    Boolean warn_ptr_int_conv;
    Boolean warn_ptr_int_conv_as_error;
    Boolean warn_no_explicit_virtual;
    Boolean warn_no_explicit_virtual_as_error;
    Boolean warn_uninitializedvar;
    Boolean warn_uninitializedvar_as_error;
    Boolean warn_possiblyuninitializedvar;
    Boolean warn_possiblyuninitializedvar_as_error;
    Boolean warn_illunionmembers;
    Boolean warn_hiddenlocals;
    Boolean warn_hiddenlocals_as_error;
    Boolean warn_missingreturn;
    Boolean warn_missingreturn_as_error;
    Boolean warn_any_ptr_int_conv;
    Boolean warn_any_ptr_int_conv_as_error;
    Boolean warn_nonpod_vararg;
    Boolean warn_missing_enum_case;
    Boolean warn_missing_enum_case_as_error;
    char structalignment;
    Boolean alignarraymembers;
    Boolean dont_reuse_strings;
    Boolean poolstrings;
    Boolean explicit_zero_data;
    Boolean exceptions;
    Boolean dddummy;
    Boolean dontinline;
    Boolean alwaysinline;
    Boolean aggressive_inline;
    Boolean peephole;
    Boolean globaloptimizer;
    Boolean sideeffects;
    Boolean cfm_internal;
    Boolean cfm_import;
    Boolean cfm_export;
    Boolean cfm_lib_export;
    Boolean nosyminline;
    Boolean nosymmacro;
    Boolean force_active;
    Boolean block_assign;
    Boolean nobuiltin;
    Boolean inlineintrinsics;
    Boolean strictheaderchecking;
    char optimizationlevel;
    Boolean optimizesize;
    Boolean optimizewithasm;
    Boolean limitedoptlevel;
    Boolean commonsubs;
    Boolean loopinvariants;
    Boolean propagation;
    Boolean deadstore;
    Boolean strengthreduction;
    Boolean strengthreductionstrict;
    Boolean deadcode;
    Boolean lifetimes;
    Boolean splitlifetimes;
    Boolean unrolling;
    Boolean vectorizeloops;
    Boolean multiplepasses;
    char unrollfactor;
    char fullunrolllimit;
    char unrollinstrfactor;
    Boolean scalarize;
    Boolean prelinearize;
    Boolean treetransformation;
    Boolean arithtransformation;
    Boolean rebuildconditionals;
    Boolean rebuildlogicals;
    Boolean generateconditionalassignments;
    Boolean removeemptyloops;
    Boolean alwaysdumpopts;
    Boolean rotateloops;
    Boolean optimizenonregaccess;
    Boolean movepostops;
    Boolean findoptimalunrollfactor;
    Boolean findoptimalvectorunrollfactor;
    Boolean unrollpostloop;
    Boolean unrollvectorpostloop;
    Boolean createvectortemps;
    Boolean unpromotetypes;
    Boolean dumptypes;
    Boolean scalarizebitfields;
    Boolean propagatemultiplydivideassignments;
    Boolean moveinvariantsinaddressexpr;
    Boolean convertfpconsts;
    Boolean transformcounterloops;
    Boolean markcounterloops;
    Boolean markvariablecounterloops;
    Boolean pulloutconstants;
    Boolean defineargorder;
    Boolean useexpressioncostswhenlinearizingassignments;
    Boolean intelcompareoplinearizationhack;
    Boolean rearrangemultiplesubscripts;
    Boolean returnloopdepthinstatementvalue;
    Boolean marknonregtemps;
    Boolean addgotounrolledlooptest;
    Boolean addtogetherconstantpartsoftreeexpressions;
    Boolean propagateequivalentscalartypes;
    Boolean unpromoteenumtointconversions;
    Boolean unpromotemultipliestochartype;
    Boolean dospecialmultiplyunpromotion;
    Boolean docrossconversionofinductionreference;
    Boolean intrinsicfunctioncallscanhavesideeffects;
    Boolean collapselogicalselectionintobittest;
    Boolean collapselogicalselectionintobittest_uselonglong;
    Boolean reverseloops;
    Boolean foldconstants;
    Boolean optimizecontrolflow;
    Boolean serializeassignments;
    Boolean branchlessexpensivethandiv;
    Boolean repositioncode;
    Boolean decomposeaggregates;
    Boolean recomposeaggregates;
    Boolean scalarizeliveranges;
    Boolean obeyfunceffects;
    Boolean inlineforsize;
    long inline_optimization_max_size;
    long inline_const_arg_analysis_max;
    Boolean econdsfromarithmeticexpressions;
    Boolean altransformlandtoand;
    Boolean removetailrecursions;
    Boolean filesyminfo;
    Boolean symstringtypes;
    Boolean debuginline;
    Boolean showalldwarftypes;
    Boolean nulldwarfentries;
    Boolean reladdends;
    Boolean absolutepath;
    char dwarfversion;
    Boolean compilestats;
    CWDataType appltype;
    CWDataType headtype;
    CWDataType texttype;
    HashNameNode *cursegmentname;
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
// #pragma options align=reset
#endif

#endif
