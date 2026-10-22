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

#నిర్వచితం_కాకపోతే __CWCHAR_H__
#నిర్వచించు __CWCHAR_H__

#అన్తర్భావ <string.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#ఒకవేళ U_HAVE_WCHAR_H
#   include <wchar.h>
#అయితే_ముగింపు

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#ఒకవేళ U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#లేకపోతే
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscpy(wchar_t *dst, స్థిరరాశి wchar_t *src);
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscat(wchar_t *dst, స్థిరరాశి wchar_t *src);
U_CAPI size_t U_EXPORT2 
uprv_wcslen(స్థిరరాశి wchar_t *src);
#అయితే_ముగింపు

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#నిర్వచించు uprv_wcstombs(mbstr, wcstr, లెక్క) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, లెక్క)
#నిర్వచించు uprv_mbstowcs(wcstr, mbstr, లెక్క) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, లెక్క)


#అయితే_ముగింపు
