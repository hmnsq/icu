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
#अन्तर्भाव "uposixdefs.h"

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "udatamem.h"
#अन्तर्भाव "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#जइ MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   जइ U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       जइ !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma प्रक्षिप_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_मानम्
#       endif
#   endif

#   include <windows.h>

#   जइ U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_मानम्)
#       pragma निष्कासय_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

प्रकारनिर्देश HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#नोचेद्यदि MAP_IMPLEMENTATION==MAP_POSIX
    प्रकारनिर्देश size_t MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((शून्य*)-1)
#   endif
#नोचेद्यदि MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    प्रकारनिर्देश शून्य *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#endif

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#जइ MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ध्रुव वर्ण *path, UErrorCode *status) {
        जइ (U_FAILURE(*status)) {
            प्रत्यागम मिच्छ;
        }
        UDataMemory_init(pData); /* Clear the output struct. */
        प्रत्यागम मिच्छ;            /* no file access */
    }

    U_CFUNC शून्य uprv_unmapFile(UDataMemory *pData) {
        /* nothing to do */
    }
#नोचेद्यदि MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ध्रुव वर्ण *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        जइ (U_FAILURE(*status)) {
            प्रत्यागम मिच्छ;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_मानम्;
        DWORD fileLength = 0;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
#जइ U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#अण्णहा
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        जइ (U_FAILURE(*status)) {
            प्रत्यागम मिच्छ;
        }
        जइ (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम मिच्छ;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        जइ (file == INVALID_HANDLE_मानम्) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            जइ (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            प्रत्यागम मिच्छ;
        }

        fileLength = GetFileSize(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        जइ (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            जइ (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            प्रत्यागम मिच्छ;
        }

        /* map a view of the file into our address space */
        pData->pHeader = reinterpret_cast<ध्रुव DataHeader *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        जइ (pData->pHeader == nullptr) {
            CloseHandle(map);
            प्रत्यागम मिच्छ;
        }
        pData->map = map;
        pData->length = fileLength;

        प्रत्यागम सच्च;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        जइ (pData != nullptr && pData->map != nullptr) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader = nullptr;
            pData->map = nullptr;
        }
    }



#नोचेद्यदि MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ध्रुव वर्ण *path, UErrorCode *status) {
        पूर्णाङ्क fd;
        पूर्णाङ्क length;
        संरचना stat mystat;
        शून्य *data;

        जइ (U_FAILURE(*status)) {
            प्रत्यागम मिच्छ;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the file */
        fd=open(path, O_RDONLY);
        जइ(fd==-1) {
            प्रत्यागम मिच्छ;
        }

        /* determine the length of the file */
        जइ(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            प्रत्यागम मिच्छ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#जइ U_PLATFORM != U_PF_HPUX
        data=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#अण्णहा
        data=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        जइ(data==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            प्रत्यागम मिच्छ;
        }

        pData->map = (वर्ण *)data + length;
        pData->pHeader=(ध्रुव DataHeader *)data;
        pData->mapAddr = data;
        pData->length = length;
#जइ U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(data, length, POSIX_MADV_RANDOM);
#   endif
#endif
        प्रत्यागम सच्च;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        जइ(pData!=nullptr && pData->map!=nullptr) {
            size_t dataLen = (वर्ण *)pData->map - (वर्ण *)pData->mapAddr;
            जइ(munmap(pData->mapAddr, dataLen)==-1) {
            }
            pData->pHeader=nullptr;
            pData->map=nullptr;
            pData->mapAddr=nullptr;
        }
    }



#नोचेद्यदि MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    स्थिर int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        प्रत्यागम size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ध्रुव वर्ण *path, UErrorCode *status) {
        FILE *file;
        int32_t fileLength;
        शून्य *p;

        जइ (U_FAILURE(*status)) {
            प्रत्यागम मिच्छ;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(path, "rb");
        जइ(file==nullptr) {
            प्रत्यागम मिच्छ;
        }

        /* get the file length */
        fileLength=umap_fsize(file);
        जइ(ferror(file) || fileLength<=20) {
            fclose(file);
            प्रत्यागम मिच्छ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(fileLength);
        जइ(p==nullptr) {
            fclose(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम मिच्छ;
        }

        /* read the file */
        जइ(fileLength!=fread(p, 1, fileLength, file)) {
            uprv_free(p);
            fclose(file);
            प्रत्यागम मिच्छ;
        }

        fclose(file);
        pData->map=p;
        pData->pHeader=(ध्रुव DataHeader *)p;
        pData->mapAddr=p;
        pData->length = fileLength;
        प्रत्यागम सच्च;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        जइ(pData!=nullptr && pData->map!=nullptr) {
            uprv_free(pData->map);
            pData->map     = nullptr;
            pData->mapAddr = nullptr;
            pData->pHeader = nullptr;
        }
    }
#अण्णहा
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
