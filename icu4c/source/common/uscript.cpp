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

#ضمن "unicode/uchar.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/uloc.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "ulocimp.h"

ساكن ثابت UScript_ڪوڊ JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساكن ثابت UScript_ڪوڊ KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساكن ثابت UScript_ڪوڊ HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساكن int32_t
سيٽ_Codes(ثابت UScript_ڪوڊ *src, int32_t length,
         UScript_ڪوڊ *dest, int32_t capacity, UError_ڪوڊ *err) {
    int32_t i;
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(length > capacity) {
        *err = U_بفر_حد_کان_مٿي_ERROR;
        أرجع length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    أرجع length;
}

ساكن int32_t
سيٽ_One_ڪوڊ(UScript_ڪوڊ script, UScript_ڪوڊ *scripts, int32_t capacity, UError_ڪوڊ *err) {
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(1 > capacity) {
        *err = U_بفر_حد_کان_مٿي_ERROR;
        أرجع 1;
    }
    scripts[0] = script;
    أرجع 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScript_ڪوڊ *scripts, int32_t capacity, UError_ڪوڊ *err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    icu::CharString lang;
    icu::CharString script;
    إذا (locale == خالي_اشارو) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, خالي_اشارو, خالي_اشارو, خالي_اشارو, *err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    إذا (lang == "ja") {
        أرجع سيٽ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    إذا (lang == "ko") {
        أرجع سيٽ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    إذا (lang == "zh" && script == "Hant") {
        أرجع سيٽ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    إذا (!script.is_خالي_جايون()) {
        UScript_ڪوڊ script_ڪوڊ = static_cast<UScript_ڪوڊ>(u_get_Property_قدر_Enum(UCHAR_SCRIPT, script.معلومات()));
        إذا(script_ڪوڊ != USCRIPT_INVALID_ڪوڊ) {
            إذا(script_ڪوڊ == USCRIPT_SIMPLIFIED_HAN || script_ڪوڊ == USCRIPT_TRADITIONAL_HAN) {
                script_ڪوڊ = USCRIPT_HAN;
            }
            أرجع سيٽ_One_ڪوڊ(script_ڪوڊ, scripts, capacity, err);
        }
    }
    أرجع 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_ڪوڊ(ثابت حرف* nameOrAbbrOrLocale,
                UScript_ڪوڊ* fill_اندريون,
                int32_t capacity,
                UError_ڪوڊ* err){
    UBool tried_ڪوڊ;
    UError_ڪوڊ اندروني_Error_ڪوڊ;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع 0;
    }
    إذا(nameOrAbbrOrLocale==خالي_اشارو ||
            (fill_اندريون == خالي_اشارو ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    tried_ڪوڊ = خطأ;
    ثابت حرف* آخري_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    إذا (آخري_Sep_Ptr==خالي_اشارو) {
        آخري_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    إذا (آخري_Sep_Ptr==خالي_اشارو
            || (آخري_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (آخري_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_ڪوڊ ڪوڊ = (UScript_ڪوڊ) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(ڪوڊ!=USCRIPT_INVALID_ڪوڊ) {
            أرجع سيٽ_One_ڪوڊ(ڪوڊ, fill_اندريون, capacity, err);
        }
        tried_ڪوڊ = سچ;
    }
    اندروني_Error_ڪوڊ = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_اندريون, capacity, err);
    إذا(U_FAILURE(*err) || length != 0) {
        أرجع length;
    }
    icu::CharString likely = ulocimp_جوڙيو_Likely_Subtags(nameOrAbbrOrLocale, اندروني_Error_ڪوڊ);
    إذا(U_SUCCESS(اندروني_Error_ڪوڊ) && اندروني_Error_ڪوڊ != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.معلومات(), fill_اندريون, capacity, err);
        إذا(U_FAILURE(*err) || length != 0) {
            أرجع length;
        }
    }
    إذا(!tried_ڪوڊ) {
        /* still not found .. try long and abbreviated script names again */
        UScript_ڪوڊ ڪوڊ = (UScript_ڪوڊ) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(ڪوڊ!=USCRIPT_INVALID_ڪوڊ) {
            أرجع سيٽ_One_ڪوڊ(ڪوڊ, fill_اندريون, capacity, err);
        }
    }
    أرجع 0;
}
