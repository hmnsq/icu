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

स्थिर ध्रुव UScript_संहिता JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
स्थिर ध्रुव UScript_संहिता KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
स्थिर ध्रुव UScript_संहिता HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

स्थिर int32_t
समुच्चयः_Codes(ध्रुव UScript_संहिता *src, int32_t length,
         UScript_संहिता *dest, int32_t capacity, UError_संहिता *err) {
    int32_t i;
    यदि(U_FAILURE(*err)) { प्रत्यागम 0; }
    यदि(length > capacity) {
        *err = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम length;
    }
    प्रति(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    प्रत्यागम length;
}

स्थिर int32_t
समुच्चयः_One_संहिता(UScript_संहिता script, UScript_संहिता *scripts, int32_t capacity, UError_संहिता *err) {
    यदि(U_FAILURE(*err)) { प्रत्यागम 0; }
    यदि(1 > capacity) {
        *err = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम 1;
    }
    scripts[0] = script;
    प्रत्यागम 1;
}

स्थिर int32_t
getCodesFromLocale(ध्रुव वर्ण *locale,
                   UScript_संहिता *scripts, int32_t capacity, UError_संहिता *err) {
    यदि (U_FAILURE(*err)) { प्रत्यागम 0; }
    icu::CharString lang;
    icu::CharString script;
    यदि (locale == शून्यसूचकः) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, *err);
    यदि (U_FAILURE(*err)) { प्रत्यागम 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    यदि (lang == "ja") {
        प्रत्यागम समुच्चयः_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    यदि (lang == "ko") {
        प्रत्यागम समुच्चयः_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    यदि (lang == "zh" && script == "Hant") {
        प्रत्यागम समुच्चयः_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    यदि (!script.is_रिक्तानि()) {
        UScript_संहिता script_संहिता = static_cast<UScript_संहिता>(u_get_Property_मानम्_Enum(UCHAR_SCRIPT, script.दत्त()));
        यदि(script_संहिता != USCRIPT_INVALID_संहिता) {
            यदि(script_संहिता == USCRIPT_SIMPLIFIED_HAN || script_संहिता == USCRIPT_TRADITIONAL_HAN) {
                script_संहिता = USCRIPT_HAN;
            }
            प्रत्यागम समुच्चयः_One_संहिता(script_संहिता, scripts, capacity, err);
        }
    }
    प्रत्यागम 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_संहिता(ध्रुव वर्ण* nameOrAbbrOrLocale,
                UScript_संहिता* fill_अन्तर्,
                int32_t capacity,
                UError_संहिता* err){
    UBool tried_संहिता;
    UError_संहिता आन्तरिकम्_Error_संहिता;
    int32_t length;

    यदि(U_FAILURE(*err)) {
        प्रत्यागम 0;
    }
    यदि(nameOrAbbrOrLocale==शून्यसूचकः ||
            (fill_अन्तर् == शून्यसूचकः ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    tried_संहिता = असत्य;
    ध्रुव वर्ण* चरमः_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    यदि (चरमः_Sep_Ptr==शून्यसूचकः) {
        चरमः_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    यदि (चरमः_Sep_Ptr==शून्यसूचकः
            || (चरमः_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (चरमः_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_संहिता संहिता = (UScript_संहिता) u_get_Property_मानम्_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        यदि(संहिता!=USCRIPT_INVALID_संहिता) {
            प्रत्यागम समुच्चयः_One_संहिता(संहिता, fill_अन्तर्, capacity, err);
        }
        tried_संहिता = सत्य;
    }
    आन्तरिकम्_Error_संहिता = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_अन्तर्, capacity, err);
    यदि(U_FAILURE(*err) || length != 0) {
        प्रत्यागम length;
    }
    icu::CharString likely = ulocimp_सङ्कलय_Likely_Subtags(nameOrAbbrOrLocale, आन्तरिकम्_Error_संहिता);
    यदि(U_SUCCESS(आन्तरिकम्_Error_संहिता) && आन्तरिकम्_Error_संहिता != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.दत्त(), fill_अन्तर्, capacity, err);
        यदि(U_FAILURE(*err) || length != 0) {
            प्रत्यागम length;
        }
    }
    यदि(!tried_संहिता) {
        /* still not found .. try long and abbreviated script names again */
        UScript_संहिता संहिता = (UScript_संहिता) u_get_Property_मानम्_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        यदि(संहिता!=USCRIPT_INVALID_संहिता) {
            प्रत्यागम समुच्चयः_One_संहिता(संहिता, fill_अन्तर्, capacity, err);
        }
    }
    प्रत्यागम 0;
}
