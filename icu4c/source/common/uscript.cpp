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

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UScript_ಸಂಕೇತ JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UScript_ಸಂಕೇತ KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UScript_ಸಂಕೇತ HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

ಸ್ಥಿರ int32_t
ಗಣ_Codes(ಸ್ಥಿರಾಂಕ UScript_ಸಂಕೇತ *src, int32_t length,
         UScript_ಸಂಕೇತ *dest, int32_t capacity, UError_ಸಂಕೇತ *err) {
    int32_t i;
    ಆದರೆ(U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ(length > capacity) {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ length;
    }
    ಗಾಗಿ(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t
ಗಣ_One_ಸಂಕೇತ(UScript_ಸಂಕೇತ script, UScript_ಸಂಕೇತ *scripts, int32_t capacity, UError_ಸಂಕೇತ *err) {
    ಆದರೆ(U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ(1 > capacity) {
        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ 1;
    }
    scripts[0] = script;
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ int32_t
getCodesFromLocale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                   UScript_ಸಂಕೇತ *scripts, int32_t capacity, UError_ಸಂಕೇತ *err) {
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    icu::CharString lang;
    icu::CharString script;
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, *err);
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ಆದರೆ (lang == "ja") {
        ಹಿಂತಿರುಗಿ ಗಣ_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ಆದರೆ (lang == "ko") {
        ಹಿಂತಿರುಗಿ ಗಣ_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ಆದರೆ (lang == "zh" && script == "Hant") {
        ಹಿಂತಿರುಗಿ ಗಣ_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ಆದರೆ (!script.is_ಖಾಲಿಗಳು()) {
        UScript_ಸಂಕೇತ script_ಸಂಕೇತ = static_cast<UScript_ಸಂಕೇತ>(u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, script.ದತ್ತಾಂಶ()));
        ಆದರೆ(script_ಸಂಕೇತ != USCRIPT_INVALID_ಸಂಕೇತ) {
            ಆದರೆ(script_ಸಂಕೇತ == USCRIPT_SIMPLIFIED_HAN || script_ಸಂಕೇತ == USCRIPT_TRADITIONAL_HAN) {
                script_ಸಂಕೇತ = USCRIPT_HAN;
            }
            ಹಿಂತಿರುಗಿ ಗಣ_One_ಸಂಕೇತ(script_ಸಂಕೇತ, scripts, capacity, err);
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* nameOrAbbrOrLocale,
                UScript_ಸಂಕೇತ* fill_ಒಳ,
                int32_t capacity,
                UError_ಸಂಕೇತ* err){
    UBool tried_ಸಂಕೇತ;
    UError_ಸಂಕೇತ ಆಂತರಿಕ_Error_ಸಂಕೇತ;
    int32_t length;

    ಆದರೆ(U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(nameOrAbbrOrLocale==ಶೂನ್ಯಸೂಚಕ ||
            (fill_ಒಳ == ಶೂನ್ಯಸೂಚಕ ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    tried_ಸಂಕೇತ = ಸುಳ್ಳು;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೊನೆಯ_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ಆದರೆ (ಕೊನೆಯ_Sep_Ptr==ಶೂನ್ಯಸೂಚಕ) {
        ಕೊನೆಯ_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ಆದರೆ (ಕೊನೆಯ_Sep_Ptr==ಶೂನ್ಯಸೂಚಕ
            || (ಕೊನೆಯ_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (ಕೊನೆಯ_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_ಸಂಕೇತ ಸಂಕೇತ_2 = (UScript_ಸಂಕೇತ) u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ಆದರೆ(ಸಂಕೇತ_2!=USCRIPT_INVALID_ಸಂಕೇತ) {
            ಹಿಂತಿರುಗಿ ಗಣ_One_ಸಂಕೇತ(ಸಂಕೇತ_2, fill_ಒಳ, capacity, err);
        }
        tried_ಸಂಕೇತ = ನಿಜ;
    }
    ಆಂತರಿಕ_Error_ಸಂಕೇತ = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_ಒಳ, capacity, err);
    ಆದರೆ(U_FAILURE(*err) || length != 0) {
        ಹಿಂತಿರುಗಿ length;
    }
    icu::CharString likely = ulocimp_ಕೂಡಿಸು_Likely_Subtags(nameOrAbbrOrLocale, ಆಂತರಿಕ_Error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(ಆಂತರಿಕ_Error_ಸಂಕೇತ) && ಆಂತರಿಕ_Error_ಸಂಕೇತ != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.ದತ್ತಾಂಶ(), fill_ಒಳ, capacity, err);
        ಆದರೆ(U_FAILURE(*err) || length != 0) {
            ಹಿಂತಿರುಗಿ length;
        }
    }
    ಆದರೆ(!tried_ಸಂಕೇತ) {
        /* still not found .. try long and abbreviated script names again */
        UScript_ಸಂಕೇತ ಸಂಕೇತ_2 = (UScript_ಸಂಕೇತ) u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ಆದರೆ(ಸಂಕೇತ_2!=USCRIPT_INVALID_ಸಂಕೇತ) {
            ಹಿಂತಿರುಗಿ ಗಣ_One_ಸಂಕೇತ(ಸಂಕೇತ_2, fill_ಒಳ, capacity, err);
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}
