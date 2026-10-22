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
ساكن int32_t  available_Locale_List_عدد;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_available_cleanup()
{
    إذا (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_عدد = 0;
    gInitOnceLocale.reset();

    أرجع true;
}

}  // namespace

فراغ U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_عدد = uloc_عدد_Available();
    إذا(available_Locale_List_عدد) {
       availableLocaleList = جديد Locale[available_Locale_List_عدد];
    }
    إذا (availableLocaleList == nullptr) {
        available_Locale_List_عدد= 0;
    }
    لكل (int32_t loc_عدد=available_Locale_List_عدد-1; loc_عدد>=0; --loc_عدد) {
        availableLocaleList[loc_عدد].setFromPOSIXID(uloc_getAvailable(loc_عدد));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_عدد;
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
    فراغ put(ثابت حرف *مفتاح, Resource_قيمة &value, UBool /*noFallback*/, UErrorCode &status) override {
        إذا (U_FAILURE(status)) { أرجع; }
        ResourceTable resIndexTable = value.getTable(status);
        إذا (U_FAILURE(status)) { أرجع; }
        لكل (int32_t i = 0; resIndexTable.get_مفتاح_And_قيمة(i, مفتاح, value); ++i) {
            ULocAvailableType type;
            إذا (uprv_strcmp(مفتاح, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } وإلا إذا (uprv_strcmp(مفتاح, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            إذا (U_FAILURE(status)) {
                أرجع;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ثابت حرف**>(
                uprv_malloc(gAvailableLocaleCounts[type] * حجم(ثابت حرف*)));
            إذا (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; availableLocalesTable.get_مفتاح_And_قيمة(j, مفتاح, value); ++j) {
                gAvailableLocaleNames[type][j] = مفتاح;
            }
        }
    }
};

صنف AvailableLocalesStringEnumeration : عام StringEnumeration {
  عام:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ثابت حرف* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(status)) { أرجع nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        إذا (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_عدد = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            إذا (actualIndex < default_Locales_عدد) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } وإلا {
                actualIndex -= default_Locales_عدد;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ثابت حرف* result;
        إذا (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            إذا (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } وإلا {
            result = nullptr;
            إذا (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        أرجع result;
    }

    فراغ reset(UErrorCode &status) override {
        إذا (U_FAILURE(status)) { أرجع; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ثابت override {
        إذا (U_FAILURE(status)) { أرجع 0; }
        إذا (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
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
    ures_get_All_عناصر_With_Fallback(rb.getAlias(), "", sink, status);
}

فراغ _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    إذا (status.isFailure()) {
        أرجع nullptr;
    }
    إذا (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    أرجع gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_عدد_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    إذا (status.isFailure()) {
        أرجع 0;
    }
    أرجع gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    إذا (type < 0 || type >= ULOC_AVAILABLE_عدد) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    _load_installedLocales(*status);
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        جديد AvailableLocalesStringEnumeration(type), *status);
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    أرجع uenum_openFromStringEnumeration(result.orphan(), status);
}
