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

#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ulocimp.h"

स्थिर अचर UScriptCode JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
स्थिर अचर UScriptCode KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
स्थिर अचर UScriptCode HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

स्थिर int32_t
setCodes(अचर UScriptCode *src, int32_t length,
         UScriptCode *dest, int32_t capacity, UErrorCode *err) {
    int32_t i;
    यदि(U_FAILURE(*err)) { फर्काऊ 0; }
    यदि(length > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        फर्काऊ length;
    }
    लागि(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    फर्काऊ length;
}

स्थिर int32_t
setOneCode(UScriptCode script, UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    यदि(U_FAILURE(*err)) { फर्काऊ 0; }
    यदि(1 > capacity) {
        *err = U_BUFFER_OVERFLOW_ERROR;
        फर्काऊ 1;
    }
    scripts[0] = script;
    फर्काऊ 1;
}

स्थिर int32_t
getCodesFromLocale(अचर वर्ण *locale,
                   UScriptCode *scripts, int32_t capacity, UErrorCode *err) {
    यदि (U_FAILURE(*err)) { फर्काऊ 0; }
    icu::CharString lang;
    icu::CharString script;
    यदि (locale == nullptr) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, nullptr, nullptr, nullptr, *err);
    यदि (U_FAILURE(*err)) { फर्काऊ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    यदि (lang == "ja") {
        फर्काऊ setCodes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    यदि (lang == "ko") {
        फर्काऊ setCodes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    यदि (lang == "zh" && script == "Hant") {
        फर्काऊ setCodes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    यदि (!script.isEmpty()) {
        UScriptCode scriptCode = static_cast<UScriptCode>(u_get_Property_मान_Enum(UCHAR_SCRIPT, script.data()));
        यदि(scriptCode != USCRIPT_INVALID_CODE) {
            यदि(scriptCode == USCRIPT_SIMPLIFIED_HAN || scriptCode == USCRIPT_TRADITIONAL_HAN) {
                scriptCode = USCRIPT_HAN;
            }
            फर्काऊ setOneCode(scriptCode, scripts, capacity, err);
        }
    }
    फर्काऊ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_getCode(अचर वर्ण* nameOrAbbrOrLocale,
                UScriptCode* fillIn,
                int32_t capacity,
                UErrorCode* err){
    UBool triedCode;
    UErrorCode internalErrorCode;
    int32_t length;

    यदि(U_FAILURE(*err)) {
        फर्काऊ 0;
    }
    यदि(nameOrAbbrOrLocale==nullptr ||
            (fillIn == nullptr ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    triedCode = असत्य;
    अचर वर्ण* lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    यदि (lastSepPtr==nullptr) {
        lastSepPtr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    यदि (lastSepPtr==nullptr
            || (lastSepPtr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (lastSepPtr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScriptCode code = (UScriptCode) u_get_Property_मान_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        यदि(code!=USCRIPT_INVALID_CODE) {
            फर्काऊ setOneCode(code, fillIn, capacity, err);
        }
        triedCode = सत्य;
    }
    internalErrorCode = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fillIn, capacity, err);
    यदि(U_FAILURE(*err) || length != 0) {
        फर्काऊ length;
    }
    icu::CharString likely = ulocimp_addLikelySubtags(nameOrAbbrOrLocale, internalErrorCode);
    यदि(U_SUCCESS(internalErrorCode) && internalErrorCode != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.data(), fillIn, capacity, err);
        यदि(U_FAILURE(*err) || length != 0) {
            फर्काऊ length;
        }
    }
    यदि(!triedCode) {
        /* still not found .. try long and abbreviated script names again */
        UScriptCode code = (UScriptCode) u_get_Property_मान_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        यदि(code!=USCRIPT_INVALID_CODE) {
            फर्काऊ setOneCode(code, fillIn, capacity, err);
        }
    }
    फर्काऊ 0;
}
