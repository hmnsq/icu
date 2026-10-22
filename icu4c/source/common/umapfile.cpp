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
#అన్తర్భావ "uposixdefs.h"

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "udatamem.h"
#అన్తర్భావ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#ఒకవేళ MAP_IMPLEMENTATION==MAP_WIN32
#నిర్వచితం_కాకపోతే WIN32_LEAN_AND_సగటు
#   define WIN32_LEAN_AND_సగటు
#అయితే_ముగింపు
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       ఒకవేళ !(WINAPI_FAMILY_విభజన(WINAPI_విభజన_DESKTOP | WINAPI_విభజన_SYSTEM))
#           pragma నెట్టు_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_విభజన_DESKTOP
#           define WINAPI_విభజన_DESKTOP 1
#           define మారింది_WINAPI_విభజన_DESKTOP_విలువ
#       endif
#   endif

#   include <windows.h>

#   ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 1 && defined(మారింది_WINAPI_విభజన_DESKTOP_విలువ)
#       pragma తీయి_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

రకనిర్వచనం HANDLE జ్ఞాపకం_Map;

#   define IS_MAP(map) ((map)!=శూన్యసూచిక)

#లేదావేళ MAP_IMPLEMENTATION==MAP_POSIX
    రకనిర్వచనం size_t జ్ఞాపకం_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((శూన్య*)-1)
#   endif
#లేదావేళ MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    రకనిర్వచనం శూన్య *జ్ఞాపకం_Map;

#   define IS_MAP(map) ((map)!=శూన్యసూచిక)
#అయితే_ముగింపు

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#ఒకవేళ MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_జ్ఞాపకం *p_దత్తాంశం, స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి అబద్ధం;
        }
        UData_జ్ఞాపకం_init(p_దత్తాంశం); /* Clear the output struct. */
        తిరిగి అబద్ధం;            /* no file access */
    }

    U_CFUNC శూన్య uprv_unmapFile(UData_జ్ఞాపకం *p_దత్తాంశం) {
        /* nothing to do */
    }
#లేదావేళ MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_జ్ఞాపకం *p_దత్తాంశం,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         స్థిరరాశి అక్షరం *మార్గం,      /* File path to be opened/mapped.                     */
         UError_సంకేతం *స్థితి_2     /* Error status, used to report out-of-memory errors. */
         )
    {
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి అబద్ధం;
        }

        HANDLE map = శూన్యసూచిక;
        HANDLE file = INVALID_HANDLE_విలువ;
        DWORD file_నిడివి = 0;

        UData_జ్ఞాపకం_init(p_దత్తాంశం); /* Clear the output struct.        */

        /* open the input file */
#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(మార్గం, GENERIC_READ, FILE_SHARE_READ, శూన్యసూచిక,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_సాధారణ|FILE_FLAG_RANDOM_ACCESS, శూన్యసూచిక);
#లేకపోతే
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_మార్గం[MAX_మార్గం];
        int32_t మార్గం_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_మార్గం), static_cast<int32_t>(UPRV_LENGTHOF(utf16_మార్గం)), &మార్గం_Utf16_Len, మార్గం, -1, స్థితి_2);

        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి అబద్ధం;
        }
        ఒకవేళ (*స్థితి_2 == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి అబద్ధం;
        }

        file = CreateFileW(utf16_మార్గం, GENERIC_READ, FILE_SHARE_READ, శూన్యసూచిక,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_సాధారణ | FILE_FLAG_RANDOM_ACCESS, శూన్యసూచిక);
#అయితే_ముగింపు
        ఒకవేళ (file == INVALID_HANDLE_విలువ) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            ఒకవేళ (HRESULT_FROM_WIN32(Get_చివరి_Error()) == E_OUTOFMEMORY) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
            తిరిగి అబద్ధం;
        }

        file_నిడివి = Get_File_కొలత(file, శూన్యసూచిక);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, శూన్యసూచిక, పుట_READONLY, 0, 0, శూన్యసూచిక);

        CloseHandle(file);
        ఒకవేళ (map == శూన్యసూచిక) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            ఒకవేళ (HRESULT_FROM_WIN32(Get_చివరి_Error()) == E_OUTOFMEMORY) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
            తిరిగి అబద్ధం;
        }

        /* map a view of the file into our address space */
        p_దత్తాంశం->p_శీర్షిక = reinterpret_cast<స్థిరరాశి దత్తాంశం_శీర్షిక *>(Map_View_యొక్క_File(map, FILE_MAP_READ, 0, 0, 0));
        ఒకవేళ (p_దత్తాంశం->p_శీర్షిక == శూన్యసూచిక) {
            CloseHandle(map);
            తిరిగి అబద్ధం;
        }
        p_దత్తాంశం->map = map;
        p_దత్తాంశం->length = file_నిడివి;

        తిరిగి నిజం;
    }

    U_CFUNC శూన్య
    uprv_unmapFile(UData_జ్ఞాపకం *p_దత్తాంశం) {
        ఒకవేళ (p_దత్తాంశం != శూన్యసూచిక && p_దత్తాంశం->map != శూన్యసూచిక) {
            Unmap_View_యొక్క_File(p_దత్తాంశం->p_శీర్షిక);
            CloseHandle(p_దత్తాంశం->map);
            p_దత్తాంశం->p_శీర్షిక = శూన్యసూచిక;
            p_దత్తాంశం->map = శూన్యసూచిక;
        }
    }



