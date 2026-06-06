#include <common.h>
#include <compiler/CABI.h>
#include <compiler/CompilerTools.h>
#include <compiler/CClass.h>
#include <compiler/CError.h>
#include <compiler/CExpr.h>
#include <compiler/CInit.h>
#include <compiler/CMachine.h>
#include <compiler/CMangler.h>
#include <compiler/CScope.h>
#include <compiler/CParser.h>
#include <compiler/objects.h>
#include <compiler/scopes.h>
#include <compiler/types.h>

void CABI_AddVTable(TypeClass *tclass) {
    tclass->vtable = galloc(sizeof(VTable));
    memclrw(tclass->vtable, sizeof(VTable));
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

SInt32 CABI_GetVTableOffset(TypeClass *tclass) {
    return 0;
}

SInt32 CABI_ComputeNewArrayPadding(void) {
    return 0x10;
}

ENode *CABI_ReleaseGuardVariable(ENode *expr) {
    ENode *ref = checkreference(CExpr_New_EINDIRECT_Node(expr));
    return makediadicnode(ref, intconstnode(TYPE(&stsignedchar), 1), EASS);
}

ENode *CABI_AcquireGuardVariable(ENode *expr) {
    return checkreference(CExpr_New_EINDIRECT_Node(expr));
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

void CABI_ReverseBitField(TypeBitfield *tbitfield) {
    UInt32 bits;
    UInt8 a;
    UInt8 b;

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

    b = tbitfield->bitlength;
    a = tbitfield->offset;
    tbitfield->offset = (bits - a) - b;
}
