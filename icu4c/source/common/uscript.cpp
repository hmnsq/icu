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

#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "ulocimp.h"

স্থির ধ্রুৱ UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
স্থির ধ্রুৱ UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
স্থির ধ্রুৱ UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

স্থির int32_t
setCodes(ধ্রুৱ UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    যদি(U_FAILURE(*err)) { ঘূৰাই 0; }
    যদি(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই length;
    }
    বাবে(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ঘূৰাই length;
}

স্থির int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    যদি(U_FAILURE(*err)) { ঘূৰাই 0; }
    যদি(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই 1;
    }
    scripts[0] = script;
    ঘূৰাই 1;
}

স্থির int32_t
getCodesFromLocale(ধ্রুৱ ৱর্ণ *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    যদি (U_FAILURE(*err)) { ঘূৰাই 0; }
    icu::CharString lang;
    icu::CharString script;
    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    যদি (U_FAILURE(*err)) { ঘূৰাই 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    যদি (lang == "ja") {
        ঘূৰাই setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    যদি (lang == "ko") {
        ঘূৰাই setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    যদি (lang == "zh" && script == "Hant") {
        ঘূৰাই setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    যদি (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_মান_Enum(UCHAR_SCRIPT, script.data()));
        যদি(scriptCode != USCRIPT_INVALID_CODE) {
            যদি(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            ঘূৰাই setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    ঘূৰাই 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(ধ্রুৱ ৱর্ণ* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    যদি(U_FAILURE(*err)) {
        ঘূৰাই 0;
    }
    যদি(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    triedCode = মিছা;
    ধ্রুৱ ৱর্ণ* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
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
            ঘূৰাই setOneCode(code, fillIn, capacity, err);
        }
        triedCode = সঁচা;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    যদি(U_FAILURE(*err) || length != 0) {
        ঘূৰাই length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    যদি(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        যদি(U_FAILURE(*err) || length != 0) {
            ঘূৰাই length;
        }
    }
    যদি(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_মান_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        যদি(code!=USCRIPT_INVALID_CODE) {
            ঘূৰাই setOneCode(code, fillIn, capacity, err);
        }
    }
    ঘূৰাই 0;
}
