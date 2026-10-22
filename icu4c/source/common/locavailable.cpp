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

#അന്തര്ഭാവ "unicode/errorcode.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

സ്ഥിതം icu::Locale*  availableLocaleList = nullptr;
സ്ഥിതം int32_t  available_Locale_List_എണ്ണം;
സ്ഥിതം icu::UInitOnce gInitOnceLocale {};

നാമാകാശ {

UBool U_CALLCONV locale_available_cleanup()
{
    എങ്കിൽ (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_എണ്ണം = 0;
    gInitOnceLocale.reset();

    തിരികെ സത്യം;
}

}  // namespace

ശൂന്യം U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_എണ്ണം = uloc_എണ്ണം_Available();
    എങ്കിൽ(available_Locale_List_എണ്ണം) {
       availableLocaleList = പുതിയ Locale[available_Locale_List_എണ്ണം];
    }
    എങ്കിൽ (availableLocaleList == nullptr) {
        available_Locale_List_എണ്ണം= 0;
    }
    വേണ്ടി (int32_t loc_എണ്ണം=available_Locale_List_എണ്ണം-1; loc_എണ്ണം>=0; --loc_എണ്ണം) {
        availableLocaleList[loc_എണ്ണം].setFromPOSIXID(uloc_getAvailable(loc_എണ്ണം));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

സ്ഥിരം Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_എണ്ണം;
    തിരികെ availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

നാമാകാശ {

// Enough capacity for the two lists in the res_index.res file
സ്ഥിരം അക്ഷരം** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

വർഗ്ഗം AvailableLocalesSink : പൊതു ResourceSink {
  പൊതു:
    ശൂന്യം put(സ്ഥിരം അക്ഷരം *താക്കോൽ, Resource_മൂല്യം &value, UBool /*noFallback*/, UErrorCode &status) override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        ResourceTable resIndexTable = value.getTable(status);
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        വേണ്ടി (int32_t i = 0; resIndexTable.get_താക്കോൽ_And_മൂല്യം(i, താക്കോൽ, value); ++i) {
            ULocAvailableType type;
            എങ്കിൽ (uprv_strcmp(താക്കോൽ, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } അല്ലെങ്കിൽ {
                // CLDRVersion, etc.
                തുടരുക;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<സ്ഥിരം അക്ഷരം**>(
                uprv_malloc(gAvailableLocaleCounts[type] * വലുപ്പം(സ്ഥിരം അക്ഷരം*)));
            എങ്കിൽ (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            വേണ്ടി (int32_t j = 0; availableLocalesTable.get_താക്കോൽ_And_മൂല്യം(j, താക്കോൽ, value); ++j) {
                gAvailableLocaleNames[type][j] = താക്കോൽ;
            }
        }
    }
};

വർഗ്ഗം AvailableLocalesStringEnumeration : പൊതു StringEnumeration {
  പൊതു:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    സ്ഥിരം അക്ഷരം* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        എങ്കിൽ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_എണ്ണം = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            എങ്കിൽ (actualIndex < default_Locales_എണ്ണം) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } അല്ലെങ്കിൽ {
                actualIndex -= default_Locales_എണ്ണം;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        സ്ഥിരം അക്ഷരം* result;
        എങ്കിൽ (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            എങ്കിൽ (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } അല്ലെങ്കിൽ {
            result = nullptr;
            എങ്കിൽ (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        തിരികെ result;
    }

    ശൂന്യം reset(UErrorCode &status) override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) സ്ഥിരം override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ 0; }
        എങ്കിൽ (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            തിരികെ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } അല്ലെങ്കിൽ {
            തിരികെ gAvailableLocaleCounts[fType];
        }
    }

  സ്വകാര്യ:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    തിരികെ സത്യം;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

ശൂന്യം U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_ഇനങ്ങൾ_With_Fallback(rb.getAlias(), "", sink, status);
}

ശൂന്യം _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    എങ്കിൽ (status.isFailure()) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_എണ്ണം_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    എങ്കിൽ (status.isFailure()) {
        തിരികെ 0;
    }
    തിരികെ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (type < 0 || type >= ULOC_AVAILABLE_എണ്ണം) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    _load_installedLocales(*status);
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        പുതിയ AvailableLocalesStringEnumeration(type), *status);
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    തിരികെ uenum_openFromStringEnumeration(result.orphan(), status);
}
