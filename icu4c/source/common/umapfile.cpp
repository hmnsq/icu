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
#شامل "uposixdefs.h"

#شامل "unicode/putil.h"
#شامل "unicode/ustring.h"
#شامل "udatamem.h"
#شامل "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#اگر MAP_IMPLEMENTATION==MAP_WIN32
#اگر_غیر_تعریف_شدہ WIN32_LEAN_AND_اوسط
#   define WIN32_LEAN_AND_اوسط
#اگر_ختم
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   اگر U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       اگر !(WINAPI_FAMILY_تقسیم(WINAPI_تقسیم_DESKTOP | WINAPI_تقسیم_SYSTEM))
#           pragma ڈالو_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_تقسیم_DESKTOP
#           define WINAPI_تقسیم_DESKTOP 1
#           define بدلا_WINAPI_تقسیم_DESKTOP_قدر
#       endif
#   endif

#   include <windows.h>

#   اگر U_PLATFORM_HAS_WINUWP_API == 1 && defined(بدلا_WINAPI_تقسیم_DESKTOP_قدر)
#       pragma نکالو_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

قسمتعریف HANDLE حافظہ_Map;

#   define IS_MAP(map) ((map)!=خالی_اشاریہ)

#وگرنہ_اگر MAP_IMPLEMENTATION==MAP_POSIX
    قسمتعریف size_t حافظہ_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((خالی*)-1)
#   endif
#وگرنہ_اگر MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    قسمتعریف خالی *حافظہ_Map;

#   define IS_MAP(map) ((map)!=خالی_اشاریہ)
#اگر_ختم

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#اگر MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_حافظہ *p_ڈیٹا, ثابت حرف *راستہ, UError_رمز *حالت_2) {
        اگر (U_FAILURE(*حالت_2)) {
            واپس جھوٹ;
        }
        UData_حافظہ_init(p_ڈیٹا); /* Clear the output struct. */
        واپس جھوٹ;            /* no file access */
    }

    U_CFUNC خالی uprv_unmapFile(UData_حافظہ *p_ڈیٹا) {
        /* nothing to do */
    }
#وگرنہ_اگر MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_حافظہ *p_ڈیٹا,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ثابت حرف *راستہ,      /* File path to be opened/mapped.                     */
         UError_رمز *حالت_2     /* Error status, used to report out-of-memory errors. */
         )
    {
        اگر (U_FAILURE(*حالت_2)) {
            واپس جھوٹ;
        }

        HANDLE map = خالی_اشاریہ;
        HANDLE file = INVALID_HANDLE_قدر;
        DWORD file_لمبائی = 0;

        UData_حافظہ_init(p_ڈیٹا); /* Clear the output struct.        */

        /* open the input file */
#اگر U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(راستہ, GENERIC_READ, FILE_SHARE_READ, خالی_اشاریہ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_معمول|FILE_FLAG_RANDOM_ACCESS, خالی_اشاریہ);
#ورنہ
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_راستہ[MAX_راستہ];
        int32_t راستہ_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_راستہ), static_cast<int32_t>(UPRV_LENGTHOF(utf16_راستہ)), &راستہ_Utf16_Len, راستہ, -1, حالت_2);

        اگر (U_FAILURE(*حالت_2)) {
            واپس جھوٹ;
        }
        اگر (*حالت_2 == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
            واپس جھوٹ;
        }

        file = CreateFileW(utf16_راستہ, GENERIC_READ, FILE_SHARE_READ, خالی_اشاریہ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_معمول | FILE_FLAG_RANDOM_ACCESS, خالی_اشاریہ);
#اگر_ختم
        اگر (file == INVALID_HANDLE_قدر) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            اگر (HRESULT_FROM_WIN32(Get_آخری_Error()) == E_OUTOFMEMORY) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
            }
            واپس جھوٹ;
        }

        file_لمبائی = Get_File_سائز(file, خالی_اشاریہ);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, خالی_اشاریہ, صفحہ_READONLY, 0, 0, خالی_اشاریہ);

        CloseHandle(file);
        اگر (map == خالی_اشاریہ) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            اگر (HRESULT_FROM_WIN32(Get_آخری_Error()) == E_OUTOFMEMORY) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
            }
            واپس جھوٹ;
        }

        /* map a view of the file into our address space */
        p_ڈیٹا->p_سرخی = reinterpret_cast<ثابت ڈیٹا_سرخی *>(Map_View_کا_File(map, FILE_MAP_READ, 0, 0, 0));
        اگر (p_ڈیٹا->p_سرخی == خالی_اشاریہ) {
            CloseHandle(map);
            واپس جھوٹ;
        }
        p_ڈیٹا->map = map;
        p_ڈیٹا->length = file_لمبائی;

        واپس سچ;
    }

    U_CFUNC خالی
    uprv_unmapFile(UData_حافظہ *p_ڈیٹا) {
        اگر (p_ڈیٹا != خالی_اشاریہ && p_ڈیٹا->map != خالی_اشاریہ) {
            Unmap_View_کا_File(p_ڈیٹا->p_سرخی);
            CloseHandle(p_ڈیٹا->map);
            p_ڈیٹا->p_سرخی = خالی_اشاریہ;
            p_ڈیٹا->map = خالی_اشاریہ;
        }
    }



