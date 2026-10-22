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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !U_HAVE_WCSCPY

#অন্তর্ভাব "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ধ্রুবক wchar_t *src) {
    wchar_t *start=dst;
    যখন(*dst!=0) {
        ++dst;
    }
    যখন((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ফেরত start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ধ্রুবক wchar_t *src) {
    wchar_t *start=dst;
    যখন((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ফেরত start;
}

U_CAPI size_t uprv_wcslen(ধ্রুবক wchar_t *src) {
    ধ্রুবক wchar_t *start=src;
    যখন(*src!=0) {
        ++src;
    }
    ফেরত src-start;
}

#endif

