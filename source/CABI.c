#include <common.h>
#include <compiler/CABI.h>
#include <compiler/CompilerTools.h>
#include <compiler/CClass.h>
#include <compiler/CDecl.h>
#include <compiler/CError.h>
#include <compiler/CException.h>
#include <compiler/CExpr.h>
#include <compiler/CFunc.h>
#include <compiler/CInit.h>
#include <compiler/CMachine.h>
#include <compiler/CMangler.h>
#include <compiler/CMid.h>
#include <compiler/CScope.h>
#include <compiler/CParser.h>
#include <compiler/CTemplateTools.h>
#include <compiler/objects.h>
#include <compiler/scopes.h>
#include <compiler/types.h>

static OffsetList *trans_vtboffsets;

short CABI_GetStructResultArgumentIndex(TypeFunc *tfunc) {
    return 0;
}

Type *CABI_GetSizeTType(void) {
    return TYPE(&stunsignedlong);
}

Type *CABI_GetPtrDiffTType(void) {
    return TYPE(&stsignedlong);
}

void CABI_GetVTableHeaderInfo(VTableHeaderInfo *info) {
    info->size = stsignedlong.size + 4;
    info->m_04 = 0;
    info->m_08 = 4;
}

ENode *CABI_AddPointerOffset(ENode *base, SInt32 offset) {
    if (offset != 0) {
        if (canadd(base, offset) == 0) {
            base = makediadicnode(base, intconstnode(TYPE(&stsignedlong), offset), EADD);
        }
    }
    return base;
}

Boolean CABI_PassedByReference(TypeClass *type) {
    if (type->type == TYPECLASS && CClass_ReferenceArgument(type)) {
        return 1;
    }
    if (CMach_PassAddressOf(type)) {
        return 1;
    }
    return 0;
}

SInt16 CABI_StructSizeAlignValue(Type *type, UInt32 qual, SInt32 size) {
    SInt16 align = CMach_GetTypeAlign(type, qual);
    if (align < 1U) {
        return 0;
    }
    return (align - 1) & (align - ((size & (align - 1))));
}

void CABI_ReverseBitField(TypeBitfield *tbitfield) {
    UInt32 bits;

    switch (tbitfield->bitfieldtype->size) {
        case 1:
            bits = 8;
            break;
        case 2:
            bits = 16;
            break;
        case 4:
            bits = 32;
            break;
        case 8:
            bits = 64;
            break;
        default:
            #line 358
            CError_FATAL();
    }

    tbitfield->offset = (bits - tbitfield->offset) - tbitfield->bitlength;
}

Object *CABI_NewGuardVariable(Object *expr) {
    Object *obj;

    if (expr == NULL) {
        #line 372
        CError_FATAL();
    }
    obj = CParser_NewCompilerDefDataObject();
    obj->type = TYPE(&stsignedchar);
    obj->name = CParser_GetUniqueName();
    obj->sclass = expr->sclass;
    obj->qual |= expr->qual & 0x60000;
    CParser_UpdateObject(obj, NULL);
    CMangler_SetupGuardVarName(obj, expr);
    CInit_DeclareData(obj, NULL, NULL, obj->type->size);
    return obj;
}

ENode *CABI_AcquireGuardVariable(Object *obj) {
    return checkreference(CExpr_New_EINDIRECT_Node(obj));
}

ENode *CABI_ReleaseGuardVariable(Object *obj) {
    return makediadicnode(CABI_AcquireGuardVariable(obj), intconstnode(TYPE(&stsignedchar), 1), EASS);
}

SInt32 CABI_ComputeNewArrayPadding(void) {
    return 0x10;
}

SInt32 CABI_GetVTableOffset(TypeClass *tclass) {
    return 0;
}

static Object *CABI_FindZeroVirtualBaseMember(TypeClass *tclass, Object *obj) {
    NameSpaceObjectList *nsol;
    ClassList *base;
    Object *chk;

    for (nsol = CScope_FindName(tclass->nspace, obj->name); nsol; nsol = nsol->next) {
        chk = OBJECT(nsol->object);
        if (
            chk->otype == OT_OBJECT &&
            chk->datatype == DVFUNC &&
            CClass_GetOverrideKind(chk, obj, 0) == OVERRIDE_VIRTUAL
        ) {
            return chk;
        }
    }

    for (base = tclass->bases; base; base = base->next) {
        if (!base->is_virtual && !base->offset && !base->voffset && base->base->vtable) {
            chk = CABI_FindZeroVirtualBaseMember(base->base, obj);
            if (chk) {
                return chk;
            }
        }
    }

    return NULL;
}

void CABI_AddVTable(TypeClass *tclass) {
    tclass->vtable = galloc(sizeof(VTable));
    memclrw(tclass->vtable, sizeof(VTable));
}

static void CABI_ApplyClassFlags(Object *obj, UInt8 flags) {
    if (flags & CLASS_EFLAGS_INTERNAL) {
        obj->flags |= OBJECT_INTERNAL;
    }
    if (flags & CLASS_EFLAGS_IMPORT) {
        obj->flags |= OBJECT_IMPORT;
    }
    if (flags & CLASS_EFLAGS_EXPORT) {
        obj->flags |= OBJECT_EXPORT;
    }
}

