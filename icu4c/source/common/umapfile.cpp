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
#جے_بنا_تعریف WIN32_LEAN_AND_اوسط
#   define WIN32_LEAN_AND_اوسط
#جے_انت
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
#       إذا !(WINAPI_FAMILY_حصے_کرو(WINAPI_حصے_کرو_DESKTOP | WINAPI_حصے_کرو_SYSTEM))
#           pragma ڈالو_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_حصے_کرو_DESKTOP
#           define WINAPI_حصے_کرو_DESKTOP 1
#           define بدلیا_WINAPI_حصے_کرو_DESKTOP_قدر
#       endif
#   endif

#   include <windows.h>

#   إذا U_PLATFORM_HAS_WINUWP_API == 1 && defined(بدلیا_WINAPI_حصے_کرو_DESKTOP_قدر)
#       pragma نکالو_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

تعريفنوع HANDLE حافظہ_Map;

#   define IS_MAP(map) ((map)!=خالی_سوچک)

#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    تعريفنوع size_t حافظہ_Map;

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

    تعريفنوع فراغ *حافظہ_Map;

#   define IS_MAP(map) ((map)!=خالی_سوچک)
#جے_انت

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#إذا MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_حافظہ *p_انکڑے, ثابت حرف *رستہ, UError_کوڈ *حالت) {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        UData_حافظہ_init(p_انکڑے); /* Clear the output struct. */
        أرجع خطأ;            /* no file access */
    }

    U_CFUNC فراغ uprv_unmapFile(UData_حافظہ *p_انکڑے) {
        /* nothing to do */
    }
#وإلاإذا MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_حافظہ *p_انکڑے,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ثابت حرف *رستہ,      /* File path to be opened/mapped.                     */
         UError_کوڈ *حالت     /* Error status, used to report out-of-memory errors. */
         )
    {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        HANDLE map = خالی_سوچک;
        HANDLE file = INVALID_HANDLE_قدر;
        DWORD file_لمائی = 0;

        UData_حافظہ_init(p_انکڑے); /* Clear the output struct.        */

        /* open the input file */
#إذا U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(رستہ, GENERIC_READ, FILE_SHARE_READ, خالی_سوچک,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_عام|FILE_FLAG_RANDOM_ACCESS, خالی_سوچک);
#وإلا
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_رستہ[MAX_رستہ];
        int32_t رستہ_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_رستہ), static_cast<int32_t>(UPRV_LENGTHOF(utf16_رستہ)), &رستہ_Utf16_Len, رستہ, -1, حالت);

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        إذا (*حالت == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *حالت = U_بفر_حد_توں_ودھ_ERROR;
            أرجع خطأ;
        }

        file = CreateFileW(utf16_رستہ, GENERIC_READ, FILE_SHARE_READ, خالی_سوچک,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_عام | FILE_FLAG_RANDOM_ACCESS, خالی_سوچک);
#جے_انت
        إذا (file == INVALID_HANDLE_قدر) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخری_Error()) == E_OUTOFMEMORY) {
                *حالت = U_حافظہ_ونڈ_ERROR;
            }
            أرجع خطأ;
        }

        file_لمائی = Get_File_ناپ(file, خالی_سوچک);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, خالی_سوچک, ورقہ_READONLY, 0, 0, خالی_سوچک);

        CloseHandle(file);
        إذا (map == خالی_سوچک) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخری_Error()) == E_OUTOFMEMORY) {
                *حالت = U_حافظہ_ونڈ_ERROR;
            }
            أرجع خطأ;
        }

        /* map a view of the file into our address space */
        p_انکڑے->p_سرخی = reinterpret_cast<ثابت انکڑے_سرخی *>(Map_View_دا_File(map, FILE_MAP_READ, 0, 0, 0));
        إذا (p_انکڑے->p_سرخی == خالی_سوچک) {
            CloseHandle(map);
            أرجع خطأ;
        }
        p_انکڑے->map = map;
        p_انکڑے->length = file_لمائی;

        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظہ *p_انکڑے) {
        إذا (p_انکڑے != خالی_سوچک && p_انکڑے->map != خالی_سوچک) {
            Unmap_View_دا_File(p_انکڑے->p_سرخی);
            CloseHandle(p_انکڑے->map);
            p_انکڑے->p_سرخی = خالی_سوچک;
            p_انکڑے->map = خالی_سوچک;
        }
    }



#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_حافظہ *p_انکڑے, ثابت حرف *رستہ, UError_کوڈ *حالت) {
        صحيح fd;
        صحيح length;
        بنية stat mystat;
        فراغ *انکڑے;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_حافظہ_init(p_انکڑے); /* Clear the output struct.        */

        /* open the file */
        fd=open(رستہ, O_RDONLY);
        إذا(fd==-1) {
            أرجع خطأ;
        }

        /* determine the length of the file */
        إذا(stat(رستہ, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            أرجع خطأ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#إذا U_PLATFORM != U_PF_HPUX
        انکڑے=mmap(خالی_سوچک, length, PROT_READ, MAP_SHARED, fd, 0);
#وإلا
        انکڑے=mmap(خالی_سوچک, length, PROT_READ, MAP_نجی, fd, 0);
#جے_انت
        close(fd); /* no longer needed */
        إذا(انکڑے==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            أرجع خطأ;
        }

        p_انکڑے->map = (حرف *)انکڑے + length;
        p_انکڑے->p_سرخی=(ثابت انکڑے_سرخی *)انکڑے;
        p_انکڑے->mapAddr = انکڑے;
        p_انکڑے->length = length;
#إذا U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(انکڑے, length, POSIX_MADV_RANDOM);
#   endif
#جے_انت
        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظہ *p_انکڑے) {
        إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->map!=خالی_سوچک) {
            size_t انکڑے_Len = (حرف *)p_انکڑے->map - (حرف *)p_انکڑے->mapAddr;
            إذا(munmap(p_انکڑے->mapAddr, انکڑے_Len)==-1) {
            }
            p_انکڑے->p_سرخی=خالی_سوچک;
            p_انکڑے->map=خالی_سوچک;
            p_انکڑے->mapAddr=خالی_سوچک;
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
    uprv_mapFile(UData_حافظہ *p_انکڑے, ثابت حرف *رستہ, UError_کوڈ *حالت) {
        FILE *file;
        int32_t file_لمائی;
        فراغ *p;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_حافظہ_init(p_انکڑے); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(رستہ, "rb");
        إذا(file==خالی_سوچک) {
            أرجع خطأ;
        }

        /* get the file length */
        file_لمائی=umap_fsize(file);
        إذا(ferror(file) || file_لمائی<=20) {
            fclose(file);
            أرجع خطأ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_لمائی);
        إذا(p==خالی_سوچک) {
            fclose(file);
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خطأ;
        }

        /* read the file */
        إذا(file_لمائی!=fread(p, 1, file_لمائی, file)) {
            uprv_free(p);
            fclose(file);
            أرجع خطأ;
        }

        fclose(file);
        p_انکڑے->map=p;
        p_انکڑے->p_سرخی=(ثابت انکڑے_سرخی *)p;
        p_انکڑے->mapAddr=p;
        p_انکڑے->length = file_لمائی;
        أرجع سچ;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظہ *p_انکڑے) {
        إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->map!=خالی_سوچک) {
            uprv_free(p_انکڑے->map);
            p_انکڑے->map     = خالی_سوچک;
            p_انکڑے->mapAddr = خالی_سوچک;
            p_انکڑے->p_سرخی = خالی_سوچک;
        }
    }
#وإلا
#   error MAP_IMPLEMENTATION is سیٹ incorrectly
#جے_انت
