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

स्थिर icu::Locale*  availableLocaleList = nullptr;
स्थिर int32_t  available_Locale_List_गणनम्;
स्थिर icu::UInitOnce gInitOnceLocale {};

नामाकाश {

UBool U_CALLCONV locale_available_cleanup()
{
    जइ (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_गणनम् = 0;
    gInitOnceLocale.reset();

    प्रत्यागम सच्च;
}

}  // namespace

शून्य U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_गणनम् = uloc_गणनम्_Available();
    जइ(available_Locale_List_गणनम्) {
       availableLocaleList = नूतन Locale[available_Locale_List_गणनम्];
    }
    जइ (availableLocaleList == nullptr) {
        available_Locale_List_गणनम्= 0;
    }
    प्रति (int32_t loc_गणनम्=available_Locale_List_गणनम्-1; loc_गणनम्>=0; --loc_गणनम्) {
        availableLocaleList[loc_गणनम्].setFromPOSIXID(uloc_getAvailable(loc_गणनम्));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ध्रुव Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_गणनम्;
    प्रत्यागम availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

नामाकाश {

// Enough capacity for the two lists in the res_index.res file
ध्रुव वर्ण** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

वर्ग AvailableLocalesSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value, UBool /*noFallback*/, UErrorCode &status) override {
        जइ (U_FAILURE(status)) { प्रत्यागम; }
        ResourceTable resIndexTable = value.getTable(status);
        जइ (U_FAILURE(status)) { प्रत्यागम; }
        प्रति (int32_t i = 0; resIndexTable.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {
            ULocAvailableType type;
            जइ (uprv_strcmp(कुञ्चिका, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } अण्णहा {
                // CLDRVersion, etc.
                अनुवर्त;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            जइ (U_FAILURE(status)) {
                प्रत्यागम;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ध्रुव वर्ण**>(
                uprv_malloc(gAvailableLocaleCounts[type] * परिमाण(ध्रुव वर्ण*)));
            जइ (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t j = 0; availableLocalesTable.get_कुञ्चिका_And_मानम्(j, कुञ्चिका, value); ++j) {
                gAvailableLocaleNames[type][j] = कुञ्चिका;
            }
        }
    }
};

वर्ग AvailableLocalesStringEnumeration : सार्वजनिक StringEnumeration {
  सार्वजनिक:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ध्रुव वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        जइ (U_FAILURE(status)) { प्रत्यागम nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        जइ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_गणनम् = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            जइ (actualIndex < default_Locales_गणनम्) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } अण्णहा {
                actualIndex -= default_Locales_गणनम्;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ध्रुव वर्ण* result;
        जइ (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            जइ (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } अण्णहा {
            result = nullptr;
            जइ (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        प्रत्यागम result;
    }

    शून्य reset(UErrorCode &status) override {
        जइ (U_FAILURE(status)) { प्रत्यागम; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ध्रुव override {
        जइ (U_FAILURE(status)) { प्रत्यागम 0; }
        जइ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            प्रत्यागम gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } अण्णहा {
            प्रत्यागम gAvailableLocaleCounts[fType];
        }
    }

  गुप्त:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    प्रत्यागम सच्च;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

शून्य U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_वस्तूनि_With_Fallback(rb.getAlias(), "", sink, status);
}

शून्य _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    जइ (status.isFailure()) {
        प्रत्यागम nullptr;
    }
    जइ (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_गणनम्_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    जइ (status.isFailure()) {
        प्रत्यागम 0;
    }
    प्रत्यागम gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (type < 0 || type >= ULOC_AVAILABLE_गणनम्) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    _load_installedLocales(*status);
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        नूतन AvailableLocalesStringEnumeration(type), *status);
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम uenum_openFromStringEnumeration(result.orphan(), status);
}
