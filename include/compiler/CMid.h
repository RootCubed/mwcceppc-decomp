#ifndef COMPILER_CMID_H
#define COMPILER_CMID_H

#include <compiler/common.h>

#ifdef __MWERKS__
#pragma options align=mac68k
#endif

extern void CMid_RegisterDummyCtorFunction(Object *func, Object *obj);

#ifdef __MWERKS__
#pragma options align=reset
#endif

#endif
