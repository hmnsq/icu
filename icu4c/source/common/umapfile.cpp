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
#ضمن "uposixdefs.h"

#ضمن "unicode/putil.h"
#ضمن "unicode/ustring.h"
#ضمن "udatamem.h"
#ضمن "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#إذا MAP_IMPLEMENTATION==MAP_WIN32
#جيڪڏهن_وصف_نه_ٿيل WIN32_LEAN_AND_سراسري
#   define WIN32_LEAN_AND_سراسري
#جيڪڏهن_پورو
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   إذا U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       إذا !(WINAPI_FAMILY_حصا_ڪريو(WINAPI_حصا_ڪريو_DESKTOP | WINAPI_حصا_ڪريو_SYSTEM))
#           pragma ڌڪو_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_حصا_ڪريو_DESKTOP
#           define WINAPI_حصا_ڪريو_DESKTOP 1
#           define بدليو_WINAPI_حصا_ڪريو_DESKTOP_قدر
#       endif
#   endif

#   include <windows.h>

#   إذا U_PLATFORM_HAS_WINUWP_API == 1 && defined(بدليو_WINAPI_حصا_ڪريو_DESKTOP_قدر)
#       pragma ڪڍو_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

تعريفنوع HANDLE يادگيري_Map;

#   define IS_MAP(map) ((map)!=خالي_اشارو)

#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    تعريفنوع size_t يادگيري_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((فراغ*)-1)
#   endif
#وإلاإذا MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    تعريفنوع فراغ *يادگيري_Map;

#   define IS_MAP(map) ((map)!=خالي_اشارو)
#جيڪڏهن_پورو

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#إذا MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_يادگيري *p_معلومات, ثابت حرف *رستو, UError_ڪوڊ *حالت) {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        UData_يادگيري_init(p_معلومات); /* Clear the output struct. */
        أرجع خطأ;            /* no file access */
    }

    U_CFUNC فراغ uprv_unmapFile(UData_يادگيري *p_معلومات) {
        /* nothing to do */
    }
#وإلاإذا MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_يادگيري *p_معلومات,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ثابت حرف *رستو,      /* File path to be opened/mapped.                     */
         UError_ڪوڊ *حالت     /* Error status, used to report out-of-memory errors. */
         )
    {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        HANDLE map = خالي_اشارو;
        HANDLE file = INVALID_HANDLE_قدر;
        DWORD file_ڊيگهائي = 0;

        UData_يادگيري_init(p_معلومات); /* Clear the output struct.        */

        /* open the input file */
#إذا U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(رستو, GENERIC_READ, FILE_SHARE_READ, خالي_اشارو,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_عام|FILE_FLAG_RANDOM_ACCESS, خالي_اشارو);
#وإلا
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_رستو[MAX_رستو];
        int32_t رستو_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_رستو), static_cast<int32_t>(UPRV_LENGTHOF(utf16_رستو)), &رستو_Utf16_Len, رستو, -1, حالت);

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        إذا (*حالت == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *حالت = U_بفر_حد_کان_مٿي_ERROR;
            أرجع خطأ;
        }

        file = CreateFileW(utf16_رستو, GENERIC_READ, FILE_SHARE_READ, خالي_اشارو,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_عام | FILE_FLAG_RANDOM_ACCESS, خالي_اشارو);
#جيڪڏهن_پورو
        إذا (file == INVALID_HANDLE_قدر) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخري_Error()) == E_OUTOFMEMORY) {
                *حالت = U_يادگيري_ورهاست_ERROR;
            }
            أرجع خطأ;
        }

        file_ڊيگهائي = Get_File_سائيز(file, خالي_اشارو);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, خالي_اشارو, صفحو_READONLY, 0, 0, خالي_اشارو);

        CloseHandle(file);
        إذا (map == خالي_اشارو) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخري_Error()) == E_OUTOFMEMORY) {
                *حالت = U_يادگيري_ورهاست_ERROR;
            }
            أرجع خطأ;
        }

        /* map a view of the file into our address space */
        p_معلومات->p_سرو = reinterpret_cast<ثابت معلومات_سرو *>(Map_View_جو_File(map, FILE_MAP_READ, 0, 0, 0));
        إذا (p_معلومات->p_سرو == خالي_اشارو) {
            CloseHandle(map);
            أرجع خطأ;
        }
        p_معلومات->map = map;
        p_معلومات->length = file_ڊيگهائي;

        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_يادگيري *p_معلومات) {
        إذا (p_معلومات != خالي_اشارو && p_معلومات->map != خالي_اشارو) {
            Unmap_View_جو_File(p_معلومات->p_سرو);
            CloseHandle(p_معلومات->map);
            p_معلومات->p_سرو = خالي_اشارو;
            p_معلومات->map = خالي_اشارو;
        }
    }



