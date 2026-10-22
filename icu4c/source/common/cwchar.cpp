// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
******************************************************************************
*
*   Copyright (C) 2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cwchar.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001may25
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"

#सचे !U_HAVE_WCSCPY

#अन्तर्भाव "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ध्रुव wchar_t *src) {
    wchar_t *start=dst;
    याव(*dst!=0) {
        ++dst;
    }
    याव((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    पटिनिवत्त start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ध्रुव wchar_t *src) {
    wchar_t *start=dst;
    याव((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    पटिनिवत्त start;
}

U_CAPI size_t uprv_wcslen(ध्रुव wchar_t *src) {
    ध्रुव wchar_t *start=src;
    याव(*src!=0) {
        ++src;
    }
    पटिनिवत्त src-start;
}

#endif

