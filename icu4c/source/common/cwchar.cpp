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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !U_HAVE_WCSCPY

#অন্তর্ভাৱ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ধ্রুৱ wchar_t *src) {
    wchar_t *start=dst;
    যেতিয়ালৈ(*dst!=0) {
        ++dst;
    }
    যেতিয়ালৈ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ঘূৰাই start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ধ্রুৱ wchar_t *src) {
    wchar_t *start=dst;
    যেতিয়ালৈ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ঘূৰাই start;
}

U_CAPI size_t uprv_wcslen(ধ্রুৱ wchar_t *src) {
    ধ্রুৱ wchar_t *start=src;
    যেতিয়ালৈ(*src!=0) {
        ++src;
    }
    ঘূৰাই src-start;
}

#endif

