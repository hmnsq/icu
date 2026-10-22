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
#यदि_अपरिभाषितम् WIN32_LEAN_AND_माध्यम्
#   define WIN32_LEAN_AND_माध्यम्
#यदि_अन्तः
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
#           pragma प्रक्षिप_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_PARTITION_DESKTOP
#           define WINAPI_PARTITION_DESKTOP 1
#           define परिवर्तितम्_WINAPI_PARTITION_DESKTOP_मानम्
#       endif
#   endif

#   include <windows.h>

#   यदि U_PLATFORM_HAS_WINUWP_API == 1 && defined(परिवर्तितम्_WINAPI_PARTITION_DESKTOP_मानम्)
#       pragma निष्कासय_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

प्रकारनिर्देश HANDLE स्मृतिः_Map;

#   define IS_MAP(map) ((map)!=शून्यसूचकः)

#नोचेद्यदि MAP_IMPLEMENTATION==MAP_POSIX
    प्रकारनिर्देश size_t स्मृतिः_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((शून्य*)-1)
#   endif
#नोचेद्यदि MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    प्रकारनिर्देश शून्य *स्मृतिः_Map;

#   define IS_MAP(map) ((map)!=शून्यसूचकः)
#यदि_अन्तः

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#यदि MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_स्मृतिः *p_दत्त, ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम असत्य;
        }
        UData_स्मृतिः_init(p_दत्त); /* Clear the output struct. */
        प्रत्यागम असत्य;            /* no file access */
    }

    U_CFUNC शून्य uprv_unmapFile(UData_स्मृतिः *p_दत्त) {
        /* nothing to do */
    }
#नोचेद्यदि MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_स्मृतिः *p_दत्त,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ध्रुव वर्ण *मार्गः,      /* File path to be opened/mapped.                     */
         UError_संहिता *स्थितिः     /* Error status, used to report out-of-memory errors. */
         )
    {
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम असत्य;
        }

        HANDLE map = शून्यसूचकः;
        HANDLE file = INVALID_HANDLE_मानम्;
        DWORD file_दीर्घता = 0;

        UData_स्मृतिः_init(p_दत्त); /* Clear the output struct.        */

        /* open the input file */
#यदि U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(मार्गः, GENERIC_READ, FILE_SHARE_READ, शून्यसूचकः,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_प्रसामान्यम्|FILE_FLAG_RANDOM_ACCESS, शून्यसूचकः);
#अन्यथा
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_मार्गः[MAX_मार्गः];
        int32_t मार्गः_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_मार्गः), static_cast<int32_t>(UPRV_LENGTHOF(utf16_मार्गः)), &मार्गः_Utf16_Len, मार्गः, -1, स्थितिः);

        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम असत्य;
        }
        यदि (*स्थितिः == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम असत्य;
        }

        file = CreateFileW(utf16_मार्गः, GENERIC_READ, FILE_SHARE_READ, शून्यसूचकः,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_प्रसामान्यम् | FILE_FLAG_RANDOM_ACCESS, शून्यसूचकः);
#यदि_अन्तः
        यदि (file == INVALID_HANDLE_मानम्) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            यदि (HRESULT_FROM_WIN32(Get_चरमः_Error()) == E_OUTOFMEMORY) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            }
            प्रत्यागम असत्य;
        }

        file_दीर्घता = Get_File_आकार(file, शून्यसूचकः);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, शून्यसूचकः, PAGE_READONLY, 0, 0, शून्यसूचकः);

        CloseHandle(file);
        यदि (map == शून्यसूचकः) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            यदि (HRESULT_FROM_WIN32(Get_चरमः_Error()) == E_OUTOFMEMORY) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            }
            प्रत्यागम असत्य;
        }

        /* map a view of the file into our address space */
        p_दत्त->p_शीर्षकम् = reinterpret_cast<ध्रुव दत्त_शीर्षकम् *>(Map_View_सम्बन्धि_File(map, FILE_MAP_READ, 0, 0, 0));
        यदि (p_दत्त->p_शीर्षकम् == शून्यसूचकः) {
            CloseHandle(map);
            प्रत्यागम असत्य;
        }
        p_दत्त->map = map;
        p_दत्त->length = file_दीर्घता;

        प्रत्यागम सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UData_स्मृतिः *p_दत्त) {
        यदि (p_दत्त != शून्यसूचकः && p_दत्त->map != शून्यसूचकः) {
            Unmap_View_सम्बन्धि_File(p_दत्त->p_शीर्षकम्);
            CloseHandle(p_दत्त->map);
            p_दत्त->p_शीर्षकम् = शून्यसूचकः;
            p_दत्त->map = शून्यसूचकः;
        }
    }



