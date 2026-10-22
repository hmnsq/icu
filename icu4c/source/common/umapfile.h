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

#ifndef __UMAPFILE_H__
#define __UMAPFILE_H__

#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UDataMemory *pdm, மாறிலி எழுத்து *path, UErrorCode *status);
U_CFUNC வெறுமை  uprv_unmapFile(UDataMemory *pData);

/* MAP_NONE: no memory mapping, no file access at all */
#define MAP_NONE        0
#define MAP_WIN32       1
#define MAP_POSIX       2
#define MAP_STDIO       3

#எனில் UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#அல்லதெனில் U_PLATFORM_USES_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#அல்லதெனில் U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#இல்லையெனில் /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#endif

#endif
