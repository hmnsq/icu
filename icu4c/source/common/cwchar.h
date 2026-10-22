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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CWCHAR_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CWCHAR_H__

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#ਜੇ U_HAVE_WCHAR_H
#   include <wchar.h>
#ਜੇ_ਅੰਤ

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#ਜੇ U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#ਨਹੀਂਤਾਂ
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscpy(wchar_t *dst, ਅਚਲ wchar_t *src);
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscat(wchar_t *dst, ਅਚਲ wchar_t *src);
U_CAPI size_t U_EXPORT2 
uprv_wcslen(ਅਚਲ wchar_t *src);
#ਜੇ_ਅੰਤ

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_wcstombs(mbstr, wcstr, ਗਿਣਤੀ_2) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, ਗਿਣਤੀ_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_mbstowcs(wcstr, mbstr, ਗਿਣਤੀ_2) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, ਗਿਣਤੀ_2)


#ਜੇ_ਅੰਤ
