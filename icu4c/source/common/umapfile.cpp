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
#ಅನ್ತರ್ಭಾವ "uposixdefs.h"

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"
#ಅನ್ತರ್ಭಾವ "umapfile.h"

/* memory-mapping base definitions ------------------------------------------ */

#ಆದರೆ MAP_IMPLEMENTATION==MAP_WIN32
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
#   define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX

#   ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 1
        // Some previous versions of the Windows 10 SDK don't expose various APIs for UWP applications
        // to use, even though UWP apps are allowed to call and use them.  Temporarily change the
        // WINAPI family partition below to Desktop, so that function declarations are visible for UWP.
#       include <winapifamily.h>
#       ಆದರೆ !(WINAPI_FAMILY_ವಿಭಜನೆ(WINAPI_ವಿಭಜನೆ_DESKTOP | WINAPI_ವಿಭಜನೆ_SYSTEM))
#           pragma ತಳ್ಳು_macro("WINAPI_PARTITION_DESKTOP")
#           undef WINAPI_ವಿಭಜನೆ_DESKTOP
#           define WINAPI_ವಿಭಜನೆ_DESKTOP 1
#           define ಬದಲಾಯಿತು_WINAPI_ವಿಭಜನೆ_DESKTOP_ಮೌಲ್ಯ
#       endif
#   endif

#   include <windows.h>

#   ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 1 && defined(ಬದಲಾಯಿತು_WINAPI_ವಿಭಜನೆ_DESKTOP_ಮೌಲ್ಯ)
#       pragma ತೆಗೆ_macro("WINAPI_PARTITION_DESKTOP")
#   endif

#   include "cmemory.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HANDLE ಸ್ಮರಣೆ_Map;

#   define IS_MAP(map) ((map)!=ಶೂನ್ಯಸೂಚಕ)

#ಅಥವಾಆದರೆ MAP_IMPLEMENTATION==MAP_POSIX
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ size_t ಸ್ಮರಣೆ_Map;

#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((ಶೂನ್ಯ*)-1)
#   endif
#ಅಥವಾಆದರೆ MAP_IMPLEMENTATION==MAP_STDIO
#   include <stdio.h>
#   include "cmemory.h"

    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *ಸ್ಮರಣೆ_Map;

#   define IS_MAP(map) ((map)!=ಶೂನ್ಯಸೂಚಕ)
#ಆದರೆ_ಅಂತ್ಯ

/*----------------------------------------------------------------------------*
 *                                                                            *
 *   Memory Mapped File support.  Platform dependent implementation of        *
 *                           functions used by the rest of the implementation.*
 *                                                                            *
 *----------------------------------------------------------------------------*/
#ಆದರೆ MAP_IMPLEMENTATION==MAP_NONE
    U_CFUNC UBool
    uprv_mapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        UData_ಸ್ಮರಣೆ_init(p_ದತ್ತಾಂಶ); /* Clear the output struct. */
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;            /* no file access */
    }

    U_CFUNC ಶೂನ್ಯ uprv_unmapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
        /* nothing to do */
    }
