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

#ضمن "unicode/uchar.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/uloc.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "ulocimp.h"

ساكن ثابت UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساكن ثابت UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساكن ثابت UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساكن int32_t
setCodes(ثابت UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        أرجع length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    أرجع length;
}

ساكن int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        أرجع 1;
    }
    scripts[0] = script;
    أرجع 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    icu::CharString lang;
    icu::CharString script;
    إذا (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    إذا (lang == "ja") {
        أرجع setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    إذا (lang == "ko") {
        أرجع setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    إذا (lang == "zh" && script == "Hant") {
        أرجع setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    إذا (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_نيلاي_Enum(UCHAR_SCRIPT, script.data()));
        إذا(scriptCode != USCRIPT_INVALID_CODE) {
            إذا(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            أرجع setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    أرجع 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ثابت حرف* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع 0;
    }
    إذا(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    triedCode = خطأ;
    ثابت حرف* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    إذا (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    إذا (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_نيلاي_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(code!=USCRIPT_INVALID_CODE) {
            أرجع setOneCode(code, fillIn, capacity, err);
        }
        triedCode = true;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    إذا(U_FAILURE(*err) || length != 0) {
        أرجع length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    إذا(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        إذا(U_FAILURE(*err) || length != 0) {
            أرجع length;
        }
    }
    إذا(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_نيلاي_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(code!=USCRIPT_INVALID_CODE) {
            أرجع setOneCode(code, fillIn, capacity, err);
        }
    }
    أرجع 0;
}
