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

#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"

ସ୍ଥିର ଅଚଳ UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ସ୍ଥିର ଅଚଳ UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ସ୍ଥିର ଅଚଳ UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ସ୍ଥିର int32_t
setCodes(ଅଚଳ UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    ଯଦି(U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    ଯଦି(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ length;
    }
    ପାଇଁ(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ଯଦି(U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    ଯଦି(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ 1;
    }
    scripts[0] = script;
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର int32_t
getCodesFromLocale(ଅଚଳ ଅକ୍ଷର *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    icu::CharString lang;
    icu::CharString script;
    ଯଦି (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ଯଦି (lang == "ja") {
        ଫେରନ୍ତୁ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ଯଦି (lang == "ko") {
        ଫେରନ୍ତୁ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ଯଦି (lang == "zh" && script == "Hant") {
        ଫେରନ୍ତୁ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ଯଦି (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_ମୂଲ୍ୟ_Enum(UCHAR_SCRIPT, script.data()));
        ଯଦି(scriptCode != USCRIPT_INVALID_CODE) {
            ଯଦି(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ଫେରନ୍ତୁ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ଫେରନ୍ତୁ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ଅଚଳ ଅକ୍ଷର* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    ଯଦି(U_FAILURE(*err)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    triedCode = ମିଥ୍ୟା;
    ଅଚଳ ଅକ୍ଷର* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ଯଦି (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ଯଦି (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_ମୂଲ୍ୟ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ଯଦି(code!=USCRIPT_INVALID_CODE) {
            ଫେରନ୍ତୁ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = ସତ୍ୟ;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    ଯଦି(U_FAILURE(*err) || length != 0) {
        ଫେରନ୍ତୁ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    ଯଦି(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        ଯଦି(U_FAILURE(*err) || length != 0) {
            ଫେରନ୍ତୁ length;
        }
    }
    ଯଦି(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_ମୂଲ୍ୟ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ଯଦି(code!=USCRIPT_INVALID_CODE) {
            ଫେରନ୍ତୁ setOneCode(code, fillIn, capacity, err);
        }
    }
    ଫେରନ୍ତୁ 0;
}
