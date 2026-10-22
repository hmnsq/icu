// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cpputils.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*/

#నిర్వచితం_కాకపోతే CPPUTILS_H
#నిర్వచించు CPPUTILS_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి ద్విగుణ* src, ద్విగుణ* dst, int32_t లెక్క)
{ uprv_memcpy(dst, src, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి ద్విగుణ* src, int32_t src_ప్రారంభం,
              ద్విగుణ* dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_memcpy(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int8_t* src, int8_t* dst, int32_t లెక్క)
    { uprv_memcpy(dst, src, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int8_t* src, int32_t src_ప్రారంభం,
              int8_t* dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_memcpy(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int16_t* src, int16_t* dst, int32_t లెక్క)
{ uprv_memcpy(dst, src, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int16_t* src, int32_t src_ప్రారంభం,
              int16_t* dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_memcpy(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int32_t* src, int32_t* dst, int32_t లెక్క)
{ uprv_memcpy(dst, src, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య uprv_arrayCopy(స్థిరరాశి int32_t* src, int32_t src_ప్రారంభం,
              int32_t* dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_memcpy(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)లెక్క * పరిమాణం(*src)); }

స్థిర
అంతఃవరుస శూన్య
uprv_arrayCopy(స్థిరరాశి char16_t *src, int32_t src_ప్రారంభం,
        char16_t *dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_memcpy(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)లెక్క * పరిమాణం(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
స్థిర అంతఃవరుస శూన్య
uprv_arrayCopy(స్థిరరాశి icu::UnicodeString *src, icu::UnicodeString *dst, int32_t లెక్క)
{ అయితే(లెక్క-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
స్థిర అంతఃవరుస శూన్య
uprv_arrayCopy(స్థిరరాశి icu::UnicodeString *src, int32_t src_ప్రారంభం,
               icu::UnicodeString *dst, int32_t dst_ప్రారంభం, int32_t లెక్క)
{ uprv_arrayCopy(src+src_ప్రారంభం, dst+dst_ప్రారంభం, లెక్క); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
అంతఃవరుస శూన్య
uprv_check_Can_Get_బఫరు(స్థిరరాశి icu::UnicodeString &s, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && s.isBogus()) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
}

#అయితే_ముగింపు /* _CPPUTILS */
