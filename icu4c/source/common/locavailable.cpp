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

#અન્તર્ભાવ "unicode/errorcode.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

સ્થિર icu::Locale*  availableLocaleList = nullptr;
સ્થિર int32_t  available_Locale_List_ગણતરી;
સ્થિર icu::UInitOnce gInitOnceLocale {};

નામાકાશ {

UBool U_CALLCONV locale_available_cleanup()
{
    જો (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_ગણતરી = 0;
    gInitOnceLocale.reset();

    પાછા સાચું;
}

}  // namespace

શૂન્ય U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_ગણતરી = uloc_ગણતરી_Available();
    જો(available_Locale_List_ગણતરી) {
       availableLocaleList = નવું Locale[available_Locale_List_ગણતરી];
    }
    જો (availableLocaleList == nullptr) {
        available_Locale_List_ગણતરી= 0;
    }
    માટે (int32_t loc_ગણતરી=available_Locale_List_ગણતરી-1; loc_ગણતરી>=0; --loc_ગણતરી) {
        availableLocaleList[loc_ગણતરી].setFromPOSIXID(uloc_getAvailable(loc_ગણતરી));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

અચલ Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_ગણતરી;
    પાછા availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

નામાકાશ {

// Enough capacity for the two lists in the res_index.res file
અચલ અક્ષર** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

વર્ગ AvailableLocalesSink : જાહેર ResourceSink {
  જાહેર:
    શૂન્ય put(અચલ અક્ષર *ચાવી, Resource_કિંમત &value, UBool /*noFallback*/, UErrorCode &status) override {
        જો (U_FAILURE(status)) { પાછા; }
        ResourceTable resIndexTable = value.getTable(status);
        જો (U_FAILURE(status)) { પાછા; }
        માટે (int32_t i = 0; resIndexTable.get_ચાવી_And_કિંમત(i, ચાવી, value); ++i) {
            ULocAvailableType type;
            જો (uprv_strcmp(ચાવી, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } નહીંતર જો (uprv_strcmp(ચાવી, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } નહીંતર {
                // CLDRVersion, etc.
                ચાલુ;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            જો (U_FAILURE(status)) {
                પાછા;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<અચલ અક્ષર**>(
                uprv_malloc(gAvailableLocaleCounts[type] * કદ(અચલ અક્ષર*)));
            જો (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            માટે (int32_t j = 0; availableLocalesTable.get_ચાવી_And_કિંમત(j, ચાવી, value); ++j) {
                gAvailableLocaleNames[type][j] = ચાવી;
            }
        }
    }
};

વર્ગ AvailableLocalesStringEnumeration : જાહેર StringEnumeration {
  જાહેર:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    અચલ અક્ષર* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        જો (U_FAILURE(status)) { પાછા nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        જો (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ગણતરી = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            જો (actualIndex < default_Locales_ગણતરી) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } નહીંતર {
                actualIndex -= default_Locales_ગણતરી;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        અચલ અક્ષર* result;
        જો (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            જો (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } નહીંતર {
            result = nullptr;
            જો (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        પાછા result;
    }

    શૂન્ય reset(UErrorCode &status) override {
        જો (U_FAILURE(status)) { પાછા; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) અચલ override {
        જો (U_FAILURE(status)) { પાછા 0; }
        જો (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            પાછા gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } નહીંતર {
            પાછા gAvailableLocaleCounts[fType];
        }
    }

  ખાનગી:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    પાછા સાચું;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

શૂન્ય U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_વસ્તુઓ_With_Fallback(rb.getAlias(), "", sink, status);
}

શૂન્ય _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI અચલ અક્ષર* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    જો (status.isFailure()) {
        પાછા nullptr;
    }
    જો (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    પાછા gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_ગણતરી_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    જો (status.isFailure()) {
        પાછા 0;
    }
    પાછા gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (type < 0 || type >= ULOC_AVAILABLE_ગણતરી) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    _load_installedLocales(*status);
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        નવું AvailableLocalesStringEnumeration(type), *status);
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    પાછા uenum_openFromStringEnumeration(result.orphan(), status);
}
