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
    အကယ်၍(U_FAILURE(*err)) { ပြန်ပေး 0; }
    အကယ်၍(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ပြန်ပေး length;
    }
    အတွက်(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ပြန်ပေး length;
}

static int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    အကယ်၍(U_FAILURE(*err)) { ပြန်ပေး 0; }
    အကယ်၍(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ပြန်ပေး 1;
    }
    scripts[0] = script;
    ပြန်ပေး 1;
}

static int32_t
getCodesFromLocale(const var_na *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    အကယ်၍ (U_FAILURE(*err)) { ပြန်ပေး 0; }
    icu::CharString lang;
    icu::CharString script;
    အကယ်၍ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    အကယ်၍ (U_FAILURE(*err)) { ပြန်ပေး 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    အကယ်၍ (lang == "ja") {
        ပြန်ပေး setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    အကယ်၍ (lang == "ko") {
        ပြန်ပေး setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    အကယ်၍ (lang == "zh" && script == "Hant") {
        ပြန်ပေး setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    အကယ်၍ (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_getPropertyValueEnum(UCHAR_SCRIPT, script.data()));
        အကယ်၍(scriptCode != USCRIPT_INVALID_CODE) {
            အကယ်၍(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ပြန်ပေး setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ပြန်ပေး 0;
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

    အကယ်၍(U_FAILURE(*err)) {
        ပြန်ပေး 0;
    }
    အကယ်၍(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး 0;
    }

    triedCode = false;
    const var_na* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    အကယ်၍ (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    အကယ်၍ (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        အကယ်၍(code!=USCRIPT_INVALID_CODE) {
            ပြန်ပေး setOneCode(code, fillIn, capacity, err);
        }
        triedCode = true;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    အကယ်၍(U_FAILURE(*err) || length != 0) {
        ပြန်ပေး length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    အကယ်၍(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        အကယ်၍(U_FAILURE(*err) || length != 0) {
            ပြန်ပေး length;
        }
    }
    အကယ်၍(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        အကယ်၍(code!=USCRIPT_INVALID_CODE) {
            ပြန်ပေး setOneCode(code, fillIn, capacity, err);
        }
    }
    ပြန်ပေး 0;
}
