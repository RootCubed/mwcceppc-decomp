#ifndef COMPILER_CERROR_H
#define COMPILER_CERROR_H

#define CError_ASSERT(cond) do { if (!(cond)) { CError_Internal(__FILE__, __LINE__); } } while (0)
#define CError_FAIL(cond) do { if (cond) { CError_Internal(__FILE__, __LINE__); } } while (0)
#define CError_FATAL() do { CError_Internal(__FILE__, __LINE__); } while (0)

void CError_Internal(const char *fileName, int errorCode);

#endif // COMPILER_CERROR_H
