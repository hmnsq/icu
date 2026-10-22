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

#ifndef __CWCHAR_H__
#define __CWCHAR_H__

#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#जर U_HAVE_WCHAR_H
#   include <wchar.h>
#endif

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#जर U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#अन्यथा
U_CAPI ब_अक्षर_प्रकार* U_EXPORT2 
uprv_wcscpy(ब_अक्षर_प्रकार *dst, स्थिरांक ब_अक्षर_प्रकार *src);
U_CAPI ब_अक्षर_प्रकार* U_EXPORT2 
uprv_wcscat(ब_अक्षर_प्रकार *dst, स्थिरांक ब_अक्षर_प्रकार *src);
U_CAPI माप_प्रकार U_EXPORT2 
uprv_wcslen(स्थिरांक ब_अक्षर_प्रकार *src);
#endif

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#define uprv_wcstombs(mbstr, wcstr, मोजणी) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, मोजणी)
#define uprv_mbstowcs(wcstr, mbstr, मोजणी) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, मोजणी)


#endif