static void CABI_AllocateVTable(ClassListList *clsList, ClassLayout *layout, TypeClass *tclass) {
    ObjMemberVar *member;
    TypeClass *cls;
    int i;

    if (!tclass->vtable) {
        CABI_AddVTable(tclass);
        layout->xA = layout->lex_order_count - 1;
    }

    if (clsList != NULL && clsList->m_0c) {
        #line 1381
        CError_ASSERT(clsList->base != NULL);
        cls = clsList->base->base;
    } else {
        cls = NULL;
    }

    if (cls == NULL) {
        member = galloc(sizeof(ObjMemberVar));
        memclrw(member, sizeof(ObjMemberVar));

        member->otype = OT_MEMBERVAR;
        member->access = ACCESSPUBLIC;
        member->name = vptr_name_node;
        member->type = TYPE(&vtable_ptr);
        layout->vtable_ivar = member;

        for (i = layout->xA; ; i--) {
            if (i < 0) {
                member->next = tclass->ivars;
                tclass->ivars = member;
                break;
            }

            #line 1413
            CError_ASSERT(layout->objlist[i]);

            if (layout->objlist[i]->otype == OT_MEMBERVAR) {
                member->next = OBJ_MEMBER_VAR(layout->objlist[i])->next;
                OBJ_MEMBER_VAR(layout->objlist[i])->next = member;
                break;
            }
        }
    } else {
        tclass->vtable->owner = cls->vtable->owner;
        layout->vtable_ivar = NULL;
    }
}

static void CABI_MakeVTableLayout(ClassListList *clsList, ClassLayout *layout, TypeClass *tclass) {
    TypeClass *cls;
    Object *obj;
    ClassListList *cur;
    SInt32 vtsize;
    int i;
    UInt16 flags;
    VTableHeaderInfo hi;

    vtsize = 0;
    CABI_GetVTableHeaderInfo(&hi);
    if (clsList != NULL && clsList->m_0c) {
        #line 1450
        CError_ASSERT(clsList->base != NULL);
        cls = clsList->base->base;
        vtsize = 0;
    } else {
        cls = NULL;
        if (tclass->flags & (CLASS_SINGLE_OBJECT | CLASS_COM_OBJECT)) {
            vtsize = vtable_ptr.size;
        } else {
            vtsize = hi.size;
        }
    }

    for (cur = clsList; cur; cur = cur->next) {
        ClassList *curbase = cur->base;
        if (curbase != NULL && curbase->base->vtable != NULL) {
            curbase->voffset = vtsize;
            vtsize += curbase->base->vtable->size;
        }
    }

    for (i = 0; i < layout->lex_order_count; i++) {
        Object *base;
        #line 1481
        CError_ASSERT(base = OBJECT(layout->objlist[i]));
        if (base->otype == OT_OBJECT && base->datatype == DVFUNC) {
            TypeMemberFunc *tmethod = TYPE_METHOD(base->type);
            if (cls == NULL) {
                vtsize += 4;
            } else {
                Object *baseobj = CABI_FindZeroVirtualBaseMember(cls, base);
                if (baseobj) {
                    tmethod->vtbl_index = TYPE_METHOD(baseobj->type)->vtbl_index;
                } else {
                    tmethod->vtbl_index = vtsize;
                    vtsize += 4;
                }
            }
        }
    }

    tclass->vtable->size = vtsize;

    for (cur = clsList; cur; cur = cur->next) {
        VClassList *curvbase = cur->vbase;
        if (curvbase != NULL && curvbase->base->vtable != NULL) {
            curvbase->voffset = vtsize;
            vtsize += curvbase->base->vtable->size;
        }
    }

    obj = CParser_NewCompilerDefDataObject();
    CABI_ApplyClassFlags(obj, tclass->eflags);

    obj->name = CMangler_VTableName(tclass);
    obj->type = CDecl_NewOpaqueType(vtsize, 4);
    obj->qual = Q_CONST;
    obj->nspace = tclass->nspace;
    switch (tclass->action) {
        case CLASS_ACTION_0:
            obj->sclass = TK_STATIC;
            obj->qual |= Q_20000;
            break;
    }
    CParser_UpdateObject(obj, NULL);
    tclass->vtable->object = obj;
    tclass->vtable->offset = vtsize;
}

static Object *CABI_ThisArg(void) {
    #line 1534
    CError_ASSERT(arguments && IS_TYPE_POINTER_ONLY(arguments->object->type));
    return arguments->object;
}

ENode *CABI_MakeThisExpr(TypeClass *tclass, SInt32 offset) {
    ENode *expr;

    if (tclass) {
        expr = CABI_AcquireGuardVariable(CABI_ThisArg());
        if (tclass->flags & CLASS_HANDLEOBJECT) {
            expr = makemonadicnode(expr, EINDIRECT);
        }
    } else {
        expr = CABI_AcquireGuardVariable(CABI_ThisArg());
    }

    return CABI_AddPointerOffset(expr, offset);
}

static Boolean CABI_FindOffset(SInt32 offset) {
    OffsetList *offsetlist;
    for (offsetlist = trans_vtboffsets; offsetlist; offsetlist = offsetlist->next) {
        if (offsetlist->offset == offset && offsetlist->m_08 == 0) {
            return 1;
        }
    }

    offsetlist = lalloc(sizeof(OffsetList));
    offsetlist->next = trans_vtboffsets;
    offsetlist->offset = offset;
    offsetlist->m_08 = 0;
    trans_vtboffsets = offsetlist;
    return 0;
}