#ಅಥವಾಆದರೆ MAP_IMPLEMENTATION==MAP_WIN32
    U_CFUNC UBool
    uprv_mapFile(
         UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,      /* File path to be opened/mapped.                     */
         UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2     /* Error status, used to report out-of-memory errors. */
         )
    {
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        HANDLE map = ಶೂನ್ಯಸೂಚಕ;
        HANDLE file = INVALID_HANDLE_ಮೌಲ್ಯ;
        DWORD file_ಉದ್ದಳತೆ = 0;

        UData_ಸ್ಮರಣೆ_init(p_ದತ್ತಾಂಶ); /* Clear the output struct.        */

        /* open the input file */
#ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 0
        // Note: In the non-UWP code-path (ie: Win32), the value of the path variable might have come from 
        // the CRT 'getenv' function, and would be therefore be encoded in the default ANSI code page.
        // This means that we can't call the *W version of API below, whereas in the UWP code-path
        // there is no 'getenv' call, and thus the string will be only UTF-8/Invariant characters.
        file=CreateFileA(ಮಾರ್ಗ, GENERIC_READ, FILE_SHARE_READ, ಶೂನ್ಯಸೂಚಕ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ|FILE_FLAG_RANDOM_ACCESS, ಶೂನ್ಯಸೂಚಕ);
#ಇಲ್ಲದಿದ್ದರೆ
        // Convert from UTF-8 string to UTF-16 string.
        wchar_t utf16_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];
        int32_t ಮಾರ್ಗ_Utf16_Len = 0;
        u_strFromUTF8(reinterpret_cast<char16_t*>(utf16_ಮಾರ್ಗ), static_cast<int32_t>(UPRV_LENGTHOF(utf16_ಮಾರ್ಗ)), &ಮಾರ್ಗ_Utf16_Len, ಮಾರ್ಗ, -1, ಸ್ಥಿತಿ_2);

        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಆದರೆ (*ಸ್ಥಿತಿ_2 == U_STRING_NOT_TERMINATED_WARNING) {
            // Report back an error instead of a warning.
            *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        file = CreateFileW(utf16_ಮಾರ್ಗ, GENERIC_READ, FILE_SHARE_READ, ಶೂನ್ಯಸೂಚಕ,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ | FILE_FLAG_RANDOM_ACCESS, ಶೂನ್ಯಸೂಚಕ);
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (file == INVALID_HANDLE_ಮೌಲ್ಯ) {
            // If we failed to open the file due to an out-of-memory error, then we want
            // to report that error back to the caller.
            ಆದರೆ (HRESULT_FROM_WIN32(Get_ಕೊನೆಯ_Error()) == E_OUTOFMEMORY) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        file_ಉದ್ದಳತೆ = Get_File_ಗಾತ್ರ(file, ಶೂನ್ಯಸೂಚಕ);

        // Note: We use nullptr/nullptr for lpAttributes parameter below.
        // This means our handle cannot be inherited and we will get the default security descriptor.
        /* create an unnamed Windows file-mapping object for the specified file */
        map = CreateFileMappingW(file, ಶೂನ್ಯಸೂಚಕ, ಪುಟ_READONLY, 0, 0, ಶೂನ್ಯಸೂಚಕ);

        CloseHandle(file);
        ಆದರೆ (map == ಶೂನ್ಯಸೂಚಕ) {
            // If we failed to create the mapping due to an out-of-memory error, then 
            // we want to report that error back to the caller.
            ಆದರೆ (HRESULT_FROM_WIN32(Get_ಕೊನೆಯ_Error()) == E_OUTOFMEMORY) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* map a view of the file into our address space */
        p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *>(Map_View_ದ_File(map, FILE_MAP_READ, 0, 0, 0));
        ಆದರೆ (p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
            CloseHandle(map);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        p_ದತ್ತಾಂಶ->map = map;
        p_ದತ್ತಾಂಶ->length = file_ಉದ್ದಳತೆ;

        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    U_CFUNC ಶೂನ್ಯ
    uprv_unmapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
        ಆದರೆ (p_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->map != ಶೂನ್ಯಸೂಚಕ) {
            Unmap_View_ದ_File(p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ);
            CloseHandle(p_ದತ್ತಾಂಶ->map);
            p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ = ಶೂನ್ಯಸೂಚಕ;
            p_ದತ್ತಾಂಶ->map = ಶೂನ್ಯಸೂಚಕ;
        }
    }



#ಅಥವಾಆದರೆ MAP_IMPLEMENTATION==MAP_POSIX
    U_CFUNC UBool
    uprv_mapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
        ಪೂರ್ಣಾಂಕ fd;
        ಪೂರ್ಣಾಂಕ length;
        ರಚನೆ stat mystat;
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;

        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        UData_ಸ್ಮರಣೆ_init(p_ದತ್ತಾಂಶ); /* Clear the output struct.        */

        /* open the file */
        fd=open(ಮಾರ್ಗ, O_RDONLY);
        ಆದರೆ(fd==-1) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* determine the length of the file */
        ಆದರೆ(stat(ಮಾರ್ಗ, &mystat)!=0 || mystat.st_size<=0) {
            close(fd);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        length=mystat.st_size;

        /* get a view of the mapping */
#ಆದರೆ U_PLATFORM != U_PF_HPUX
        ದತ್ತಾಂಶ=mmap(ಶೂನ್ಯಸೂಚಕ, length, PROT_READ, MAP_SHARED, fd, 0);
#ಇಲ್ಲದಿದ್ದರೆ
        ದತ್ತಾಂಶ=mmap(ಶೂನ್ಯಸೂಚಕ, length, PROT_READ, MAP_ಖಾಸಗಿ, fd, 0);
#ಆದರೆ_ಅಂತ್ಯ
        close(fd); /* no longer needed */
        ಆದರೆ(ದತ್ತಾಂಶ==MAP_FAILED) {
            // Possibly check the errno value for ENOMEM, and report U_MEMORY_ALLOCATION_ERROR?
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        p_ದತ್ತಾಂಶ->map = (ಅಕ್ಷರ *)ದತ್ತಾಂಶ + length;
        p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
        p_ದತ್ತಾಂಶ->mapAddr = ದತ್ತಾಂಶ;
        p_ದತ್ತಾಂಶ->length = length;
#ಆದರೆ U_PLATFORM == U_PF_IPHONE || U_PLATFORM == U_PF_ANDROID
    // Apparently supported from Android 23 and higher:
    //   https://github.com/ggml-org/llama.cpp/pull/3631
    // Checking for the flag itself is safer than checking for __ANDROID_API__.
#   ifdef POSIX_MADV_RANDOM
        posix_madvise(ದತ್ತಾಂಶ, length, POSIX_MADV_RANDOM);
#   endif
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    U_CFUNC ಶೂನ್ಯ
    uprv_unmapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
        ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->map!=ಶೂನ್ಯಸೂಚಕ) {
            size_t ದತ್ತಾಂಶ_Len = (ಅಕ್ಷರ *)p_ದತ್ತಾಂಶ->map - (ಅಕ್ಷರ *)p_ದತ್ತಾಂಶ->mapAddr;
            ಆದರೆ(munmap(p_ದತ್ತಾಂಶ->mapAddr, ದತ್ತಾಂಶ_Len)==-1) {
            }
            p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ=ಶೂನ್ಯಸೂಚಕ;
            p_ದತ್ತಾಂಶ->map=ಶೂನ್ಯಸೂಚಕ;
            p_ದತ್ತಾಂಶ->mapAddr=ಶೂನ್ಯಸೂಚಕ;
        }
    }



#ಅಥವಾಆದರೆ MAP_IMPLEMENTATION==MAP_STDIO
    /* copy of the filestrm.c/T_FileStream_size() implementation */
    ಸ್ಥಿರ int32_t
    umap_fsize(FILE *f) {
        int32_t savedPos = ftell(f);
        int32_t size = 0;

        /*Changes by Bertrand A. D. doesn't affect the current position
        goes to the end of the file before ftell*/
        fseek(f, 0, SEEK_END);
        size = (int32_t)ftell(f);
        fseek(f, savedPos, SEEK_SET);
        ಹಿಂತಿರುಗಿ size;
    }

    U_CFUNC UBool
    uprv_mapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
        FILE *file;
        int32_t file_ಉದ್ದಳತೆ;
        ಶೂನ್ಯ *p;

        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        UData_ಸ್ಮರಣೆ_init(p_ದತ್ತಾಂಶ); /* Clear the output struct.        */
        /* open the input file */
        file=fopen(ಮಾರ್ಗ, "rb");
        ಆದರೆ(file==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* get the file length */
        file_ಉದ್ದಳತೆ=umap_fsize(file);
        ಆದರೆ(ferror(file) || file_ಉದ್ದಳತೆ<=20) {
            fclose(file);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* allocate the memory to hold the file data */
        p=uprv_malloc(file_ಉದ್ದಳತೆ);
        ಆದರೆ(p==ಶೂನ್ಯಸೂಚಕ) {
            fclose(file);
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* read the file */
        ಆದರೆ(file_ಉದ್ದಳತೆ!=fread(p, 1, file_ಉದ್ದಳತೆ, file)) {
            uprv_free(p);
            fclose(file);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        fclose(file);
        p_ದತ್ತಾಂಶ->map=p;
        p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)p;
        p_ದತ್ತಾಂಶ->mapAddr=p;
        p_ದತ್ತಾಂಶ->length = file_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    U_CFUNC ಶೂನ್ಯ
    uprv_unmapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ) {
        ಆದರೆ(p_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && p_ದತ್ತಾಂಶ->map!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(p_ದತ್ತಾಂಶ->map);
            p_ದತ್ತಾಂಶ->map     = ಶೂನ್ಯಸೂಚಕ;
            p_ದತ್ತಾಂಶ->mapAddr = ಶೂನ್ಯಸೂಚಕ;
            p_ದತ್ತಾಂಶ->p_ಶೀರ್ಷಿಕೆ = ಶೂನ್ಯಸೂಚಕ;
        }
    }
#ಇಲ್ಲದಿದ್ದರೆ
#   error MAP_IMPLEMENTATION is ಗಣ incorrectly
#ಆದರೆ_ಅಂತ್ಯ
