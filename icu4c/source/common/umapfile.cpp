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
#ਅਨ੍ਤਰ੍ਭਾਵ "uposixdefs.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#ਜੇ MAP_IMPLEMENTATION==MAP_WIN32
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32_LEAN_AND_ਔਸਤ
#   define WIN32_LEAN_AND_ਔਸਤ
#ਜੇ_ਅੰਤ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   ਜੇ U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       ਜੇ !(WINAPI_FAMILY_ਹਿੱਸੇ_ਕਰੋ(WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP | WINAPI_ਹਿੱਸੇ_ਕਰੋ_SYSTEM))
#           pragma ਧੱਕੋ_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP
#           define WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP 1
#           define ਬਦਲਿਆ_WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP_ਮੁੱਲ
#       endif
#   endif

#   include <windows.h>

#   ਜੇ U_PLATFORM_HAS_WINUWP_API == 1 && defined(ਬਦਲਿਆ_WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP_ਮੁੱਲ)
#       pragma ਕੱਢੋ_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ HANDLE ਸਮ੍ਰਿਤੀ_Map;

#   define IS_MAP(map) ((map)!=ਖਾਲੀ_ਸੂਚਕ)

#ਜਾਂਜੇ MAP_IMPLEMENTATION==MAP_POSIX
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ size_t ਸਮ੍ਰਿਤੀ_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((ਖਾਲੀ*)-1)
#   endif
#ਜਾਂਜੇ MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *ਸਮ੍ਰਿਤੀ_Map;

#   define IS_MAP(map) ((map)!=ਖਾਲੀ_ਸੂਚਕ)
#ਜੇ_ਅੰਤ

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#ਜੇ MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        UData_ਸਮ੍ਰਿਤੀ_init(p_ਅੰਕੜੇ); /* Clear the output struct. */
        ਵਾਪਸ ਝੂਠ;            /* no file access */
    }

    U_CFUNC ਖਾਲੀ uprv_unmapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
        /* nothing to do */
    }
#ਜਾਂਜੇ MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,      /* File path to be opened/mapped.                     */
         UError_ਕੋਡ *ਸਥਿਤੀ     /* Error status, used to report out-of-memory errors. */
         )
    {
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }

        HANDLE map = ਖਾਲੀ_ਸੂਚਕ;
        HANDLE file = INVALID_HANDLE_ਮੁੱਲ;
        DWORD file_ਲੰਮਾਈ = 0;

        UData_ਸਮ੍ਰਿਤੀ_init(p_ਅੰਕੜੇ); /* Clear the output struct.        */

        /* open the input file */
#ਜੇ U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(ਰਸਤਾ, GENERIC_READ, FILE_SHARE_READ, ਖਾਲੀ_ਸੂਚਕ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_ਸਾਧਾਰਨ|FILE_FLAG_RANDOM_ACCESS, ਖਾਲੀ_ਸੂਚਕ);
#ਨਹੀਂਤਾਂ
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_ਰਸਤਾ[MAX_ਰਸਤਾ];
        int32_t ਰਸਤਾ_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_ਰਸਤਾ), static_cast<int32_t>(UPRV_LENGTHOF(utf16_ਰਸਤਾ)), &ਰਸਤਾ_Utf16_Len, ਰਸਤਾ, -1, ਸਥਿਤੀ);

        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਜੇ (*ਸਥਿਤੀ == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }

        file = CreateFileW(utf16_ਰਸਤਾ, GENERIC_READ, FILE_SHARE_READ, ਖਾਲੀ_ਸੂਚਕ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_ਸਾਧਾਰਨ | FILE_FLAG_RANDOM_ACCESS, ਖਾਲੀ_ਸੂਚਕ);
#ਜੇ_ਅੰਤ
        ਜੇ (file == INVALID_HANDLE_ਮੁੱਲ) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            ਜੇ (HRESULT_FROM_WIN32(Get_ਆਖਰੀ_Error()) == E_OUTOFMEMORY) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
            ਵਾਪਸ ਝੂਠ;
        }

        file_ਲੰਮਾਈ = Get_File_ਮਾਪ(file, ਖਾਲੀ_ਸੂਚਕ);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, ਖਾਲੀ_ਸੂਚਕ, ਪੰਨਾ_READONLY, 0, 0, ਖਾਲੀ_ਸੂਚਕ);

        CloseHandle(file);
        ਜੇ (map == ਖਾਲੀ_ਸੂਚਕ) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            ਜੇ (HRESULT_FROM_WIN32(Get_ਆਖਰੀ_Error()) == E_OUTOFMEMORY) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
            ਵਾਪਸ ਝੂਠ;
        }

        /* map a view of the file into our address space */
        p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ = reinterpret_cast<ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *>(Map_View_ਦਾ_File(map, FILE_MAP_READ, 0, 0, 0));
        ਜੇ (p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ == ਖਾਲੀ_ਸੂਚਕ) {
            CloseHandle(map);
            ਵਾਪਸ ਝੂਠ;
        }
        p_ਅੰਕੜੇ->map = map;
        p_ਅੰਕੜੇ->length = file_ਲੰਮਾਈ;

        ਵਾਪਸ ਸੱਚ;
    }

    U_CFUNC ਖਾਲੀ
    uprv_unmapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
        ਜੇ (p_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->map != ਖਾਲੀ_ਸੂਚਕ) {
            Unmap_View_ਦਾ_File(p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ);
            CloseHandle(p_ਅੰਕੜੇ->map);
            p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ = ਖਾਲੀ_ਸੂਚਕ;
            p_ਅੰਕੜੇ->map = ਖਾਲੀ_ਸੂਚਕ;
        }
    }



