#ifndef COMPILER_CERROR_H
#define COMPILER_CERROR_H

#define CError_FATAL(errorCode) CError_Internal(__FILE__, __LINE__)

void CError_Internal(const char *fileName, int errorCode);

#endif // COMPILER_CERROR_H
