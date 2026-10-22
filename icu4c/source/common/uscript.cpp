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

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"

ಸ್ಥಿರ ಧ್ರುವ UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ಸ್ಥಿರ ಧ್ರುವ UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ಸ್ಥಿರ ಧ್ರುವ UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ಸ್ಥಿರ int32_t
setCodes(ಧ್ರುವ UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    ಯದಿ(U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ length;
    }
    ಪ್ರತಿ(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ಯದಿ(U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    scripts[0] = script;
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ int32_t
getCodesFromLocale(ಧ್ರುವ ವರ್ಣ *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    icu::CharString lang;
    icu::CharString script;
    ಯದಿ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ಯದಿ (lang == "ja") {
        ಪ್ರತ್ಯಾಗಮ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ಯದಿ (lang == "ko") {
        ಪ್ರತ್ಯಾಗಮ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ಯದಿ (lang == "zh" && script == "Hant") {
        ಪ್ರತ್ಯಾಗಮ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ಯದಿ (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, script.data()));
        ಯದಿ(scriptCode != USCRIPT_INVALID_CODE) {
            ಯದಿ(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ಪ್ರತ್ಯಾಗಮ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ಧ್ರುವ ವರ್ಣ* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    ಯದಿ(U_FAILURE(*err)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    triedCode = ಅಸತ್ಯ;
    ಧ್ರುವ ವರ್ಣ* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ಯದಿ (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ಯದಿ (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ಯದಿ(code!=USCRIPT_INVALID_CODE) {
            ಪ್ರತ್ಯಾಗಮ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = ಸತ್ಯ;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    ಯದಿ(U_FAILURE(*err) || length != 0) {
        ಪ್ರತ್ಯಾಗಮ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    ಯದಿ(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        ಯದಿ(U_FAILURE(*err) || length != 0) {
            ಪ್ರತ್ಯಾಗಮ length;
        }
    }
    ಯದಿ(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ಯದಿ(code!=USCRIPT_INVALID_CODE) {
            ಪ್ರತ್ಯಾಗಮ setOneCode(code, fillIn, capacity, err);
        }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}
