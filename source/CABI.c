#include <common.h>
#include <compiler/CABI.h>
#include <compiler/CompilerTools.h>
#include <compiler/CClass.h>
#include <compiler/CDecl.h>
#include <compiler/CError.h>
#include <compiler/CExpr.h>
#include <compiler/CFunc.h>
#include <compiler/CInit.h>
#include <compiler/CMachine.h>
#include <compiler/CMangler.h>
#include <compiler/CMid.h>
#include <compiler/CScope.h>
#include <compiler/CParser.h>
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

    if (!(tclass->flags & CLASS_HANDLEOBJECT)) {
        return NULL;
    }

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
