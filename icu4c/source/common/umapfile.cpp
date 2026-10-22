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
#اگر_تعریف_نشده WIN32_LEAN_AND_میانگین
#   define WIN32_LEAN_AND_میانگین
#پایان_اگر
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
#       إذا !(WINAPI_FAMILY_افراز(WINAPI_افراز_DESKTOP | WINAPI_افراز_SYSTEM))
#           pragma افزودن_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_افراز_DESKTOP
#           define WINAPI_افراز_DESKTOP 1
#           define تغییریافته_WINAPI_افراز_DESKTOP_مقدار
#       endif
#   endif

#   include <windows.h>

#   إذا U_PLATFORM_HAS_WINUWP_API == 1 && defined(تغییریافته_WINAPI_افراز_DESKTOP_مقدار)
#       pragma برداشت_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

تعريفنوع HANDLE حافظه_Map;

#   define IS_MAP(map) ((map)!=اشاره_تهی)

#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    تعريفنوع size_t حافظه_Map;

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

    تعريفنوع فراغ *حافظه_Map;

#   define IS_MAP(map) ((map)!=اشاره_تهی)
#پایان_اگر

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#إذا MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_حافظه *p_داده, ثابت حرف *مسیر, UError_کد *وضعیت) {
        إذا (U_FAILURE(*وضعیت)) {
            أرجع خطأ;
        }
        UData_حافظه_init(p_داده); /* Clear the output struct. */
        أرجع خطأ;            /* no file access */
    }

    U_CFUNC فراغ uprv_unmapFile(UData_حافظه *p_داده) {
        /* nothing to do */
    }
#وإلاإذا MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_حافظه *p_داده,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ثابت حرف *مسیر,      /* File path to be opened/mapped.                     */
         UError_کد *وضعیت     /* Error status, used to report out-of-memory errors. */
         )
    {
        إذا (U_FAILURE(*وضعیت)) {
            أرجع خطأ;
        }

        HANDLE map = اشاره_تهی;
        HANDLE file = INVALID_HANDLE_مقدار;
        DWORD file_درازی = 0;

        UData_حافظه_init(p_داده); /* Clear the output struct.        */

        /* open the input file */
#إذا U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(مسیر, GENERIC_READ, FILE_SHARE_READ, اشاره_تهی,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_نرمال|FILE_FLAG_RANDOM_ACCESS, اشاره_تهی);
#وإلا
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_مسیر[MAX_مسیر];
        int32_t مسیر_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_مسیر), static_cast<int32_t>(UPRV_LENGTHOF(utf16_مسیر)), &مسیر_Utf16_Len, مسیر, -1, وضعیت);

        إذا (U_FAILURE(*وضعیت)) {
            أرجع خطأ;
        }
        إذا (*وضعیت == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *وضعیت = U_میانگیر_سرریز_ERROR;
            أرجع خطأ;
        }

        file = CreateFileW(utf16_مسیر, GENERIC_READ, FILE_SHARE_READ, اشاره_تهی,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_نرمال | FILE_FLAG_RANDOM_ACCESS, اشاره_تهی);
#پایان_اگر
        إذا (file == INVALID_HANDLE_مقدار) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخرین_Error()) == E_OUTOFMEMORY) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
            }
            أرجع خطأ;
        }

        file_درازی = Get_File_اندازه(file, اشاره_تهی);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, اشاره_تهی, صفحه_READONLY, 0, 0, اشاره_تهی);

        CloseHandle(file);
        إذا (map == اشاره_تهی) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_آخرین_Error()) == E_OUTOFMEMORY) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
            }
            أرجع خطأ;
        }

        /* map a view of the file into our address space */
        p_داده->p_سرآیند = reinterpret_cast<ثابت داده_سرآیند *>(Map_View_از_File(map, FILE_MAP_READ, 0, 0, 0));
        إذا (p_داده->p_سرآیند == اشاره_تهی) {
            CloseHandle(map);
            أرجع خطأ;
        }
        p_داده->map = map;
        p_داده->length = file_درازی;

        أرجع درست;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_داده) {
        إذا (p_داده != اشاره_تهی && p_داده->map != اشاره_تهی) {
            Unmap_View_از_File(p_داده->p_سرآیند);
            CloseHandle(p_داده->map);
            p_داده->p_سرآیند = اشاره_تهی;
            p_داده->map = اشاره_تهی;
        }
    }



#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_حافظه *p_داده, ثابت حرف *مسیر, UError_کد *وضعیت) {
        صحيح fd;
        صحيح length;
        بنية stat mystat;
        فراغ *داده;

        إذا (U_FAILURE(*وضعیت)) {
            أرجع خطأ;
        }

        UData_حافظه_init(p_داده); /* Clear the output struct.        */

        /* open the file */
        fd=open(مسیر, O_RDONLY);
        إذا(fd==-1) {
            أرجع خطأ;
        }

        /* determine the length of the file */
        إذا(stat(مسیر, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            أرجع خطأ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#إذا U_PLATFORM != U_PF_HPUX
        داده=mmap(اشاره_تهی, length, PROT_READ, MAP_SHARED, fd, 0);
#وإلا
        داده=mmap(اشاره_تهی, length, PROT_READ, MAP_خصوصی, fd, 0);
#پایان_اگر
        close(fd); /* no longer needed */
        إذا(داده==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            أرجع خطأ;
        }

        p_داده->map = (حرف *)داده + length;
        p_داده->p_سرآیند=(ثابت داده_سرآیند *)داده;
        p_داده->mapAddr = داده;
        p_داده->length = length;
#إذا U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(داده, length, POSIX_MADV_RANDOM);
#   endif
#پایان_اگر
        أرجع درست;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_داده) {
        إذا(p_داده!=اشاره_تهی && p_داده->map!=اشاره_تهی) {
            size_t داده_Len = (حرف *)p_داده->map - (حرف *)p_داده->mapAddr;
            إذا(munmap(p_داده->mapAddr, داده_Len)==-1) {
            }
            p_داده->p_سرآیند=اشاره_تهی;
            p_داده->map=اشاره_تهی;
            p_داده->mapAddr=اشاره_تهی;
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
    uprv_mapFile(UData_حافظه *p_داده, ثابت حرف *مسیر, UError_کد *وضعیت) {
        FILE *file;
        int32_t file_درازی;
        فراغ *p;

        إذا (U_FAILURE(*وضعیت)) {
            أرجع خطأ;
        }

        UData_حافظه_init(p_داده); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(مسیر, "rb");
        إذا(file==اشاره_تهی) {
            أرجع خطأ;
        }

        /* get the file length */
        file_درازی=umap_fsize(file);
        إذا(ferror(file) || file_درازی<=20) {
            fclose(file);
            أرجع خطأ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_درازی);
        إذا(p==اشاره_تهی) {
            fclose(file);
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }

        /* read the file */
        إذا(file_درازی!=fread(p, 1, file_درازی, file)) {
            uprv_free(p);
            fclose(file);
            أرجع خطأ;
        }

        fclose(file);
        p_داده->map=p;
        p_داده->p_سرآیند=(ثابت داده_سرآیند *)p;
        p_داده->mapAddr=p;
        p_داده->length = file_درازی;
        أرجع درست;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_داده) {
        إذا(p_داده!=اشاره_تهی && p_داده->map!=اشاره_تهی) {
            uprv_free(p_داده->map);
            p_داده->map     = اشاره_تهی;
            p_داده->mapAddr = اشاره_تهی;
            p_داده->p_سرآیند = اشاره_تهی;
        }
    }
#وإلا
#   error MAP_IMPLEMENTATION is مجموعه incorrectly
#پایان_اگر
