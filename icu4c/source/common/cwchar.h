// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
******************************************************************************
*
*   Copyright (C) 2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cwchar.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001may25
*   created by: Markus W. Scherer
*
*   This file contains ICU-internal definitions of wchar_t operations.
*   These definitions were moved here from cstring.h so that fewer
*   ICU implementation files include wchar.h.
*/

#جے_بنا_تعریف __CWCHAR_H__
#تعریف_کرو __CWCHAR_H__

#ضمن <string.h>
#ضمن <stdlib.h>
#ضمن "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#إذا U_HAVE_WCHAR_H
#   include <wchar.h>
#جے_انت

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#إذا U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#وإلا
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscpy(wchar_t *dst, ثابت wchar_t *src);
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscat(wchar_t *dst, ثابت wchar_t *src);
U_CAPI size_t U_EXPORT2 
uprv_wcslen(ثابت wchar_t *src);
#جے_انت

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#تعریف_کرو uprv_wcstombs(mbstr, wcstr, گنتی) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, گنتی)
#تعریف_کرو uprv_mbstowcs(wcstr, mbstr, گنتی) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, گنتی)


#جے_انت
