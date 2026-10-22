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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"

ਸਥਿਰ ਅਚਲ UScript_ਕੋਡ JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ਸਥਿਰ ਅਚਲ UScript_ਕੋਡ KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ਸਥਿਰ ਅਚਲ UScript_ਕੋਡ HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ਸਥਿਰ int32_t
ਸੈੱਟ_Codes(ਅਚਲ UScript_ਕੋਡ *src, int32_t length,
         UScript_ਕੋਡ *dest, int32_t capacity, UError_ਕੋਡ *err) {
    int32_t i;
    ਜੇ(U_FAILURE(*err)) { ਵਾਪਸ 0; }
    ਜੇ(length > capacity) {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ length;
    }
    ਲਈ(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t
ਸੈੱਟ_One_ਕੋਡ(UScript_ਕੋਡ script, UScript_ਕੋਡ *scripts, int32_t capacity, UError_ਕੋਡ *err) {
    ਜੇ(U_FAILURE(*err)) { ਵਾਪਸ 0; }
    ਜੇ(1 > capacity) {
        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ 1;
    }
    scripts[0] = script;
    ਵਾਪਸ 1;
}

ਸਥਿਰ int32_t
getCodesFromLocale(ਅਚਲ ਅੱਖਰ *locale,
                   UScript_ਕੋਡ *scripts, int32_t capacity, UError_ਕੋਡ *err) {
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ 0; }
    icu::CharString lang;
    icu::CharString script;
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, *err);
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ਜੇ (lang == "ja") {
        ਵਾਪਸ ਸੈੱਟ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ਜੇ (lang == "ko") {
        ਵਾਪਸ ਸੈੱਟ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ਜੇ (lang == "zh" && script == "Hant") {
        ਵਾਪਸ ਸੈੱਟ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ਜੇ (!script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        UScript_ਕੋਡ script_ਕੋਡ = static_cast<UScript_ਕੋਡ>(u_get_Property_ਮੁੱਲ_Enum(UCHAR_SCRIPT, script.ਅੰਕੜੇ()));
        ਜੇ(script_ਕੋਡ != USCRIPT_INVALID_ਕੋਡ) {
            ਜੇ(script_ਕੋਡ == USCRIPT_SIMPLIFIED_HAN || script_ਕੋਡ == USCRIPT_TRADITIONAL_HAN) {
                script_ਕੋਡ = USCRIPT_HAN;
            }
            ਵਾਪਸ ਸੈੱਟ_One_ਕੋਡ(script_ਕੋਡ, scripts, capacity, err);
        }
    }
    ਵਾਪਸ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_ਕੋਡ(ਅਚਲ ਅੱਖਰ* nameOrAbbrOrLocale,
                UScript_ਕੋਡ* fill_ਵਿੱਚ,
                int32_t capacity,
                UError_ਕੋਡ* err){
    UBool tried_ਕੋਡ;
    UError_ਕੋਡ ਅੰਦਰੂਨੀ_Error_ਕੋਡ;
    int32_t length;

    ਜੇ(U_FAILURE(*err)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(nameOrAbbrOrLocale==ਖਾਲੀ_ਸੂਚਕ ||
            (fill_ਵਿੱਚ == ਖਾਲੀ_ਸੂਚਕ ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    tried_ਕੋਡ = ਝੂਠ;
    ਅਚਲ ਅੱਖਰ* ਆਖਰੀ_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ਜੇ (ਆਖਰੀ_Sep_Ptr==ਖਾਲੀ_ਸੂਚਕ) {
        ਆਖਰੀ_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ਜੇ (ਆਖਰੀ_Sep_Ptr==ਖਾਲੀ_ਸੂਚਕ
            || (ਆਖਰੀ_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (ਆਖਰੀ_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_ਕੋਡ ਕੋਡ = (UScript_ਕੋਡ) u_get_Property_ਮੁੱਲ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ਜੇ(ਕੋਡ!=USCRIPT_INVALID_ਕੋਡ) {
            ਵਾਪਸ ਸੈੱਟ_One_ਕੋਡ(ਕੋਡ, fill_ਵਿੱਚ, capacity, err);
        }
        tried_ਕੋਡ = ਸੱਚ;
    }
    ਅੰਦਰੂਨੀ_Error_ਕੋਡ = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_ਵਿੱਚ, capacity, err);
    ਜੇ(U_FAILURE(*err) || length != 0) {
        ਵਾਪਸ length;
    }
    icu::CharString likely = ulocimp_ਜੋੜੋ_Likely_Subtags(nameOrAbbrOrLocale, ਅੰਦਰੂਨੀ_Error_ਕੋਡ);
    ਜੇ(U_SUCCESS(ਅੰਦਰੂਨੀ_Error_ਕੋਡ) && ਅੰਦਰੂਨੀ_Error_ਕੋਡ != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.ਅੰਕੜੇ(), fill_ਵਿੱਚ, capacity, err);
        ਜੇ(U_FAILURE(*err) || length != 0) {
            ਵਾਪਸ length;
        }
    }
    ਜੇ(!tried_ਕੋਡ) {
        /* still not found .. try long and abbreviated script names again */
        UScript_ਕੋਡ ਕੋਡ = (UScript_ਕੋਡ) u_get_Property_ਮੁੱਲ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ਜੇ(ਕੋਡ!=USCRIPT_INVALID_ਕੋਡ) {
            ਵਾਪਸ ਸੈੱਟ_One_ਕੋਡ(ਕੋਡ, fill_ਵਿੱਚ, capacity, err);
        }
    }
    ਵਾਪਸ 0;
}
