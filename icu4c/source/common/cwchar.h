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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CWCHAR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CWCHAR_H__

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

/* Do this after utypes.h so that we have U_HAVE_WCHAR_H . */
#ಆದರೆ U_HAVE_WCHAR_H
#   include <wchar.h>
#ಆದರೆ_ಅಂತ್ಯ

/*===========================================================================*/
/* Wide-character functions                                                  */
/*===========================================================================*/

/* The following are not available on all systems, defined in wchar.h or string.h. */
#ಆದರೆ U_HAVE_WCSCPY
#   define uprv_wcscpy wcscpy
#   define uprv_wcscat wcscat
#   define uprv_wcslen wcslen
#ಇಲ್ಲದಿದ್ದರೆ
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscpy(wchar_t *dst, ಸ್ಥಿರಾಂಕ wchar_t *src);
U_CAPI wchar_t* U_EXPORT2 
uprv_wcscat(wchar_t *dst, ಸ್ಥಿರಾಂಕ wchar_t *src);
U_CAPI size_t U_EXPORT2 
uprv_wcslen(ಸ್ಥಿರಾಂಕ wchar_t *src);
#ಆದರೆ_ಅಂತ್ಯ

/* The following are part of the ANSI C standard, defined in stdlib.h . */
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_wcstombs(mbstr, wcstr, ಎಣಿಕೆ_2) U_STANDARD_CPP_NAMESPACE wcstombs(mbstr, wcstr, ಎಣಿಕೆ_2)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_mbstowcs(wcstr, mbstr, ಎಣಿಕೆ_2) U_STANDARD_CPP_NAMESPACE mbstowcs(wcstr, mbstr, ಎಣಿಕೆ_2)


#ಆದರೆ_ಅಂತ್ಯ
