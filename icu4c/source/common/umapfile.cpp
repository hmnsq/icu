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
#که_نه_تعریف_شوی WIN32_LEAN_AND_منځنۍ
#   define WIN32_LEAN_AND_منځنۍ
#د_که_پای
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
#       إذا !(WINAPI_FAMILY_برخه_کول(WINAPI_برخه_کول_DESKTOP | WINAPI_برخه_کول_SYSTEM))
#           pragma ټېل_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_برخه_کول_DESKTOP
#           define WINAPI_برخه_کول_DESKTOP 1
#           define بدل_شوی_WINAPI_برخه_کول_DESKTOP_ارزښت
#       endif
#   endif

#   include <windows.h>

#   إذا U_PLATFORM_HAS_WINUWP_API == 1 && defined(بدل_شوی_WINAPI_برخه_کول_DESKTOP_ارزښت)
#       pragma راوباسه_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

تعريفنوع HANDLE حافظه_Map;

#   define IS_MAP(map) ((map)!=تش_اشاره)

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

#   define IS_MAP(map) ((map)!=تش_اشاره)
#د_که_پای

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#إذا MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_حافظه *p_معلومات, ثابت حرف *لار, UError_کوډ *حالت) {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        UData_حافظه_init(p_معلومات); /* Clear the output struct. */
        أرجع خطأ;            /* no file access */
    }

    U_CFUNC فراغ uprv_unmapFile(UData_حافظه *p_معلومات) {
        /* nothing to do */
    }
#وإلاإذا MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_حافظه *p_معلومات,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ثابت حرف *لار,      /* File path to be opened/mapped.                     */
         UError_کوډ *حالت     /* Error status, used to report out-of-memory errors. */
         )
    {
        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        HANDLE map = تش_اشاره;
        HANDLE file = INVALID_HANDLE_ارزښت;
        DWORD file_اوږدتیا = 0;

        UData_حافظه_init(p_معلومات); /* Clear the output struct.        */

        /* open the input file */
#إذا U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(لار, GENERIC_READ, FILE_SHARE_READ, تش_اشاره,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_نورمال|FILE_FLAG_RANDOM_ACCESS, تش_اشاره);
#وإلا
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_لار[MAX_لار];
        int32_t لار_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_لار), static_cast<int32_t>(UPRV_LENGTHOF(utf16_لار)), &لار_Utf16_Len, لار, -1, حالت);

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }
        إذا (*حالت == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *حالت = U_بفر_ډکېدنه_ERROR;
            أرجع خطأ;
        }

        file = CreateFileW(utf16_لار, GENERIC_READ, FILE_SHARE_READ, تش_اشاره,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_نورمال | FILE_FLAG_RANDOM_ACCESS, تش_اشاره);
#د_که_پای
        إذا (file == INVALID_HANDLE_ارزښت) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_وروستی_Error()) == E_OUTOFMEMORY) {
                *حالت = U_حافظه_ویش_ERROR;
            }
            أرجع خطأ;
        }

        file_اوږدتیا = Get_File_اندازه(file, تش_اشاره);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, تش_اشاره, پاڼه_READONLY, 0, 0, تش_اشاره);

        CloseHandle(file);
        إذا (map == تش_اشاره) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            إذا (HRESULT_FROM_WIN32(Get_وروستی_Error()) == E_OUTOFMEMORY) {
                *حالت = U_حافظه_ویش_ERROR;
            }
            أرجع خطأ;
        }

        /* map a view of the file into our address space */
        p_معلومات->p_سرلیک = reinterpret_cast<ثابت معلومات_سرلیک *>(Map_View_د_File(map, FILE_MAP_READ, 0, 0, 0));
        إذا (p_معلومات->p_سرلیک == تش_اشاره) {
            CloseHandle(map);
            أرجع خطأ;
        }
        p_معلومات->map = map;
        p_معلومات->length = file_اوږدتیا;

        أرجع ریښتیا;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_معلومات) {
        إذا (p_معلومات != تش_اشاره && p_معلومات->map != تش_اشاره) {
            Unmap_View_د_File(p_معلومات->p_سرلیک);
            CloseHandle(p_معلومات->map);
            p_معلومات->p_سرلیک = تش_اشاره;
            p_معلومات->map = تش_اشاره;
        }
    }



