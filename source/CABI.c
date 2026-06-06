#include <common.h>
#include <compiler/CABI.h>
#include <compiler/CompilerTools.h>
#include <compiler/CClass.h>
#include <compiler/CError.h>
#include <compiler/CExpr.h>
#include <compiler/CFunc.h>
#include <compiler/CInit.h>
#include <compiler/CMachine.h>
#include <compiler/CMangler.h>
#include <compiler/CScope.h>
#include <compiler/CParser.h>
#include <compiler/objects.h>
#include <compiler/scopes.h>
#include <compiler/types.h>

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
