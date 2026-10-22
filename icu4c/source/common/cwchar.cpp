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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !U_HAVE_WCSCPY

#અન્તર્ભાવ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, અચલ wchar_t *src) {
    wchar_t *start=dst;
    જ્યારે(*dst!=0) {
        ++dst;
    }
    જ્યારે((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    પાછા start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, અચલ wchar_t *src) {
    wchar_t *start=dst;
    જ્યારે((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    પાછા start;
}

U_CAPI size_t uprv_wcslen(અચલ wchar_t *src) {
    અચલ wchar_t *start=src;
    જ્યારે(*src!=0) {
        ++src;
    }
    પાછા src-start;
}

#endif

