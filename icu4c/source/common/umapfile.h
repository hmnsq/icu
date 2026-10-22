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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UMAPFILE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UMAPFILE_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UData_ਸਮ੍ਰਿਤੀ *pdm, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ);
U_CFUNC ਖਾਲੀ  uprv_unmapFile(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ);

/* MAP_NONE: no memory mapping, no file access at all */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_NONE        0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_WIN32       1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_POSIX       2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAP_STDIO       3

#ਜੇ UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#ਜਾਂਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#ਜਾਂਜੇ U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#ਨਹੀਂਤਾਂ /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
