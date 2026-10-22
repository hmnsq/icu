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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !U_HAVE_WCSCPY

#అన్తర్భావ "cwchar.h"

U_CAPI wchar_t *uprv_wcscat(wchar_t *dst, స్థిరరాశి wchar_t *src) {
    wchar_t *ప్రారంభం=dst;
    అయితే(*dst!=0) {
        ++dst;
    }
    అయితే((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    తిరిగి ప్రారంభం;
}

U_CAPI wchar_t *uprv_wcscpy(wchar_t *dst, స్థిరరాశి wchar_t *src) {
    wchar_t *ప్రారంభం=dst;
    అయితే((*dst=*src)!=0) {
        ++dst;
        ++src;
    }
    తిరిగి ప్రారంభం;
}

U_CAPI size_t uprv_wcslen(స్థిరరాశి wchar_t *src) {
    స్థిరరాశి wchar_t *ప్రారంభం=src;
    అయితే(*src!=0) {
        ++src;
    }
    తిరిగి src-ప్రారంభం;
}

#అయితే_ముగింపు