#وإلاإذا MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_حافظه *p_معلومات, ثابت حرف *لار, UError_کوډ *حالت) {
        صحيح fd;
        صحيح length;
        بنية stat mystat;
        فراغ *معلومات;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_حافظه_init(p_معلومات); /* Clear the output struct.        */

        /* open the file */
        fd=open(لار, O_RDONLY);
        إذا(fd==-1) {
            أرجع خطأ;
        }

        /* determine the length of the file */
        إذا(stat(لار, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            أرجع خطأ;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#إذا U_PLATFORM != U_PF_HPUX
        معلومات=mmap(تش_اشاره, length, PROT_READ, MAP_SHARED, fd, 0);
#وإلا
        معلومات=mmap(تش_اشاره, length, PROT_READ, MAP_شخصي, fd, 0);
#د_که_پای
        close(fd); /* no longer needed */
        إذا(معلومات==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            أرجع خطأ;
        }

        p_معلومات->map = (حرف *)معلومات + length;
        p_معلومات->p_سرلیک=(ثابت معلومات_سرلیک *)معلومات;
        p_معلومات->mapAddr = معلومات;
        p_معلومات->length = length;
#إذا U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(معلومات, length, POSIX_MADV_RANDOM);
#   endif
#د_که_پای
        أرجع ریښتیا;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_معلومات) {
        إذا(p_معلومات!=تش_اشاره && p_معلومات->map!=تش_اشاره) {
            size_t معلومات_Len = (حرف *)p_معلومات->map - (حرف *)p_معلومات->mapAddr;
            إذا(munmap(p_معلومات->mapAddr, معلومات_Len)==-1) {
            }
            p_معلومات->p_سرلیک=تش_اشاره;
            p_معلومات->map=تش_اشاره;
            p_معلومات->mapAddr=تش_اشاره;
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
    uprv_mapFile(UData_حافظه *p_معلومات, ثابت حرف *لار, UError_کوډ *حالت) {
        FILE *file;
        int32_t file_اوږدتیا;
        فراغ *p;

        إذا (U_FAILURE(*حالت)) {
            أرجع خطأ;
        }

        UData_حافظه_init(p_معلومات); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(لار, "rb");
        إذا(file==تش_اشاره) {
            أرجع خطأ;
        }

        /* get the file length */
        file_اوږدتیا=umap_fsize(file);
        إذا(ferror(file) || file_اوږدتیا<=20) {
            fclose(file);
            أرجع خطأ;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_اوږدتیا);
        إذا(p==تش_اشاره) {
            fclose(file);
            *حالت = U_حافظه_ویش_ERROR;
            أرجع خطأ;
        }

        /* read the file */
        إذا(file_اوږدتیا!=fread(p, 1, file_اوږدتیا, file)) {
            uprv_free(p);
            fclose(file);
            أرجع خطأ;
        }

        fclose(file);
        p_معلومات->map=p;
        p_معلومات->p_سرلیک=(ثابت معلومات_سرلیک *)p;
        p_معلومات->mapAddr=p;
        p_معلومات->length = file_اوږدتیا;
        أرجع ریښتیا;
    }

    U_CFUNC فراغ
    uprv_unmapFile(UData_حافظه *p_معلومات) {
        إذا(p_معلومات!=تش_اشاره && p_معلومات->map!=تش_اشاره) {
            uprv_free(p_معلومات->map);
            p_معلومات->map     = تش_اشاره;
            p_معلومات->mapAddr = تش_اشاره;
            p_معلومات->p_سرلیک = تش_اشاره;
        }
    }
#وإلا
#   error MAP_IMPLEMENTATION is سېټ incorrectly
#د_که_پای
