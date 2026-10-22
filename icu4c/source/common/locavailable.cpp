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
    nếu (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    availableLocaleListCount = 0;
    gInitOnceLocale.reset();

    trả_về true;
}

}  // namespace

void U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    availableLocaleListCount = uloc_countAvailable();
    nếu(availableLocaleListCount) {
       availableLocaleList = new Locale[availableLocaleListCount];
    }
    nếu (availableLocaleList == nullptr) {
        availableLocaleListCount= 0;
    }
    cho (int32_t locCount=availableLocaleListCount-1; locCount>=0; --locCount) {
        availableLocaleList[locCount].setFromPOSIXID(uloc_getAvailable(locCount));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

const Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = availableLocaleListCount;
    trả_về availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

namespace {

// Enough capacity for the two lists in the res_index.res file
const char** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

class AvailableLocalesSink : public ResourceSink {
  public:
    void put(const char *key, ResourceValue &value, UBool /*noFallback*/, UErrorCode &status) override {
        nếu (U_FAILURE(status)) { trả_về; }
        ResourceTable resIndexTable = value.getTable(status);
        nếu (U_FAILURE(status)) { trả_về; }
        cho (int32_t i = 0; resIndexTable.getKeyAndValue(i, key, value); ++i) {
            ULocAvailableType type;
            nếu (uprv_strcmp(key, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } không_thì nếu (uprv_strcmp(key, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } không_thì {
                // CLDRVersion, etc.
                continue;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            nếu (U_FAILURE(status)) {
                trả_về;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<const char**>(
                uprv_malloc(gAvailableLocaleCounts[type] * sizeof(const char*)));
            nếu (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                trả_về;
            }
            cho (int32_t j = 0; availableLocalesTable.getKeyAndValue(j, key, value); ++j) {
                gAvailableLocaleNames[type][j] = key;
            }
        }
    }
};

class AvailableLocalesStringEnumeration : public StringEnumeration {
  public:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    const char* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        nếu (U_FAILURE(status)) { trả_về nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        nếu (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t defaultLocalesCount = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            nếu (actualIndex < defaultLocalesCount) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } không_thì {
                actualIndex -= defaultLocalesCount;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        const char* result;
        nếu (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            nếu (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } không_thì {
            result = nullptr;
            nếu (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        trả_về result;
    }

    void reset(UErrorCode &status) override {
        nếu (U_FAILURE(status)) { trả_về; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) const override {
        nếu (U_FAILURE(status)) { trả_về 0; }
        nếu (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            trả_về gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } không_thì {
            trả_về gAvailableLocaleCounts[fType];
        }
    }

  private:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    cho (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    trả_về true;
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

U_CAPI const char* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    nếu (status.isFailure()) {
        trả_về nullptr;
    }
    nếu (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    trả_về gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_countAvailable() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    nếu (status.isFailure()) {
        trả_về 0;
    }
    trả_về gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    nếu (type < 0 || type >= ULOC_AVAILABLE_COUNT) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    _load_installedLocales(*status);
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        new AvailableLocalesStringEnumeration(type), *status);
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    trả_về uenum_openFromStringEnumeration(result.orphan(), status);
}
