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

ساكن ثابت UScript_کوډ JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساكن ثابت UScript_کوډ KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساكن ثابت UScript_کوډ HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساكن int32_t
سېټ_Codes(ثابت UScript_کوډ *src, int32_t length,
         UScript_کوډ *dest, int32_t capacity, UError_کوډ *err) {
    int32_t i;
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(length > capacity) {
        *err = U_بفر_ډکېدنه_ERROR;
        أرجع length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    أرجع length;
}

ساكن int32_t
سېټ_One_کوډ(UScript_کوډ script, UScript_کوډ *scripts, int32_t capacity, UError_کوډ *err) {
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(1 > capacity) {
        *err = U_بفر_ډکېدنه_ERROR;
        أرجع 1;
    }
    scripts[0] = script;
    أرجع 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScript_کوډ *scripts, int32_t capacity, UError_کوډ *err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    icu::CharString lang;
    icu::CharString script;
    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, تش_اشاره, تش_اشاره, تش_اشاره, *err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    إذا (lang == "ja") {
        أرجع سېټ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    إذا (lang == "ko") {
        أرجع سېټ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    إذا (lang == "zh" && script == "Hant") {
        أرجع سېټ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    إذا (!script.is_تشه()) {
        UScript_کوډ script_کوډ = static_cast<UScript_کوډ>(u_get_Property_ارزښت_Enum(UCHAR_SCRIPT, script.معلومات()));
        إذا(script_کوډ != USCRIPT_INVALID_کوډ) {
            إذا(script_کوډ == USCRIPT_SIMPLIFIED_HAN || script_کوډ == USCRIPT_TRADITIONAL_HAN) {
                script_کوډ = USCRIPT_HAN;
            }
            أرجع سېټ_One_کوډ(script_کوډ, scripts, capacity, err);
        }
    }
    أرجع 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_کوډ(ثابت حرف* nameOrAbbrOrLocale,
                UScript_کوډ* fill_دننه,
                int32_t capacity,
                UError_کوډ* err){
    UBool tried_کوډ;
    UError_کوډ دننی_Error_کوډ;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع 0;
    }
    إذا(nameOrAbbrOrLocale==تش_اشاره ||
            (fill_دننه == تش_اشاره ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    tried_کوډ = خطأ;
    ثابت حرف* وروستی_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    إذا (وروستی_Sep_Ptr==تش_اشاره) {
        وروستی_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    إذا (وروستی_Sep_Ptr==تش_اشاره
            || (وروستی_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (وروستی_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_کوډ کوډ = (UScript_کوډ) u_get_Property_ارزښت_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کوډ!=USCRIPT_INVALID_کوډ) {
            أرجع سېټ_One_کوډ(کوډ, fill_دننه, capacity, err);
        }
        tried_کوډ = ریښتیا;
    }
    دننی_Error_کوډ = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_دننه, capacity, err);
    إذا(U_FAILURE(*err) || length != 0) {
        أرجع length;
    }
    icu::CharString likely = ulocimp_جمع_کړه_Likely_Subtags(nameOrAbbrOrLocale, دننی_Error_کوډ);
    إذا(U_SUCCESS(دننی_Error_کوډ) && دننی_Error_کوډ != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.معلومات(), fill_دننه, capacity, err);
        إذا(U_FAILURE(*err) || length != 0) {
            أرجع length;
        }
    }
    إذا(!tried_کوډ) {
        /* still not found .. try long and abbreviated script names again */
        UScript_کوډ کوډ = (UScript_کوډ) u_get_Property_ارزښت_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کوډ!=USCRIPT_INVALID_کوډ) {
            أرجع سېټ_One_کوډ(کوډ, fill_دننه, capacity, err);
        }
    }
    أرجع 0;
}
