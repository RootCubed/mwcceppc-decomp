#include "winerror.h"
#include <common.h>

#include <OS/OS.h>
#include <OS/MemUtils.h>
#include <OS/win32.h>
#include <OS/StringExtras.h>

#include <Windows.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static char *MW_CYGDRIVE_PREFIX;
static char *MW_CYGWIN_ROOT;
static UInt8 old_cygwin_softlinks;
static Boolean COMSTA_init;
static char spec_buffer[MAX_PATH];
static char file_buffer[0x20];
static char error_buffer[0x100];

const char* __stdcall OS_GetErrText(int err) {
    char* cr;

    if (err == 0xfafafafa) {
        strcpy(error_buffer, "Unknown process spawning error");
    } else {
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, error_buffer, 0x100, NULL);
    }

    cr = &error_buffer[strlen(error_buffer) - 2];

    if (((error_buffer < cr) && (*cr == '\r')) && (*(cr + 1) == '\n')) {
        *cr = '\0';
    }
    
    return error_buffer;
}

int __stdcall OS_InitProgram(int *pArgc, char ***pArgv) {
    int i;

    MW_CYGDRIVE_PREFIX = getenv("MW_CYGDRIVE_PREFIX");
    MW_CYGWIN_ROOT = getenv("MW_CYGWIN_ROOT");
    old_cygwin_softlinks = FALSE;

    if (pArgc && *pArgc > 1) {
        if (strcmp((*pArgv)[1], "--old-cygwin-softlinks") == 0) {
            old_cygwin_softlinks = TRUE;
            for (i = 2; i <= *pArgc; i++) {
                (*pArgv)[i - 1] = (*pArgv)[i];
            }
            (*pArgc)--;
        }
    }

    COMSTA_init = CoInitialize(NULL) >= 0;

    return 0;
}

