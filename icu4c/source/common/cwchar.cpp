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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !U_HAVE_WCSCPY

#ਅਨ੍ਤਰ੍ਭਾਵ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ਅਚਲ wchar_t *src) {
    wchar_t *ਸ਼ੁਰੂ=dst;
    ਜਦੋਂ(*dst!=0) {
        ++dst;
    }
    ਜਦੋਂ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ਵਾਪਸ ਸ਼ੁਰੂ;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ਅਚਲ wchar_t *src) {
    wchar_t *ਸ਼ੁਰੂ=dst;
    ਜਦੋਂ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ਵਾਪਸ ਸ਼ੁਰੂ;
}

U_CAPI size_t uprv_wcslen(ਅਚਲ wchar_t *src) {
    ਅਚਲ wchar_t *ਸ਼ੁਰੂ=src;
    ਜਦੋਂ(*src!=0) {
        ++src;
    }
    ਵਾਪਸ src-ਸ਼ੁਰੂ;
}

#ਜੇ_ਅੰਤ

