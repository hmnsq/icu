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

#ಅನ್ತರ್ಭಾವ "unicode/errorcode.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

ಸ್ಥಿರ icu::Locale*  ಲಭ್ಯ_Locale_List = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ int32_t  ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ;
ಸ್ಥಿರ icu::UInitOnce gInitOnceLocale {};

ನಾಮಾಕಾಶ {

UBool U_CALLCONV locale_ಲಭ್ಯ_cleanup()
{
    ಆದರೆ (ಲಭ್ಯ_Locale_List) {
        ಅಳಿಸು []ಲಭ್ಯ_Locale_List;
        ಲಭ್ಯ_Locale_List = ಶೂನ್ಯಸೂಚಕ;
    }
    ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ = 0;
    gInitOnceLocale.reset();

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV locale_ಲಭ್ಯ_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ = uloc_ಎಣಿಕೆ_ಲಭ್ಯ();
    ಆದರೆ(ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ) {
       ಲಭ್ಯ_Locale_List = ಹೊಸ Locale[ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ];
    }
    ಆದರೆ (ಲಭ್ಯ_Locale_List == ಶೂನ್ಯಸೂಚಕ) {
        ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ= 0;
    }
    ಗಾಗಿ (int32_t loc_ಎಣಿಕೆ=ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ-1; loc_ಎಣಿಕೆ>=0; --loc_ಎಣಿಕೆ) {
        ಲಭ್ಯ_Locale_List[loc_ಎಣಿಕೆ].ಗಣ_From_POSIXID(uloc_get_ಲಭ್ಯ(loc_ಎಣಿಕೆ));
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE_ಲಭ್ಯ, locale_ಲಭ್ಯ_cleanup);
}

ಸ್ಥಿರಾಂಕ Locale* U_EXPORT2
Locale::get_ಲಭ್ಯ_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_ಲಭ್ಯ_init);
    count = ಲಭ್ಯ_Locale_List_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ ಲಭ್ಯ_Locale_List;
}


U_NAMESPACE_ಕೊನೆ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

ನಾಮಾಕಾಶ {

// Enough capacity for the two lists in the res_index.res file
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[2] = {};
int32_t g_ಲಭ್ಯ_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

ವರ್ಗ ಲಭ್ಯ_Locales_Sink : ಸಾರ್ವಜನಿಕ ResourceSink {
  ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool /*noFallback*/, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        Resource_ಕೋಷ್ಟಕ res_ಸೂಚ್ಯಂಕ_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        ಗಾಗಿ (int32_t i = 0; res_ಸೂಚ್ಯಂಕ_ಕೋಷ್ಟಕ.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {
            ULoc_ಲಭ್ಯ_Type type;
            ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "InstalledLocales") == 0) {
                type = ULOC_ಲಭ್ಯ_DEFAULT;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "AliasLocales") == 0) {
                type = ULOC_ಲಭ್ಯ_ONLY_LEGACY_ALIASES;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // CLDRVersion, etc.
                ಮುಂದುವರಿ;
            }
            Resource_ಕೋಷ್ಟಕ ಲಭ್ಯ_Locales_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ;
            }
            g_ಲಭ್ಯ_Locale_Counts[type] = ಲಭ್ಯ_Locales_ಕೋಷ್ಟಕ.get_ಗಾತ್ರ();
            g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[type] = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**>(
                uprv_malloc(g_ಲಭ್ಯ_Locale_Counts[type] * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)));
            ಆದರೆ (g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[type] == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಗಾಗಿ (int32_t j = 0; ಲಭ್ಯ_Locales_ಕೋಷ್ಟಕ.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(j, ಕೀಲಿ, value); ++j) {
                g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[type][j] = ಕೀಲಿ;
            }
        }
    }
};

