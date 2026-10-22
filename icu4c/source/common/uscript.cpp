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

#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ulocimp.h"

স্থির ধ্রুবক UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
স্থির ধ্রুবক UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
স্থির ধ্রুবক UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

স্থির int32_t
setCodes(ধ্রুবক UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    যদি(U_FAILURE(*err)) { ফেরত 0; }
    যদি(length > capacity) {
        *err = U_বাফার_OVERFLOW_ERROR;
        ফেরত length;
    }
    জন্য(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ফেরত length;
}

স্থির int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    যদি(U_FAILURE(*err)) { ফেরত 0; }
    যদি(1 > capacity) {
        *err = U_বাফার_OVERFLOW_ERROR;
        ফেরত 1;
    }
    scripts[0] = script;
    ফেরত 1;
}

স্থির int32_t
getCodesFromLocale(ধ্রুবক অক্ষর *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    যদি (U_FAILURE(*err)) { ফেরত 0; }
    icu::CharString lang;
    icu::CharString script;
    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    যদি (U_FAILURE(*err)) { ফেরত 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    যদি (lang == "ja") {
        ফেরত setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    যদি (lang == "ko") {
        ফেরত setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    যদি (lang == "zh" && script == "Hant") {
        ফেরত setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    যদি (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_মান_Enum(UCHAR_SCRIPT, script.তথ্য()));
        যদি(scriptCode != USCRIPT_INVALID_CODE) {
            যদি(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ফেরত setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ফেরত 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ধ্রুবক অক্ষর* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    যদি(U_FAILURE(*err)) {
        ফেরত 0;
    }
    যদি(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    triedCode = মিথ্যা;
    ধ্রুবক অক্ষর* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    যদি (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    যদি (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_মান_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        যদি(code!=USCRIPT_INVALID_CODE) {
            ফেরত setOneCode(code, fillIn, capacity, err);
        }
        triedCode = সত্য;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    যদি(U_FAILURE(*err) || length != 0) {
        ফেরত length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    যদি(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.তথ্য(), fillIn, capacity, err);
        যদি(U_FAILURE(*err) || length != 0) {
            ফেরত length;
        }
    }
    যদি(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_মান_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        যদি(code!=USCRIPT_INVALID_CODE) {
            ফেরত setOneCode(code, fillIn, capacity, err);
        }
    }
    ফেরত 0;
}
