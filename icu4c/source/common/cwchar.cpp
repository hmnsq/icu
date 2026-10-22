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

#ຖ້າ !U_HAVE_WCSCPY

#include "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, const wchar_t *src) {
    wchar_t *start=dst;
    ໃນຂະນະທີ່(*dst!=0) {
        ++dst;
    }
    ໃນຂະນະທີ່((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ສົ່ງຄືນ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, const wchar_t *src) {
    wchar_t *start=dst;
    ໃນຂະນະທີ່((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    ສົ່ງຄືນ start;
}

U_CAPI size_t uprv_wcslen(const wchar_t *src) {
    const wchar_t *start=src;
    ໃນຂະນະທີ່(*src!=0) {
        ++src;
    }
    ສົ່ງຄືນ src-start;
}

#endif

