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

#ضمن "unicode/errorcode.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"
#ضمن "umutex.h"
#ضمن "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

ساكن icu::Locale*  availableLocaleList = nullptr;
ساكن int32_t  availableLocaleListCount;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_available_cleanup()
{
    𐤀𐤌 (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    availableLocaleListCount = 0;
    gInitOnceLocale.reset();

    أرجع true;
}

}  // namespace

فراغ U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    availableLocaleListCount = uloc_countAvailable();
    𐤀𐤌(availableLocaleListCount) {
       availableLocaleList = جديد Locale[availableLocaleListCount];
    }
    𐤀𐤌 (availableLocaleList == nullptr) {
        availableLocaleListCount= 0;
    }
    لكل (int32_t locCount=availableLocaleListCount-1; locCount>=0; --locCount) {
        availableLocaleList[locCount].setFromPOSIXID(uloc_getAvailable(locCount));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = availableLocaleListCount;
    أرجع availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نطاق {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

صنف AvailableLocalesSink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف *key, ResourceValue &value, UBool /*noFallback*/, UErrorCode &status) override {
        𐤀𐤌 (U_FAILURE(status)) { أرجع; }
        ResourceTable resIndexTable = value.getTable(status);
        𐤀𐤌 (U_FAILURE(status)) { أرجع; }
        لكل (int32_t i = 0; resIndexTable.getKeyAndValue(i, key, value); ++i) {
            ULocAvailableType type;
            𐤀𐤌 (uprv_strcmp(key, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            𐤀𐤌 (U_FAILURE(status)) {
                أرجع;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ثابت حرف**>(
                uprv_malloc(gAvailableLocaleCounts[type] * حجم(ثابت حرف*)));
            𐤀𐤌 (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; availableLocalesTable.getKeyAndValue(j, key, value); ++j) {
                gAvailableLocaleNames[type][j] = key;
            }
        }
    }
};

صنف AvailableLocalesStringEnumeration : عام StringEnumeration {
  عام:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ثابت حرف* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        𐤀𐤌 (U_FAILURE(status)) { أرجع nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        𐤀𐤌 (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t defaultLocalesCount = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            𐤀𐤌 (actualIndex < defaultLocalesCount) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } وإلا {
                actualIndex -= defaultLocalesCount;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ثابت حرف* result;
        𐤀𐤌 (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            𐤀𐤌 (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } وإلا {
            result = nullptr;
            𐤀𐤌 (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        أرجع result;
    }

    فراغ reset(UErrorCode &status) override {
        𐤀𐤌 (U_FAILURE(status)) { أرجع; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ثابت override {
        𐤀𐤌 (U_FAILURE(status)) { أرجع 0; }
        𐤀𐤌 (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            أرجع gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } وإلا {
            أرجع gAvailableLocaleCounts[fType];
        }
    }

  خاص:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    أرجع true;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

فراغ U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_getAllItemsWithFallback(rb.getAlias(), "", sink, status);
}

فراغ _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    𐤀𐤌 (status.isFailure()) {
        أرجع nullptr;
    }
    𐤀𐤌 (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    أرجع gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_countAvailable() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    𐤀𐤌 (status.isFailure()) {
        أرجع 0;
    }
    أرجع gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐤀𐤌 (type < 0 || type >= ULOC_AVAILABLE_COUNT) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    _load_installedLocales(*status);
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        جديد AvailableLocalesStringEnumeration(type), *status);
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    أرجع uenum_openFromStringEnumeration(result.orphan(), status);
}
