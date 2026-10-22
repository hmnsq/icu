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
#அந்தர்பாவ "uposixdefs.h"

#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "udatamem.h"
#அந்தர்பாவ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#எனில் MAP_IMPLEMENTATION==MAP_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   எனில் U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       எனில் !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM))
#           pragma தள்ளு_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define CHANGED_WINAPI_PARTITION_DESKTOP_மதிப்பு
#       endif
#   endif

#   include <windows.h>

#   எனில் U_PLATFORM_HAS_WINUWP_API == 1 && defined(CHANGED_WINAPI_PARTITION_DESKTOP_மதிப்பு)
#       pragma எடு_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

வகைவரை HANDLE MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)

#அல்லதெனில் MAP_IMPLEMENTATION==MAP_POSIX
    வகைவரை size_t MemoryMap;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((வெறுமை*)-1)
#   endif
#அல்லதெனில் MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    வகைவரை வெறுமை *MemoryMap;

#   define IS_MAP(map) ((map)!=nullptr)
#endif

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#எனில் MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, மாறிலி எழுத்து *path, UErrorCode *status) {
        எனில் (U_FAILURE(*status)) {
            திரும்பு பொய்;
        }
        UDataMemory_init(pData); /* Clear the output struct. */
        திரும்பு பொய்;            /* no file access */
    }

    U_CFUNC வெறுமை uprv_unmapFile(UDataMemory *pData) {
        /* nothing to do */
    }
#அல்லதெனில் MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         மாறிலி எழுத்து *path,      /* File path to be opened/mapped.                     */
         UErrorCode *status     /* Error status, used to report out-of-memory errors. */
         )
    {
        எனில் (U_FAILURE(*status)) {
            திரும்பு பொய்;
        }

        HANDLE map = nullptr;
        HANDLE file = INVALID_HANDLE_மதிப்பு;
        DWORD fileLength = 0;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
#எனில் U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, nullptr);
#இல்லையெனில்
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16Path[MAX_PATH];
        int32_t pathUtf16Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16Path), static_cast<int32_t>(UPRV_LENGTHOF(utf16Path)), &pathUtf16Len, path, -1, status);

        எனில் (U_FAILURE(*status)) {
            திரும்பு பொய்;
        }
        எனில் (*status == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *status = U_BUFFER_OVERFLOW_ERROR;
            திரும்பு பொய்;
        }

        file = CreateFileW(utf16Path, GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
#endif
        எனில் (file == INVALID_HANDLE_மதிப்பு) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            எனில் (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            திரும்பு பொய்;
        }

        fileLength = GetFileSize(file, nullptr);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);

        CloseHandle(file);
        எனில் (map == nullptr) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            எனில் (HRESULT_FROM_WIN32(GetLastError()) == E_OUTOFMEMORY) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            }
            திரும்பு பொய்;
        }

        /* map a view of the file into our address space */
        pData->pHeader = reinterpret_cast<மாறிலி DataHeader *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
        எனில் (pData->pHeader == nullptr) {
            CloseHandle(map);
            திரும்பு பொய்;
        }
        pData->map = map;
        pData->length = fileLength;

        திரும்பு உண்மை;
    }

    U_CFUNC வெறுமை
    uprv_unmapFile(UDataMemory *pData) {
        எனில் (pData != nullptr && pData->map != nullptr) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader = nullptr;
            pData->map = nullptr;
        }
    }



#அல்லதெனில் MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, மாறிலி எழுத்து *path, UErrorCode *status) {
        முழு fd;
        முழு length;
        கட்டமைப்பு stat mystat;
        வெறுமை *data;

        எனில் (U_FAILURE(*status)) {
            திரும்பு பொய்;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the file */
        fd=open(path, O_RDONLY);
        எனில்(fd==-1) {
            திரும்பு பொய்;
        }

        /* determine the length of the file */
        எனில்(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            திரும்பு பொய்;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#எனில் U_PLATFORM != U_PF_HPUX
        data=mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);
#இல்லையெனில்
        data=mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        எனில்(data==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            திரும்பு பொய்;
        }

        pData->map = (எழுத்து *)data + length;
        pData->pHeader=(மாறிலி DataHeader *)data;
        pData->mapAddr = data;
        pData->length = length;
#எனில் U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(data, length, POSIX_MADV_RANDOM);
#   endif
#endif
        திரும்பு உண்மை;
    }

    U_CFUNC வெறுமை
    uprv_unmapFile(UDataMemory *pData) {
        எனில்(pData!=nullptr && pData->map!=nullptr) {
            size_t dataLen = (எழுத்து *)pData->map - (எழுத்து *)pData->mapAddr;
            எனில்(munmap(pData->mapAddr, dataLen)==-1) {
            }
            pData->pHeader=nullptr;
            pData->map=nullptr;
            pData->mapAddr=nullptr;
        }
    }



#அல்லதெனில் MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    நிலையான int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        திரும்பு size;
    }

    U_CFUNC UBool
    uprv_mapFile(UDataMemory *pData, மாறிலி எழுத்து *path, UErrorCode *status) {
        FILE *file;
        int32_t fileLength;
        வெறுமை *p;

        எனில் (U_FAILURE(*status)) {
            திரும்பு பொய்;
        }

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(path, "rb");
        எனில்(file==nullptr) {
            திரும்பு பொய்;
        }

        /* get the file length */
        fileLength=umap_fsize(file);
        எனில்(ferror(file) || fileLength<=20) {
            fclose(file);
            திரும்பு பொய்;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(fileLength);
        எனில்(p==nullptr) {
            fclose(file);
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }

        /* read the file */
        எனில்(fileLength!=fread(p, 1, fileLength, file)) {
            uprv_free(p);
            fclose(file);
            திரும்பு பொய்;
        }

        fclose(file);
        pData->map=p;
        pData->pHeader=(மாறிலி DataHeader *)p;
        pData->mapAddr=p;
        pData->length = fileLength;
        திரும்பு உண்மை;
    }

    U_CFUNC வெறுமை
    uprv_unmapFile(UDataMemory *pData) {
        எனில்(pData!=nullptr && pData->map!=nullptr) {
            uprv_free(pData->map);
            pData->map     = nullptr;
            pData->mapAddr = nullptr;
            pData->pHeader = nullptr;
        }
    }
#இல்லையெனில்
#   error MAP_IMPLEMENTATION is set incorrectly
#endif