#नोचेद्यदि MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_स्मृतिः *p_दत्त, ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
        पूर्णाङ्क fd;
        पूर्णाङ्क length;
        संरचना stat mystat;
        शून्य *दत्त;

        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम असत्य;
        }

        UData_स्मृतिः_init(p_दत्त); /* Clear the output struct.        */

        /* open the file */
        fd=उद्घाटय(मार्गः, O_RDONLY);
        यदि(fd==-1) {
            प्रत्यागम असत्य;
        }

        /* determine the length of the file */
        यदि(stat(मार्गः, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            प्रत्यागम असत्य;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#यदि U_PLATFORM != U_PF_HPUX
        दत्त=mmap(शून्यसूचकः, length, PROT_READ, MAP_SHARED, fd, 0);
#अन्यथा
        दत्त=mmap(शून्यसूचकः, length, PROT_READ, MAP_वैयक्तिकम्, fd, 0);
#यदि_अन्तः
        close(fd); /* no longer needed */
        यदि(दत्त==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            प्रत्यागम असत्य;
        }

        p_दत्त->map = (वर्ण *)दत्त + length;
        p_दत्त->p_शीर्षकम्=(ध्रुव दत्त_शीर्षकम् *)दत्त;
        p_दत्त->mapAddr = दत्त;
        p_दत्त->length = length;
#यदि U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(दत्त, length, POSIX_MADV_RANDOM);
#   endif
#यदि_अन्तः
        प्रत्यागम सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UData_स्मृतिः *p_दत्त) {
        यदि(p_दत्त!=शून्यसूचकः && p_दत्त->map!=शून्यसूचकः) {
            size_t दत्त_Len = (वर्ण *)p_दत्त->map - (वर्ण *)p_दत्त->mapAddr;
            यदि(munmap(p_दत्त->mapAddr, दत्त_Len)==-1) {
            }
            p_दत्त->p_शीर्षकम्=शून्यसूचकः;
            p_दत्त->map=शून्यसूचकः;
            p_दत्त->mapAddr=शून्यसूचकः;
        }
    }



#नोचेद्यदि MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    स्थिर int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        प्रत्यागम size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_स्मृतिः *p_दत्त, ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
        FILE *file;
        int32_t file_दीर्घता;
        शून्य *p;

        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम असत्य;
        }

        UData_स्मृतिः_init(p_दत्त); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(मार्गः, "rb");
        यदि(file==शून्यसूचकः) {
            प्रत्यागम असत्य;
        }

        /* get the file length */
        file_दीर्घता=umap_fsize(file);
        यदि(ferror(file) || file_दीर्घता<=20) {
            fclose(file);
            प्रत्यागम असत्य;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_दीर्घता);
        यदि(p==शून्यसूचकः) {
            fclose(file);
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }

        /* read the file */
        यदि(file_दीर्घता!=fread(p, 1, file_दीर्घता, file)) {
            uprv_free(p);
            fclose(file);
            प्रत्यागम असत्य;
        }

        fclose(file);
        p_दत्त->map=p;
        p_दत्त->p_शीर्षकम्=(ध्रुव दत्त_शीर्षकम् *)p;
        p_दत्त->mapAddr=p;
        p_दत्त->length = file_दीर्घता;
        प्रत्यागम सत्य;
    }

    U_CFUNC शून्य
    uprv_unmapFile(UData_स्मृतिः *p_दत्त) {
        यदि(p_दत्त!=शून्यसूचकः && p_दत्त->map!=शून्यसूचकः) {
            uprv_free(p_दत्त->map);
            p_दत्त->map     = शून्यसूचकः;
            p_दत्त->mapAddr = शून्यसूचकः;
            p_दत्त->p_शीर्षकम् = शून्यसूचकः;
        }
    }
#अन्यथा
#   error MAP_IMPLEMENTATION is समुच्चयः incorrectly
#यदि_अन्तः
