#ifndef COMPILER_CERROR_H
#define COMPILER_CERROR_H

#include <compiler/common.h>

#define CError_ASSERT(cond) do { if (!(cond)) { CError_Internal(__FILE__, __LINE__); } } while (0)
#define CError_FAIL(cond) do { if (cond) { CError_Internal(__FILE__, __LINE__); } } while (0)
#define CError_FATAL() do { CError_Internal(__FILE__, __LINE__); } while (0)

enum {
    // "class has no default constructor"
    CErrorStr10203 = 10203,
    CErrorStrMAX
};

extern void CError_Internal(const char *fileName, int errorCode);
extern void CError_Error(int code, ...);
extern void CError_OverloadedFunctionError(Object *obj, ObjectList *olst);

#endif // COMPILER_CERROR_H
