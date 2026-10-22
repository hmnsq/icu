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

ساکن ثابت UScript_رمز JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساکن ثابت UScript_رمز KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساکن ثابت UScript_رمز HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساکن int32_t
سیٹ_Codes(ثابت UScript_رمز *src, int32_t length,
         UScript_رمز *dest, int32_t capacity, UError_رمز *err) {
    int32_t i;
    اگر(U_FAILURE(*err)) { واپس 0; }
    اگر(length > capacity) {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
        واپس length;
    }
    برائے(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    واپس length;
}

ساکن int32_t
سیٹ_One_رمز(UScript_رمز script, UScript_رمز *scripts, int32_t capacity, UError_رمز *err) {
    اگر(U_FAILURE(*err)) { واپس 0; }
    اگر(1 > capacity) {
        *err = U_بفر_حد_سے_تجاوز_ERROR;
        واپس 1;
    }
    scripts[0] = script;
    واپس 1;
}

ساکن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScript_رمز *scripts, int32_t capacity, UError_رمز *err) {
    اگر (U_FAILURE(*err)) { واپس 0; }
    icu::CharString lang;
    icu::CharString script;
    اگر (locale == خالی_اشاریہ) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, *err);
    اگر (U_FAILURE(*err)) { واپس 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    اگر (lang == "ja") {
        واپس سیٹ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    اگر (lang == "ko") {
        واپس سیٹ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    اگر (lang == "zh" && script == "Hant") {
        واپس سیٹ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    اگر (!script.is_خالی_خانے()) {
        UScript_رمز script_رمز = static_cast<UScript_رمز>(u_get_Property_قدر_Enum(UCHAR_SCRIPT, script.ڈیٹا()));
        اگر(script_رمز != USCRIPT_INVALID_رمز) {
            اگر(script_رمز == USCRIPT_SIMPLIFIED_HAN || script_رمز == USCRIPT_TRADITIONAL_HAN) {
                script_رمز = USCRIPT_HAN;
            }
            واپس سیٹ_One_رمز(script_رمز, scripts, capacity, err);
        }
    }
    واپس 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_رمز(ثابت حرف* nameOrAbbrOrLocale,
                UScript_رمز* fill_اندر,
                int32_t capacity,
                UError_رمز* err){
    UBool tried_رمز;
    UError_رمز اندرونی_Error_رمز;
    int32_t length;

    اگر(U_FAILURE(*err)) {
        واپس 0;
    }
    اگر(nameOrAbbrOrLocale==خالی_اشاریہ ||
            (fill_اندر == خالی_اشاریہ ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    tried_رمز = جھوٹ;
    ثابت حرف* آخری_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    اگر (آخری_Sep_Ptr==خالی_اشاریہ) {
        آخری_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    اگر (آخری_Sep_Ptr==خالی_اشاریہ
            || (آخری_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (آخری_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_رمز رمز = (UScript_رمز) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        اگر(رمز!=USCRIPT_INVALID_رمز) {
            واپس سیٹ_One_رمز(رمز, fill_اندر, capacity, err);
        }
        tried_رمز = سچ;
    }
    اندرونی_Error_رمز = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_اندر, capacity, err);
    اگر(U_FAILURE(*err) || length != 0) {
        واپس length;
    }
    icu::CharString likely = ulocimp_جوڑو_Likely_Subtags(nameOrAbbrOrLocale, اندرونی_Error_رمز);
    اگر(U_SUCCESS(اندرونی_Error_رمز) && اندرونی_Error_رمز != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.ڈیٹا(), fill_اندر, capacity, err);
        اگر(U_FAILURE(*err) || length != 0) {
            واپس length;
        }
    }
    اگر(!tried_رمز) {
        /* still not found .. try long and abbreviated script names again */
        UScript_رمز رمز = (UScript_رمز) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        اگر(رمز!=USCRIPT_INVALID_رمز) {
            واپس سیٹ_One_رمز(رمز, fill_اندر, capacity, err);
        }
    }
    واپس 0;
}
