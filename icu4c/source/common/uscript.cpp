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

ساكن ثابت UScript_کد JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ساكن ثابت UScript_کد KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ساكن ثابت UScript_کد HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ساكن int32_t
مجموعه_Codes(ثابت UScript_کد *src, int32_t length,
         UScript_کد *dest, int32_t capacity, UError_کد *err) {
    int32_t i;
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(length > capacity) {
        *err = U_میانگیر_سرریز_ERROR;
        أرجع length;
    }
    لكل(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    أرجع length;
}

ساكن int32_t
مجموعه_One_کد(UScript_کد script, UScript_کد *scripts, int32_t capacity, UError_کد *err) {
    إذا(U_FAILURE(*err)) { أرجع 0; }
    إذا(1 > capacity) {
        *err = U_میانگیر_سرریز_ERROR;
        أرجع 1;
    }
    scripts[0] = script;
    أرجع 1;
}

ساكن int32_t
getCodesFromLocale(ثابت حرف *locale,
                   UScript_کد *scripts, int32_t capacity, UError_کد *err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    icu::CharString lang;
    icu::CharString script;
    إذا (locale == اشاره_تهی) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, اشاره_تهی, اشاره_تهی, اشاره_تهی, *err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    إذا (lang == "ja") {
        أرجع مجموعه_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    إذا (lang == "ko") {
        أرجع مجموعه_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    إذا (lang == "zh" && script == "Hant") {
        أرجع مجموعه_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    إذا (!script.is_خالی()) {
        UScript_کد script_کد = static_cast<UScript_کد>(u_get_Property_مقدار_Enum(UCHAR_SCRIPT, script.داده()));
        إذا(script_کد != USCRIPT_INVALID_کد) {
            إذا(script_کد == USCRIPT_SIMPLIFIED_HAN || script_کد == USCRIPT_TRADITIONAL_HAN) {
                script_کد = USCRIPT_HAN;
            }
            أرجع مجموعه_One_کد(script_کد, scripts, capacity, err);
        }
    }
    أرجع 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_کد(ثابت حرف* nameOrAbbrOrLocale,
                UScript_کد* fill_درون,
                int32_t capacity,
                UError_کد* err){
    UBool tried_کد;
    UError_کد داخلی_Error_کد;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع 0;
    }
    إذا(nameOrAbbrOrLocale==اشاره_تهی ||
            (fill_درون == اشاره_تهی ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    tried_کد = خطأ;
    ثابت حرف* آخرین_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    إذا (آخرین_Sep_Ptr==اشاره_تهی) {
        آخرین_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    إذا (آخرین_Sep_Ptr==اشاره_تهی
            || (آخرین_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (آخرین_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_کد کد = (UScript_کد) u_get_Property_مقدار_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کد!=USCRIPT_INVALID_کد) {
            أرجع مجموعه_One_کد(کد, fill_درون, capacity, err);
        }
        tried_کد = درست;
    }
    داخلی_Error_کد = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_درون, capacity, err);
    إذا(U_FAILURE(*err) || length != 0) {
        أرجع length;
    }
    icu::CharString likely = ulocimp_جمع_کن_Likely_Subtags(nameOrAbbrOrLocale, داخلی_Error_کد);
    إذا(U_SUCCESS(داخلی_Error_کد) && داخلی_Error_کد != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.داده(), fill_درون, capacity, err);
        إذا(U_FAILURE(*err) || length != 0) {
            أرجع length;
        }
    }
    إذا(!tried_کد) {
        /* still not found .. try long and abbreviated script names again */
        UScript_کد کد = (UScript_کد) u_get_Property_مقدار_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        إذا(کد!=USCRIPT_INVALID_کد) {
            أرجع مجموعه_One_کد(کد, fill_درون, capacity, err);
        }
    }
    أرجع 0;
}
