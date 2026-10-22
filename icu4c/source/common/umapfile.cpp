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
#অন্তর্ভাব "uposixdefs.h"

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "udatamem.h"
#অন্তর্ভাব "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#যদি MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   যদি U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       যদি !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma ঠেলো_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_মান
#       endif
#   endif

#   include <windows.h>

#   যদি U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_মান)
#       pragma বের_করো_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

প্রকারলেখ HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#নতুবা_যদি MAP_IMPLEMENTATION==MAP_POSIX
    প্রকারলেখ size_t MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((শূন্য*)-1)
#   endif
#নতুবা_যদি MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    প্রকারলেখ শূন্য *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#endif

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#যদি MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_তথ্য, ধ্রুবক অক্ষর *path, UErrorCode *status) {
        যদি (U_FAILURE(*status)) {
            ফেরত মিথ্যা;
        }
        UDataMemory_init(p_তথ্য); /* Clear the output struct. */
        ফেরত মিথ্যা;            /* no file access */
    }

    U_CFUNC শূন্য uprv_unmapFile(UDataMemory *p_তথ্য) {
        /* nothing to do */
    }
#নতুবা_যদি MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *p_তথ্য,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ধ্রুবক অক্ষর *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        যদি (U_FAILURE(*status)) {
            ফেরত মিথ্যা;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_মান;
        DWORD file_দৈর্ঘ্য = 0;

        UDataMemory_init(p_তথ্য); /* Clear the output struct.        */

        /* open the input file */
#যদি U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#অন্যথা
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        যদি (U_FAILURE(*status)) {
            ফেরত মিথ্যা;
        }
        যদি (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_বাফার_OVERFLOW_ERROR;
            ফেরত মিথ্যা;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        যদি (file == INVALID_HANDLE_মান) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            যদি (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            ফেরত মিথ্যা;
        }

        file_দৈর্ঘ্য = Get_File_আকার(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        যদি (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            যদি (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            ফেরত মিথ্যা;
        }

        /* map a view of the file into our address space */
        p_তথ্য->pHeader = reinterpret_cast<ধ্রুবক তথ্য_Header *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        যদি (p_তথ্য->pHeader == nullptr) {
            CloseHandle(map);
            ফেরত মিথ্যা;
        }
        p_তথ্য->map = map;
        p_তথ্য->length = file_দৈর্ঘ্য;

        ফেরত সত্য;
    }

    U_CFUNC শূন্য
    uprv_unmapFile(UDataMemory *p_তথ্য) {
        যদি (p_তথ্য != nullptr && p_তথ্য->map != nullptr) {
            UnmapViewOfFile(p_তথ্য->pHeader);
            CloseHandle(p_তথ্য->map);
            p_তথ্য->pHeader = nullptr;
            p_তথ্য->map = nullptr;
        }
    }



#নতুবা_যদি MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_তথ্য, ধ্রুবক অক্ষর *path, UErrorCode *status) {
        পূর্ণসংখ্যা fd;
        পূর্ণসংখ্যা length;
        কাঠামো stat mystat;
        শূন্য *তথ্য;

        যদি (U_FAILURE(*status)) {
            ফেরত মিথ্যা;
        }

        UDataMemory_init(p_তথ্য); /* Clear the output struct.        */

        /* open the file */
        fd=open(path, O_RDONLY);
        যদি(fd==-1) {
            ফেরত মিথ্যা;
        }

        /* determine the length of the file */
        যদি(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            ফেরত মিথ্যা;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#যদি U_PLATFORM != U_PF_HPUX
        তথ্য=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#অন্যথা
        তথ্য=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        যদি(তথ্য==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            ফেরত মিথ্যা;
        }

        p_তথ্য->map = (অক্ষর *)তথ্য + length;
        p_তথ্য->pHeader=(ধ্রুবক তথ্য_Header *)তথ্য;
        p_তথ্য->mapAddr = তথ্য;
        p_তথ্য->length = length;
#যদি U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(তথ্য, length, POSIX_MADV_RANDOM);
#   endif
#endif
        ফেরত সত্য;
    }

    U_CFUNC শূন্য
    uprv_unmapFile(UDataMemory *p_তথ্য) {
        যদি(p_তথ্য!=nullptr && p_তথ্য->map!=nullptr) {
            size_t তথ্য_Len = (অক্ষর *)p_তথ্য->map - (অক্ষর *)p_তথ্য->mapAddr;
            যদি(munmap(p_তথ্য->mapAddr, তথ্য_Len)==-1) {
            }
            p_তথ্য->pHeader=nullptr;
            p_তথ্য->map=nullptr;
            p_তথ্য->mapAddr=nullptr;
        }
    }



#নতুবা_যদি MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    স্থির int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        ফেরত size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_তথ্য, ধ্রুবক অক্ষর *path, UErrorCode *status) {
        FILE *file;
        int32_t file_দৈর্ঘ্য;
        শূন্য *p;

        যদি (U_FAILURE(*status)) {
            ফেরত মিথ্যা;
        }

        UDataMemory_init(p_তথ্য); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(path, "rb");
        যদি(file==nullptr) {
            ফেরত মিথ্যা;
        }

        /* get the file length */
        file_দৈর্ঘ্য=umap_fsize(file);
        যদি(ferror(file) || file_দৈর্ঘ্য<=20) {
            fclose(file);
            ফেরত মিথ্যা;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_দৈর্ঘ্য);
        যদি(p==nullptr) {
            fclose(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }

        /* read the file */
        যদি(file_দৈর্ঘ্য!=fread(p, 1, file_দৈর্ঘ্য, file)) {
            uprv_free(p);
            fclose(file);
            ফেরত মিথ্যা;
        }

        fclose(file);
        p_তথ্য->map=p;
        p_তথ্য->pHeader=(ধ্রুবক তথ্য_Header *)p;
        p_তথ্য->mapAddr=p;
        p_তথ্য->length = file_দৈর্ঘ্য;
        ফেরত সত্য;
    }

    U_CFUNC শূন্য
    uprv_unmapFile(UDataMemory *p_তথ্য) {
        যদি(p_তথ্য!=nullptr && p_তথ্য->map!=nullptr) {
            uprv_free(p_তথ্য->map);
            p_তথ্য->map     = nullptr;
            p_তথ্য->mapAddr = nullptr;
            p_তথ্য->pHeader = nullptr;
        }
    }
#অন্যথা
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
