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
    wchar_t *start=dst;
    وقتی(*dst!=0) {
        ++dst;
    }
    وقتی((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    بازگشت start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ثابت wchar_t *src) {
    wchar_t *start=dst;
    وقتی((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    بازگشت start;
}

U_CAPI size_t uprv_wcslen(ثابت wchar_t *src) {
    ثابت wchar_t *start=src;
    وقتی(*src!=0) {
        ++src;
    }
    بازگشت src-start;
}

#endif

