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

#include "unicode/utypes.h"

#ꦪꦼꦤ꧀ !U_HAVE_WCSCPY

#include "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, const wchar_t *src) {
    wchar_t *start=dst;
    ꦱꦼꦢꦼꦭ(*dst!=0) {
        ++dst;
    }
    ꦱꦼꦢꦼꦭ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ꦮꦭꦶ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, const wchar_t *src) {
    wchar_t *start=dst;
    ꦱꦼꦢꦼꦭ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ꦮꦭꦶ start;
}

U_CAPI size_t uprv_wcslen(const wchar_t *src) {
    const wchar_t *start=src;
    ꦱꦼꦢꦼꦭ(*src!=0) {
        ++src;
    }
    ꦮꦭꦶ src-start;
}

#endif

