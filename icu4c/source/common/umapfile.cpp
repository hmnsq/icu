// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/


/*----------------------------------------------------------------------------
 *
 *       Memory mapped file wrappers for use by the ICU Data Implementation
 *       All of the platform-specific implementation for mapping data files
 *         is here.  The rest of the ICU Data implementation uses only the
 *         wrapper functions.
 *
 *----------------------------------------------------------------------------*/
/* Defines _XOPEN_SOURCE for access to POSIX functions.
 * Must be before any other #includes. */
#അന്തര്ഭാവ "uposixdefs.h"

#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "udatamem.h"
#അന്തര്ഭാവ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#എങ്കിൽ MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       എങ്കിൽ !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma തള്ളുക_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_മൂല്യം
#       endif
#   endif

#   include <windows.h>

#   എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_മൂല്യം)
#       pragma എടുക്കുക_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

തരനിർവചനം HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#അല്ലെങ്കിലെങ്കിൽ MAP_IMPLEMENTATION==MAP_POSIX
    തരനിർവചനം size_t MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((ശൂന്യം*)-1)
#   endif
#അല്ലെങ്കിലെങ്കിൽ MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    തരനിർവചനം ശൂന്യം *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#endif

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#എങ്കിൽ MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ അസത്യം;
        }
        UDataMemory_init(pData); /* Clear the output struct. */
        തിരികെ അസത്യം;            /* no file access */
    }

    U_CFUNC ശൂന്യം uprv_unmapFile(UDataMemory *pData) {
        /* nothing to do */
    }
#അല്ലെങ്കിലെങ്കിൽ MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         സ്ഥിരം അക്ഷരം *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ അസത്യം;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_മൂല്യം;
        DWORD fileLength = 0;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
#എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#അല്ലെങ്കിൽ
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ അസത്യം;
        }
        എങ്കിൽ (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_BUFFER_OVERFLOW_ERROR;
            തിരികെ അസത്യം;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        എങ്കിൽ (file == INVALID_HANDLE_മൂല്യം) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            എങ്കിൽ (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            തിരികെ അസത്യം;
        }

        fileLength = GetFileSize(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        എങ്കിൽ (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            എങ്കിൽ (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            തിരികെ അസത്യം;
        }

        /* map a view of the file into our address space */
        pData->pHeader = reinterpret_cast<സ്ഥിരം DataHeader *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        എങ്കിൽ (pData->pHeader == nullptr) {
            CloseHandle(map);
            തിരികെ അസത്യം;
        }
        pData->map = map;
        pData->length = fileLength;

        തിരികെ സത്യം;
    }

    U_CFUNC ശൂന്യം
    uprv_unmapFile(UDataMemory *pData) {
        എങ്കിൽ (pData != nullptr && pData->map != nullptr) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader = nullptr;
            pData->map = nullptr;
        }
    }



#അല്ലെങ്കിലെങ്കിൽ MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
        പൂർണ്ണസംഖ്യ fd;
        പൂർണ്ണസംഖ്യ length;
        ഘടന stat mystat;
        ശൂന്യം *data;

        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ അസത്യം;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the file */
        fd=open(path, O_RDONLY);
        എങ്കിൽ(fd==-1) {
            തിരികെ അസത്യം;
        }

        /* determine the length of the file */
        എങ്കിൽ(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            തിരികെ അസത്യം;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#എങ്കിൽ U_PLATFORM != U_PF_HPUX
        data=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#അല്ലെങ്കിൽ
        data=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        എങ്കിൽ(data==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            തിരികെ അസത്യം;
        }

        pData->map = (അക്ഷരം *)data + length;
        pData->pHeader=(സ്ഥിരം DataHeader *)data;
        pData->mapAddr = data;
        pData->length = length;
#എങ്കിൽ U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(data, length, POSIX_MADV_RANDOM);
#   endif
#endif
        തിരികെ സത്യം;
    }

    U_CFUNC ശൂന്യം
    uprv_unmapFile(UDataMemory *pData) {
        എങ്കിൽ(pData!=nullptr && pData->map!=nullptr) {
            size_t dataLen = (അക്ഷരം *)pData->map - (അക്ഷരം *)pData->mapAddr;
            എങ്കിൽ(munmap(pData->mapAddr, dataLen)==-1) {
            }
            pData->pHeader=nullptr;
            pData->map=nullptr;
            pData->mapAddr=nullptr;
        }
    }



#അല്ലെങ്കിലെങ്കിൽ MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    സ്ഥിതം int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        തിരികെ size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
        FILE *file;
        int32_t fileLength;
        ശൂന്യം *p;

        എങ്കിൽ (U_FAILURE(*status)) {
            തിരികെ അസത്യം;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(path, "rb");
        എങ്കിൽ(file==nullptr) {
            തിരികെ അസത്യം;
        }

        /* get the file length */
        fileLength=umap_fsize(file);
        എങ്കിൽ(ferror(file) || fileLength<=20) {
            fclose(file);
            തിരികെ അസത്യം;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(fileLength);
        എങ്കിൽ(p==nullptr) {
            fclose(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }

        /* read the file */
        എങ്കിൽ(fileLength!=fread(p, 1, fileLength, file)) {
            uprv_free(p);
            fclose(file);
            തിരികെ അസത്യം;
        }

        fclose(file);
        pData->map=p;
        pData->pHeader=(സ്ഥിരം DataHeader *)p;
        pData->mapAddr=p;
        pData->length = fileLength;
        തിരികെ സത്യം;
    }

    U_CFUNC ശൂന്യം
    uprv_unmapFile(UDataMemory *pData) {
        എങ്കിൽ(pData!=nullptr && pData->map!=nullptr) {
            uprv_free(pData->map);
            pData->map     = nullptr;
            pData->mapAddr = nullptr;
            pData->pHeader = nullptr;
        }
    }
#അല്ലെങ്കിൽ
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
