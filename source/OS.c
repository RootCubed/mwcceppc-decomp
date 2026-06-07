#include <common.h>

static char *MW_CYGDRIVE_PREFIX;
static char *MW_CYGWIN_ROOT;
static UInt8 old_cygwin_softlinks;
static Boolean COMSTA_init;

__declspec(dllimport) int __stdcall CoInitialize(void *);

int __stdcall OS_InitProgram(int *pArgc, char ***pArgv) {
    int i;

    MW_CYGDRIVE_PREFIX = getenv("MW_CYGDRIVE_PREFIX");
    MW_CYGWIN_ROOT = getenv("MW_CYGWIN_ROOT");
    old_cygwin_softlinks = 0;

    if (pArgc && *pArgc > 1) {
        if (strcmp((*pArgv)[1], "--old-cygwin-softlinks") == 0) {
            old_cygwin_softlinks = 1;
            for (i = 2; i <= *pArgc; i++) {
                (*pArgv)[i - 1] = (*pArgv)[i];
            }
            (*pArgc)--;
        }
    }

    COMSTA_init = CoInitialize(NULL) >= 0;

    return 0;
}
