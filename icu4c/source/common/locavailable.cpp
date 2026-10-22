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

#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

स्थिर icu::Locale*  availableLocaleList = nullptr;
स्थिर int32_t  available_Locale_List_मोजणी;
स्थिर icu::UInitOnce gInitOnceLocale {};

नामाकाश {

UBool U_CALLCONV locale_available_cleanup()
{
    जर (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_मोजणी = 0;
    gInitOnceLocale.reset();

    परत सत्य;
}

}  // namespace

शून्य U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_मोजणी = uloc_मोजणी_Available();
    जर(available_Locale_List_मोजणी) {
       availableLocaleList = नवीन Locale[available_Locale_List_मोजणी];
    }
    जर (availableLocaleList == nullptr) {
        available_Locale_List_मोजणी= 0;
    }
    साठी (int32_t loc_मोजणी=available_Locale_List_मोजणी-1; loc_मोजणी>=0; --loc_मोजणी) {
        availableLocaleList[loc_मोजणी].setFromPOSIXID(uloc_getAvailable(loc_मोजणी));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

स्थिरांक Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_मोजणी;
    परत availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

नामाकाश {

// Enough capacity for the two lists in the res_index.res file
स्थिरांक अक्षर** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

वर्ग AvailableLocalesSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(स्थिरांक अक्षर *किल्ली, Resource_मूल्य &value, UBool /*noFallback*/, UErrorCode &status) override {
        जर (U_FAILURE(status)) { परत; }
        ResourceTable resIndexTable = value.getTable(status);
        जर (U_FAILURE(status)) { परत; }
        साठी (int32_t i = 0; resIndexTable.get_किल्ली_And_मूल्य(i, किल्ली, value); ++i) {
            ULocAvailableType type;
            जर (uprv_strcmp(किल्ली, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा जर (uprv_strcmp(किल्ली, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } अन्यथा {
                // CLDRVersion, etc.
                सुरूठेवा;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            जर (U_FAILURE(status)) {
                परत;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<स्थिरांक अक्षर**>(
                uprv_malloc(gAvailableLocaleCounts[type] * आकार(स्थिरांक अक्षर*)));
            जर (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            साठी (int32_t j = 0; availableLocalesTable.get_किल्ली_And_मूल्य(j, किल्ली, value); ++j) {
                gAvailableLocaleNames[type][j] = किल्ली;
            }
        }
    }
};

वर्ग AvailableLocalesStringEnumeration : सार्वजनिक StringEnumeration {
  सार्वजनिक:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    स्थिरांक अक्षर* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        जर (U_FAILURE(status)) { परत nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        जर (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_मोजणी = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            जर (actualIndex < default_Locales_मोजणी) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा {
                actualIndex -= default_Locales_मोजणी;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        स्थिरांक अक्षर* result;
        जर (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            जर (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } अन्यथा {
            result = nullptr;
            जर (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        परत result;
    }

    शून्य reset(UErrorCode &status) override {
        जर (U_FAILURE(status)) { परत; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) स्थिरांक override {
        जर (U_FAILURE(status)) { परत 0; }
        जर (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            परत gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } अन्यथा {
            परत gAvailableLocaleCounts[fType];
        }
    }

  खाजगी:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    साठी (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    परत सत्य;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

शून्य U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_वस्तुसंच_With_Fallback(rb.getAlias(), "", sink, status);
}

शून्य _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    जर (status.isFailure()) {
        परत nullptr;
    }
    जर (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    परत gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_मोजणी_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    जर (status.isFailure()) {
        परत 0;
    }
    परत gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (type < 0 || type >= ULOC_AVAILABLE_मोजणी) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    _load_installedLocales(*status);
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        नवीन AvailableLocalesStringEnumeration(type), *status);
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    परत uenum_openFromStringEnumeration(result.orphan(), status);
}
