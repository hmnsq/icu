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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !U_HAVE_WCSCPY

#ଅନ୍ତର୍ଭାଵ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ଅଚଳ wchar_t *src) {
    wchar_t *start=dst;
    ଯେତେବେଳେ(*dst!=0) {
        ++dst;
    }
    ଯେତେବେଳେ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ଫେରନ୍ତୁ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ଅଚଳ wchar_t *src) {
    wchar_t *start=dst;
    ଯେତେବେଳେ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ଫେରନ୍ତୁ start;
}

U_CAPI size_t uprv_wcslen(ଅଚଳ wchar_t *src) {
    ଅଚଳ wchar_t *start=src;
    ଯେତେବେଳେ(*src!=0) {
        ++src;
    }
    ଫେରନ୍ତୁ src-start;
}

#endif

