// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1997-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File USCRIPT.C
*
* Modification History:
*
*   Date        Name        Description
*   07/06/2001    Ram         Creation.
******************************************************************************
*/

#include "unicode/uchar.h"
#include "unicode/uscript.h"
#include "unicode/uloc.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "ulocimp.h"

static const UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
static const UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
static const UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

static int32_t
setCodes(const UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    만약(U_FAILURE(*err)) { 반환 0; }
    만약(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        반환 length;
    }
    위해(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    반환 length;
}

static int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    만약(U_FAILURE(*err)) { 반환 0; }
    만약(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        반환 1;
    }
    scripts[0] = script;
    반환 1;
}

static int32_t
getCodesFromLocale(const char *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    만약 (U_FAILURE(*err)) { 반환 0; }
    icu::CharString lang;
    icu::CharString script;
    만약 (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    만약 (U_FAILURE(*err)) { 반환 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    만약 (lang == "ja") {
        반환 setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    만약 (lang == "ko") {
        반환 setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    만약 (lang == "zh" && script == "Hant") {
        반환 setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    만약 (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_getPropertyValueEnum(UCHAR_SCRIPT, script.data()));
        만약(scriptCode != USCRIPT_INVALID_CODE) {
            만약(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            반환 setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    반환 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(const char* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    만약(U_FAILURE(*err)) {
        반환 0;
    }
    만약(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        반환 0;
    }

    triedCode = false;
    const char* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    만약 (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    만약 (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        만약(code!=USCRIPT_INVALID_CODE) {
            반환 setOneCode(code, fillIn, capacity, err);
        }
        triedCode = true;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    만약(U_FAILURE(*err) || length != 0) {
        반환 length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    만약(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        만약(U_FAILURE(*err) || length != 0) {
            반환 length;
        }
    }
    만약(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        만약(code!=USCRIPT_INVALID_CODE) {
            반환 setOneCode(code, fillIn, capacity, err);
        }
    }
    반환 0;
}
