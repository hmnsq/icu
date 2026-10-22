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

#شامل "unicode/uchar.h"
#شامل "unicode/uscript.h"
#شامل "unicode/uloc.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "ulocimp.h"

ایستا ثابت UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ایستا ثابت UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ایستا ثابت UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ایستا int32_t
setCodes(ثابت UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    اگر(U_FAILURE(*err)) { بازگشت 0; }
    اگر(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        بازگشت length;
    }
    برای(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    بازگشت length;
}

ایستا int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    اگر(U_FAILURE(*err)) { بازگشت 0; }
    اگر(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        بازگشت 1;
    }
    scripts[0] = script;
    بازگشت 1;
}

ایستا int32_t
getCodesFromLocale(ثابت نویسه *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    اگر (U_FAILURE(*err)) { بازگشت 0; }
    icu::CharString lang;
    icu::CharString script;
    اگر (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    اگر (U_FAILURE(*err)) { بازگشت 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    اگر (lang == "ja") {
        بازگشت setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    اگر (lang == "ko") {
        بازگشت setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    اگر (lang == "zh" && script == "Hant") {
        بازگشت setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    اگر (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_مقدار_Enum(UCHAR_SCRIPT, script.data()));
        اگر(scriptCode != USCRIPT_INVALID_CODE) {
            اگر(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            بازگشت setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    بازگشت 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ثابت نویسه* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    اگر(U_FAILURE(*err)) {
        بازگشت 0;
    }
    اگر(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    triedCode = نادرست;
    ثابت نویسه* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    اگر (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    اگر (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_مقدار_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        اگر(code!=USCRIPT_INVALID_CODE) {
            بازگشت setOneCode(code, fillIn, capacity, err);
        }
        triedCode = درست;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    اگر(U_FAILURE(*err) || length != 0) {
        بازگشت length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    اگر(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        اگر(U_FAILURE(*err) || length != 0) {
            بازگشت length;
        }
    }
    اگر(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_مقدار_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        اگر(code!=USCRIPT_INVALID_CODE) {
            بازگشت setOneCode(code, fillIn, capacity, err);
        }
    }
    بازگشت 0;
}
