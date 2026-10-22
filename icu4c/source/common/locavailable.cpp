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

#அந்தர்பாவ "unicode/errorcode.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

நிலையான icu::Locale*  availableLocaleList = nullptr;
நிலையான int32_t  available_Locale_List_மொத்தம்;
நிலையான icu::UInitOnce gInitOnceLocale {};

நாமாகாஶ {

UBool U_CALLCONV locale_available_cleanup()
{
    எனில் (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_மொத்தம் = 0;
    gInitOnceLocale.reset();

    திரும்பு உண்மை;
}

}  // namespace

வெறுமை U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_மொத்தம் = uloc_மொத்தம்_Available();
    எனில்(available_Locale_List_மொத்தம்) {
       availableLocaleList = புதிய Locale[available_Locale_List_மொத்தம்];
    }
    எனில் (availableLocaleList == nullptr) {
        available_Locale_List_மொத்தம்= 0;
    }
    க்கு (int32_t loc_மொத்தம்=available_Locale_List_மொத்தம்-1; loc_மொத்தம்>=0; --loc_மொத்தம்) {
        availableLocaleList[loc_மொத்தம்].setFromPOSIXID(uloc_getAvailable(loc_மொத்தம்));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

மாறிலி Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_மொத்தம்;
    திரும்பு availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

நாமாகாஶ {

// Enough capacity for the two lists in the res_index.res file
மாறிலி எழுத்து** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

வகுப்பு AvailableLocalesSink : பொது ResourceSink {
  பொது:
    வெறுமை put(மாறிலி எழுத்து *சாவி, Resource_மதிப்பு &value, UBool /*noFallback*/, UErrorCode &status) override {
        எனில் (U_FAILURE(status)) { திரும்பு; }
        ResourceTable resIndexTable = value.getTable(status);
        எனில் (U_FAILURE(status)) { திரும்பு; }
        க்கு (int32_t i = 0; resIndexTable.get_சாவி_And_மதிப்பு(i, சாவி, value); ++i) {
            ULocAvailableType type;
            எனில் (uprv_strcmp(சாவி, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } இல்லையெனில் {
                // CLDRVersion, etc.
                தொடர்;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            எனில் (U_FAILURE(status)) {
                திரும்பு;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<மாறிலி எழுத்து**>(
                uprv_malloc(gAvailableLocaleCounts[type] * அளவு(மாறிலி எழுத்து*)));
            எனில் (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            க்கு (int32_t j = 0; availableLocalesTable.get_சாவி_And_மதிப்பு(j, சாவி, value); ++j) {
                gAvailableLocaleNames[type][j] = சாவி;
            }
        }
    }
};

வகுப்பு AvailableLocalesStringEnumeration : பொது StringEnumeration {
  பொது:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    மாறிலி எழுத்து* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        எனில் (U_FAILURE(status)) { திரும்பு nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        எனில் (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_மொத்தம் = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            எனில் (actualIndex < default_Locales_மொத்தம்) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } இல்லையெனில் {
                actualIndex -= default_Locales_மொத்தம்;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        மாறிலி எழுத்து* result;
        எனில் (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            எனில் (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } இல்லையெனில் {
            result = nullptr;
            எனில் (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        திரும்பு result;
    }

    வெறுமை reset(UErrorCode &status) override {
        எனில் (U_FAILURE(status)) { திரும்பு; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) மாறிலி override {
        எனில் (U_FAILURE(status)) { திரும்பு 0; }
        எனில் (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            திரும்பு gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } இல்லையெனில் {
            திரும்பு gAvailableLocaleCounts[fType];
        }
    }

  தனிப்பட்ட:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    திரும்பு உண்மை;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

வெறுமை U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_பொருட்கள்_With_Fallback(rb.getAlias(), "", sink, status);
}

வெறுமை _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    எனில் (status.isFailure()) {
        திரும்பு nullptr;
    }
    எனில் (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_மொத்தம்_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    எனில் (status.isFailure()) {
        திரும்பு 0;
    }
    திரும்பு gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (type < 0 || type >= ULOC_AVAILABLE_மொத்தம்) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    _load_installedLocales(*status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        புதிய AvailableLocalesStringEnumeration(type), *status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    திரும்பு uenum_openFromStringEnumeration(result.orphan(), status);
}
