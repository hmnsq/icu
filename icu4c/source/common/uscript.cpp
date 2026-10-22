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
    ܐܢ(U_FAILURE(*err)) { ܦܢܐ 0; }
    ܐܢ(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ܦܢܐ length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ܦܢܐ length;
}

ساكن int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ܐܢ(U_FAILURE(*err)) { ܦܢܐ 0; }
    ܐܢ(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ܦܢܐ 1;
    }
    scripts[0] = script;
    ܦܢܐ 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ 0; }
    icu::CharString lang;
    icu::CharString script;
    ܐܢ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ܐܢ (lang == "ja") {
        ܦܢܐ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ܐܢ (lang == "ko") {
        ܦܢܐ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ܐܢ (lang == "zh" && script == "Hant") {
        ܦܢܐ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ܐܢ (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_getPropertyValueEnum(UCHAR_SCRIPT, script.data()));
        ܐܢ(scriptCode != USCRIPT_INVALID_CODE) {
            ܐܢ(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ܦܢܐ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ܦܢܐ 0;
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

    ܐܢ(U_FAILURE(*err)) {
        ܦܢܐ 0;
    }
    ܐܢ(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }

    triedCode = ܕܓܠܐ;
    ثابت حرف* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ܐܢ (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ܐܢ (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ܐܢ(code!=USCRIPT_INVALID_CODE) {
            ܦܢܐ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = ܫܪܪܐ;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    ܐܢ(U_FAILURE(*err) || length != 0) {
        ܦܢܐ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    ܐܢ(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        ܐܢ(U_FAILURE(*err) || length != 0) {
            ܦܢܐ length;
        }
    }
    ܐܢ(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_getPropertyValueEnum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ܐܢ(code!=USCRIPT_INVALID_CODE) {
            ܦܢܐ setOneCode(code, fillIn, capacity, err);
        }
    }
    ܦܢܐ 0;
}
