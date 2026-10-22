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

#অন্তর্ভাব "unicode/errorcode.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

স্থির icu::Locale*  availableLocaleList = nullptr;
স্থির int32_t  available_Locale_List_গুনতি;
স্থির icu::UInitOnce gInitOnceLocale {};

নামাকাশ {

UBool U_CALLCONV locale_available_cleanup()
{
    যদি (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_গুনতি = 0;
    gInitOnceLocale.reset();

    প্রত্যাগম সত্য;
}

}  // namespace

শূন্য U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_গুনতি = uloc_গুনতি_Available();
    যদি(available_Locale_List_গুনতি) {
       availableLocaleList = নূতন Locale[available_Locale_List_গুনতি];
    }
    যদি (availableLocaleList == nullptr) {
        available_Locale_List_গুনতি= 0;
    }
    প্রতি (int32_t loc_গুনতি=available_Locale_List_গুনতি-1; loc_গুনতি>=0; --loc_গুনতি) {
        availableLocaleList[loc_গুনতি].setFromPOSIXID(uloc_getAvailable(loc_গুনতি));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ধ্রুব Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_গুনতি;
    প্রত্যাগম availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

নামাকাশ {

// Enough capacity for the two lists in the res_index.res file
ধ্রুব বর্ণ** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

বর্গ AvailableLocalesSink : সার্বজনিক ResourceSink {
  সার্বজনিক:
    শূন্য put(ধ্রুব বর্ণ *চাবি, Resource_মান &value, UBool /*noFallback*/, UErrorCode &status) override {
        যদি (U_FAILURE(status)) { প্রত্যাগম; }
        ResourceTable resIndexTable = value.getTable(status);
        যদি (U_FAILURE(status)) { প্রত্যাগম; }
        প্রতি (int32_t i = 0; resIndexTable.get_চাবি_And_মান(i, চাবি, value); ++i) {
            ULocAvailableType type;
            যদি (uprv_strcmp(চাবি, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } অন্যথা যদি (uprv_strcmp(চাবি, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } অন্যথা {
                // CLDRVersion, etc.
                অনুবর্ত;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            যদি (U_FAILURE(status)) {
                প্রত্যাগম;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ধ্রুব বর্ণ**>(
                uprv_malloc(gAvailableLocaleCounts[type] * পরিমাণ(ধ্রুব বর্ণ*)));
            যদি (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            প্রতি (int32_t j = 0; availableLocalesTable.get_চাবি_And_মান(j, চাবি, value); ++j) {
                gAvailableLocaleNames[type][j] = চাবি;
            }
        }
    }
};

বর্গ AvailableLocalesStringEnumeration : সার্বজনিক StringEnumeration {
  সার্বজনিক:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ধ্রুব বর্ণ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (U_FAILURE(status)) { প্রত্যাগম nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        যদি (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_গুনতি = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            যদি (actualIndex < default_Locales_গুনতি) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } অন্যথা {
                actualIndex -= default_Locales_গুনতি;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ধ্রুব বর্ণ* result;
        যদি (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            যদি (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } অন্যথা {
            result = nullptr;
            যদি (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        প্রত্যাগম result;
    }

    শূন্য reset(UErrorCode &status) override {
        যদি (U_FAILURE(status)) { প্রত্যাগম; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ধ্রুব override {
        যদি (U_FAILURE(status)) { প্রত্যাগম 0; }
        যদি (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            প্রত্যাগম gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } অন্যথা {
            প্রত্যাগম gAvailableLocaleCounts[fType];
        }
    }

  গুপ্ত:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    প্রতি (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    প্রত্যাগম সত্য;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

শূন্য U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_বস্তুসমূহ_With_Fallback(rb.getAlias(), "", sink, status);
}

শূন্য _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ধ্রুব বর্ণ* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    যদি (status.isFailure()) {
        প্রত্যাগম nullptr;
    }
    যদি (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_গুনতি_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    যদি (status.isFailure()) {
        প্রত্যাগম 0;
    }
    প্রত্যাগম gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }
    যদি (type < 0 || type >= ULOC_AVAILABLE_গুনতি) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    _load_installedLocales(*status);
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        নূতন AvailableLocalesStringEnumeration(type), *status);
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম uenum_openFromStringEnumeration(result.orphan(), status);
}
