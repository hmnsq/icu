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

#جيڪڏهن_وصف_نه_ٿيل __UMAPFILE_H__
#وصف_ڪريو __UMAPFILE_H__

#ضمن "unicode/putil.h"
#ضمن "unicode/udata.h"
#ضمن "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UData_يادگيري *pdm, ثابت حرف *رستو, UError_ڪوڊ *حالت);
U_CFUNC فراغ  uprv_unmapFile(UData_يادگيري *p_معلومات);

/* MAP_NONE: no memory mapping, no file access at all */
#وصف_ڪريو MAP_NONE        0
#وصف_ڪريو MAP_WIN32       1
#وصف_ڪريو MAP_POSIX       2
#وصف_ڪريو MAP_STDIO       3

#إذا UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#وإلاإذا U_PLATFORM_استعمال_ڪري_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#وإلاإذا U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#وإلا /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#جيڪڏهن_پورو

#جيڪڏهن_پورو
