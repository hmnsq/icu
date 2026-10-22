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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !U_HAVE_WCSCPY

#അന്തര്ഭാവ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, സ്ഥിരം wchar_t *src) {
    wchar_t *start=dst;
    ആകുമ്പോൾ(*dst!=0) {
        ++dst;
    }
    ആകുമ്പോൾ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    തിരികെ start;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, സ്ഥിരം wchar_t *src) {
    wchar_t *start=dst;
    ആകുമ്പോൾ((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    തിരികെ start;
}

U_CAPI size_t uprv_wcslen(സ്ഥിരം wchar_t *src) {
    സ്ഥിരം wchar_t *start=src;
    ആകുമ്പോൾ(*src!=0) {
        ++src;
    }
    തിരികെ src-start;
}

#endif

