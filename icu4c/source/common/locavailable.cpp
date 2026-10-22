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
    ប្រសិនបើ (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    availableLocaleListCount = 0;
    gInitOnceLocale.reset();

    ត្រឡប់ true;
}

}  // namespace

void U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    availableLocaleListCount = uloc_countAvailable();
    ប្រសិនបើ(availableLocaleListCount) {
       availableLocaleList = new Locale[availableLocaleListCount];
    }
    ប្រសិនបើ (availableLocaleList == nullptr) {
        availableLocaleListCount= 0;
    }
    សម្រាប់ (int32_t locCount=availableLocaleListCount-1; locCount>=0; --locCount) {
        availableLocaleList[locCount].setFromPOSIXID(uloc_getAvailable(locCount));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

const Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = availableLocaleListCount;
    ត្រឡប់ availableLocaleList;
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
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់; }
        ResourceTable resIndexTable = value.getTable(status);
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់; }
        សម្រាប់ (int32_t i = 0; resIndexTable.getKeyAndValue(i, key, value); ++i) {
            ULocAvailableType type;
            ប្រសិនបើ (uprv_strcmp(key, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } បើមិនដូច្នេះ {
                // CLDRVersion, etc.
                continue;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            ប្រសិនបើ (U_FAILURE(status)) {
                ត្រឡប់;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<const var_na**>(
                uprv_malloc(gAvailableLocaleCounts[type] * parimaa_na(const var_na*)));
            ប្រសិនបើ (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់;
            }
            សម្រាប់ (int32_t j = 0; availableLocalesTable.getKeyAndValue(j, key, value); ++j) {
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
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        ប្រសិនបើ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t defaultLocalesCount = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            ប្រសិនបើ (actualIndex < defaultLocalesCount) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } បើមិនដូច្នេះ {
                actualIndex -= defaultLocalesCount;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        const var_na* result;
        ប្រសិនបើ (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            ប្រសិនបើ (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } បើមិនដូច្នេះ {
            result = nullptr;
            ប្រសិនបើ (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        ត្រឡប់ result;
    }

    void reset(UErrorCode &status) override {
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) const override {
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់ 0; }
        ប្រសិនបើ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            ត្រឡប់ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } បើមិនដូច្នេះ {
            ត្រឡប់ gAvailableLocaleCounts[fType];
        }
    }

  private:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    សម្រាប់ (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ត្រឡប់ true;
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
    ប្រសិនបើ (status.isFailure()) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_countAvailable() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    ប្រសិនបើ (status.isFailure()) {
        ត្រឡប់ 0;
    }
    ត្រឡប់ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (type < 0 || type >= ULOC_AVAILABLE_COUNT) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    _load_installedLocales(*status);
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        new AvailableLocalesStringEnumeration(type), *status);
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ uenum_openFromStringEnumeration(result.orphan(), status);
}
