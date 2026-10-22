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

    ফেরত সত্য;
}

}  // namespace

শূন্য U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_গুনতি = uloc_গুনতি_Available();
    যদি(available_Locale_List_গুনতি) {
       availableLocaleList = নতুন Locale[available_Locale_List_গুনতি];
    }
    যদি (availableLocaleList == nullptr) {
        available_Locale_List_গুনতি= 0;
    }
    জন্য (int32_t loc_গুনতি=available_Locale_List_গুনতি-1; loc_গুনতি>=0; --loc_গুনতি) {
        availableLocaleList[loc_গুনতি].setFromPOSIXID(uloc_getAvailable(loc_গুনতি));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ধ্রুবক Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_গুনতি;
    ফেরত availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

নামাকাশ {

// Enough capacity for the two lists in the res_index.res file
ধ্রুবক অক্ষর** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

শ্রেণী AvailableLocalesSink : সর্বজনীন ResourceSink {
  সর্বজনীন:
    শূন্য put(ধ্রুবক অক্ষর *চাবি, Resource_মান &value, UBool /*noFallback*/, UErrorCode &status) override {
        যদি (U_FAILURE(status)) { ফেরত; }
        ResourceTable res_সূচক_Table = value.getTable(status);
        যদি (U_FAILURE(status)) { ফেরত; }
        জন্য (int32_t i = 0; res_সূচক_Table.get_চাবি_And_মান(i, চাবি, value); ++i) {
            ULocAvailableType type;
            যদি (uprv_strcmp(চাবি, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } অন্যথা যদি (uprv_strcmp(চাবি, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } অন্যথা {
                // CLDRVersion, etc.
                চালু;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            যদি (U_FAILURE(status)) {
                ফেরত;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.get_আকার();
            gAvailableLocaleNames[type] = static_cast<ধ্রুবক অক্ষর**>(
                uprv_malloc(gAvailableLocaleCounts[type] * আকার(ধ্রুবক অক্ষর*)));
            যদি (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            জন্য (int32_t j = 0; availableLocalesTable.get_চাবি_And_মান(j, চাবি, value); ++j) {
                gAvailableLocaleNames[type][j] = চাবি;
            }
        }
    }
};

শ্রেণী AvailableLocalesStringEnumeration : সর্বজনীন StringEnumeration {
  সর্বজনীন:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ধ্রুবক অক্ষর* next(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (U_FAILURE(status)) { ফেরত nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actual_সূচক = f_সূচক++;

        // If the "combined" list was requested, resolve that now
        যদি (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_গুনতি = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            যদি (actual_সূচক < default_Locales_গুনতি) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } অন্যথা {
                actual_সূচক -= default_Locales_গুনতি;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ধ্রুবক অক্ষর* ফলাফল;
        যদি (actual_সূচক < count) {
            ফলাফল = gAvailableLocaleNames[actualType][actual_সূচক];
            যদি (ফলাফল_দৈর্ঘ্য != nullptr) {
                *ফলাফল_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(ফলাফল));
            }
        } অন্যথা {
            ফলাফল = nullptr;
            যদি (ফলাফল_দৈর্ঘ্য != nullptr) {
                *ফলাফল_দৈর্ঘ্য = 0;
            }
        }
        ফেরত ফলাফল;
    }

    শূন্য reset(UErrorCode &status) override {
        যদি (U_FAILURE(status)) { ফেরত; }
        f_সূচক = 0;
    }

    int32_t count(UErrorCode &status) ধ্রুবক override {
        যদি (U_FAILURE(status)) { ফেরত 0; }
        যদি (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            ফেরত gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } অন্যথা {
            ফেরত gAvailableLocaleCounts[fType];
        }
    }

  ব্যক্তিগত:
    ULocAvailableType fType;
    int32_t f_সূচক = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    জন্য (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ফেরত সত্য;
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

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    যদি (status.isFailure()) {
        ফেরত nullptr;
    }
    যদি (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ফেরত gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_গুনতি_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    যদি (status.isFailure()) {
        ফেরত 0;
    }
    ফেরত gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (type < 0 || type >= ULOC_AVAILABLE_গুনতি) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    _load_installedLocales(*status);
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> ফলাফল(
        নতুন AvailableLocalesStringEnumeration(type), *status);
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    ফেরত uenum_openFromStringEnumeration(ফলাফল.orphan(), status);
}
