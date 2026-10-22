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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !U_HAVE_WCSCPY

#அந்தர்பாவ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, மாறிலி wchar_t *src) {
    wchar_t *start=dst;
    போது(*dst!=0) {
        ++dst;
    }
    போது((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    திரும்பு start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, மாறிலி wchar_t *src) {
    wchar_t *start=dst;
    போது((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    திரும்பு start;
}

U_CAPI size_t uprv_wcslen(மாறிலி wchar_t *src) {
    மாறிலி wchar_t *start=src;
    போது(*src!=0) {
        ++src;
    }
    திரும்பு src-start;
}

#endif