#లేదావేళ MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_జ్ఞాపకం *p_దత్తాంశం, స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
        పూర్ణాంకం fd;
        పూర్ణాంకం length;
        నిర్మాణం stat mystat;
        శూన్య *దత్తాంశం;

        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి అబద్ధం;
        }

        UData_జ్ఞాపకం_init(p_దత్తాంశం); /* Clear the output struct.        */

        /* open the file */
        fd=open(మార్గం, O_RDONLY);
        ఒకవేళ(fd==-1) {
            తిరిగి అబద్ధం;
        }

        /* determine the length of the file */
        ఒకవేళ(stat(మార్గం, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            తిరిగి అబద్ధం;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#ఒకవేళ U_PLATFORM != U_PF_HPUX
        దత్తాంశం=mmap(శూన్యసూచిక, length, PROT_READ, MAP_SHARED, fd, 0);
#లేకపోతే
        దత్తాంశం=mmap(శూన్యసూచిక, length, PROT_READ, MAP_వ్యక్తిగత, fd, 0);
#అయితే_ముగింపు
        close(fd); /* no longer needed */
        ఒకవేళ(దత్తాంశం==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            తిరిగి అబద్ధం;
        }

        p_దత్తాంశం->map = (అక్షరం *)దత్తాంశం + length;
        p_దత్తాంశం->p_శీర్షిక=(స్థిరరాశి దత్తాంశం_శీర్షిక *)దత్తాంశం;
        p_దత్తాంశం->mapAddr = దత్తాంశం;
        p_దత్తాంశం->length = length;
#ఒకవేళ U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(దత్తాంశం, length, POSIX_MADV_RANDOM);
#   endif
#అయితే_ముగింపు
        తిరిగి నిజం;
    }

    U_CFUNC శూన్య
    uprv_unmapFile(UData_జ్ఞాపకం *p_దత్తాంశం) {
        ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->map!=శూన్యసూచిక) {
            size_t దత్తాంశం_Len = (అక్షరం *)p_దత్తాంశం->map - (అక్షరం *)p_దత్తాంశం->mapAddr;
            ఒకవేళ(munmap(p_దత్తాంశం->mapAddr, దత్తాంశం_Len)==-1) {
            }
            p_దత్తాంశం->p_శీర్షిక=శూన్యసూచిక;
            p_దత్తాంశం->map=శూన్యసూచిక;
            p_దత్తాంశం->mapAddr=శూన్యసూచిక;
        }
    }



#లేదావేళ MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    స్థిర int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        తిరిగి size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_జ్ఞాపకం *p_దత్తాంశం, స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
        FILE *file;
        int32_t file_నిడివి;
        శూన్య *p;

        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి అబద్ధం;
        }

        UData_జ్ఞాపకం_init(p_దత్తాంశం); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(మార్గం, "rb");
        ఒకవేళ(file==శూన్యసూచిక) {
            తిరిగి అబద్ధం;
        }

        /* get the file length */
        file_నిడివి=umap_fsize(file);
        ఒకవేళ(ferror(file) || file_నిడివి<=20) {
            fclose(file);
            తిరిగి అబద్ధం;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_నిడివి);
        ఒకవేళ(p==శూన్యసూచిక) {
            fclose(file);
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }

        /* read the file */
        ఒకవేళ(file_నిడివి!=fread(p, 1, file_నిడివి, file)) {
            uprv_free(p);
            fclose(file);
            తిరిగి అబద్ధం;
        }

        fclose(file);
        p_దత్తాంశం->map=p;
        p_దత్తాంశం->p_శీర్షిక=(స్థిరరాశి దత్తాంశం_శీర్షిక *)p;
        p_దత్తాంశం->mapAddr=p;
        p_దత్తాంశం->length = file_నిడివి;
        తిరిగి నిజం;
    }

    U_CFUNC శూన్య
    uprv_unmapFile(UData_జ్ఞాపకం *p_దత్తాంశం) {
        ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->map!=శూన్యసూచిక) {
            uprv_free(p_దత్తాంశం->map);
            p_దత్తాంశం->map     = శూన్యసూచిక;
            p_దత్తాంశం->mapAddr = శూన్యసూచిక;
            p_దత్తాంశం->p_శీర్షిక = శూన్యసూచిక;
        }
    }
#లేకపోతే
#   error MAP_IMPLEMENTATION is సమితి incorrectly
#అయితే_ముగింపు