static ENode *CABI_0056e210(ENode *node, TypeClass *cls1, TypeClass *cls2, TypeClass *cls3, SInt32 offset) {
    ClassList *list;
    Object *obj;
    OffsetList *offsetlist;
    SInt32 newOffset;
    ENode *vtptr;

    for (list = cls2->bases; list; list = list->next) {
        if (list->base == cls3 && list->is_virtual) {
            newOffset = offset + list->offset;

            if (!CABI_FindOffset(newOffset)) {
                vtptr = makemonadicnode(
                    CABI_AddPointerOffset(CABI_AcquireGuardVariable(CABI_ThisArg()), newOffset),
                    EINDIRECT
                );
                vtptr->rtype = TYPE(&void_ptr);

                node = makediadicnode(
                    vtptr,
                    node,
                    EASS
                );
            }
        }

        if (list->is_virtual) {
            newOffset = CClass_FindVirtualBase(cls1, list->base)->offset;
        } else {
            newOffset = offset + list->offset;
        }
        node = CABI_0056e210(node, cls1, list->base, cls3, newOffset);
    }
    return node;
}

static OffsetList *CABI_GetVBasePath(TypeClass *tclass, TypeClass *baseclass) {
    ClassList *base;
    OffsetList *best;
    OffsetList *list;
    OffsetList *scan;
    short bestLength;
    short length;

    for (base = tclass->bases; base; base = base->next) {
        if (base->base == baseclass && base->is_virtual) {
            best = lalloc(sizeof(OffsetList));
            best->next = NULL;
            best->offset = base->offset;
            return best;
        }
    }

    best = NULL;

    for (base = tclass->bases; base; base = base->next) {
        list = CABI_GetVBasePath(base->base, baseclass);
        if (list) {
            length = 1;
            for (scan = list->next; scan; scan = scan->next) {
                length++;
            }

            if (base->is_virtual) {
                length++;
            }

            if (!best || length < bestLength) {
                if (base->is_virtual) {
                    best = lalloc(sizeof(OffsetList));
                    best->next = list;
                    best->offset = base->offset;
                } else {
                    best = list;
                    best->offset += base->offset;
                }
                bestLength = length;
            }
        }
    }

    return best;
}

static SInt32 CABI_FindNVBase(TypeClass *tclass, TypeClass *baseclass, SInt32 offset) {
    ClassList *base;
    SInt32 tmp;

    if (tclass == baseclass)
        return offset;

    for (base = tclass->bases; base; base = base->next) {
        if (!base->is_virtual && (tmp = CABI_FindNVBase(base->base, baseclass, offset + base->offset)) >= 0) {
            return tmp;
        }
    }

    return -1;
}

SInt32 CABI_GetCtorOffsetOffset(TypeClass *tclass, TypeClass *baseclass) {
    SInt32 baseSize;
    SInt32 size;
    char saveAlignMode;

    size = tclass->othersize;
    if (baseclass) {
        baseSize = CABI_FindNVBase(tclass, baseclass, 0);
        #line 1791
        CError_ASSERT(baseSize >= 0);
        size -= baseSize;
    }

    saveAlignMode = copts.structalignment;
    if (tclass->eflags & CLASS_EFLAGS_F0) {
        copts.structalignment = ((tclass->eflags & CLASS_EFLAGS_F0) >> 4) - 1;
    }
    size += CMach_MemberAlignValue(TYPE(&stunsignedlong), size);
    copts.structalignment = saveAlignMode;

    return size;
}

static Boolean CABI_IsOperatorNew(Object *obj) {
    return
        obj->otype == OT_OBJECT &&
        IS_TYPE_FUNC(obj->type) &&
        TYPE_FUNC(obj->type)->args &&
        TYPE_FUNC(obj->type)->args->type == CABI_GetSizeTType() &&
        !TYPE_FUNC(obj->type)->args->next;
}

Object *CABI_ConstructorCallsNew(TypeClass *tclass) {
    NameSpaceObjectList *nsol;
    NameResult pr;

    if (tclass->flags & CLASS_HANDLEOBJECT) {
        if (CScope_FindClassMemberObject(tclass, &pr, CMangler_OperatorName(TK_NEW))) {
            if (pr.obj_10) {
                if (CABI_IsOperatorNew(OBJECT(pr.obj_10))) {
                    return OBJECT(pr.obj_10);
                }
            } else {
                for (nsol = pr.nsol_14; nsol; nsol = nsol->next) {
                    if (CABI_IsOperatorNew(OBJECT(nsol->object))) {
                        return OBJECT(nsol->object);
                    }
                }
            }
        }
        return newh_func;
    }

    return NULL;
}

Boolean CABI_ConstructorReturnsThis(TypeClass *tclass) {
    return 1;
}

Boolean CABI_DestructorReturnsThis(TypeClass *tclass) {
    return 1;
}

FuncArg *CABI_GetFirstRealArgument(TypeFunc *tfunc) {
    FuncArg *arg;
    TypeClass *thiscls;
    TypeMemberFunc *tmethod;

    #line 2229
    CError_ASSERT(tfunc->type == TYPEFUNC);

    arg = tfunc->args;
    if (tfunc->flags & FUNC_METHOD) {
        tmethod = TYPE_METHOD(tfunc);
        if (!tmethod->is_static) {
            thiscls = tmethod->theclass;
            #line 2237
            CError_ASSERT(arg != NULL);
            arg = arg->next;
            if (tmethod->flags & FUNC_IS_DTOR) {
                return NULL;
            }
            if (thiscls->flags & CLASS_HAS_VBASES && tmethod->flags & FUNC_IS_CTOR) {
                #line 2257
                CError_ASSERT(arg != NULL);
                arg = arg->next;
            }
        }
    }

    return arg;
}