#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_يادگيري *p_معلومات, ثابت حرف *رستو, UError_ڪوڊ *حالت) {
        صحيح fd;
        صحيح length;
        بنية stat mystat;
        فراغ *معلومات;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_يادگيري_init(p_معلومات); /* Clear the output struct.        */

        /* open the file */
        fd=open(رستو, O_RDONLY);
        إذا(fd==-1) {
            أرجع خطأ;
        }

        /* determine the length of the file */
        إذا(stat(رستو, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            أرجع خطأ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#إذا U_PLATFORM != U_PF_HPUX
        معلومات=mmap(خالي_اشارو, length, PROT_READ, MAP_SHARED, fd, 0);
#وإلا
        معلومات=mmap(خالي_اشارو, length, PROT_READ, MAP_ذاتي, fd, 0);
#جيڪڏهن_پورو
        close(fd); /* no longer needed */
        إذا(معلومات==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            أرجع خطأ;
        }

        p_معلومات->map = (حرف *)معلومات + length;
        p_معلومات->p_سرو=(ثابت معلومات_سرو *)معلومات;
        p_معلومات->mapAddr = معلومات;
        p_معلومات->length = length;
#إذا U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(معلومات, length, POSIX_MADV_RANDOM);
#   endif
#جيڪڏهن_پورو
        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_يادگيري *p_معلومات) {
        إذا(p_معلومات!=خالي_اشارو && p_معلومات->map!=خالي_اشارو) {
            size_t معلومات_Len = (حرف *)p_معلومات->map - (حرف *)p_معلومات->mapAddr;
            إذا(munmap(p_معلومات->mapAddr, معلومات_Len)==-1) {
            }
            p_معلومات->p_سرو=خالي_اشارو;
            p_معلومات->map=خالي_اشارو;
            p_معلومات->mapAddr=خالي_اشارو;
        }
    }



#وإلاإذا MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    ساكن int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        أرجع size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_يادگيري *p_معلومات, ثابت حرف *رستو, UError_ڪوڊ *حالت) {
        FILE *file;
        int32_t file_ڊيگهائي;
        فراغ *p;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_يادگيري_init(p_معلومات); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(رستو, "rb");
        إذا(file==خالي_اشارو) {
            أرجع خطأ;
        }

        /* get the file length */
        file_ڊيگهائي=umap_fsize(file);
        إذا(ferror(file) || file_ڊيگهائي<=20) {
            fclose(file);
            أرجع خطأ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_ڊيگهائي);
        إذا(p==خالي_اشارو) {
            fclose(file);
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }

        /* read the file */
        إذا(file_ڊيگهائي!=fread(p, 1, file_ڊيگهائي, file)) {
            uprv_free(p);
            fclose(file);
            أرجع خطأ;
        }

        fclose(file);
        p_معلومات->map=p;
        p_معلومات->p_سرو=(ثابت معلومات_سرو *)p;
        p_معلومات->mapAddr=p;
        p_معلومات->length = file_ڊيگهائي;
        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_يادگيري *p_معلومات) {
        إذا(p_معلومات!=خالي_اشارو && p_معلومات->map!=خالي_اشارو) {
            uprv_free(p_معلومات->map);
            p_معلومات->map     = خالي_اشارو;
            p_معلومات->mapAddr = خالي_اشارو;
            p_معلومات->p_سرو = خالي_اشارو;
        }
    }
#وإلا
#   error MAP_IMPLEMENTATION is سيٽ incorrectly
#جيڪڏهن_پورو
