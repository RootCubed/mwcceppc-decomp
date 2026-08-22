#include <assert.h>
#include <common.h>

#include <OS.h>

#include <Windows.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static char *MW_CYGDRIVE_PREFIX;
static char *MW_CYGWIN_ROOT;
static Boolean old_cygwin_softlinks;
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

int __stdcall OS_GetFileTime(const OSSpec *spec, MacTime *crtm, MacTime *chtm) {
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
        chtm->low = lastWrite.dwLowDateTime;
        chtm->high = lastWrite.dwHighDateTime;
    }
    if (crtm) {
        crtm->low = creation.dwLowDateTime;
        crtm->high = creation.dwHighDateTime;
    }

    return err;
}

int __stdcall OS_SetFileTime(const OSSpec *spec, const MacTime *crtm, const MacTime *chtm) {
    FILETIME creation;
    FILETIME lastWrite;
    FILETIME *pLastWrite;
    FILETIME *pCreation;
    HANDLE handle;
    int err;
    
    if (crtm) {
        creation.dwLowDateTime = crtm->low;
        creation.dwHighDateTime = crtm->high;
    }

    if (chtm) {
        lastWrite.dwLowDateTime = chtm->low;
        lastWrite.dwHighDateTime = chtm->high;
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
    BOOL out;
    BOOL absolute;
    BOOL in_cygdrive;
    BOOL unc_path;
    int len;

    unc_path = FALSE;
    out = TRUE;
    in_cygdrive = TRUE;
    absolute = TRUE;

    if (path[0] == '\\' && path[1] == '\\') {
        unc_path = TRUE;
    }

    if (!unc_path) {
        if (!isalpha(path[0]) || path[1] != ':' || path[2] != '\\') {
            absolute = FALSE;
        }
    }

    if (!absolute) {
        if (MW_CYGDRIVE_PREFIX != NULL) {
            if (ustrncmp(path, MW_CYGDRIVE_PREFIX, strlen(MW_CYGDRIVE_PREFIX) == 0) == 0) {
                in_cygdrive = FALSE;
            }
        }
    }

    if (!in_cygdrive && (MW_CYGWIN_ROOT == NULL || path[0] != '/' || path[1] == '/')) {
        out = FALSE;
    }

    return out;
}

BOOL __stdcall OS_EqualPath(const char *a, const char *b) {
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
        return ERROR_ACCESS_DENIED;
    }

    return ERROR_SUCCESS;
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
    if (!isfile) {
        return ERROR_DIRECTORY;
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
        return ERROR_ACCESS_DENIED;
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
        size = MAX_PATH;
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

char* __stdcall OS_PathSpecToString(const OSPathSpec *pspec, char *path, int size) {
    int len;
    
    if (size == 0) {
        size = MAX_PATH;
    }

    if (path == NULL) {
        path = xmalloc_or_null(size);
        if (path == NULL) {
            return NULL;
        }
    }

    len = strlen(pspec->s);
    if (len >= size) {
        len = size - 1;
    }

    memcpy(path, pspec->s, len);
    path[len] = '\0';
    return path;
}

char* __stdcall OS_NameSpecToString(const OSNameSpec *nspec, char *name, int size) {
    int len;
    
    if (size == 0) {
        size = 0x100;
    }

    if (name == NULL) {
        name = xmalloc_or_null(size);
        if (name == NULL) {
            return NULL;
        }
    }

    len = strlen(nspec->s);
    if (len >= size) {
        len = size - 1;
    }

    memcpy(name, nspec->s, len);
    name[len] = '\0';
    return name;
}

BOOL __stdcall OS_EqualSpec(const OSSpec *a, const OSSpec *b) {
    return OS_EqualPathSpec(&a->path, &b->path) && OS_EqualNameSpec(&a->name, &b->name);
}

BOOL __stdcall OS_EqualPathSpec(const OSPathSpec *a, const OSPathSpec *b) {
    return OS_EqualPath(a->s, b->s);
}

BOOL __stdcall OS_EqualNameSpec(const OSNameSpec *a, const OSNameSpec *b) {
    return OS_EqualPath(a->s, b->s);
}

BOOL __stdcall OS_IsDir(const OSSpec *spec) {
    int len;
    int attrs;

    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    len = strlen(spec_buffer) - 1;
    if (spec_buffer[len] == '\\') {
        spec_buffer[len] = '\0';
    }
    attrs = GetFileAttributesA(spec_buffer);
    if (attrs != -1) {
        return (attrs & 0x10) != 0;
    }

    return FALSE;
}

BOOL __stdcall OS_IsFile(const OSSpec *spec) {
    int len;
    int attrs;

    if (OS_SpecToString(spec,spec_buffer,MAX_PATH) == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }

    len = strlen(spec_buffer) - 1;
    if (spec_buffer[len] == '\\') {
        spec_buffer[len] = '\0';
    }
    attrs = GetFileAttributesA(spec_buffer);
    if (attrs != -1) {
        return (attrs & 0x10) == 0;
    }

    return FALSE;
}

// Not sure where to put this
extern BOOL __cdecl LookupShortcut(char* in, char* out);

BOOL ReadShortcut(char* link, char* target) {
    char* end;
    int len;
    char path[MAX_PATH];

    end = link + strlen(link);

    if (end > link + 4) {
        if (ustrcmp(end - 4, ".lnk") == 0) {
            return LookupShortcut(link, target);
        }
    }

    len = end - link;

    if ((len + 4) >= MAX_PATH) {
        return FALSE;
    }

    memcpy(path, link, len);
    strcpy(path + len, ".lnk");
    return LookupShortcut(path, target);
}

BOOL ReadShortcutSpec(const OSSpec* spec, char* param_2) {
    char path[MAX_PATH];

    OS_SpecToString(spec, path, MAX_PATH);
    return ReadShortcut(path, param_2);
}

BOOL ReadCygwinSoftlink(char* path, char* param_2) {
    char buf[12];
    FILE* file;
    int read;

    file = fopen(path, "r");
    if (file == NULL) {
        return FALSE;
    }

    if (fread(buf, 1, 10, file) == 10) {
        if (memcmp(buf, "!<symlink>", 10) == 0) {
            read = fread(param_2, 1, MAX_PATH - 1, file);
            if (read > 0 && file->state.eof != 0) {
                param_2[read] = 0;
                fclose(file);
                return TRUE;
            }
        }
    }

    fclose(file);
    return FALSE;
}

BOOL ReadCygwinSoftlinkSpec(const OSSpec* spec, char* param_2) {
    HANDLE handle = NULL;
    UInt32 len = 10;
    char buf[12];

    if (OS_Open(spec, 0, &handle) == 0) {
        if (OS_Read(handle, buf, &len) == 0 && len == 10) {
            if (memcmp(buf, "!<symlink>", 10) == 0) {
                if (OS_GetSize(handle, &len) == 0) {
                    len -= 10;

                    if (len < MAX_PATH - 1) {
                        if (OS_Read(handle, param_2, &len) == 0) {
                            param_2[len] = 0;
                            OS_Close(handle);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    OS_Close(handle);
    return FALSE;
}

BOOL __stdcall OS_IsLink(const OSSpec *spec) {
    char* dot;
    char buf[260];
    BOOL out;

    dot = strrchr(spec->name.s, '.');
    if (dot != NULL) {
        if (ustrcmp(dot, ".lnk") == 0) {
            return ReadShortcutSpec(spec, buf);
        }
    }

    if (old_cygwin_softlinks) {
        return ReadCygwinSoftlinkSpec(spec, buf);
    }

    return FALSE;
}

int __stdcall OS_ResolveLink(const OSSpec *link, OSSpec *target) {
    char* dot;
    char path[MAX_PATH];
    
    dot = strrchr(link->name.s, '.');
    if (dot != NULL) {
        if (ustrcmp(dot, ".lnk") == 0) {
            if (ReadShortcutSpec(link, path)) {
                return OS_MakeSpec(path, target, NULL);
            }
        }
    }

    if (!old_cygwin_softlinks) {
        memcpy(target->path.s, link->path.s, 0x204);
    } else {
        if (ReadCygwinSoftlinkSpec(link, path)) {
            return OS_MakeSpec(path, target, NULL);
        }
    }

    return 2;
}

int __stdcall OS_OpenDir(const OSPathSpec *spec, OSOpenedDir *ref) {
    OSPathSpec spec_buf;

    ref->data = xmalloc_or_null(sizeof(WIN32_FIND_DATAA));
    if (ref->data == NULL) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    memcpy(ref->spec.s, spec->s, MAX_PATH);

    strcpy(spec_buf.s, spec->s);
    strcat(spec_buf.s, "*");
    ref->dir = FindFirstFile(spec_buf.s, ref->data);
    if (ref->dir != INVALID_HANDLE_VALUE) {
        return 0;
    } else {
        return GetLastError();
    }
}

int __stdcall OS_ReadDir(OSOpenedDir *ref, OSSpec *spec, char *filename, Boolean *isfile) {
    LPWIN32_FIND_DATAA data = ref->data;
    OSPathSpec spec_buffer;
    int filename_len;
    int spec_len;
    char* question_mark;
    int i;
    int old_attrs;

    if (isfile != NULL) {
        *isfile = FALSE;
    }

    do {
        do {
            do {
                if (ref->dir == INVALID_HANDLE_VALUE) {
                    return ERROR_FILE_NOT_FOUND;
                }
                filename_len = strlen(data->cFileName);

                if (filename_len + 1 < 0x100 && strchr(data->cFileName, '?') == NULL) {
                    memcpy(spec_buffer.s, data->cFileName, filename_len);
                } else {
                    filename_len = strlen(data->cAlternateFileName);
                    if (filename_len > 0x100) {
                        filename_len = 0xff;
                    }

                    for (i = 0; i < filename_len; i++) {
                        spec_buffer.s[i] = tolower(data->cAlternateFileName[i]);
                    }
                }

                spec_buffer.s[filename_len] = 0;

                old_attrs = data->dwFileAttributes;
                if (!FindNextFile(ref->dir, data)) {
                    OS_CloseDir(ref);
                }
            } while(memcmp(spec_buffer.s, ".", 2) == 0);
        } while(memcmp(spec_buffer.s, "..", 3) == 0);

        old_attrs &= 0x10;
    } while(MAX_PATH <= strlen(ref->spec.s) + filename_len + (int)(old_attrs != 0));

    if (old_attrs != 0) {
        spec_len = strlen(ref->spec.s);
        memcpy(spec->path.s, ref->spec.s, spec_len);
        memcpy(spec->path.s + spec_len, spec_buffer.s, filename_len);
        spec->path.s[spec_len + filename_len] = '\\';
        spec->path.s[spec_len + filename_len + 1] = '\0';
        spec->name.s[0] = '\0';
        *isfile |= 2;
    } else {
        strcpy(spec->path.s, ref->spec.s);
        memcpy(spec->name.s, spec_buffer.s, filename_len + 1);
        *isfile |= 1;
    }

    memcpy(filename,spec_buffer.s,filename_len + 1);
    return 0;
}

int __stdcall OS_CloseDir(OSOpenedDir *ref) {
    if (ref->dir != INVALID_HANDLE_VALUE) {
        if (!FindClose(ref->dir)) {
            return GetLastError();
        }

        if (ref->data != NULL) {
            xfree(ref->data);
        }

        ref->data = NULL;
        ref->dir = INVALID_HANDLE_VALUE;
    }

    return 0;
}

UInt32 __stdcall OS_GetMilliseconds() {
    return GetTickCount();
}

void __stdcall OS_GetTime(MacTime *p) {
    long high;
    long low;
    FILETIME file_time;
    SYSTEMTIME system_time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    
    low = file_time.dwLowDateTime;
    high = file_time.dwHighDateTime;

    p->low = low;
    p->high = high;
}

int __stdcall OS_NewHandle(UInt32 size, OSHandle *hand) {
    hand->addr = GlobalAlloc(0x40, size);
    hand->used = size;
    if (hand->addr != NULL) {
        return 0;
    } else {
        return GetLastError();
    }
}

int __stdcall OS_ResizeHandle(OSHandle *hand, UInt32 size) {
    void* addr = GlobalReAlloc(hand->addr, size, 0x42);

    if (addr != NULL) {
        hand->addr = addr;
        hand->used = size;
        return 0;
    } else {
        hand->addr = NULL;
        hand->used = 0;
        return GetLastError();
    }
}

void* __stdcall OS_LockHandle(OSHandle *hand) {
    if (GlobalFlags(hand->addr) != GMEM_INVALID_HANDLE) {
        return hand->addr;
    }

    return NULL;
}

void __stdcall OS_UnlockHandle(OSHandle *hand) {}

int __stdcall OS_FreeHandle(OSHandle *hand) {
    if (GlobalFree(hand->addr) != NULL) {
        return GetLastError();
    }
    hand->addr = NULL;
    hand->used = 0;

    return 0;
}

int __stdcall OS_GetHandleSize(OSHandle *hand, UInt32 *size) {
    if (GlobalFlags(hand->addr) != GMEM_INVALID_HANDLE) {
        *size = hand->used;
        return 0;
    }

    *size = 0;
    return ERROR_NOT_ENOUGH_MEMORY;
}

void __stdcall OS_InvalidateHandle(OSHandle *hand) {
    hand->addr = NULL;
    hand->used = 0;
}

Boolean __stdcall OS_ValidHandle(OSHandle *hand) {
    return hand != NULL && hand->addr != NULL;
}

// Non-matching
int __stdcall OS_OSErrorToMacError(int err) {
    /*
    // Unfinished
    switch (err) {
        case ERROR_SUCCESS:             return noErr;
        case ERROR_FILE_NOT_FOUND:      return fnfErr;
        case ERROR_PATH_NOT_FOUND:      return dirNFErr;
        case ERROR_TOO_MANY_OPEN_FILES: return tmfoErr;
        case ERROR_ACCESS_DENIED:       return permErr;
        case ERROR_NOT_ENOUGH_MEMORY:   return memFullErr;
        case ERROR_INVALID_DATA:        return paramErr;
        case ERROR_OUTOFMEMORY:         return memFullErr;
        case ERROR_INVALID_DRIVE:       return nsvErr;
        case ERROR_CURRENT_DIRECTORY:   return paramErr;
        case ERROR_WRITE_PROTECT:       return wrPermErr;
        case ERROR_NOT_READY:
        case ERROR_CRC:                 return ioErr;
        case ERROR_BAD_LENGTH:          return paramErr;
        case ERROR_SEEK:                return ioErr;
    }
    */

    return 0;
}

void __stdcall OS_TimeToMac(MacTime sectm, UInt32 *secs) {
    static int days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    FILETIME filetime;
    SYSTEMTIME systemtime;
    int years;
    int days;
    int days_simple;
    int leap_years_simple;
    int skip_centuries;
    int leap_centuries;
    int leap_years;
    short month;

    filetime.dwLowDateTime = sectm.low;
    filetime.dwHighDateTime = sectm.high;
    FileTimeToSystemTime(&filetime, &systemtime);

    years = systemtime.wYear;
    leap_years_simple = (years - 1901) / 4;
    skip_centuries = (years - 1900) / 100;
    leap_centuries = (years - 1601) / 400;
    leap_years = leap_years_simple - skip_centuries + leap_centuries;
    days = days_simple + leap_years;

    if (((systemtime.wYear & 3) == 0) && (years != years / 100 * 100) || (years == years / 400 * 400)) {
        days_in_month[1] = 29;
    } else {
        days_in_month[1] = 28;
    }

    if (systemtime.wMonth > 12 || systemtime.wMonth == 0) {
        systemtime.wMonth = 1;
        systemtime.wDayOfWeek = 0;
    }

    month = systemtime.wMonth - 1;
    for (month = systemtime.wMonth - 1; month != 0; month--) {
        days += days_in_month[(short)month - 1];
    }
    *secs = systemtime.wHour * 60 * 60 + (days + systemtime.wDay - 1) * 24 * 60 * 60 + systemtime.wMinute * 60 + systemtime.wSecond;
}

// Non-matching
void __stdcall OS_MacToTime(UInt32 secs, MacTime *sectm) {}

SInt16 __stdcall OS_RefToMac(HANDLE ref) {
    if (ref == INVALID_HANDLE_VALUE || (long)ref < 0) {
        return 0;
    }

#line 2027
    assert((long)ref < 0xffff);
    return (SInt16)(long)ref + 1;
}

int __stdcall OS_MacToRef(SInt16 refnum) {
    if (refnum == 0) {
        return -1;
    } else {
        return (int)refnum - 1;
    }
}

int __stdcall OS_CloseLibrary(void *a) {
    if (FreeLibrary(a)) {
        return 0;
    } else {
        return GetLastError();
    }
}

int __stdcall OS_LoadMacResourceFork(const OSSpec *spec, void **file_data, SInt32 *file_len) {
    HANDLE handle;
    HANDLE rsrcInfo;
    HANDLE rsrc;
    void* ptr;

    if (OS_SpecToString(spec, spec_buffer, 0x104) == NULL) {
        return ERROR_BUFFER_OVERFLOW;
    }

    handle = GetModuleHandle(spec_buffer);
    if (handle == NULL) {
        return GetLastError();
    }

    rsrcInfo = FindResource(handle, "#101", "MACRSRC");
    if (rsrcInfo == NULL) {
        return GetLastError();
    }

    rsrc = LoadResource(handle, rsrcInfo);
    if (rsrc == NULL) {
        return GetLastError();
    }

    ptr = LockResource(rsrc);
    if (ptr == NULL) {
        return GetLastError();
    }

    *file_data = ptr;
    *file_len = SizeofResource(handle, rsrcInfo);
    return 0;
}

int __stdcall OS_CreateMutex(OSMutex *mutex) {
    void* ptr;
    
    mutex->lock = malloc(sizeof(CRITICAL_SECTION));

    InitializeCriticalSection(mutex->lock);

    return mutex->lock == NULL ? GetLastError() : 0;
}

int __stdcall OS_MapFile(HANDLE *ref, void **mapping, HANDLE file, DWORD size, Boolean readonly, Boolean executable) {
    int access;
    int protect;
  
    if (readonly) {
        protect = PAGE_READONLY;
        access = FILE_MAP_READ;
    } else {
        protect = PAGE_READWRITE;
        access = FILE_MAP_READ | FILE_MAP_WRITE;
    }

    if (executable) {
        protect = protect | PAGE_EXECUTE;
    }

    *ref = CreateFileMapping(file, NULL, protect, 0, size, NULL);
    if (*ref == NULL) {
        return GetLastError();
    }

    *mapping = MapViewOfFile(*ref, access, 0, 0, size);
    if (*mapping == NULL) {
        return GetLastError();
    }

    return 0;
}

int __stdcall OS_UnMapFile(HANDLE handle, void* mapping) {
    if (!UnmapViewOfFile(mapping)) {
        return GetLastError();
    }

    if (!CloseHandle(handle)) {
        return GetLastError();
    }

    return 0;
}
