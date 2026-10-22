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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !U_HAVE_WCSCPY

#ಅನ್ತರ್ಭಾವ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ಧ್ರುವ wchar_t *src) {
    wchar_t *start=dst;
    ಯಾವತ್(*dst!=0) {
        ++dst;
    }
    ಯಾವತ್((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ಪ್ರತ್ಯಾಗಮ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ಧ್ರುವ wchar_t *src) {
    wchar_t *start=dst;
    ಯಾವತ್((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ಪ್ರತ್ಯಾಗಮ start;
}

U_CAPI size_t uprv_wcslen(ಧ್ರುವ wchar_t *src) {
    ಧ್ರುವ wchar_t *start=src;
    ಯಾವತ್(*src!=0) {
        ++src;
    }
    ಪ್ರತ್ಯಾಗಮ src-start;
}

#endif

