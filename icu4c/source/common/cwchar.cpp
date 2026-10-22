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

#ಆದರೆ !U_HAVE_WCSCPY

#ಅನ್ತರ್ಭಾವ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, ಸ್ಥಿರಾಂಕ wchar_t *src) {
    wchar_t *ಆರಂಭ=dst;
    ಆಗ(*dst!=0) {
        ++dst;
    }
    ಆಗ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ಹಿಂತಿರುಗಿ ಆರಂಭ;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, ಸ್ಥಿರಾಂಕ wchar_t *src) {
    wchar_t *ಆರಂಭ=dst;
    ಆಗ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ಹಿಂತಿರುಗಿ ಆರಂಭ;
}

U_CAPI size_t uprv_wcslen(ಸ್ಥಿರಾಂಕ wchar_t *src) {
    ಸ್ಥಿರಾಂಕ wchar_t *ಆರಂಭ=src;
    ಆಗ(*src!=0) {
        ++src;
    }
    ಹಿಂತಿರುಗಿ src-ಆರಂಭ;
}

#ಆದರೆ_ಅಂತ್ಯ

