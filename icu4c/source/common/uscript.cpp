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
    ຖ້າ(U_FAILURE(*err)) { ສົ່ງຄືນ 0; }
    ຖ້າ(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ສົ່ງຄືນ length;
    }
    ສຳລັບ(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ສົ່ງຄືນ length;
}

static int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ຖ້າ(U_FAILURE(*err)) { ສົ່ງຄືນ 0; }
    ຖ້າ(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ສົ່ງຄືນ 1;
    }
    scripts[0] = script;
    ສົ່ງຄືນ 1;
}

static int32_t
getCodesFromLocale(const var_na *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ຖ້າ (U_FAILURE(*err)) { ສົ່ງຄືນ 0; }
    icu::CharString lang;
    icu::CharString script;
    ຖ້າ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    ຖ້າ (U_FAILURE(*err)) { ສົ່ງຄືນ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ຖ້າ (lang == "ja") {
        ສົ່ງຄືນ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ຖ້າ (lang == "ko") {
        ສົ່ງຄືນ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ຖ້າ (lang == "zh" && script == "Hant") {
        ສົ່ງຄືນ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ຖ້າ (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_getPropertyValueEnum(UCHAR_SCRIPT, script.data()));
        ຖ້າ(scriptCode != USCRIPT_INVALID_CODE) {
            ຖ້າ(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ສົ່ງຄືນ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ສົ່ງຄືນ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(const var_na* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    ຖ້າ(U_FAILURE(*err)) {
        ສົ່ງຄືນ 0;
    }
    ຖ້າ(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ 0;
    }

    triedCode = false;
    const var_na* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ຖ້າ (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ຖ້າ (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ຖ້າ(code!=USCRIPT_INVALID_CODE) {
            ສົ່ງຄືນ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = true;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    ຖ້າ(U_FAILURE(*err) || length != 0) {
        ສົ່ງຄືນ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    ຖ້າ(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        ຖ້າ(U_FAILURE(*err) || length != 0) {
            ສົ່ງຄືນ length;
        }
    }
    ຖ້າ(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ຖ້າ(code!=USCRIPT_INVALID_CODE) {
            ສົ່ງຄືນ setOneCode(code, fillIn, capacity, err);
        }
    }
    ສົ່ງຄືນ 0;
}