int __stdcall OS_Create(const OSSpec *spec, const uOSTypePair *type) {
    HANDLE handle;
    int err;

    if (OS_SpecToString(spec, spec_buffer, MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    handle = CreateFileA(spec_buffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
        return OS_SetFileType(spec, type);
    } else {
        return GetLastError();
    }
}

int __stdcall OS_Status(const OSSpec* spec) {
    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }
    if (GetFileAttributesA(spec_buffer) != -1) {
        return 0;
    }
    return GetLastError();
}

int __stdcall OS_SetFileType(const OSSpec *spec, const uOSTypePair *type) {
    return 0;
}

int __stdcall OS_GetFileTime(const OSSpec *spec, time_t *crtm, time_t *chtm) {
    HANDLE handle;
    int err;
    FILETIME creation;
    FILETIME lastWrite;

    err = OS_Open(spec, OSReadOnly, &handle);
    if (err != 0) {
        return err;
    }

    if (!GetFileTime(handle, &creation, NULL, &lastWrite)) {
        err = GetLastError();
    } else {
        err = 0;
    }

    OS_Close(handle);

    if (chtm) {
        chtm[0] = lastWrite.dwLowDateTime;
        chtm[1] = lastWrite.dwHighDateTime;
    }
    if (crtm) {
        crtm[0] = creation.dwLowDateTime;
        crtm[1] = creation.dwHighDateTime;
    }

    return err;
}

int __stdcall OS_SetFileTime(const OSSpec *spec, const time_t *crtm, const time_t *chtm) {
    FILETIME creation;
    FILETIME lastWrite;
    FILETIME *pLastWrite;
    FILETIME *pCreation;
    HANDLE handle;
    int err;
    
    if (crtm) {
        creation.dwLowDateTime = crtm[0];
        creation.dwHighDateTime = crtm[1];
    }

    if (chtm) {
        lastWrite.dwLowDateTime = chtm[0];
        lastWrite.dwHighDateTime = chtm[1];
    }

    err = OS_Open(spec, OSWrite, &handle);
    
    if (err != 0) {
        return err;
    }

    if (chtm) {
        pLastWrite = &lastWrite;
    } else {
        pLastWrite = NULL;
    }
    if (crtm) {
        pCreation = &creation;
    } else {
        pCreation = NULL;
    }

    if (!SetFileTime(handle,pCreation,NULL,pLastWrite)) {
        err = GetLastError();
    } else {
        err = 0;
    }

    OS_Close(handle);

    return err;
}

int __stdcall OS_Open(const OSSpec *spec, OSOpenMode mode, HANDLE *ref) {
    static long OSOpenMode_to_DesiredAccess[4] = {
        GENERIC_READ, // OSReadOnly
        GENERIC_WRITE, // OSWrite
        GENERIC_READ | GENERIC_WRITE, // OSReadWrite
        GENERIC_WRITE // OSAppend
    };

    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == NULL) {
        return ERROR_BUFFER_OVERFLOW;
    }
    *ref = CreateFileA(spec_buffer,OSOpenMode_to_DesiredAccess[mode],FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (*ref == INVALID_HANDLE_VALUE) {
        return GetLastError();
    } else {
        if (mode == OSAppend) {
            if (SetFilePointer(*ref,0,NULL,FILE_END) == -1) {
                return GetLastError();
            }
        }
        return 0;
    }
}

int __stdcall OS_Write(HANDLE ref, const void *buffer, UInt32 *length) {
    UInt32 fileSize;
    UInt32 bytesToWrite;
    UInt32 pos;
    UInt32 bytesWritten;

    pos = SetFilePointer(ref, 0, NULL, FILE_CURRENT);

    if (pos == -1 || (fileSize = GetFileSize(ref,NULL), fileSize == -1)) {
        return GetLastError();
    }

    if (pos > fileSize) {
        if (SetFilePointer(ref, fileSize, NULL, FILE_BEGIN) == -1) {
            return GetLastError();
        }

        while (pos > fileSize) {
            if (pos - fileSize > 0x20) {
                bytesToWrite = 0x20;
            } else {
                bytesToWrite = pos - fileSize;
            }

            if (!WriteFile(ref, file_buffer, bytesToWrite, &bytesWritten, NULL)) {
                return GetLastError();
            }
            if (bytesWritten < bytesToWrite) {
                *length = 0;
                return 0;
            }

            fileSize += bytesToWrite;
        }
    }

    if (!WriteFile(ref, buffer, *length, length, NULL)) {
        return GetLastError();
    }
    return 0;
}

int __stdcall OS_Read(HANDLE ref, void *buffer, UInt32 *length) {
    if (!ReadFile(ref, buffer, *length, length, NULL)) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_Seek(HANDLE ref, OSSeekMode how, SInt32 offset) {
    static int OSSeekMode_to_MoveMethod[3] = {
        FILE_CURRENT,
        FILE_BEGIN,
        FILE_END,
    };

    if (SetFilePointer(ref, offset, NULL, OSSeekMode_to_MoveMethod[how]) == -1) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_Tell(HANDLE ref, SInt32 *offset) {
    *offset = SetFilePointer(ref, 0, NULL, FILE_CURRENT);
    if (*offset == -1) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_Close(HANDLE ref) {
    if (!CloseHandle(ref)) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_GetSize(HANDLE ref, UInt32 *length) {
    *length = GetFileSize(ref,NULL);
    if (*length == -1) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_SetSize(HANDLE ref, UInt32 size) {
    DWORD pos;
  
    pos = SetFilePointer(ref,0,NULL,FILE_CURRENT);
    if (pos != 0xffffffff) {
        if (SetFilePointer(ref,size,NULL,FILE_BEGIN) != 0xffffffff) {
            if (SetEndOfFile(ref)) {
                if (SetFilePointer(ref,pos,NULL,FILE_BEGIN) != 0xffffffff) {
                    return 0;
                }
            }
        }
    }
    return GetLastError();
}

int __stdcall OS_Delete(const OSSpec *spec) {
    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == NULL) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (!DeleteFileA(spec_buffer)) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_Rename(const OSSpec *oldspec, const OSSpec *newspec) {
    char spec_buffer2[MAX_PATH];

    if (OS_SpecToString(oldspec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }
    if (OS_SpecToString(newspec,spec_buffer2,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (MoveFileA(spec_buffer,spec_buffer2) == 0) {
        return GetLastError();
    } else {
        return 0;
    }
}

int __stdcall OS_Mkdir(const OSSpec *spec) {
    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (!CreateDirectoryA(spec_buffer, NULL)) {
        return GetLastError();
    }

    return 0;
}

int __stdcall OS_Rmdir(const OSPathSpec *spec) {
    if (OS_PathSpecToString(spec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (!RemoveDirectoryA(spec_buffer)) {
        return GetLastError();
    }

    return 0;
}

void EnsureTrailingBackslash(char* path) {
    char* end;
    int len;

    path += strlen(path);
    if (path[-1] != '\\') {
        path[0] = '\\';
        path[1] = '\0';
    }
}

int __stdcall OS_GetCWD(OSPathSpec *spec) {
    if (!GetCurrentDirectoryA(MAX_PATH, spec->s)) {
        return GetLastError();
    }

    EnsureTrailingBackslash(spec->s);
    return 0;
}

int RedirectStream(HANDLE* outHandle, unsigned int stdHandle, const char* filename) {
    HANDLE oldHandle;
    HANDLE newHandle;
    SECURITY_ATTRIBUTES security;

    security.nLength = 0xc;
    security.bInheritHandle = 1;
    security.lpSecurityDescriptor = NULL;

    oldHandle = GetStdHandle(stdHandle);

    newHandle = CreateFileA(filename,GENERIC_WRITE,FILE_SHARE_READ,&security,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (newHandle == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    if (!SetStdHandle(stdHandle, newHandle)) {
        return GetLastError();
    }

    *outHandle = oldHandle;
    return 0;
}

// Non-matching
int __stdcall OS_Execute(OSSpec *spec, char **argv, char **envp, const char *stdoutfile, const char *stderrfile, int *exitcode) {
    int sum;
    int i;
    int j;
    char* cli;
    int err;
    char** arg;
    char** arg2;
    HANDLE stdout_handle;
    HANDLE stderr_handle;
    STARTUPINFOA startup;
    BOOL process_created;
    PROCESS_INFORMATION process_information;

    sum = 0;
    for (arg = argv; *arg != NULL; arg++) {
        sum += strlen(*arg) + 3;
    }

    cli = xmalloc_or_null(sum);
    if (cli == NULL) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    i = 0;
    arg2 = argv;
    while (TRUE) {
        if (*arg2 != NULL) {
            if (**arg2 == '\0') {
                label:

                cli[i++] = '"';
                strcpy(cli + i,*arg2);
                i += strlen(*arg2);
                cli[i++] = '"';
            } else {
                if (strchr(*arg2, ' ') != NULL) goto label;

                if (strchr(*arg2,'"') == NULL) {
                    strcpy(cli + i,*arg2);
                    i += strlen(*arg2);
                } else {
                    while (**arg2 != '\0') {
                        if (**arg2 != '\"') {
                            cli[i] = **arg2;
                        }
                        else {
                            cli[i] = '\\';
                            i++;
                            cli[i] = '\"';
                        }
                        i++;
                    }
                }
            }

            cli[i] = ' ';
        } else {
            cli[i] = '\0';

            if (stdoutfile != NULL) {
                err = RedirectStream(&stdout_handle,STD_OUTPUT_HANDLE,stdoutfile);
                if (err != 0) {
                    return err;
                }
            }
            if (stderrfile != NULL) {
                err = RedirectStream(&stderr_handle,STD_ERROR_HANDLE,stderrfile);
                if (err != 0) {
                    if (stdoutfile != NULL) {
                        SetStdHandle(STD_OUTPUT_HANDLE,stdout_handle);
                    }
                    return err;
                }
            }

            memset(&startup, 0, sizeof(STARTUPINFOA));
            startup.cb = 0x44;
            startup.lpTitle = "Linking";
            if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == NULL) {
                return ERROR_BUFFER_OVERFLOW;
            }

            process_created = CreateProcessA(spec_buffer, cli, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &process_information);
            
            if (stdoutfile != NULL) {
                SetStdHandle(STD_OUTPUT_HANDLE,stdout_handle);
            }
            if (stderrfile != NULL) {
                SetStdHandle(STD_ERROR_HANDLE,stderr_handle);
            }
            if (process_created != 0) {
                xfree(cli);
                WaitForSingleObject(process_information.hProcess,-1);
                process_created = GetExitCodeProcess(process_information.hProcess,(unsigned long*)exitcode);
                if (process_created == 0) {
                    return GetLastError();
                }
                if (*exitcode == 0x103) {
                    fprintf(stderr,"??? OS_Exec: process still active ???\n");
                    return 0xfafafafa;
                }
                return 0;
            }
            return GetLastError();
        }

        arg2++;
    }
}

char* __stdcall OS_GetDirPtr(char *path) {
    char* ptr;

    if (path[0] == '\\' && path[1] == '\\') {
        ptr = strchr(path + 2, '\\');

        if (ptr == NULL) {
            ptr = path + strlen(path);
        }

        return ptr;
    } else {
        if (isalpha(path[0]) != 0 && path[1] == ':') {
            return path + 2;
        }
        return path;
    }
}

// Non-matching
int __stdcall OS_CanonPath(const char *src, char *dst) {
    const char* end;
    char last;
    char* final_dest;
    const char* pcVar2;
    int len;
    int final_size;

    if (MAX_PATH - 1 < strlen(src)) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (dst == NULL) {
        dst = (char*)src;
    }

    pcVar2 = src;
    final_dest = dst;
    if (MW_CYGDRIVE_PREFIX != NULL) {
        if (ustrncmp(MW_CYGDRIVE_PREFIX, src, strlen(MW_CYGDRIVE_PREFIX)) == 0) {
            end = src + strlen(MW_CYGDRIVE_PREFIX);
            if (end[0] == '/' || end[0] == '\\') {
                end++;
            }

            last = end[0];

            if (isalpha(last) != 0 && end[1] == '/' || end[1] == '\\') {
                dst[0] = toupper(last);
                dst[1] = ':';
                final_dest = dst + 3;
                dst[2] = '\\';

                pcVar2 = end + 2;
            }
        }
    }

    if (MW_CYGWIN_ROOT != NULL && pcVar2[0] == '/' && pcVar2[1] == '/') {
        len = strlen(MW_CYGWIN_ROOT);
        final_size = len + 1;
        
        if (0x103 < len + 2) {
            return ERROR_BUFFER_OVERFLOW;
        }

        if (MW_CYGWIN_ROOT[len] == '\t' || MW_CYGWIN_ROOT[len] == '\\') {
            final_size = len;
        }

        if (0x103 < (final_size - 1) + strlen(src) + 3) {
            return ERROR_BUFFER_OVERFLOW;
        }

        strncpy(final_dest, MW_CYGWIN_ROOT, final_size);

        final_dest[final_size] = '\\';
        final_dest = final_dest + final_size + 1;
        pcVar2 = pcVar2 + 1;
    }

    last = *pcVar2;
    if ((isalpha(last) != 0) && (pcVar2[1] == ':')) {
        *final_dest = toupper(last);
        final_dest[1] = ':';
        final_dest = final_dest + 2;
        pcVar2 = pcVar2 + 2;
    }
    last = *pcVar2;
    while (last != '\0') {
        if (last == '/') {
            *final_dest = '\\';
            final_dest++;
        }
        else {
            *final_dest = last;
        }
        pcVar2++;
        final_dest++;
        last = *pcVar2;
    }
    *final_dest = '\0';
    return 0;
}

// Non-matching
int __stdcall OS_IsFullPath(const char *path) {
    return 0;
}

int __stdcall OS_EqualPath(const char *a, const char *b) {
    return ustrcmp(a, b) == 0;
}

// Non-matching
int FUN_004107d0(char* param_1, char* param_2) {
    return 0;
}

int __stdcall OS_MakeSpecEx(const char *path, Boolean bool1, Boolean bool2, OSSpec *spec, Boolean *isfile) {
    int err;
    char canon_path[MAX_PATH];
    
    spec->path.s[0] = '\0';

    spec->name.s[0] = spec->path.s[0];

    if (isfile != NULL) {
        *isfile = FALSE;
    }

    err = OS_CanonPath(path, canon_path);
    if (err != 0) {
        return err;
    }

    return 0;
}

int __stdcall OS_MakeSpec(const char *path, OSSpec *spec, Boolean *isfile) {
    return OS_MakeSpecEx(path,0,0,spec,isfile);
}

int __stdcall OS_MakeFileSpecEx(const char *path, Boolean bool1, Boolean bool2, OSSpec *spec) {
    int err;
    Boolean isfile;

    err = OS_MakeSpecEx(path, bool1, bool2, spec, &isfile);
    if (err != 0) {
        return err;
    }
    
    if ((isfile & 2U) != 0) {
        return 5;
    }

    return 0;
}

int __stdcall OS_MakeFileSpec(const char *path, OSSpec *spec) {
    return OS_MakeFileSpecEx(path, FALSE, FALSE, spec);
}

int __stdcall OS_MakePathSpecEx(const char *vol, const char *dir, Boolean bool1, Boolean bool2, OSPathSpec *spec) {
    UInt32 dir_size;
    UInt32 vol_size;
    char local_spec_buf[MAX_PATH];
    char buf[324];
    char* path_sep;
    int i;
    int j;
    Boolean isfile;
    OSSpec ex_spec;

    if (dir == NULL) {
        dir_size = 0;
    } else {
        dir_size = strlen(dir);
    }

    if (vol == NULL) {
        vol_size = 0;
    } else {
        vol_size = strlen(vol);
    }

    if (0x144 < vol_size + dir_size + 2) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (vol == NULL) {
        if (dir == NULL) {
            buf[0] = '.';
            buf[1] = '\0';
        }
        else {
            strcpy(buf, dir);
        }
    } else if (vol[0] == '\0') {
        if (dir == NULL) {
            dir = "/";
        }
        strcpy(buf, dir);
    } else if (vol[1] == '\0') {
        if (dir == NULL) {
            dir = "/";
        }
        sprintf(buf, "%s:%s", vol, dir);
    } else if (dir == NULL) {
        sprintf(buf, "%s", vol);
    } else {
        if (dir[0] == '\\') {
            path_sep = "\\";
        } else {
            path_sep = "";
        }
        sprintf(buf, "%s%s%s", vol, path_sep, dir);
    }

    do {
        if (!bool1 || ++j < 0x10) {
            vol_size = OS_MakeSpecEx(buf,bool1,bool2,&ex_spec,&isfile);
        } else {
            vol_size = 3;
        }
        if (!bool1 || (vol_size != 3 && (vol_size != 0 || ((isfile & 1U) == 0)))) break;

        OS_SpecToString(&ex_spec,local_spec_buf,0x104);
        i = FUN_004107d0(local_spec_buf,buf);
    } while (i != 0);
    strcpy(spec->s,(char *)&ex_spec);
    if ((vol_size != 0) && (bool2)) {
        return 0;
    }
    if ((vol_size == 0) && (!isfile)) {
        vol_size = 3;
    }
    if (vol_size != 0) {
        return vol_size;
    }
    if ((isfile & 1U) != 0) {
        return 0x10b;
    }
    return 0;
}

int __stdcall OS_MakePathSpec(const char *vol, const char *dir, OSPathSpec *spec) {
    return OS_MakePathSpecEx(vol, dir, FALSE, FALSE, spec);
}

int __stdcall OS_MakeNameSpec(const char *name, OSNameSpec *spec) {
    int name_len;
    
    name_len = strlen(name);
    spec->s[0] = '\0';

    if (0xff < name_len) {
        return ERROR_BUFFER_OVERFLOW;
    }

    if (strchr(name, '\\') != NULL) {
        return 5;
    }

    if (strpbrk(name, "<>:\"/\\|") != NULL) {
        return ERROR_INVALID_NAME;
    }

    memcpy(spec->s, name, name_len + 1);
    return 0;
}

char* __stdcall OS_SpecToString(const OSSpec *spec, char *path, int size) {
    int path_size;
    int name_size;
    
    if (size == 0) {
        size = 0x104;
    }

    if (path == NULL) {
        path = xmalloc_or_null(size);
        if (path == NULL) {
            return 0;
        }
    }

    path_size = strlen(spec->path.s);
    name_size = strlen(spec->name.s);
    
    if (path_size + name_size >= size) {
        if (path_size >= size) {
            name_size = 0;
            path_size = size - 1;
        } else {
            name_size = size - path_size - 1;
        }
    }

    memcpy(path, spec->path.s, path_size);
    memcpy(path + path_size, spec->name.s, name_size);
    path[path_size + name_size] = '\0';
    return path;
}
