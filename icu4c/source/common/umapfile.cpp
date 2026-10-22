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
#समावेश "uposixdefs.h"

#समावेश "unicode/putil.h"
#समावेश "unicode/ustring.h"
#समावेश "udatamem.h"
#समावेश "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#अगर MAP_IMPLEMENTATION==MAP_WIN32
#अगर_अघोषित WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#पूर्ण_अगर
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   अगर U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       अगर !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma धकेलो_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_मूल्य
#       endif
#   endif

#   include <windows.h>

#   अगर U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_मूल्य)
#       pragma निकालो_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

प्रकार HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#या_अगर MAP_IMPLEMENTATION==MAP_POSIX
    प्रकार माप_प्रकार MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((व्योम*)-1)
#   endif
#या_अगर MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    प्रकार व्योम *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#पूर्ण_अगर

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#अगर MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_तथ्य, स्थिर अक्षर *path, UErrorCode *status) {
        अगर (U_FAILURE(*status)) {
            वापस false;
        }
        UDataMemory_init(p_तथ्य); /* Clear the output struct. */
        वापस false;            /* no file access */
    }

    U_CFUNC व्योम uprv_unmapFile(UDataMemory *p_तथ्य) {
        /* nothing to do */
    }
#या_अगर MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *p_तथ्य,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         स्थिर अक्षर *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        अगर (U_FAILURE(*status)) {
            वापस false;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_मूल्य;
        DWORD file_लंबाई = 0;

        UDataMemory_init(p_तथ्य); /* Clear the output struct.        */

        /* open the input file */
#अगर U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#अन्यथा
        // Convert from UTF-8 string to UTF-16 string.
        ब_अक्षर_प्रकार utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        अगर (U_FAILURE(*status)) {
            वापस false;
        }
        अगर (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_बफर_OVERFLOW_ERROR;
            वापस false;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#पूर्ण_अगर
        अगर (file == INVALID_HANDLE_मूल्य) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            अगर (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            वापस false;
        }

        file_लंबाई = Get_File_आकार(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        अगर (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            अगर (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            वापस false;
        }

        /* map a view of the file into our address space */
        p_तथ्य->pHeader = reinterpret_cast<स्थिर तथ्य_Header *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        अगर (p_तथ्य->pHeader == nullptr) {
            CloseHandle(map);
            वापस false;
        }
        p_तथ्य->map = map;
        p_तथ्य->length = file_लंबाई;

        वापस true;
    }

    U_CFUNC व्योम
    uprv_unmapFile(UDataMemory *p_तथ्य) {
        अगर (p_तथ्य != nullptr && p_तथ्य->map != nullptr) {
            UnmapViewOfFile(p_तथ्य->pHeader);
            CloseHandle(p_तथ्य->map);
            p_तथ्य->pHeader = nullptr;
            p_तथ्य->map = nullptr;
        }
    }



#या_अगर MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_तथ्य, स्थिर अक्षर *path, UErrorCode *status) {
        पूर्णांक fd;
        पूर्णांक length;
        काष्ठा stat mystat;
        व्योम *तथ्य;

        अगर (U_FAILURE(*status)) {
            वापस false;
        }

        UDataMemory_init(p_तथ्य); /* Clear the output struct.        */

        /* open the file */
        fd=खोलो(path, O_RDONLY);
        अगर(fd==-1) {
            वापस false;
        }

        /* determine the length of the file */
        अगर(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            बंद(fd);
            वापस false;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#अगर U_PLATFORM != U_PF_HPUX
        तथ्य=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#अन्यथा
        तथ्य=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#पूर्ण_अगर
        बंद(fd); /* no longer needed */
        अगर(तथ्य==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            वापस false;
        }

        p_तथ्य->map = (अक्षर *)तथ्य + length;
        p_तथ्य->pHeader=(स्थिर तथ्य_Header *)तथ्य;
        p_तथ्य->mapAddr = तथ्य;
        p_तथ्य->length = length;
#अगर U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(तथ्य, length, POSIX_MADV_RANDOM);
#   endif
#पूर्ण_अगर
        वापस true;
    }

    U_CFUNC व्योम
    uprv_unmapFile(UDataMemory *p_तथ्य) {
        अगर(p_तथ्य!=nullptr && p_तथ्य->map!=nullptr) {
            माप_प्रकार तथ्य_Len = (अक्षर *)p_तथ्य->map - (अक्षर *)p_तथ्य->mapAddr;
            अगर(munmap(p_तथ्य->mapAddr, तथ्य_Len)==-1) {
            }
            p_तथ्य->pHeader=nullptr;
            p_तथ्य->map=nullptr;
            p_तथ्य->mapAddr=nullptr;
        }
    }



#या_अगर MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    अटल int32_t
    umap_fsize(खाता *f) {
        int32_t savedPos = ख_बताओ(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        ख_जाओ(f, 0, अंत_से);
        size = (int32_t)ख_बताओ(f);
        ख_जाओ(f, savedPos, शुरू_से);
        वापस size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *p_तथ्य, स्थिर अक्षर *path, UErrorCode *status) {
        खाता *file;
        int32_t file_लंबाई;
        व्योम *p;

        अगर (U_FAILURE(*status)) {
            वापस false;
        }

        UDataMemory_init(p_तथ्य); /* Clear the output struct.        */
        /* open the input file */
        file=ख_खोलो(path, "rb");
        अगर(file==nullptr) {
            वापस false;
        }

        /* get the file length */
        file_लंबाई=umap_fsize(file);
        अगर(ख_त्रुटि(file) || file_लंबाई<=20) {
            ख_बंद(file);
            वापस false;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_लंबाई);
        अगर(p==nullptr) {
            ख_बंद(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }

        /* read the file */
        अगर(file_लंबाई!=ख_पढ़ो(p, 1, file_लंबाई, file)) {
            uprv_free(p);
            ख_बंद(file);
            वापस false;
        }

        ख_बंद(file);
        p_तथ्य->map=p;
        p_तथ्य->pHeader=(स्थिर तथ्य_Header *)p;
        p_तथ्य->mapAddr=p;
        p_तथ्य->length = file_लंबाई;
        वापस true;
    }

    U_CFUNC व्योम
    uprv_unmapFile(UDataMemory *p_तथ्य) {
        अगर(p_तथ्य!=nullptr && p_तथ्य->map!=nullptr) {
            uprv_free(p_तथ्य->map);
            p_तथ्य->map     = nullptr;
            p_तथ्य->mapAddr = nullptr;
            p_तथ्य->pHeader = nullptr;
        }
    }
#अन्यथा
#   error MAP_IMPLEMENTATION is set incorrectly
#पूर्ण_अगर
