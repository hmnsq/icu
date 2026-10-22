// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  locavailable.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for available locales, separated out from other .cpp files
*   that then do not depend on resource bundle code and res_index bundles.
*/

#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

स्थिर icu::Locale*  उपलब्धम्_Locale_List = शून्यसूचकः;
स्थिर int32_t  उपलब्धम्_Locale_List_गणनम्;
स्थिर icu::UInitOnce gInitOnceLocale {};

नामाकाश {

UBool U_CALLCONV locale_उपलब्धम्_cleanup()
{
    यदि (उपलब्धम्_Locale_List) {
        विलोपय []उपलब्धम्_Locale_List;
        उपलब्धम्_Locale_List = शून्यसूचकः;
    }
    उपलब्धम्_Locale_List_गणनम् = 0;
    gInitOnceLocale.reset();

    प्रत्यागम सत्य;
}

}  // namespace

शून्य U_CALLCONV locale_उपलब्धम्_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    उपलब्धम्_Locale_List_गणनम् = uloc_गणनम्_उपलब्धम्();
    यदि(उपलब्धम्_Locale_List_गणनम्) {
       उपलब्धम्_Locale_List = नूतन Locale[उपलब्धम्_Locale_List_गणनम्];
    }
    यदि (उपलब्धम्_Locale_List == शून्यसूचकः) {
        उपलब्धम्_Locale_List_गणनम्= 0;
    }
    प्रति (int32_t loc_गणनम्=उपलब्धम्_Locale_List_गणनम्-1; loc_गणनम्>=0; --loc_गणनम्) {
        उपलब्धम्_Locale_List[loc_गणनम्].समुच्चयः_From_POSIXID(uloc_get_उपलब्धम्(loc_गणनम्));
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE_उपलब्धम्, locale_उपलब्धम्_cleanup);
}

ध्रुव Locale* U_EXPORT2
Locale::get_उपलब्धम्_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_उपलब्धम्_init);
    count = उपलब्धम्_Locale_List_गणनम्;
    प्रत्यागम उपलब्धम्_Locale_List;
}


U_NAMESPACE_अन्तः

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

नामाकाश {

// Enough capacity for the two lists in the res_index.res file
ध्रुव वर्ण** g_उपलब्धम्_Locale_नामानि[2] = {};
int32_t g_उपलब्धम्_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

वर्ग उपलब्धम्_Locales_Sink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value, UBool /*noFallback*/, UError_संहिता &स्थितिः) override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        Resource_सारणी res_सूचकांक_सारणी = value.get_सारणी(स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        प्रति (int32_t i = 0; res_सूचकांक_सारणी.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {
            ULoc_उपलब्धम्_Type type;
            यदि (uprv_strcmp(कुञ्चिका, "InstalledLocales") == 0) {
                type = ULOC_उपलब्धम्_DEFAULT;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "AliasLocales") == 0) {
                type = ULOC_उपलब्धम्_ONLY_LEGACY_ALIASES;
            } अन्यथा {
                // CLDRVersion, etc.
                अनुवर्त;
            }
            Resource_सारणी उपलब्धम्_Locales_सारणी = value.get_सारणी(स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम;
            }
            g_उपलब्धम्_Locale_Counts[type] = उपलब्धम्_Locales_सारणी.get_आकार();
            g_उपलब्धम्_Locale_नामानि[type] = static_cast<ध्रुव वर्ण**>(
                uprv_malloc(g_उपलब्धम्_Locale_Counts[type] * परिमाण(ध्रुव वर्ण*)));
            यदि (g_उपलब्धम्_Locale_नामानि[type] == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t j = 0; उपलब्धम्_Locales_सारणी.get_कुञ्चिका_And_मानम्(j, कुञ्चिका, value); ++j) {
                g_उपलब्धम्_Locale_नामानि[type][j] = कुञ्चिका;
            }
        }
    }
};

