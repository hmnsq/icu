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

#include "unicode/errorcode.h"
#include "unicode/utypes.h"
#include "unicode/locid.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "cmemory.h"
#include "cstring.h"
#include "ucln_cmn.h"
#include "uassert.h"
#include "umutex.h"
#include "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

static icu::Locale*  availableLocaleList = nullptr;
static int32_t  availableLocaleListCount;
static icu::UInitOnce gInitOnceLocale {};

namespace {

UBool U_CALLCONV locale_available_cleanup()
{
    ᬬᬾᬦ᭄ (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    availableLocaleListCount = 0;
    gInitOnceLocale.reset();

    ᬯᬮᬶ true;
}

}  // namespace

void U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    availableLocaleListCount = uloc_countAvailable();
    ᬬᬾᬦ᭄(availableLocaleListCount) {
       availableLocaleList = new Locale[availableLocaleListCount];
    }
    ᬬᬾᬦ᭄ (availableLocaleList == nullptr) {
        availableLocaleListCount= 0;
    }
    for (int32_t locCount=availableLocaleListCount-1; locCount>=0; --locCount) {
        availableLocaleList[locCount].setFromPOSIXID(uloc_getAvailable(locCount));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

const Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = availableLocaleListCount;
    ᬯᬮᬶ availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

namespace {

// Enough capacity for the two lists in the res_index.res file
const var_na** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

class AvailableLocalesSink : public ResourceSink {
  public:
    void put(const var_na *key, ResourceValue &value, UBool /*noFallback*/, UErrorCode &status) override {
        ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ; }
        ResourceTable resIndexTable = value.getTable(status);
        ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ; }
        for (int32_t i = 0; resIndexTable.getKeyAndValue(i, key, value); ++i) {
            ULocAvailableType type;
            ᬬᬾᬦ᭄ (uprv_strcmp(key, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_strcmp(key, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } ᬮᬶᬬ {
                // CLDRVersion, etc.
                continue;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            ᬬᬾᬦ᭄ (U_FAILURE(status)) {
                ᬯᬮᬶ;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<const var_na**>(
                uprv_malloc(gAvailableLocaleCounts[type] * parimaa_na(const var_na*)));
            ᬬᬾᬦ᭄ (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ᬯᬮᬶ;
            }
            for (int32_t j = 0; availableLocalesTable.getKeyAndValue(j, key, value); ++j) {
                gAvailableLocaleNames[type][j] = key;
            }
        }
    }
};

class AvailableLocalesStringEnumeration : public StringEnumeration {
  public:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    const var_na* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        ᬬᬾᬦ᭄ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t defaultLocalesCount = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            ᬬᬾᬦ᭄ (actualIndex < defaultLocalesCount) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } ᬮᬶᬬ {
                actualIndex -= defaultLocalesCount;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        const var_na* result;
        ᬬᬾᬦ᭄ (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            ᬬᬾᬦ᭄ (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } ᬮᬶᬬ {
            result = nullptr;
            ᬬᬾᬦ᭄ (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        ᬯᬮᬶ result;
    }

    void reset(UErrorCode &status) override {
        ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) const override {
        ᬬᬾᬦ᭄ (U_FAILURE(status)) { ᬯᬮᬶ 0; }
        ᬬᬾᬦ᭄ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            ᬯᬮᬶ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } ᬮᬶᬬ {
            ᬯᬮᬶ gAvailableLocaleCounts[fType];
        }
    }

  private:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    for (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ᬯᬮᬶ true;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

void U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_getAllItemsWithFallback(rb.getAlias(), "", sink, status);
}

void _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI const var_na* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ᬬᬾᬦ᭄ (status.isFailure()) {
        ᬯᬮᬶ nullptr;
    }
    ᬬᬾᬦ᭄ (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ nullptr;
    }
    ᬯᬮᬶ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_countAvailable() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ᬬᬾᬦ᭄ (status.isFailure()) {
        ᬯᬮᬶ 0;
    }
    ᬯᬮᬶ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    ᬬᬾᬦ᭄ (U_FAILURE(*status)) {
        ᬯᬮᬶ nullptr;
    }
    ᬬᬾᬦ᭄ (type < 0 || type >= ULOC_AVAILABLE_COUNT) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ nullptr;
    }
    _load_installedLocales(*status);
    ᬬᬾᬦ᭄ (U_FAILURE(*status)) {
        ᬯᬮᬶ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        new AvailableLocalesStringEnumeration(type), *status);
    ᬬᬾᬦ᭄ (U_FAILURE(*status)) {
        ᬯᬮᬶ nullptr;
    }
    ᬯᬮᬶ uenum_openFromStringEnumeration(result.orphan(), status);
}
