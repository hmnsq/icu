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

#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ulocimp.h"

സ്ഥിതം സ്ഥിരം UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
സ്ഥിതം സ്ഥിരം UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
സ്ഥിതം സ്ഥിരം UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

സ്ഥിതം int32_t
setCodes(സ്ഥിരം UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    എങ്കിൽ(U_FAILURE(*err)) { തിരികെ 0; }
    എങ്കിൽ(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ length;
    }
    വേണ്ടി(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    തിരികെ length;
}

സ്ഥിതം int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    എങ്കിൽ(U_FAILURE(*err)) { തിരികെ 0; }
    എങ്കിൽ(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ 1;
    }
    scripts[0] = script;
    തിരികെ 1;
}

സ്ഥിതം int32_t
getCodesFromLocale(സ്ഥിരം അക്ഷരം *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ 0; }
    icu::CharString lang;
    icu::CharString script;
    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    എങ്കിൽ (lang == "ja") {
        തിരികെ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    എങ്കിൽ (lang == "ko") {
        തിരികെ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    എങ്കിൽ (lang == "zh" && script == "Hant") {
        തിരികെ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    എങ്കിൽ (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_മൂല്യം_Enum(UCHAR_SCRIPT, script.data()));
        എങ്കിൽ(scriptCode != USCRIPT_INVALID_CODE) {
            എങ്കിൽ(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            തിരികെ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    തിരികെ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(സ്ഥിരം അക്ഷരം* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    എങ്കിൽ(U_FAILURE(*err)) {
        തിരികെ 0;
    }
    എങ്കിൽ(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    triedCode = അസത്യം;
    സ്ഥിരം അക്ഷരം* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    എങ്കിൽ (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    എങ്കിൽ (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_മൂല്യം_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        എങ്കിൽ(code!=USCRIPT_INVALID_CODE) {
            തിരികെ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = സത്യം;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    എങ്കിൽ(U_FAILURE(*err) || length != 0) {
        തിരികെ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    എങ്കിൽ(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        എങ്കിൽ(U_FAILURE(*err) || length != 0) {
            തിരികെ length;
        }
    }
    എങ്കിൽ(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_മൂല്യം_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        എങ്കിൽ(code!=USCRIPT_INVALID_CODE) {
            തിരികെ setOneCode(code, fillIn, capacity, err);
        }
    }
    തിരികെ 0;
}