वर्ग उपलब्धम्_Locales_String_Enumeration : सार्वजनिक StringEnumeration {
  सार्वजनिक:
    उपलब्धम्_Locales_String_Enumeration(ULoc_उपलब्धम्_Type type) : fType(type) {
    }

    ध्रुव वर्ण* next(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम शून्यसूचकः; }
        ULoc_उपलब्धम्_Type actualType = fType;
        int32_t actual_सूचकांक = f_सूचकांक++;

        // If the "combined" list was requested, resolve that now
        यदि (fType == ULOC_उपलब्धम्_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_गणनम् = g_उपलब्धम्_Locale_Counts[ULOC_उपलब्धम्_DEFAULT];
            यदि (actual_सूचकांक < default_Locales_गणनम्) {
                actualType = ULOC_उपलब्धम्_DEFAULT;
            } अन्यथा {
                actual_सूचकांक -= default_Locales_गणनम्;
                actualType = ULOC_उपलब्धम्_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_उपलब्धम्_Locale_Counts[actualType];
        ध्रुव वर्ण* परिणाम;
        यदि (actual_सूचकांक < count) {
            परिणाम = g_उपलब्धम्_Locale_नामानि[actualType][actual_सूचकांक];
            यदि (परिणाम_दीर्घता != शून्यसूचकः) {
                *परिणाम_दीर्घता = static_cast<int32_t>(uprv_strlen(परिणाम));
            }
        } अन्यथा {
            परिणाम = शून्यसूचकः;
            यदि (परिणाम_दीर्घता != शून्यसूचकः) {
                *परिणाम_दीर्घता = 0;
            }
        }
        प्रत्यागम परिणाम;
    }

    शून्य reset(UError_संहिता &स्थितिः) override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        f_सूचकांक = 0;
    }

    int32_t count(UError_संहिता &स्थितिः) ध्रुव override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }
        यदि (fType == ULOC_उपलब्धम्_WITH_LEGACY_ALIASES) {
            प्रत्यागम g_उपलब्धम्_Locale_Counts[ULOC_उपलब्धम्_DEFAULT]
                + g_उपलब्धम्_Locale_Counts[ULOC_उपलब्धम्_ONLY_LEGACY_ALIASES];
        } अन्यथा {
            प्रत्यागम g_उपलब्धम्_Locale_Counts[fType];
        }
    }

  गुप्त:
    ULoc_उपलब्धम्_Type fType;
    int32_t f_सूचकांक = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(g_उपलब्धम्_Locale_नामानि); i++) {
        uprv_free(g_उपलब्धम्_Locale_नामानि[i]);
        g_उपलब्धम्_Locale_नामानि[i] = शून्यसूचकः;
        g_उपलब्धम्_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    प्रत्यागम सत्य;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

शून्य U_CALLCONV आरोपय_Installed_Locales(UError_संहिता& स्थितिः) {
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_open_प्रत्यक्षम्(शून्यसूचकः, "res_index", &स्थितिः));
    उपलब्धम्_Locales_Sink sink;
    ures_get_All_वस्तूनि_With_Fallback(rb.getAlias(), "", sink, स्थितिः);
}

शून्य आरोपय_installed_Locales(UError_संहिता& स्थितिः) {
    umtx_initOnce(ginstalledLocalesInitOnce, &आरोपय_Installed_Locales, स्थितिः);
}

} // namespace

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_get_उपलब्धम्(int32_t विस्थापनम्) {
    icu::Error_संहिता स्थितिः;
    आरोपय_installed_Locales(स्थितिः);
    यदि (स्थितिः.isFailure()) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (विस्थापनम् > g_उपलब्धम्_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम g_उपलब्धम्_Locale_नामानि[0][विस्थापनम्];
}

U_CAPI int32_t  U_EXPORT2
uloc_गणनम्_उपलब्धम्() {
    icu::Error_संहिता स्थितिः;
    आरोपय_installed_Locales(स्थितिः);
    यदि (स्थितिः.isFailure()) {
        प्रत्यागम 0;
    }
    प्रत्यागम g_उपलब्धम्_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_उपलब्धम्_By_Type(ULoc_उपलब्धम्_Type type, UError_संहिता* स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (type < 0 || type >= ULOC_उपलब्धम्_गणनम्) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    आरोपय_installed_Locales(*स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    LocalPointer<उपलब्धम्_Locales_String_Enumeration> परिणाम(
        नूतन उपलब्धम्_Locales_String_Enumeration(type), *स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम uenum_openFromStringEnumeration(परिणाम.orphan(), स्थितिः);
}
