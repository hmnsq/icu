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

#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ulocimp.h"

நிலையான மாறிலி UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
நிலையான மாறிலி UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
நிலையான மாறிலி UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

நிலையான int32_t
setCodes(மாறிலி UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    எனில்(U_FAILURE(*err)) { திரும்பு 0; }
    எனில்(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு length;
    }
    க்கு(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    திரும்பு length;
}

நிலையான int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    எனில்(U_FAILURE(*err)) { திரும்பு 0; }
    எனில்(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு 1;
    }
    scripts[0] = script;
    திரும்பு 1;
}

நிலையான int32_t
getCodesFromLocale(மாறிலி எழுத்து *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    எனில் (U_FAILURE(*err)) { திரும்பு 0; }
    icu::CharString lang;
    icu::CharString script;
    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    எனில் (U_FAILURE(*err)) { திரும்பு 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    எனில் (lang == "ja") {
        திரும்பு setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    எனில் (lang == "ko") {
        திரும்பு setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    எனில் (lang == "zh" && script == "Hant") {
        திரும்பு setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    எனில் (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_மதிப்பு_Enum(UCHAR_SCRIPT, script.data()));
        எனில்(scriptCode != USCRIPT_INVALID_CODE) {
            எனில்(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            திரும்பு setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    திரும்பு 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(மாறிலி எழுத்து* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    எனில்(U_FAILURE(*err)) {
        திரும்பு 0;
    }
    எனில்(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    triedCode = பொய்;
    மாறிலி எழுத்து* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    எனில் (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    எனில் (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_மதிப்பு_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        எனில்(code!=USCRIPT_INVALID_CODE) {
            திரும்பு setOneCode(code, fillIn, capacity, err);
        }
        triedCode = உண்மை;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    எனில்(U_FAILURE(*err) || length != 0) {
        திரும்பு length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    எனில்(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        எனில்(U_FAILURE(*err) || length != 0) {
            திரும்பு length;
        }
    }
    எனில்(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_மதிப்பு_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        எனில்(code!=USCRIPT_INVALID_CODE) {
            திரும்பு setOneCode(code, fillIn, capacity, err);
        }
    }
    திரும்பு 0;
}
