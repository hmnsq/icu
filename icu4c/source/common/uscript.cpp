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

#समावेश "unicode/uchar.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/uloc.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "ulocimp.h"

अटल स्थिर UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
अटल स्थिर UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
अटल स्थिर UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

अटल int32_t
setCodes(स्थिर UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    अगर(U_FAILURE(*err)) { वापस 0; }
    अगर(length > capacity) {
        *err = U_बफर_OVERFLOW_ERROR;
        वापस length;
    }
    क्रम(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    वापस length;
}

अटल int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    अगर(U_FAILURE(*err)) { वापस 0; }
    अगर(1 > capacity) {
        *err = U_बफर_OVERFLOW_ERROR;
        वापस 1;
    }
    scripts[0] = script;
    वापस 1;
}

अटल int32_t
getCodesFromLocale(स्थिर अक्षर *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    अगर (U_FAILURE(*err)) { वापस 0; }
    icu::CharString lang;
    icu::CharString script;
    अगर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    अगर (U_FAILURE(*err)) { वापस 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    अगर (lang == "ja") {
        वापस setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    अगर (lang == "ko") {
        वापस setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    अगर (lang == "zh" && script == "Hant") {
        वापस setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    अगर (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, script.तथ्य()));
        अगर(scriptCode != USCRIPT_INVALID_CODE) {
            अगर(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            वापस setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    वापस 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(स्थिर अक्षर* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    अगर(U_FAILURE(*err)) {
        वापस 0;
    }
    अगर(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    triedCode = false;
    स्थिर अक्षर* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    अगर (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    अगर (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        अगर(code!=USCRIPT_INVALID_CODE) {
            वापस setOneCode(code, fillIn, capacity, err);
        }
        triedCode = true;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    अगर(U_FAILURE(*err) || length != 0) {
        वापस length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    अगर(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.तथ्य(), fillIn, capacity, err);
        अगर(U_FAILURE(*err) || length != 0) {
            वापस length;
        }
    }
    अगर(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        अगर(code!=USCRIPT_INVALID_CODE) {
            वापस setOneCode(code, fillIn, capacity, err);
        }
    }
    वापस 0;
}