static Boolean CABI_ArgCheck(FuncArg *arg) {
    return arg != NULL && arg->dexpr == NULL && !(arg->m_1a & 2);
}

Object *CABI_DummyDefaultConstructor(TypeClass *tclass) {
    NameSpaceObjectList *nsol;
    Object *ctor;
    Object *currobj;
    FuncArg *arg;
    Object *funcobj;
    HashNameNode *name;
    TypeFunc *newconstructor;
    ObjectList olst;

    ctor = NULL;
    for (nsol = CScope_FindName(tclass->nspace, constructor_name_node); nsol; nsol = nsol->next) {
        currobj = OBJECT(nsol->object);
        if (currobj->otype == OT_OBJECT && currobj->type->type == TYPEFUNC) {
            if (TYPE_FUNC(currobj->type)->flags & FUNC_IS_TEMPL) {
                continue;
            }

            arg = CABI_GetFirstRealArgument(TYPE_FUNC(currobj->type));
            #line 2288
            CError_ASSERT(arg != NULL);

            if (!CABI_ArgCheck(arg)) {
                if (ctor != NULL) {
                    olst.next = NULL;
                    olst.object = currobj;
                    CError_OverloadedFunctionError(ctor, &olst);
                    break;
                }
                ctor = currobj;
            }
        }
    }

    if (ctor == NULL) {
        CError_Error(CErrorStr10203);
        return NULL;
    }

    name = GetHashNameNodeExport("__defctor");
    nsol = CScope_FindName(tclass->nspace, name);
    if (nsol != NULL) {
        #line 2315
        CError_ASSERT(nsol->object->otype == OT_OBJECT && OBJECT(nsol->object)->type->type == TYPEFUNC);
        return OBJECT(nsol->object);
    }

    newconstructor = galloc(sizeof(TypeMemberFunc));
    memclrw(newconstructor, sizeof(TypeMemberFunc));
    newconstructor->type = TYPEFUNC;
    newconstructor->functype = TYPE(&void_ptr);
    newconstructor->flags = FUNC_METHOD;
    TYPE_METHOD(newconstructor)->theclass = tclass;

    CDecl_SetFuncFlags(newconstructor, 0);
    if (tclass->flags & CLASS_HAS_VBASES) {
        CDecl_AddArgument(newconstructor, TYPE(&stsignedshort));
    }
    CDecl_AddThisPointerArgument(newconstructor, tclass);

    funcobj = CParser_NewCompilerDefFunctionObject();
    funcobj->type = TYPE(newconstructor);
    funcobj->qual = Q_INLINE | Q_MANGLE_NAME;
    funcobj->nspace = tclass->nspace;
    funcobj->name = name;

    CScope_AddObject(tclass->nspace, name, OBJ_BASE(funcobj));
    CMid_RegisterDummyCtorFunction(funcobj, ctor);

    return funcobj;
}

ENode *CABI_GetVBaseCTorArg(TypeClass *tclass, Type *type, Boolean value) {
    if (tclass->flags & CLASS_HAS_VBASES) {
        return intconstnode(TYPE(&stsignedshort), value ? 1 : 0);
    }
    return NULL;
}

static Object *CABI_VArg(void) {
    CError_ASSERT(arguments && arguments->next && IS_TYPE_INT(arguments->next->object->type));
    return arguments->next->object;
}

static Statement *CABI_InitVBasePtrs(Statement *stmt, TypeClass *tclass) {
    ENode *expr;
    VClassList *vbase;

    for (vbase = tclass->vbases, trans_vtboffsets = NULL; vbase; vbase = vbase->next) {
        expr = CABI_0056e210(CABI_MakeThisExpr(NULL, vbase->offset), tclass, tclass, vbase->base, 0);
        stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
        stmt->expr = expr;
    }

    return stmt;
}

static Statement *CABI_InitVBaseCtorOffsets(Statement *stmt, TypeClass *tclass);

static ENode *CABI_0056bb30(TypeClass *tclass, TypeClass *base, SInt32 offset, Boolean p4, Boolean p5, Boolean p6);

typedef struct WeirdClassList {
    TypeClass *cls1;
    TypeClass *cls2;
    UInt32 smth;
    UInt32 *smthptr;
    TypeClass *cls3;
    UInt8 pad[5];
    UInt8 m_1d;
} WeirdClassList;
static Statement *CABI_InitVTablePtrs(Statement *stmt, WeirdClassList *vtableObj, Boolean p3, Boolean p4);

static Statement *CABI_0056abb0(Statement *stmt, TypeClass *tclass, Boolean p3, Boolean p4);

static ENode *CABI_0056d340(TypeClass *cls, TypeClass *tclass, ENode *expr, Boolean p4, Boolean p5, Boolean p6, Boolean *errorflag);

static ENode *CABI_NewCall(Object *func, SInt32 size) {
    return funccallexpr(func, intconstnode(CABI_GetSizeTType(), size), NULL, NULL, NULL);
}

