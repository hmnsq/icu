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

ಸ್ಥಿರ icu::Locale*  availableLocaleList = nullptr;
ಸ್ಥಿರ int32_t  available_Locale_List_ಎಣಿಕೆ;
ಸ್ಥಿರ icu::UInitOnce gInitOnceLocale {};

ನಾಮಾಕಾಶ {

UBool U_CALLCONV locale_available_cleanup()
{
    ಯದಿ (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_ಎಣಿಕೆ = 0;
    gInitOnceLocale.reset();

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_ಎಣಿಕೆ = uloc_ಎಣಿಕೆ_Available();
    ಯದಿ(available_Locale_List_ಎಣಿಕೆ) {
       availableLocaleList = ನೂತನ Locale[available_Locale_List_ಎಣಿಕೆ];
    }
    ಯದಿ (availableLocaleList == nullptr) {
        available_Locale_List_ಎಣಿಕೆ= 0;
    }
    ಪ್ರತಿ (int32_t loc_ಎಣಿಕೆ=available_Locale_List_ಎಣಿಕೆ-1; loc_ಎಣಿಕೆ>=0; --loc_ಎಣಿಕೆ) {
        availableLocaleList[loc_ಎಣಿಕೆ].setFromPOSIXID(uloc_getAvailable(loc_ಎಣಿಕೆ));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ಧ್ರುವ Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

ನಾಮಾಕಾಶ {

// Enough capacity for the two lists in the res_index.res file
ಧ್ರುವ ವರ್ಣ** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

ವರ್ಗ AvailableLocalesSink : ಸಾರ್ವಜನಿಕ ResourceSink {
  ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool /*noFallback*/, UErrorCode &status) override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        ResourceTable resIndexTable = value.getTable(status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        ಪ್ರತಿ (int32_t i = 0; resIndexTable.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {
            ULocAvailableType type;
            ಯದಿ (uprv_strcmp(ಕೀಲಿ, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } ಅನ್ಯಥಾ {
                // CLDRVersion, etc.
                ಅನುವರ್ತ;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ಧ್ರುವ ವರ್ಣ**>(
                uprv_malloc(gAvailableLocaleCounts[type] * ಪರಿಮಾಣ(ಧ್ರುವ ವರ್ಣ*)));
            ಯದಿ (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಪ್ರತಿ (int32_t j = 0; availableLocalesTable.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(j, ಕೀಲಿ, value); ++j) {
                gAvailableLocaleNames[type][j] = ಕೀಲಿ;
            }
        }
    }
};

ವರ್ಗ AvailableLocalesStringEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
  ಸಾರ್ವಜನಿಕ:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ಧ್ರುವ ವರ್ಣ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        ಯದಿ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ಎಣಿಕೆ = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            ಯದಿ (actualIndex < default_Locales_ಎಣಿಕೆ) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } ಅನ್ಯಥಾ {
                actualIndex -= default_Locales_ಎಣಿಕೆ;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ಧ್ರುವ ವರ್ಣ* result;
        ಯದಿ (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            ಯದಿ (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } ಅನ್ಯಥಾ {
            result = nullptr;
            ಯದಿ (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಶೂನ್ಯ reset(UErrorCode &status) override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ಧ್ರುವ override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }
        ಯದಿ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            ಪ್ರತ್ಯಾಗಮ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ gAvailableLocaleCounts[fType];
        }
    }

  ಗುಪ್ತ:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

ಶೂನ್ಯ U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(rb.getAlias(), "", sink, status);
}

ಶೂನ್ಯ _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ಯದಿ (status.isFailure()) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_ಎಣಿಕೆ_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ಯದಿ (status.isFailure()) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (type < 0 || type >= ULOC_AVAILABLE_ಎಣಿಕೆ) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    _load_installedLocales(*status);
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        ನೂತನ AvailableLocalesStringEnumeration(type), *status);
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ uenum_openFromStringEnumeration(result.orphan(), status);
}
