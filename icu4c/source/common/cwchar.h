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

#अगर_अघोषित __CWCHAR_H__
#घोषणा __CWCHAR_H__

#समावेश <string.h>
#समावेश <stdlib.h>
#समावेश "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#अगर U_HAVE_WCHAR_H
#   include <wchar.h>
#पूर्ण_अगर

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#अगर U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#अन्यथा
U_CAPI ब_अक्षर_प्रकार* U_EXPORT2 
uprv_wcscpy(ब_अक्षर_प्रकार *dst, स्थिर ब_अक्षर_प्रकार *src);
U_CAPI ब_अक्षर_प्रकार* U_EXPORT2 
uprv_wcscat(ब_अक्षर_प्रकार *dst, स्थिर ब_अक्षर_प्रकार *src);
U_CAPI माप_प्रकार U_EXPORT2 
uprv_wcslen(स्थिर ब_अक्षर_प्रकार *src);
#पूर्ण_अगर

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#घोषणा uprv_wcstombs(mbstr, wcstr, गिनती) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, गिनती)
#घोषणा uprv_mbstowcs(wcstr, mbstr, गिनती) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, गिनती)


#पूर्ण_अगर
