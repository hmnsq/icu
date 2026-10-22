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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ध्रुव वर्ण *locale) {
    यदि (locale == शून्यसूचकः) {
        locale = uloc_getDefault();
    }
    यदि (*locale == 0) {
        प्रत्यागम UCASE_LOC_मूलम्;
    } अन्यथा {
        प्रत्यागम ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_गन्तव्यम्_अधस्तनम्(char16_t *dest, int32_t dest_क्षमता,
             ध्रुव char16_t *src, int32_t src_दीर्घता,
             ध्रुव वर्ण *locale,
             UError_संहिता *p_Error_संहिता) {
    प्रत्यागम ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_अधस्तनम्, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
u_str_गन्तव्यम्_ऊर्ध्वस्थम्(char16_t *dest, int32_t dest_क्षमता,
             ध्रुव char16_t *src, int32_t src_दीर्घता,
             ध्रुव वर्ण *locale,
             UError_संहिता *p_Error_संहिता) {
    प्रत्यागम ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_ऊर्ध्वस्थम्, *p_Error_संहिता);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::गन्तव्यम्_अधस्तनम्(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव char16_t *src, int32_t src_दीर्घता,
        char16_t *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_अधस्तनम्, edits, error_संहिता);
}

int32_t CaseMap::गन्तव्यम्_ऊर्ध्वस्थम्(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव char16_t *src, int32_t src_दीर्घता,
        char16_t *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_ऊर्ध्वस्थम्, edits, error_संहिता);
}

U_NAMESPACE_अन्तः
