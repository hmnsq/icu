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
#ଅନ୍ତର୍ଭାଵ "uposixdefs.h"

#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "udatamem.h"
#ଅନ୍ତର୍ଭାଵ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#ଯଦି MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   ଯଦି U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       ଯଦି !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma ଠେଲ_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_ମୂଲ୍ୟ
#       endif
#   endif

#   include <windows.h>

#   ଯଦି U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_ମୂଲ୍ୟ)
#       pragma କାଢ଼_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#ନଚେତଯଦି MAP_IMPLEMENTATION==MAP_POSIX
    ପ୍ରକାରବ୍ୟାଖ୍ୟା size_t MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((ଶୂନ୍ୟ*)-1)
#   endif
#ନଚେତଯଦି MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#endif

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#ଯଦି MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status) {
        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        UDataMemory_init(pData); /* Clear the output struct. */
        ଫେରନ୍ତୁ ମିଥ୍ୟା;            /* no file access */
    }

    U_CFUNC ଶୂନ୍ୟ uprv_unmapFile(UDataMemory *pData) {
        /* nothing to do */
    }
#ନଚେତଯଦି MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ଅଚଳ ଅକ୍ଷର *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_ମୂଲ୍ୟ;
        DWORD fileLength = 0;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
#ଯଦି U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#ଅନ୍ୟଥା
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        ଯଦି (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_BUFFER_OVERFLOW_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        ଯଦି (file == INVALID_HANDLE_ମୂଲ୍ୟ) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            ଯଦି (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        fileLength = GetFileSize(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        ଯଦି (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            ଯଦି (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        /* map a view of the file into our address space */
        pData->pHeader = reinterpret_cast<ଅଚଳ DataHeader *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        ଯଦି (pData->pHeader == nullptr) {
            CloseHandle(map);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        pData->map = map;
        pData->length = fileLength;

        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    U_CFUNC ଶୂନ୍ୟ
    uprv_unmapFile(UDataMemory *pData) {
        ଯଦି (pData != nullptr && pData->map != nullptr) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader = nullptr;
            pData->map = nullptr;
        }
    }



#ନଚେତଯଦି MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;
        ସଂରଚନା stat mystat;
        ଶୂନ୍ୟ *data;

        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the file */
        fd=open(path, O_RDONLY);
        ଯଦି(fd==-1) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        /* determine the length of the file */
        ଯଦି(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#ଯଦି U_PLATFORM != U_PF_HPUX
        data=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#ଅନ୍ୟଥା
        data=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        ଯଦି(data==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        pData->map = (ଅକ୍ଷର *)data + length;
        pData->pHeader=(ଅଚଳ DataHeader *)data;
        pData->mapAddr = data;
        pData->length = length;
#ଯଦି U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(data, length, POSIX_MADV_RANDOM);
#   endif
#endif
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    U_CFUNC ଶୂନ୍ୟ
    uprv_unmapFile(UDataMemory *pData) {
        ଯଦି(pData!=nullptr && pData->map!=nullptr) {
            size_t dataLen = (ଅକ୍ଷର *)pData->map - (ଅକ୍ଷର *)pData->mapAddr;
            ଯଦି(munmap(pData->mapAddr, dataLen)==-1) {
            }
            pData->pHeader=nullptr;
            pData->map=nullptr;
            pData->mapAddr=nullptr;
        }
    }



#ନଚେତଯଦି MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    ସ୍ଥିର int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        ଫେରନ୍ତୁ size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, ଅଚଳ ଅକ୍ଷର *path, UErrorCode *status) {
        FILE *file;
        int32_t fileLength;
        ଶୂନ୍ୟ *p;

        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(path, "rb");
        ଯଦି(file==nullptr) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        /* get the file length */
        fileLength=umap_fsize(file);
        ଯଦି(ferror(file) || fileLength<=20) {
            fclose(file);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(fileLength);
        ଯଦି(p==nullptr) {
            fclose(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        /* read the file */
        ଯଦି(fileLength!=fread(p, 1, fileLength, file)) {
            uprv_free(p);
            fclose(file);
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }

        fclose(file);
        pData->map=p;
        pData->pHeader=(ଅଚଳ DataHeader *)p;
        pData->mapAddr=p;
        pData->length = fileLength;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    U_CFUNC ଶୂନ୍ୟ
    uprv_unmapFile(UDataMemory *pData) {
        ଯଦି(pData!=nullptr && pData->map!=nullptr) {
            uprv_free(pData->map);
            pData->map     = nullptr;
            pData->mapAddr = nullptr;
            pData->pHeader = nullptr;
        }
    }
#ଅନ୍ୟଥା
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
