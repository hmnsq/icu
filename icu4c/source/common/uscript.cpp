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

#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ulocimp.h"

స్థిర స్థిరరాశి UScript_సంకేతం JAPANESE[3] = { USCRIPT_KATAKANA, USCRIPT_HIRAGANA, USCRIPT_HAN };
స్థిర స్థిరరాశి UScript_సంకేతం KOREAN[2] = { USCRIPT_HANGUL, USCRIPT_HAN };
స్థిర స్థిరరాశి UScript_సంకేతం HAN_BOPO[2] = { USCRIPT_HAN, USCRIPT_BOPOMOFO };

స్థిర int32_t
సమితి_Codes(స్థిరరాశి UScript_సంకేతం *src, int32_t length,
         UScript_సంకేతం *dest, int32_t capacity, UError_సంకేతం *err) {
    int32_t i;
    ఒకవేళ(U_FAILURE(*err)) { తిరిగి 0; }
    ఒకవేళ(length > capacity) {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి length;
    }
    కొరకు(i = 0; i < length; ++i) {
        dest[i] = src[i];
    }
    తిరిగి length;
}

స్థిర int32_t
సమితి_One_సంకేతం(UScript_సంకేతం script, UScript_సంకేతం *scripts, int32_t capacity, UError_సంకేతం *err) {
    ఒకవేళ(U_FAILURE(*err)) { తిరిగి 0; }
    ఒకవేళ(1 > capacity) {
        *err = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి 1;
    }
    scripts[0] = script;
    తిరిగి 1;
}

స్థిర int32_t
getCodesFromLocale(స్థిరరాశి అక్షరం *locale,
                   UScript_సంకేతం *scripts, int32_t capacity, UError_సంకేతం *err) {
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి 0; }
    icu::CharString lang;
    icu::CharString script;
    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }
    ulocimp_getSubtags(locale, &lang, &script, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, *err);
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి 0; }
    // Multi-script languages, equivalent to the LocaleScript data
    // that we used to load from locale resource bundles.
    ఒకవేళ (lang == "ja") {
        తిరిగి సమితి_Codes(JAPANESE, UPRV_LENGTHOF(JAPANESE), scripts, capacity, err);
    }
    ఒకవేళ (lang == "ko") {
        తిరిగి సమితి_Codes(KOREAN, UPRV_LENGTHOF(KOREAN), scripts, capacity, err);
    }
    ఒకవేళ (lang == "zh" && script == "Hant") {
        తిరిగి సమితి_Codes(HAN_BOPO, UPRV_LENGTHOF(HAN_BOPO), scripts, capacity, err);
    }
    // Explicit script code.
    ఒకవేళ (!script.is_ఖాళీలు()) {
        UScript_సంకేతం script_సంకేతం = static_cast<UScript_సంకేతం>(u_get_Property_విలువ_Enum(UCHAR_SCRIPT, script.దత్తాంశం()));
        ఒకవేళ(script_సంకేతం != USCRIPT_INVALID_సంకేతం) {
            ఒకవేళ(script_సంకేతం == USCRIPT_SIMPLIFIED_HAN || script_సంకేతం == USCRIPT_TRADITIONAL_HAN) {
                script_సంకేతం = USCRIPT_HAN;
            }
            తిరిగి సమితి_One_సంకేతం(script_సంకేతం, scripts, capacity, err);
        }
    }
    తిరిగి 0;
}

/* TODO: this is a bad API and should be deprecated, ticket #11141 */
U_CAPI int32_t  U_EXPORT2
uscript_get_సంకేతం(స్థిరరాశి అక్షరం* nameOrAbbrOrLocale,
                UScript_సంకేతం* fill_లోపల,
                int32_t capacity,
                UError_సంకేతం* err){
    UBool tried_సంకేతం;
    UError_సంకేతం అంతర్గత_Error_సంకేతం;
    int32_t length;

    ఒకవేళ(U_FAILURE(*err)) {
        తిరిగి 0;
    }
    ఒకవేళ(nameOrAbbrOrLocale==శూన్యసూచిక ||
            (fill_లోపల == శూన్యసూచిక ? capacity != 0 : capacity < 0)) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    tried_సంకేతం = అబద్ధం;
    స్థిరరాశి అక్షరం* చివరి_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '-');
    ఒకవేళ (చివరి_Sep_Ptr==శూన్యసూచిక) {
        చివరి_Sep_Ptr = uprv_strrchr(nameOrAbbrOrLocale, '_');
    }
    // Favor interpretation of nameOrAbbrOrLocale as a script alias if either
    // 1. nameOrAbbrOrLocale does not contain -/_. Handles Han, Mro, Nko, etc.
    // 2. The last instance of -/_ is at offset 3, and the portion after that is
    //    longer than 4 characters (i.e. not a script or region code). This handles
    //    Old_Hungarian, Old_Italic, etc. ("old" is a valid language code)
    // 3. The last instance of -/_ is at offset 7, and the portion after that is
    //    3 characters. This handles New_Tai_Lue ("new" is a valid language code).
    ఒకవేళ (చివరి_Sep_Ptr==శూన్యసూచిక
            || (చివరి_Sep_Ptr-nameOrAbbrOrLocale == 3 && uprv_strlen(nameOrAbbrOrLocale) > 8)
            || (చివరి_Sep_Ptr-nameOrAbbrOrLocale == 7 && uprv_strlen(nameOrAbbrOrLocale) == 11) ) {
        /* try long and abbreviated script names first */
        UScript_సంకేతం సంకేతం_2 = (UScript_సంకేతం) u_get_Property_విలువ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ఒకవేళ(సంకేతం_2!=USCRIPT_INVALID_సంకేతం) {
            తిరిగి సమితి_One_సంకేతం(సంకేతం_2, fill_లోపల, capacity, err);
        }
        tried_సంకేతం = నిజం;
    }
    అంతర్గత_Error_సంకేతం = U_ZERO_ERROR;
    length = getCodesFromLocale(nameOrAbbrOrLocale, fill_లోపల, capacity, err);
    ఒకవేళ(U_FAILURE(*err) || length != 0) {
        తిరిగి length;
    }
    icu::CharString likely = ulocimp_కూడు_Likely_Subtags(nameOrAbbrOrLocale, అంతర్గత_Error_సంకేతం);
    ఒకవేళ(U_SUCCESS(అంతర్గత_Error_సంకేతం) && అంతర్గత_Error_సంకేతం != U_STRING_NOT_TERMINATED_WARNING) {
        length = getCodesFromLocale(likely.దత్తాంశం(), fill_లోపల, capacity, err);
        ఒకవేళ(U_FAILURE(*err) || length != 0) {
            తిరిగి length;
        }
    }
    ఒకవేళ(!tried_సంకేతం) {
        /* still not found .. try long and abbreviated script names again */
        UScript_సంకేతం సంకేతం_2 = (UScript_సంకేతం) u_get_Property_విలువ_Enum(UCHAR_SCRIPT, nameOrAbbrOrLocale);
        ఒకవేళ(సంకేతం_2!=USCRIPT_INVALID_సంకేతం) {
            తిరిగి సమితి_One_సంకేతం(సంకేతం_2, fill_లోపల, capacity, err);
        }
    }
    తిరిగి 0;
}
