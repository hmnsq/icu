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

#यदि MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   यदि U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       यदि !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma थप_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_मान
#       endif
#   endif

#   include <windows.h>

#   यदि U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_मान)
#       pragma झिक_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

प्रकारनिर्देश HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#अथवायदि MAP_IMPLEMENTATION==MAP_POSIX
    प्रकारनिर्देश माप_प्रकार MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((शून्य*)-1)
#   endif
#अथवायदि MAP_IMPLEMENTATION==MAP_STDIO
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
#यदि MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, अचर वर्ण *path, UErrorCode *status) {
        यदि (U_FAILURE(*status)) {
            फर्काऊ असत्य;
        }
        UDataMemory_init(pData); /* Clear the output struct. */
        फर्काऊ असत्य;            /* no file access */
    }

    U_CFUNC शून्य uprv_unmapFile(UDataMemory *pData) {
        /* nothing to do */
    }
#अथवायदि MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         अचर वर्ण *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        यदि (U_FAILURE(*status)) {
            फर्काऊ असत्य;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_मान;
        DWORD fileLength = 0;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
#यदि U_PLATFORM_HAS_WINUWP_API == 0
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

        यदि (U_FAILURE(*status)) {
            फर्काऊ असत्य;
        }
        यदि (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_BUFFER_OVERFLOW_ERROR;
            फर्काऊ असत्य;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        यदि (file == INVALID_HANDLE_मान) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            यदि (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            फर्काऊ असत्य;
        }

        fileLength = GetFileSize(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        यदि (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            यदि (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            फर्काऊ असत्य;
        }

        /* map a view of the file into our address space */
        pData->pHeader = reinterpret_cast<अचर DataHeader *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        यदि (pData->pHeader == nullptr) {
            CloseHandle(map);
            फर्काऊ असत्य;
        }
        pData->map = map;
        pData->length = fileLength;

        फर्काऊ सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        यदि (pData != nullptr && pData->map != nullptr) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader = nullptr;
            pData->map = nullptr;
        }
    }



#अथवायदि MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, अचर वर्ण *path, UErrorCode *status) {
        पूर्णाङ्क fd;
        पूर्णाङ्क length;
        संरचना stat mystat;
        शून्य *data;

        यदि (U_FAILURE(*status)) {
            फर्काऊ असत्य;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the file */
        fd=खोलो(path, O_RDONLY);
        यदि(fd==-1) {
            फर्काऊ असत्य;
        }

        /* determine the length of the file */
        यदि(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            बंद(fd);
            फर्काऊ असत्य;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#यदि U_PLATFORM != U_PF_HPUX
        data=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#अन्यथा
        data=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        बंद(fd); /* no longer needed */
        यदि(data==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            फर्काऊ असत्य;
        }

        pData->map = (वर्ण *)data + length;
        pData->pHeader=(अचर DataHeader *)data;
        pData->mapAddr = data;
        pData->length = length;
#यदि U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(data, length, POSIX_MADV_RANDOM);
#   endif
#endif
        फर्काऊ सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        यदि(pData!=nullptr && pData->map!=nullptr) {
            माप_प्रकार dataLen = (वर्ण *)pData->map - (वर्ण *)pData->mapAddr;
            यदि(munmap(pData->mapAddr, dataLen)==-1) {
            }
            pData->pHeader=nullptr;
            pData->map=nullptr;
            pData->mapAddr=nullptr;
        }
    }



#अथवायदि MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    स्थिर int32_t
    umap_fsize(खाता *f) {
        int32_t savedPos = ख_बताओ(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        ख_जाओ(f, 0, अंत_से);
        size = (int32_t)ख_बताओ(f);
        ख_जाओ(f, savedPos, शुरू_से);
        फर्काऊ size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, अचर वर्ण *path, UErrorCode *status) {
        खाता *file;
        int32_t fileLength;
        शून्य *p;

        यदि (U_FAILURE(*status)) {
            फर्काऊ असत्य;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=ख_खोलो(path, "rb");
        यदि(file==nullptr) {
            फर्काऊ असत्य;
        }

        /* get the file length */
        fileLength=umap_fsize(file);
        यदि(ख_त्रुटि(file) || fileLength<=20) {
            ख_बंद(file);
            फर्काऊ असत्य;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(fileLength);
        यदि(p==nullptr) {
            ख_बंद(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }

        /* read the file */
        यदि(fileLength!=ख_पढ़ो(p, 1, fileLength, file)) {
            uprv_free(p);
            ख_बंद(file);
            फर्काऊ असत्य;
        }

        ख_बंद(file);
        pData->map=p;
        pData->pHeader=(अचर DataHeader *)p;
        pData->mapAddr=p;
        pData->length = fileLength;
        फर्काऊ सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UDataMemory *pData) {
        यदि(pData!=nullptr && pData->map!=nullptr) {
            uprv_free(pData->map);
            pData->map     = nullptr;
            pData->mapAddr = nullptr;
            pData->pHeader = nullptr;
        }
    }
#अन्यथा
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
