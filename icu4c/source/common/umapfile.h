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

#अगर_अघोषित __UMAPFILE_H__
#घोषणा __UMAPFILE_H__

#समावेश "unicode/putil.h"
#समावेश "unicode/udata.h"
#समावेश "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UDataMemory *pdm, स्थिर अक्षर *path, UErrorCode *status);
U_CFUNC व्योम  uprv_unmapFile(UDataMemory *p_तथ्य);

/* MAP_NONE: no memory mapping, no file access at all */
#घोषणा MAP_NONE        0
#घोषणा MAP_WIN32       1
#घोषणा MAP_POSIX       2
#घोषणा MAP_STDIO       3

#अगर UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#या_अगर U_PLATFORM_USES_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#या_अगर U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#अन्यथा /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#पूर्ण_अगर

#पूर्ण_अगर
