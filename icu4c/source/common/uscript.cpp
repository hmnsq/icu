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

#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "ulocimp.h"

સ્થિર અચલ UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
સ્થિર અચલ UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
સ્થિર અચલ UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

સ્થિર int32_t
setCodes(અચલ UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    જો(U_FAILURE(*err)) { પાછા 0; }
    જો(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        પાછા length;
    }
    માટે(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    પાછા length;
}

સ્થિર int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    જો(U_FAILURE(*err)) { પાછા 0; }
    જો(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        પાછા 1;
    }
    scripts[0] = script;
    પાછા 1;
}

સ્થિર int32_t
getCodesFromLocale(અચલ અક્ષર *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    જો (U_FAILURE(*err)) { પાછા 0; }
    icu::CharString lang;
    icu::CharString script;
    જો (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    જો (U_FAILURE(*err)) { પાછા 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    જો (lang == "ja") {
        પાછા setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    જો (lang == "ko") {
        પાછા setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    જો (lang == "zh" && script == "Hant") {
        પાછા setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    જો (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_કિંમત_Enum(UCHAR_SCRIPT, script.data()));
        જો(scriptCode != USCRIPT_INVALID_CODE) {
            જો(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            પાછા setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    પાછા 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(અચલ અક્ષર* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    જો(U_FAILURE(*err)) {
        પાછા 0;
    }
    જો(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    triedCode = ખોટું;
    અચલ અક્ષર* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    જો (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    જો (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_કિંમત_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        જો(code!=USCRIPT_INVALID_CODE) {
            પાછા setOneCode(code, fillIn, capacity, err);
        }
        triedCode = સાચું;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    જો(U_FAILURE(*err) || length != 0) {
        પાછા length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    જો(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        જો(U_FAILURE(*err) || length != 0) {
            પાછા length;
        }
    }
    જો(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_કિંમત_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        જો(code!=USCRIPT_INVALID_CODE) {
            પાછા setOneCode(code, fillIn, capacity, err);
        }
    }
    પાછા 0;
}
