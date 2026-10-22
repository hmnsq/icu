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

#यदि_अपरिभाषितम् __UMAPFILE_H__
#परिभाषय __UMAPFILE_H__

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UData_स्मृतिः *pdm, ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः);
U_CFUNC शून्य  uprv_unmapFile(UData_स्मृतिः *p_दत्त);

/* MAP_NONE: no memory mapping, no file access at all */
#परिभाषय MAP_NONE        0
#परिभाषय MAP_WIN32       1
#परिभाषय MAP_POSIX       2
#परिभाषय MAP_STDIO       3

#यदि UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#नोचेद्यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#नोचेद्यदि U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#अन्यथा /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#यदि_अन्तः

#यदि_अन्तः
