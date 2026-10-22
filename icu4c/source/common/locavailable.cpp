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

#समावेश "unicode/errorcode.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ures.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "ucln_cmn.h"
#समावेश "uassert.h"
#समावेश "umutex.h"
#समावेश "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

अटल icu::Locale*  availableLocaleList = nullptr;
अटल int32_t  available_Locale_List_गिनती;
अटल icu::UInitOnce gInitOnceLocale {};

namespace {

UBool U_CALLCONV locale_available_cleanup()
{
    अगर (availableLocaleList) {
        मिटाओ []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_गिनती = 0;
    gInitOnceLocale.reset();

    वापस true;
}

}  // namespace

व्योम U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_गिनती = uloc_गिनती_Available();
    अगर(available_Locale_List_गिनती) {
       availableLocaleList = नया Locale[available_Locale_List_गिनती];
    }
    अगर (availableLocaleList == nullptr) {
        available_Locale_List_गिनती= 0;
    }
    क्रम (int32_t loc_गिनती=available_Locale_List_गिनती-1; loc_गिनती>=0; --loc_गिनती) {
        availableLocaleList[loc_गिनती].setFromPOSIXID(uloc_getAvailable(loc_गिनती));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

स्थिर Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_गिनती;
    वापस availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

namespace {

// Enough capacity for the two lists in the res_index.res file
स्थिर अक्षर** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

श्रेणी AvailableLocalesSink : खुला ResourceSink {
  खुला:
    व्योम दो(स्थिर अक्षर *कुंजी, Resource_मूल्य &value, UBool /*noFallback*/, UErrorCode &status) override {
        अगर (U_FAILURE(status)) { वापस; }
        ResourceTable res_सूचकांक_Table = value.getTable(status);
        अगर (U_FAILURE(status)) { वापस; }
        क्रम (int32_t i = 0; res_सूचकांक_Table.get_कुंजी_And_मूल्य(i, कुंजी, value); ++i) {
            ULocAvailableType type;
            अगर (uprv_strcmp(कुंजी, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } अन्यथा {
                // CLDRVersion, etc.
                जारी;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            अगर (U_FAILURE(status)) {
                वापस;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.get_आकार();
            gAvailableLocaleNames[type] = static_cast<स्थिर अक्षर**>(
                uprv_malloc(gAvailableLocaleCounts[type] * माप(स्थिर अक्षर*)));
            अगर (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            क्रम (int32_t j = 0; availableLocalesTable.get_कुंजी_And_मूल्य(j, कुंजी, value); ++j) {
                gAvailableLocaleNames[type][j] = कुंजी;
            }
        }
    }
};

श्रेणी AvailableLocalesStringEnumeration : खुला StringEnumeration {
  खुला:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    स्थिर अक्षर* next(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND override {
        अगर (U_FAILURE(status)) { वापस nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actual_सूचकांक = f_सूचकांक++;

        // If the "combined" list was requested, resolve that now
        अगर (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_गिनती = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            अगर (actual_सूचकांक < default_Locales_गिनती) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा {
                actual_सूचकांक -= default_Locales_गिनती;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        स्थिर अक्षर* परिणाम;
        अगर (actual_सूचकांक < count) {
            परिणाम = gAvailableLocaleNames[actualType][actual_सूचकांक];
            अगर (परिणाम_लंबाई != nullptr) {
                *परिणाम_लंबाई = static_cast<int32_t>(uprv_strlen(परिणाम));
            }
        } अन्यथा {
            परिणाम = nullptr;
            अगर (परिणाम_लंबाई != nullptr) {
                *परिणाम_लंबाई = 0;
            }
        }
        वापस परिणाम;
    }

    व्योम reset(UErrorCode &status) override {
        अगर (U_FAILURE(status)) { वापस; }
        f_सूचकांक = 0;
    }

    int32_t count(UErrorCode &status) स्थिर override {
        अगर (U_FAILURE(status)) { वापस 0; }
        अगर (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            वापस gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } अन्यथा {
            वापस gAvailableLocaleCounts[fType];
        }
    }

  निजी:
    ULocAvailableType fType;
    int32_t f_सूचकांक = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    वापस true;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

व्योम U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_वस्तुएँ_With_Fallback(rb.getAlias(), "", sink, status);
}

व्योम _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI स्थिर अक्षर* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    अगर (status.isFailure()) {
        वापस nullptr;
    }
    अगर (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    वापस gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_गिनती_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    अगर (status.isFailure()) {
        वापस 0;
    }
    वापस gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (type < 0 || type >= ULOC_AVAILABLE_गिनती) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    _load_installedLocales(*status);
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> परिणाम(
        नया AvailableLocalesStringEnumeration(type), *status);
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    वापस uenum_openFromStringEnumeration(परिणाम.orphan(), status);
}