ವರ್ಗ ಲಭ್ಯ_Locales_String_Enumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
  ಸಾರ್ವಜನಿಕ:
    ಲಭ್ಯ_Locales_String_Enumeration(ULoc_ಲಭ್ಯ_Type type) : fType(type) {
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* next(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
        ULoc_ಲಭ್ಯ_Type actualType = fType;
        int32_t actual_ಸೂಚ್ಯಂಕ = f_ಸೂಚ್ಯಂಕ++;

        // If the "combined" list was requested, resolve that now
        ಆದರೆ (fType == ULOC_ಲಭ್ಯ_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ಎಣಿಕೆ = g_ಲಭ್ಯ_Locale_Counts[ULOC_ಲಭ್ಯ_DEFAULT];
            ಆದರೆ (actual_ಸೂಚ್ಯಂಕ < default_Locales_ಎಣಿಕೆ) {
                actualType = ULOC_ಲಭ್ಯ_DEFAULT;
            } ಇಲ್ಲದಿದ್ದರೆ {
                actual_ಸೂಚ್ಯಂಕ -= default_Locales_ಎಣಿಕೆ;
                actualType = ULOC_ಲಭ್ಯ_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_ಲಭ್ಯ_Locale_Counts[actualType];
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಫಲಿತಾಂಶ;
        ಆದರೆ (actual_ಸೂಚ್ಯಂಕ < count) {
            ಫಲಿತಾಂಶ = g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[actualType][actual_ಸೂಚ್ಯಂಕ];
            ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(ಫಲಿತಾಂಶ));
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
            ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
            }
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಶೂನ್ಯ reset(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        f_ಸೂಚ್ಯಂಕ = 0;
    }

    int32_t count(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
        ಆದರೆ (fType == ULOC_ಲಭ್ಯ_WITH_LEGACY_ALIASES) {
            ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Locale_Counts[ULOC_ಲಭ್ಯ_DEFAULT]
                + g_ಲಭ್ಯ_Locale_Counts[ULOC_ಲಭ್ಯ_ONLY_LEGACY_ALIASES];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Locale_Counts[fType];
        }
    }

  ಖಾಸಗಿ:
    ULoc_ಲಭ್ಯ_Type fType;
    int32_t f_ಸೂಚ್ಯಂಕ = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು); i++) {
        uprv_free(g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[i]);
        g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[i] = ಶೂನ್ಯಸೂಚಕ;
        g_ಲಭ್ಯ_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

ಶೂನ್ಯ U_CALLCONV ತುಂಬು_Installed_Locales(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_ಸೂಚಕ rb(ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, "res_index", &ಸ್ಥಿತಿ_2));
    ಲಭ್ಯ_Locales_Sink sink;
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(rb.getAlias(), "", sink, ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ ತುಂಬು_installed_Locales(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    umtx_initOnce(ginstalledLocalesInitOnce, &ತುಂಬು_Installed_Locales, ಸ್ಥಿತಿ_2);
}

} // namespace

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uloc_get_ಲಭ್ಯ(int32_t ಸ್ಥಾನಾಂತರ) {
    icu::Error_ಸಂಕೇತ ಸ್ಥಿತಿ_2;
    ತುಂಬು_installed_Locales(ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಸ್ಥಿತಿ_2.isFailure()) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (ಸ್ಥಾನಾಂತರ > g_ಲಭ್ಯ_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು[0][ಸ್ಥಾನಾಂತರ];
}

U_CAPI int32_t  U_EXPORT2
uloc_ಎಣಿಕೆ_ಲಭ್ಯ() {
    icu::Error_ಸಂಕೇತ ಸ್ಥಿತಿ_2;
    ತುಂಬು_installed_Locales(ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಸ್ಥಿತಿ_2.isFailure()) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ g_ಲಭ್ಯ_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_ಲಭ್ಯ_By_Type(ULoc_ಲಭ್ಯ_Type type, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (type < 0 || type >= ULOC_ಲಭ್ಯ_ಎಣಿಕೆ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ತುಂಬು_installed_Locales(*ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Local_ಸೂಚಕ<ಲಭ್ಯ_Locales_String_Enumeration> ಫಲಿತಾಂಶ(
        ಹೊಸ ಲಭ್ಯ_Locales_String_Enumeration(type), *ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ uenum_openFromStringEnumeration(ಫಲಿತಾಂಶ.orphan(), ಸ್ಥಿತಿ_2);
}