#ਜਾਂਜੇ MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
        ਪੂਰਨ fd;
        ਪੂਰਨ length;
        ਬਣਤਰ stat mystat;
        ਖਾਲੀ *ਅੰਕੜੇ;

        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }

        UData_ਸਮ੍ਰਿਤੀ_init(p_ਅੰਕੜੇ); /* Clear the output struct.        */

        /* open the file */
        fd=open(ਰਸਤਾ, O_RDONLY);
        ਜੇ(fd==-1) {
            ਵਾਪਸ ਝੂਠ;
        }

        /* determine the length of the file */
        ਜੇ(stat(ਰਸਤਾ, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            ਵਾਪਸ ਝੂਠ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#ਜੇ U_PLATFORM != U_PF_HPUX
        ਅੰਕੜੇ=mmap(ਖਾਲੀ_ਸੂਚਕ, length, PROT_READ, MAP_SHARED, fd, 0);
#ਨਹੀਂਤਾਂ
        ਅੰਕੜੇ=mmap(ਖਾਲੀ_ਸੂਚਕ, length, PROT_READ, MAP_ਨਿੱਜੀ, fd, 0);
#ਜੇ_ਅੰਤ
        close(fd); /* no longer needed */
        ਜੇ(ਅੰਕੜੇ==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            ਵਾਪਸ ਝੂਠ;
        }

        p_ਅੰਕੜੇ->map = (ਅੱਖਰ *)ਅੰਕੜੇ + length;
        p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ=(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
        p_ਅੰਕੜੇ->mapAddr = ਅੰਕੜੇ;
        p_ਅੰਕੜੇ->length = length;
#ਜੇ U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(ਅੰਕੜੇ, length, POSIX_MADV_RANDOM);
#   endif
#ਜੇ_ਅੰਤ
        ਵਾਪਸ ਸੱਚ;
    }

    U_CFUNC ਖਾਲੀ
    uprv_unmapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
        ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->map!=ਖਾਲੀ_ਸੂਚਕ) {
            size_t ਅੰਕੜੇ_Len = (ਅੱਖਰ *)p_ਅੰਕੜੇ->map - (ਅੱਖਰ *)p_ਅੰਕੜੇ->mapAddr;
            ਜੇ(munmap(p_ਅੰਕੜੇ->mapAddr, ਅੰਕੜੇ_Len)==-1) {
            }
            p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ=ਖਾਲੀ_ਸੂਚਕ;
            p_ਅੰਕੜੇ->map=ਖਾਲੀ_ਸੂਚਕ;
            p_ਅੰਕੜੇ->mapAddr=ਖਾਲੀ_ਸੂਚਕ;
        }
    }



#ਜਾਂਜੇ MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    ਸਥਿਰ int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        ਵਾਪਸ size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
        FILE *file;
        int32_t file_ਲੰਮਾਈ;
        ਖਾਲੀ *p;

        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }

        UData_ਸਮ੍ਰਿਤੀ_init(p_ਅੰਕੜੇ); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(ਰਸਤਾ, "rb");
        ਜੇ(file==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਝੂਠ;
        }

        /* get the file length */
        file_ਲੰਮਾਈ=umap_fsize(file);
        ਜੇ(ferror(file) || file_ਲੰਮਾਈ<=20) {
            fclose(file);
            ਵਾਪਸ ਝੂਠ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_ਲੰਮਾਈ);
        ਜੇ(p==ਖਾਲੀ_ਸੂਚਕ) {
            fclose(file);
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }

        /* read the file */
        ਜੇ(file_ਲੰਮਾਈ!=fread(p, 1, file_ਲੰਮਾਈ, file)) {
            uprv_free(p);
            fclose(file);
            ਵਾਪਸ ਝੂਠ;
        }

        fclose(file);
        p_ਅੰਕੜੇ->map=p;
        p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ=(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)p;
        p_ਅੰਕੜੇ->mapAddr=p;
        p_ਅੰਕੜੇ->length = file_ਲੰਮਾਈ;
        ਵਾਪਸ ਸੱਚ;
    }

    U_CFUNC ਖਾਲੀ
    uprv_unmapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
        ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->map!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(p_ਅੰਕੜੇ->map);
            p_ਅੰਕੜੇ->map     = ਖਾਲੀ_ਸੂਚਕ;
            p_ਅੰਕੜੇ->mapAddr = ਖਾਲੀ_ਸੂਚਕ;
            p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
#ਨਹੀਂਤਾਂ
#   error MAP_IMPLEMENTATION is ਸੈੱਟ incorrectly
#ਜੇ_ਅੰਤ
