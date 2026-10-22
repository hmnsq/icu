// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ustrcase_locale.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may31
*   created by: Markus W. Scherer
*
*   Locale-sensitive case mapping functions (ones that call uloc_getDefault())
*   were moved here to break dependency cycles among parts of the common library.
*/

#شامل "unicode/utypes.h"
#شامل "uassert.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ustring.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ثابت حرف *locale) {
    اگر (locale == خالی_اشاریہ) {
        locale = uloc_getDefault();
    }
    اگر (*locale == 0) {
        واپس UCASE_LOC_جڑ;
    } ورنہ {
        واپس ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_مقصد_زیریں(char16_t *dest, int32_t dest_استعداد,
             ثابت char16_t *src, int32_t src_لمبائی,
             ثابت حرف *locale,
             UError_رمز *p_Error_رمز) {
    واپس ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_زیریں, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
u_str_مقصد_بالائی(char16_t *dest, int32_t dest_استعداد,
             ثابت char16_t *src, int32_t src_لمبائی,
             ثابت حرف *locale,
             UError_رمز *p_Error_رمز) {
    واپس ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_بالائی, *p_Error_رمز);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::مقصد_زیریں(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_لمبائی,
        char16_t *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_زیریں, edits, error_رمز);
}

int32_t CaseMap::مقصد_بالائی(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_لمبائی,
        char16_t *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_بالائی, edits, error_رمز);
}

U_NAMESPACE_انتہا
