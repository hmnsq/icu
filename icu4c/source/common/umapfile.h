// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/

/*----------------------------------------------------------------------------------
 *
 *       Memory mapped file wrappers for use by the ICU Data Implementation
 *
 *           Porting note:  The implementation of these functions is very platform specific.
 *             Not all platforms can do real memory mapping.  Those that can't
 *             still must implement these functions, getting the data into memory using
 *             whatever means are available.
 *
 *            These functions are part of the ICU internal implementation, and
 *            are not intended to be used directly by applications.
 *
 *----------------------------------------------------------------------------------*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UMAPFILE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UMAPFILE_H__

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UData_ಸ್ಮರಣೆ *pdm, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);
U_CFUNC ಶೂನ್ಯ  uprv_unmapFile(UData_ಸ್ಮರಣೆ *p_ದತ್ತಾಂಶ);

/* MAP_NONE: no memory mapping, no file access at all */
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_NONE        0
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_WIN32       1
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_POSIX       2
#ವ್ಯಾಖ್ಯಾನಿಸು MAP_STDIO       3

#ಆದರೆ UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#ಅಥವಾಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#ಅಥವಾಆದರೆ U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#ಇಲ್ಲದಿದ್ದರೆ /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
