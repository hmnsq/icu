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

#ଅନ୍ତର୍ଭାଵ "unicode/errorcode.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

ସ୍ଥିର icu::Locale*  availableLocaleList = nullptr;
ସ୍ଥିର int32_t  available_Locale_List_ଗଣନା;
ସ୍ଥିର icu::UInitOnce gInitOnceLocale {};

ନାମାକାଶ {

UBool U_CALLCONV locale_available_cleanup()
{
    ଯଦି (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_ଗଣନା = 0;
    gInitOnceLocale.reset();

    ଫେରନ୍ତୁ ସତ୍ୟ;
}

}  // namespace

ଶୂନ୍ୟ U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_ଗଣନା = uloc_ଗଣନା_Available();
    ଯଦି(available_Locale_List_ଗଣନା) {
       availableLocaleList = ନୂତନ Locale[available_Locale_List_ଗଣନା];
    }
    ଯଦି (availableLocaleList == nullptr) {
        available_Locale_List_ଗଣନା= 0;
    }
    ପାଇଁ (int32_t loc_ଗଣନା=available_Locale_List_ଗଣନା-1; loc_ଗଣନା>=0; --loc_ଗଣନା) {
        availableLocaleList[loc_ଗଣନା].setFromPOSIXID(uloc_getAvailable(loc_ଗଣନା));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ଅଚଳ Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_ଗଣନା;
    ଫେରନ୍ତୁ availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

ନାମାକାଶ {

// Enough capacity for the two lists in the res_index.res file
ଅଚଳ ଅକ୍ଷର** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

ଶ୍ରେଣୀ AvailableLocalesSink : ସର୍ବସାଧାରଣ ResourceSink {
  ସର୍ବସାଧାରଣ:
    ଶୂନ୍ୟ put(ଅଚଳ ଅକ୍ଷର *ଚାବି, Resource_ମୂଲ୍ୟ &value, UBool /*noFallback*/, UErrorCode &status) override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        ResourceTable resIndexTable = value.getTable(status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        ପାଇଁ (int32_t i = 0; resIndexTable.get_ଚାବି_And_ମୂଲ୍ୟ(i, ଚାବି, value); ++i) {
            ULocAvailableType type;
            ଯଦି (uprv_strcmp(ଚାବି, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } ଅନ୍ୟଥା {
                // CLDRVersion, etc.
                ଜାରି;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ଅଚଳ ଅକ୍ଷର**>(
                uprv_malloc(gAvailableLocaleCounts[type] * ଆକାର(ଅଚଳ ଅକ୍ଷର*)));
            ଯଦି (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            ପାଇଁ (int32_t j = 0; availableLocalesTable.get_ଚାବି_And_ମୂଲ୍ୟ(j, ଚାବି, value); ++j) {
                gAvailableLocaleNames[type][j] = ଚାବି;
            }
        }
    }
};

ଶ୍ରେଣୀ AvailableLocalesStringEnumeration : ସର୍ବସାଧାରଣ StringEnumeration {
  ସର୍ବସାଧାରଣ:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ଅଚଳ ଅକ୍ଷର* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        ଯଦି (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ଗଣନା = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            ଯଦି (actualIndex < default_Locales_ଗଣନା) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } ଅନ୍ୟଥା {
                actualIndex -= default_Locales_ଗଣନା;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ଅଚଳ ଅକ୍ଷର* result;
        ଯଦି (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            ଯଦି (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } ଅନ୍ୟଥା {
            result = nullptr;
            ଯଦି (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        ଫେରନ୍ତୁ result;
    }

    ଶୂନ୍ୟ reset(UErrorCode &status) override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ଅଚଳ override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }
        ଯଦି (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            ଫେରନ୍ତୁ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ gAvailableLocaleCounts[fType];
        }
    }

  ବ୍ୟକ୍ତିଗତ:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

ଶୂନ୍ୟ U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(rb.getAlias(), "", sink, status);
}

ଶୂନ୍ୟ _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ଯଦି (status.isFailure()) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_ଗଣନା_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ଯଦି (status.isFailure()) {
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (type < 0 || type >= ULOC_AVAILABLE_ଗଣନା) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    _load_installedLocales(*status);
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        ନୂତନ AvailableLocalesStringEnumeration(type), *status);
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ uenum_openFromStringEnumeration(result.orphan(), status);
}
