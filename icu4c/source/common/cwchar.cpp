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

#यदि !U_HAVE_WCSCPY

#अन्तर्भाव "cwchar.h"

U_CAPI ब_अक्षर_प्रकार *uprv_wcscat(ब_अक्षर_प्रकार *dst, अचर ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    जबसम्म(*dst!=0) {
        ++dst;
    }
    जबसम्म((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    फर्काऊ start;
}

U_CAPI ब_अक्षर_प्रकार *uprv_wcscpy(ब_अक्षर_प्रकार *dst, अचर ब_अक्षर_प्रकार *src) {
    ब_अक्षर_प्रकार *start=dst;
    जबसम्म((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    फर्काऊ start;
}

U_CAPI माप_प्रकार uprv_wcslen(अचर ब_अक्षर_प्रकार *src) {
    अचर ब_अक्षर_प्रकार *start=src;
    जबसम्म(*src!=0) {
        ++src;
    }
    फर्काऊ src-start;
}

#endif