#وگرنہ_اگر MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_حافظہ *p_ڈیٹا, ثابت حرف *راستہ, UError_رمز *حالت_2) {
        صحیح fd;
        صحیح length;
        ساخت stat mystat;
        خالی *ڈیٹا;

        اگر (U_FAILURE(*حالت_2)) {
            واپس جھوٹ;
        }

        UData_حافظہ_init(p_ڈیٹا); /* Clear the output struct.        */

        /* open the file */
        fd=open(راستہ, O_RDONLY);
        اگر(fd==-1) {
            واپس جھوٹ;
        }

        /* determine the length of the file */
        اگر(stat(راستہ, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            واپس جھوٹ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#اگر U_PLATFORM != U_PF_HPUX
        ڈیٹا=mmap(خالی_اشاریہ, length, PROT_READ, MAP_SHARED, fd, 0);
#ورنہ
        ڈیٹا=mmap(خالی_اشاریہ, length, PROT_READ, MAP_نجی, fd, 0);
#اگر_ختم
        close(fd); /* no longer needed */
        اگر(ڈیٹا==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            واپس جھوٹ;
        }

        p_ڈیٹا->map = (حرف *)ڈیٹا + length;
        p_ڈیٹا->p_سرخی=(ثابت ڈیٹا_سرخی *)ڈیٹا;
        p_ڈیٹا->mapAddr = ڈیٹا;
        p_ڈیٹا->length = length;
#اگر U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(ڈیٹا, length, POSIX_MADV_RANDOM);
#   endif
#اگر_ختم
        واپس سچ;
    }

    U_CFUNC خالی
    uprv_unmapFile(UData_حافظہ *p_ڈیٹا) {
        اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->map!=خالی_اشاریہ) {
            size_t ڈیٹا_Len = (حرف *)p_ڈیٹا->map - (حرف *)p_ڈیٹا->mapAddr;
            اگر(munmap(p_ڈیٹا->mapAddr, ڈیٹا_Len)==-1) {
            }
            p_ڈیٹا->p_سرخی=خالی_اشاریہ;
            p_ڈیٹا->map=خالی_اشاریہ;
            p_ڈیٹا->mapAddr=خالی_اشاریہ;
        }
    }



#وگرنہ_اگر MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    ساکن int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        واپس size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_حافظہ *p_ڈیٹا, ثابت حرف *راستہ, UError_رمز *حالت_2) {
        FILE *file;
        int32_t file_لمبائی;
        خالی *p;

        اگر (U_FAILURE(*حالت_2)) {
            واپس جھوٹ;
        }

        UData_حافظہ_init(p_ڈیٹا); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(راستہ, "rb");
        اگر(file==خالی_اشاریہ) {
            واپس جھوٹ;
        }

        /* get the file length */
        file_لمبائی=umap_fsize(file);
        اگر(ferror(file) || file_لمبائی<=20) {
            fclose(file);
            واپس جھوٹ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_لمبائی);
        اگر(p==خالی_اشاریہ) {
            fclose(file);
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }

        /* read the file */
        اگر(file_لمبائی!=fread(p, 1, file_لمبائی, file)) {
            uprv_free(p);
            fclose(file);
            واپس جھوٹ;
        }

        fclose(file);
        p_ڈیٹا->map=p;
        p_ڈیٹا->p_سرخی=(ثابت ڈیٹا_سرخی *)p;
        p_ڈیٹا->mapAddr=p;
        p_ڈیٹا->length = file_لمبائی;
        واپس سچ;
    }

    U_CFUNC خالی
    uprv_unmapFile(UData_حافظہ *p_ڈیٹا) {
        اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->map!=خالی_اشاریہ) {
            uprv_free(p_ڈیٹا->map);
            p_ڈیٹا->map     = خالی_اشاریہ;
            p_ڈیٹا->mapAddr = خالی_اشاریہ;
            p_ڈیٹا->p_سرخی = خالی_اشاریہ;
        }
    }
#ورنہ
#   error MAP_IMPLEMENTATION is سیٹ incorrectly
#اگر_ختم
