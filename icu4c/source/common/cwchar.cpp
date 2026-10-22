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

#समावेश "unicode/utypes.h"

#अगर !U_HAVE_WCSCPY

#समावेश "cwchar.h"

U_CAPI ब_अक्षर_प्रकार *uprv_wcscat(ब_अक्षर_प्रकार *dst, स्थिर ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    जबतक(*dst!=0) {
        ++dst;
    }
    जबतक((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    वापस start;
}

U_CAPI ब_अक्षर_प्रकार *uprv_wcscpy(ब_अक्षर_प्रकार *dst, स्थिर ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    जबतक((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    वापस start;
}

U_CAPI माप_प्रकार uprv_wcslen(स्थिर ब_अक्षर_प्रकार *src) {
    स्थिर ब_अक्षर_प्रकार *start=src;
    जबतक(*src!=0) {
        ++src;
    }
    वापस src-start;
}

#पूर्ण_अगर

