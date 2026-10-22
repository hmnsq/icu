// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  FILE NAME : ptypes.h
*
*   Date        Name        Description
*   05/13/98    nos         Creation (content moved here from ptypes.h).
*   03/02/99    stephen     Added AS400 support.
*   03/30/99    stephen     Added Linux support.
*   04/13/99    stephen     Reworked for autoconf.
*   09/18/08    srl         Moved basic types back to ptypes.h from platform.h
******************************************************************************
*/

/**
 * \file
 * \brief C API: Definitions of integer types of various widths
 */

#اگر_تعریف_نشده _PTYPES_H
#تعریف_کن _PTYPES_H

/**
 * \def __STDC_LIMIT_MACROS
 * According to the Linux stdint.h, the ISO C99 standard specifies that in C++ implementations
 * macros like INT32_MIN and UINTPTR_MAX should only be defined if explicitly requested.
 * We need to define __STDC_LIMIT_MACROS before including stdint.h in C++ code
 * that uses such limit macros.
 * @internal
 */
#اگر_تعریف_نشده __STDC_LIMIT_MACROS
#تعریف_کن __STDC_LIMIT_MACROS
#پایان_اگر

/* NULL, size_t, wchar_t */
#ضمن <stddef.h>

/* More platform-specific definitions. */
#ضمن "unicode/platform.h"

/*===========================================================================*/
/* Generic data types                                                        */
/*===========================================================================*/

#ضمن <stdint.h>

// C++11 and C11 both specify that the data type char16_t should exist, C++11
// as a keyword and C11 as a typedef in the uchar.h header file, but not all
// implementations (looking at you, Apple, spring 2024) actually do this, so
// ICU4C must detect and deal with that.
#إذا !defined(__cplusplus) && !defined(U_درون_DOXYGEN)
#   إذا U_HAVE_CHAR16_T
#       include <uchar.h>
#   وإلا
        تعريفنوع uint16_t char16_t;
#   endif
#پایان_اگر

#پایان_اگر /* _PTYPES_H */