static Type *MakeVBasePointerType(VClassList *vbase) {
    return CDecl_NewPointerType(TYPE(vbase->base));
}

static Statement *CABI_Inline1(Statement *firstStmt) {
    Statement *stmt;

    stmt = firstStmt;
    while (1) {
        CError_ASSERT(stmt);
        if (stmt->type == ST_BEGINCATCH) {
            return stmt;
        }
        stmt = stmt->next;
    }
}

static void CABI_TransConstructor(Object *obj, Statement *firstStmt, TypeClass *tclass, Boolean is_copy_constructor, Boolean has_try) {
    Statement *stmt;
    Statement *currstmt;
    Object *newfunc;
    Object *dtorfunc;
    Object *tmpfunc2;
    Object *cond;
    CLabel *label;
    ENode *expr;
    ClassList *base;
    VClassList *vbase;
    ObjMemberVar *ivar;
    Type *type;
    CtorChain *chain_base;
    CtorChain *chain;
    Boolean errorflag;
    Boolean found;
    WeirdClassList clslist;

    stmt = firstStmt;
    while (1) {
        if (stmt == NULL) {
            chain_base = NULL;
            stmt = firstStmt;
            break;
        }
        if (stmt->type == ST_EXPRESSION && stmt->expr->type == ECTORINIT) {
            stmt->type = ST_NOP;
            chain_base = stmt->expr->data.ctorinit;
            break;
        }
        stmt = stmt->next;
    }

    newfunc = CABI_ConstructorCallsNew(tclass);
    if (newfunc) {
        label = newlabel();

        stmt = CFunc_InsertStatement(ST_IFGOTO, stmt);
        stmt->expr = CABI_MakeThisExpr(NULL, 0);
        stmt->label = label;

        expr = makediadicnode(
            CABI_MakeThisExpr(NULL, 0),
            CABI_NewCall(newfunc, tclass->size),
            EASS
        );
        stmt = CFunc_InsertStatement(ST_IFGOTO, stmt);
        stmt->expr = expr;
        stmt->label = label;

        stmt = CFunc_InsertStatement(ST_RETURN, stmt);
        stmt->expr = NULL;

        stmt = CFunc_InsertStatement(ST_LABEL, stmt);
        stmt->label = label;
        label->stmt = stmt;
    }

    if (has_try) {
        stmt = firstStmt;
        while (1) {
            #line 2707
            CError_ASSERT(stmt);
            if (stmt->type == ST_BEGINCATCH) {
                break;
            }
            stmt = stmt->next;
        }
    }

    if (tclass->flags & CLASS_HAS_VBASES) {
        label = newlabel();

        stmt = CFunc_InsertStatement(ST_IFGOTO, stmt);
        stmt->expr = CExpr_New_EEQU_Node(CABI_AcquireGuardVariable(CABI_VArg()), intconstnode(TYPE(&stsignedshort), 0));
        stmt->label = label;

        trans_vtboffsets = NULL;

        for (vbase = tclass->vbases; vbase; vbase = vbase->next) {
            expr = makemonadicnode(CABI_AddPointerOffset(CABI_AcquireGuardVariable(CABI_ThisArg()), vbase->offset), ETYPCON);
            expr->rtype = MakeVBasePointerType(vbase);

            expr = CABI_0056e210(expr, tclass, tclass, vbase->base, 0);
            stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
            stmt->expr = expr;
        }

        for (vbase = tclass->vbases; vbase; vbase = vbase->next) {
            if (is_copy_constructor) {
                expr = CABI_0056bb30(tclass, vbase->base, vbase->offset, 1, 0, 1);
            } else {
                for (chain = chain_base; chain; chain = chain->next) {
                    if (chain->u.vbase == vbase) {
                        CError_ASSERT(chain->what == CtorChain_VBase);
                        expr = chain->objexpr;
                        break;
                    }
                }

                if (!chain) {
                    expr = makemonadicnode(CABI_AddPointerOffset(CABI_AcquireGuardVariable(CABI_ThisArg()), vbase->offset), ETYPCON);
                    expr->rtype = MakeVBasePointerType(vbase);

                    expr = CABI_0056d340(vbase->base, tclass, expr, 0, 1, 1, &errorflag);
                    if (expr == NULL && errorflag) {
                        CError_Error(CErrorStr10213, tclass, 0, vbase->base, 0);
                    }
                }
            }

            if (expr) {
                stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                stmt->expr = expr;
            }

            dtorfunc = CClass_Destructor(vbase->base);
            if (dtorfunc) {
                if (!vbase) {
                    cond = NULL;
                } else {
                    cond = CABI_VArg();
                }
                CExcept_RegisterMember(stmt, CABI_ThisArg(), vbase->offset, dtorfunc, cond, 0);
            }
        }

        stmt = CFunc_InsertStatement(ST_LABEL, stmt);
        stmt->label = label;
        label->stmt = stmt;
    }

    for (base = tclass->bases; base; base = base->next) {
        SInt32 offset;
        TypeClass *basecls;
        if (base->is_virtual) {
            continue;
        }

        if (is_copy_constructor) {
            expr = CABI_0056bb30(tclass, vbase->base, vbase->offset, 1, 0, 0);
        } else {
            for (chain = chain_base; chain; chain = chain->next) {
                if (chain->u.vbase == vbase) {
                    #line 2796
                    CError_ASSERT(chain->what == CtorChain_VBase);
                    expr = chain->objexpr;
                    break;
                }
            }

            if (chain) {
                expr = CABI_AddPointerOffset(CABI_AcquireGuardVariable(CABI_ThisArg()), vbase->offset);

                expr = CABI_0056d340(vbase->base, tclass, expr, 0, 1, 0, &errorflag);
                if (expr == NULL && errorflag) {
                    CError_Error(CErrorStr10213, tclass, 0, vbase->base, 0);
                }
            }
        }

        if (expr) {
            stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
            stmt->expr = expr;
        }

        if (base != NULL) {
            basecls = base->base;
            offset = base->offset;
        } else {
            basecls = base->base;
            offset = base->offset;
        }

        dtorfunc = CClass_Destructor(basecls);
        if (dtorfunc) {
            CExcept_RegisterMember(stmt, CABI_ThisArg(), offset, dtorfunc, NULL, 0);
        }
    }

    if (tclass->vtable && tclass->flags & CLASS_FLAGS_4000 && tclass->vtable->object != NULL) {
        memclrw(&clslist, sizeof(clslist));
        clslist.smthptr = &clslist.smth;
        clslist.cls1 = tclass;
        clslist.cls2 = tclass;
        clslist.cls3 = tclass;
        clslist.m_1d = 0;
        trans_vtboffsets = NULL;

        stmt = CABI_InitVTablePtrs(stmt, &clslist, 0, 1);
    }

    if (tclass->flags & CLASS_FLAGS_8000) {
        stmt = CABI_InitVBaseCtorOffsets(stmt, tclass);
    }

    if (is_copy_constructor) {
        stmt = CABI_0056abb0(stmt, tclass, 1, 0);
    } else {
        for (ivar = tclass->ivars; ivar; ivar = ivar->next) {
            for (chain = chain_base; chain; chain = chain->next) {
                if (chain->u.vbase == vbase) {
                    #line 2929
                    CError_ASSERT(chain->u.vbase->base != NULL && chain->what == CtorChain_VBase);
                    break;
                }
            }

            if (chain) {
                stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                stmt->expr = chain->objexpr;
                type = ivar->type;
                switch (type->type) {
                    case TYPEARRAY:
                        do {
                            type = TPTR_TARGET(type);
                        } while (IS_TYPE_ARRAY(type));
                        if (IS_TYPE_CLASS(type)) {
                            if ((newfunc = CClass_Destructor(TYPE_CLASS(type)))) {
                                CError_ASSERT(type->size);
                                CExcept_RegisterMemberArray(
                                    stmt,
                                    CABI_ThisArg(),
                                    ivar->offset,
                                    newfunc,
                                    ivar->type->size / type->size,
                                    type->size);
                            }
                        }
                        break;
                    case TYPECLASS:
                        if ((newfunc = CClass_Destructor(TYPE_CLASS(type)))) {
                            CExcept_RegisterMember(
                                stmt,
                                CABI_ThisArg(),
                                ivar->offset,
                                newfunc,
                                NULL,
                                1);
                        }
                        break;
                    default:
                        break;
                }
            } else {
                type = ivar->type;
                switch (type->type) {
                    case TYPEARRAY:
                        if (type->size == 0) {
                            break;
                        }
                        do {
                            type = TPTR_TARGET(type);
                        } while (IS_TYPE_ARRAY(type));
                        if (IS_TYPE_CLASS(type) && CClass_Constructor(TYPE_CLASS(type))) {
                            if (
                                (newfunc = CClass_DefaultConstructor(TYPE_CLASS(type))) ||
                                (newfunc = CClass_DummyDefaultConstructor(TYPE_CLASS(type)))
                                )
                            {
                                tmpfunc2 = CClass_Destructor(TYPE_CLASS(type));
                                if (tmpfunc2)
                                    tmpfunc2 = CABI_GetDestructorObject(tmpfunc2, 1);

                                stmt = CInit_ConstructClassArray(
                                    stmt,
                                    TYPE_CLASS(type),
                                    newfunc,
                                    tmpfunc2,
                                    CABI_MakeThisExpr(tclass, ivar->offset),
                                    ivar->type->size / type->size);

                                if (tmpfunc2) {
                                    CExcept_RegisterMemberArray(
                                        stmt,
                                        CABI_ThisArg(),
                                        ivar->offset,
                                        tmpfunc2,
                                        ivar->type->size / type->size,
                                        type->size);
                                }
                            } else {
                                CError_Error(CErrorStr10214, tclass, 0, ivar->name->name);
                            }
                        }
                        break;
                    case TYPECLASS:
                        expr = makemonadicnode(CABI_MakeThisExpr(tclass, ivar->offset), ETYPCON);
                        expr->rtype = CDecl_NewPointerType(TYPE(type));

                        expr = CClass_DefaultConstructorCall(
                            TYPE_CLASS(type),
                            tclass,
                            expr,
                            1, 1, 0, &errorflag);

                        if (expr) {
                            stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                            stmt->expr = expr;
                        } else if (errorflag) {
                            CError_Error(CErrorStr10214, tclass, 0, ivar->name->name);
                        }

                        if ((newfunc = CClass_Destructor(TYPE_CLASS(type)))) {
                            if (!expr) {
                                stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                                stmt->expr = nullnode();
                            }
                            CExcept_RegisterMember(
                                stmt,
                                CABI_ThisArg(),
                                ivar->offset,
                                newfunc,
                                NULL,
                                1);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    for (stmt = firstStmt->next; stmt; stmt = stmt->next) {
        if (stmt->type == ST_RETURN) {
            CError_ASSERT(stmt->expr == NULL);
            stmt->expr = CABI_MakeThisExpr(NULL, 0);
        }
    }
}

void CABI_FinishConstructor(Object *object, Statement *stmt, Statement *stmt2, Boolean p4, Boolean error_check, Boolean p6) {
    #line 2992
    CError_ASSERT(cscope_currentclass != NULL);
    CABI_TransConstructor(object, stmt, cscope_currentclass, p4, p6);
    if (error_check) {
        CFunc_ErrorCheck(object, stmt);
    }
    CFunc_DestructorCleanup(stmt);
    CFunc_CodeCleanup(stmt);
    CFunc_Gen(stmt, object);
}

static VClassList *CABI_GetVClass(VClassList *vbases, TypeClass *base) {
    VClassList *vbase;
    for (vbase = vbases; vbase; vbase = vbase->next) {
        if (vbase->base == base) {
            return vbase;
        }
    }
    return NULL;
}

void CABI_MakeDefaultConstructor(TypeClass *tclass, Object *func) {
    Boolean saveDebugInfo;
    CScopeSave savedScope;
    Statement firstStmt;
    Statement returnStmt;
    TemplStack *stack;

    if (anyerrors || func->access == ACCESSNONE) {
        return;
    }

    CABI_ApplyClassFlags(func, tclass->eflags);

    stack = CTemplTool_PushInstance(NULL, func);

    CScope_SetFunctionScope(func, &savedScope);

    CFunc_FuncGenSetup(NULL, &firstStmt, func, 0);

    saveDebugInfo = copts.filesyminfo;
    copts.filesyminfo = 0;

    CFunc_SetupNewFuncArgs(NULL, func, TYPE_FUNC(func->type)->args);

    if (tclass->flags & CLASS_HAS_VBASES) {
        arguments->next->object->name = CParser_GetUniqueName();
    }

    firstStmt.next = &returnStmt;

    memclrw(&returnStmt, sizeof(Statement));
    returnStmt.type = ST_RETURN;

    CABI_TransConstructor(func, &firstStmt, cscope_currentclass, 0, 0);
    CFunc_DestructorCleanup(&firstStmt);
    CFunc_CodeCleanup(&firstStmt);
    CFunc_Gen(&firstStmt, func);

    CScope_RestoreScope(&savedScope);

    CTemplTool_PopInstance(stack);

    copts.filesyminfo = saveDebugInfo;
}


void CABI_MakeDefaultCopyConstructor(TypeClass *tclass, Object *func) {
    Boolean saveDebugInfo;
    CScopeSave savedScope;
    Statement firstStmt;
    Statement returnStmt;
    TemplStack *stack;

    if (anyerrors || func->access == ACCESSNONE) {
        return;
    }

    CABI_ApplyClassFlags(func, tclass->eflags);

    stack = CTemplTool_PushInstance(NULL, func);

    CScope_SetFunctionScope(func, &savedScope);

    CFunc_FuncGenSetup(NULL, &firstStmt, func, 0);

    saveDebugInfo = copts.filesyminfo;
    copts.filesyminfo = 0;

    CFunc_SetupNewFuncArgs(NULL, func, TYPE_FUNC(func->type)->args);

    if (tclass->flags & CLASS_HAS_VBASES) {
        arguments->next->object->name = CParser_GetUniqueName();
    }

    firstStmt.next = &returnStmt;

    memclrw(&returnStmt, sizeof(Statement));
    returnStmt.type = ST_RETURN;

    CABI_TransConstructor(func, &firstStmt, cscope_currentclass, 1, 0);
    CFunc_DestructorCleanup(&firstStmt);
    CFunc_CodeCleanup(&firstStmt);
    CFunc_Gen(&firstStmt, func);

    CScope_RestoreScope(&savedScope);

    CTemplTool_PopInstance(stack);

    copts.filesyminfo = saveDebugInfo;
}

static ENode *CABI_MakeCopyConArgExpr(TypeClass *tclass, Boolean flag) {
    ObjectList *args;

    #line 1606
    CError_ASSERT(args = arguments);
    #line 1607
    CError_ASSERT(args = args->next);
    if (flag && (tclass->flags & CLASS_HAS_VBASES)) {
        CError_ASSERT(args = args->next);
    }
    #line 1613
    CError_ASSERT(IS_TYPE_POINTER_ONLY(args->object->type));

    return CABI_AcquireGuardVariable(args->object);
}

static Statement *CABI_CopyConAssignCB(Statement *stmt, TypeClass *tclass, TypeClass *baseclass, SInt32 offset);

void CABI_MakeDefaultAssignmentOperator(TypeClass *tclass, Object *func) {
    Boolean saveDebugInfo;
    CScopeSave savedScope;
    Statement firstStmt;
    TemplStack *stack;
    Statement *stmt;
    ClassList *base;
    VClassList *vbase;
    ENode *expr;
    ENode *expr1;
    ENode *expr2;

    if (anyerrors || func->access == ACCESSNONE) {
        return;
    }

    CABI_ApplyClassFlags(func, tclass->eflags);

    stack = CTemplTool_PushInstance(NULL, func);

    CScope_SetFunctionScope(func, &savedScope);

    CFunc_FuncGenSetup(NULL, &firstStmt, func, 0);

    saveDebugInfo = copts.filesyminfo;
    copts.filesyminfo = 0;

    CFunc_SetupNewFuncArgs(NULL, func, TYPE_FUNC(func->type)->args);

    stmt = curstmt;

    if (tclass->mode == CLASS_MODE_UNION || (copts.removeemptyloops && CClass_IsTrivialCopyAssignClass(tclass))) {
        expr1 = makemonadicnode(CABI_MakeThisExpr(tclass, 0), EINDIRECT);
        expr1->rtype = TYPE(tclass);

        expr2 = CABI_MakeCopyConArgExpr(tclass, 0);
        expr2->rtype = TYPE(tclass);

        if (TYPE_CLASS(expr1->rtype)->size != 0) {
            stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
            stmt->expr = makediadicnode(expr1, expr2, EASS);
        }
    } else {
        for (base = tclass->bases; base; base = base->next) {
            if (base->is_virtual) {
                expr = CABI_0056bb30(tclass, vbase->base, vbase->offset, 0, 0, 0);
                if (expr != NULL) {
                    stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                    stmt->expr = expr;
                }
            } else {
                for (vbase = tclass->vbases; vbase; vbase = vbase->next) {
                    if (vbase->base == base->base) {
                        break;
                    }
                }
                #line 3693
                CError_ASSERT(vbase != NULL);
                expr = CABI_0056bb30(tclass, vbase->base, vbase->offset, 0, 0, 1);
                if (expr != NULL) {
                    stmt = CFunc_InsertStatement(ST_EXPRESSION, stmt);
                    stmt->expr = expr;
                }
            }
        }

        stmt = CABI_CopyConAssignCB(stmt, tclass, NULL, 0);
    }

    stmt = CFunc_InsertStatement(ST_RETURN, stmt);
    stmt->expr = CABI_MakeThisExpr(NULL, 0);

    CFunc_CodeCleanup(&firstStmt);
    CFunc_Gen(&firstStmt, func);

    CScope_RestoreScope(&savedScope);

    CTemplTool_PopInstance(stack);

    copts.filesyminfo = saveDebugInfo;
}

static void CABI_569720(Object *object, Object *object2, Statement *stmt, Statement *stmt2, Boolean p5);

void CABI_FinishDestructor(Object *object, Statement *stmt, Statement *stmt2, Boolean error_check) {
    CABI_569720(object, object, stmt, stmt2, 0);
    if (error_check) {
        CFunc_ErrorCheck(object, stmt);
    }
    CFunc_DestructorCleanup(stmt);
    CFunc_CodeCleanup(stmt);
    CFunc_Gen(stmt, object);
}

void CABI_MakeDefaultDestructor(TypeClass *tclass, Object *func) {
    Boolean saveDebugInfo;
    CScopeSave savedScope;
    Statement firstStmt;
    Statement returnStmt;
    TemplStack *stack;

    if (anyerrors || func->access == ACCESSNONE) {
        return;
    }

    CABI_ApplyClassFlags(func, tclass->eflags);

    stack = CTemplTool_PushInstance(NULL, func);

    CScope_SetFunctionScope(func, &savedScope);

    CFunc_FuncGenSetup(NULL, &firstStmt, func, 0);

    saveDebugInfo = copts.filesyminfo;
    copts.filesyminfo = 0;

    CFunc_SetupNewFuncArgs(NULL, func, TYPE_FUNC(func->type)->args);

    firstStmt.next = &returnStmt;

    memclrw(&returnStmt, sizeof(Statement));
    returnStmt.type = ST_RETURN;

    CABI_TransDestructor(func, func, &firstStmt, tclass, CABIDestroy0);
    CFunc_CodeCleanup(&firstStmt);
    CFunc_Gen(&firstStmt, func);

    CScope_RestoreScope(&savedScope);

    CTemplTool_PopInstance(stack);

    copts.filesyminfo = saveDebugInfo;
}

Object *CABI_GetDestructorObject(Object *obj, CABIDestroyMode mode) {
    return obj;
}

ENode *CABI_DestroyObject(Object *dtor, ENode *objexpr, CABIDestroyMode mode, Boolean flag1, Boolean flag2) {
    ENode *expr;
    short arg;
    ENodeList *list;

    switch (mode) {
        case CABIDestroy2:
        case CABIDestroy3:
            if (flag2)
                arg = 1;
            else
                arg = -1;
            break;
        case CABIDestroy1:
            arg = -1;
            break;
        case CABIDestroy0:
            arg = 0;
            break;
        default:
            #line 4708
            CError_FATAL();
    }

    expr = CExpr_NewENode(EFUNCCALL);
    expr->cost = 200;
    expr->rtype = &stvoid;
    expr->data.funccall.funcref = CExpr_New_EOBJREF_Node(dtor, 1);
    if (flag1) {
        expr->data.funccall.funcref->flags |= ENODE_FLAG_8;
    }
    expr->data.funccall.functype = TYPE_FUNC(dtor->type);

    dtor->flags |= OBJECT_USED;

    list = lalloc(sizeof(ENodeList));
    list->node = objexpr;
    expr->data.funccall.args = list;

    list->next = lalloc(sizeof(ENodeList));
    list = list->next;
    list->next = NULL;
    list->node = intconstnode(TYPE(&stsignedshort), arg);

    return expr;
}
