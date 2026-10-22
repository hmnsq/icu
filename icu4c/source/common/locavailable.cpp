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

#شامل "unicode/errorcode.h"
#شامل "unicode/utypes.h"
#شامل "unicode/locid.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ures.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"
#شامل "umutex.h"
#شامل "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

ایستا icu::Locale*  availableLocaleList = nullptr;
ایستا int32_t  available_Locale_List_شمار;
ایستا icu::UInitOnce gInitOnceLocale {};

فضاینام {

UBool U_CALLCONV locale_available_cleanup()
{
    اگر (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_شمار = 0;
    gInitOnceLocale.reset();

    بازگشت درست;
}

}  // namespace

تهی U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_شمار = uloc_شمار_Available();
    اگر(available_Locale_List_شمار) {
       availableLocaleList = جدید Locale[available_Locale_List_شمار];
    }
    اگر (availableLocaleList == nullptr) {
        available_Locale_List_شمار= 0;
    }
    برای (int32_t loc_شمار=available_Locale_List_شمار-1; loc_شمار>=0; --loc_شمار) {
        availableLocaleList[loc_شمار].setFromPOSIXID(uloc_getAvailable(loc_شمار));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_شمار;
    بازگشت availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

فضاینام {

// Enough capacity for the two lists in the res_index.res file
ثابت نویسه** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

رده AvailableLocalesSink : عمومی ResourceSink {
  عمومی:
    تهی put(ثابت نویسه *کلید, Resource_مقدار &value, UBool /*noFallback*/, UErrorCode &status) override {
        اگر (U_FAILURE(status)) { بازگشت; }
        ResourceTable resIndexTable = value.getTable(status);
        اگر (U_FAILURE(status)) { بازگشت; }
        برای (int32_t i = 0; resIndexTable.get_کلید_And_مقدار(i, کلید, value); ++i) {
            ULocAvailableType type;
            اگر (uprv_strcmp(کلید, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } وگرنه اگر (uprv_strcmp(کلید, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } وگرنه {
                // CLDRVersion, etc.
                ادامه;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            اگر (U_FAILURE(status)) {
                بازگشت;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<ثابت نویسه**>(
                uprv_malloc(gAvailableLocaleCounts[type] * اندازه(ثابت نویسه*)));
            اگر (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            برای (int32_t j = 0; availableLocalesTable.get_کلید_And_مقدار(j, کلید, value); ++j) {
                gAvailableLocaleNames[type][j] = کلید;
            }
        }
    }
};

رده AvailableLocalesStringEnumeration : عمومی StringEnumeration {
  عمومی:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    ثابت نویسه* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        اگر (U_FAILURE(status)) { بازگشت nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        اگر (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_شمار = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            اگر (actualIndex < default_Locales_شمار) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } وگرنه {
                actualIndex -= default_Locales_شمار;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        ثابت نویسه* result;
        اگر (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            اگر (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } وگرنه {
            result = nullptr;
            اگر (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        بازگشت result;
    }

    تهی reset(UErrorCode &status) override {
        اگر (U_FAILURE(status)) { بازگشت; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) ثابت override {
        اگر (U_FAILURE(status)) { بازگشت 0; }
        اگر (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            بازگشت gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } وگرنه {
            بازگشت gAvailableLocaleCounts[fType];
        }
    }

  خصوصی:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    برای (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    بازگشت درست;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

تهی U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_اقلام_With_Fallback(rb.getAlias(), "", sink, status);
}

تهی _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI ثابت نویسه* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    اگر (status.isFailure()) {
        بازگشت nullptr;
    }
    اگر (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    بازگشت gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_شمار_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    اگر (status.isFailure()) {
        بازگشت 0;
    }
    بازگشت gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (type < 0 || type >= ULOC_AVAILABLE_شمار) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    _load_installedLocales(*status);
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        جدید AvailableLocalesStringEnumeration(type), *status);
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    بازگشت uenum_openFromStringEnumeration(result.orphan(), status);
}
