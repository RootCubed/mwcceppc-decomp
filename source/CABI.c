#include <common.h>
#include <compiler/CompilerTools.h>
#include <compiler/CClass.h>
#include <compiler/CError.h>
#include <compiler/CScope.h>
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
