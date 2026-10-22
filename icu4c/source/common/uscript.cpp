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

ساكن ثابت UScript_کوڈ JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساكن ثابت UScript_کوڈ KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساكن ثابت UScript_کوڈ HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساكن int32_t
سیٹ_Codes(ثابت UScript_کوڈ *src, int32_t length,
         UScript_کوڈ *dest, int32_t capacity, UError_کوڈ *err) {
    int32_t i;
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(length > capacity) {
        *err = U_بفر_حد_توں_ودھ_ERROR;
        أرجع length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    أرجع length;
}

ساكن int32_t
سیٹ_One_کوڈ(UScript_کوڈ script, UScript_کوڈ *scripts, int32_t capacity, UError_کوڈ *err) {
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(1 > capacity) {
        *err = U_بفر_حد_توں_ودھ_ERROR;
        أرجع 1;
    }
    scripts[0] = script;
    أرجع 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScript_کوڈ *scripts, int32_t capacity, UError_کوڈ *err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    icu::CharString lang;
    icu::CharString script;
    إذا (locale == خالی_سوچک) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, خالی_سوچک, خالی_سوچک, خالی_سوچک, *err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    إذا (lang == "ja") {
        أرجع سیٹ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    إذا (lang == "ko") {
        أرجع سیٹ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    إذا (lang == "zh" && script == "Hant") {
        أرجع سیٹ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    إذا (!script.is_خالی_تھاواں()) {
        UScript_کوڈ script_کوڈ = static_cast<UScript_کوڈ>(u_get_Property_قدر_Enum(UCHAR_SCRIPT, script.انکڑے()));
        إذا(script_کوڈ != USCRIPT_INVALID_کوڈ) {
            إذا(script_کوڈ == USCRIPT_SIMPLIFIED_HAN || script_کوڈ == USCRIPT_TRADITIONAL_HAN) {
                script_کوڈ = USCRIPT_HAN;
            }
            أرجع سیٹ_One_کوڈ(script_کوڈ, scripts, capacity, err);
        }
    }
    أرجع 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_کوڈ(ثابت حرف* nameOrAbbrOrLocale,
                UScript_کوڈ* fill_وچ,
                int32_t capacity,
                UError_کوڈ* err){
    UBool tried_کوڈ;
    UError_کوڈ اندرونی_Error_کوڈ;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع 0;
    }
    إذا(nameOrAbbrOrLocale==خالی_سوچک ||
            (fill_وچ == خالی_سوچک ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    tried_کوڈ = خطأ;
    ثابت حرف* آخری_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    إذا (آخری_Sep_Ptr==خالی_سوچک) {
        آخری_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    إذا (آخری_Sep_Ptr==خالی_سوچک
            || (آخری_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (آخری_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_کوڈ کوڈ = (UScript_کوڈ) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کوڈ!=USCRIPT_INVALID_کوڈ) {
            أرجع سیٹ_One_کوڈ(کوڈ, fill_وچ, capacity, err);
        }
        tried_کوڈ = سچ;
    }
    اندرونی_Error_کوڈ = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_وچ, capacity, err);
    إذا(U_FAILURE(*err) || length != 0) {
        أرجع length;
    }
    icu::CharString likely = ulocimp_جوڑو_Likely_Subtags(nameOrAbbrOrLocale, اندرونی_Error_کوڈ);
    إذا(U_SUCCESS(اندرونی_Error_کوڈ) && اندرونی_Error_کوڈ != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.انکڑے(), fill_وچ, capacity, err);
        إذا(U_FAILURE(*err) || length != 0) {
            أرجع length;
        }
    }
    إذا(!tried_کوڈ) {
        /* still not found .. try long and abbreviated script names again */
        UScript_کوڈ کوڈ = (UScript_کوڈ) u_get_Property_قدر_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کوڈ!=USCRIPT_INVALID_کوڈ) {
            أرجع سیٹ_One_کوڈ(کوڈ, fill_وچ, capacity, err);
        }
    }
    أرجع 0;
}
