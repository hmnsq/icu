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

#شامل "unicode/utypes.h"

#اگر !U_HAVE_WCSCPY

#شامل "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ثابت wchar_t *src) {
    wchar_t *آغاز=dst;
    جب(*dst!=0) {
        ++dst;
    }
    جب((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    واپس آغاز;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ثابت wchar_t *src) {
    wchar_t *آغاز=dst;
    جب((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    واپس آغاز;
}

U_CAPI size_t uprv_wcslen(ثابت wchar_t *src) {
    ثابت wchar_t *آغاز=src;
    جب(*src!=0) {
        ++src;
    }
    واپس src-آغاز;
}

#اگر_ختم

