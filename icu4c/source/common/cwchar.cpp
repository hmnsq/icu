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

#ضمن "unicode/utypes.h"

#ܐܢ !U_HAVE_WCSCPY

#ضمن "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ثابت wchar_t *src) {
    wchar_t *start=dst;
    ܟܡܐ(*dst!=0) {
        ++dst;
    }
    ܟܡܐ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ܦܢܐ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ثابت wchar_t *src) {
    wchar_t *start=dst;
    ܟܡܐ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ܦܢܐ start;
}

U_CAPI size_t uprv_wcslen(ثابت wchar_t *src) {
    ثابت wchar_t *start=src;
    ܟܡܐ(*src!=0) {
        ++src;
    }
    ܦܢܐ src-start;
}

#endif

