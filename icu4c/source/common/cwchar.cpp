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

#अन्तर्भाव "unicode/utypes.h"

#जर !U_HAVE_WCSCPY

#अन्तर्भाव "cwchar.h"

U_CAPI ब_अक्षर_प्रकार *uprv_wcscat(ब_अक्षर_प्रकार *dst, स्थिरांक ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    पर्यंत(*dst!=0) {
        ++dst;
    }
    पर्यंत((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    परत start;
}

U_CAPI ब_अक्षर_प्रकार *uprv_wcscpy(ब_अक्षर_प्रकार *dst, स्थिरांक ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    पर्यंत((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    परत start;
}

U_CAPI माप_प्रकार uprv_wcslen(स्थिरांक ब_अक्षर_प्रकार *src) {
    स्थिरांक ब_अक्षर_प्रकार *start=src;
    पर्यंत(*src!=0) {
        ++src;
    }
    परत src-start;
}

#endif

