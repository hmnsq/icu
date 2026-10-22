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

#यदि_अपरिभाषितम् __CWCHAR_H__
#परिभाषय __CWCHAR_H__

#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#यदि U_HAVE_WCHAR_H
#   include <wchar.h>
#यदि_अन्तः

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#यदि U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#अन्यथा
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscpy(wchar_t *dst, ध्रुव wchar_t *src);
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscat(wchar_t *dst, ध्रुव wchar_t *src);
U_CAPI size_t U_EXPORT2 
uprv_wcslen(ध्रुव wchar_t *src);
#यदि_अन्तः

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#परिभाषय uprv_wcstombs(mbstr, wcstr, गणनम्) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, गणनम्)
#परिभाषय uprv_mbstowcs(wcstr, mbstr, गणनम्) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, गणनम्)


#यदि_अन्तः
