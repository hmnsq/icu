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
स्थिर int32_t  available_Locale_List_गन्ती;
स्थिर icu::UInitOnce gInitOnceLocale {};

नामाकाश {

UBool U_CALLCONV locale_available_cleanup()
{
    यदि (availableLocaleList) {
        delete []availableLocaleList;
        availableLocaleList = nullptr;
    }
    available_Locale_List_गन्ती = 0;
    gInitOnceLocale.reset();

    फर्काऊ सत्य;
}

}  // namespace

शून्य U_CALLCONV locale_available_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    available_Locale_List_गन्ती = uloc_गन्ती_Available();
    यदि(available_Locale_List_गन्ती) {
       availableLocaleList = नूतन Locale[available_Locale_List_गन्ती];
    }
    यदि (availableLocaleList == nullptr) {
        available_Locale_List_गन्ती= 0;
    }
    लागि (int32_t loc_गन्ती=available_Locale_List_गन्ती-1; loc_गन्ती>=0; --loc_गन्ती) {
        availableLocaleList[loc_गन्ती].setFromPOSIXID(uloc_getAvailable(loc_गन्ती));
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_AVAILABLE, locale_available_cleanup);
}

अचर Locale* U_EXPORT2
Locale::getAvailableLocales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_available_init);
    count = available_Locale_List_गन्ती;
    फर्काऊ availableLocaleList;
}


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

नामाकाश {

// Enough capacity for the two lists in the res_index.res file
अचर वर्ण** gAvailableLocaleNames[2] = {};
int32_t gAvailableLocaleCounts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

वर्ग AvailableLocalesSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(अचर वर्ण *साँचो, Resource_मान &value, UBool /*noFallback*/, UErrorCode &status) override {
        यदि (U_FAILURE(status)) { फर्काऊ; }
        ResourceTable resIndexTable = value.getTable(status);
        यदि (U_FAILURE(status)) { फर्काऊ; }
        लागि (int32_t i = 0; resIndexTable.get_साँचो_And_मान(i, साँचो, value); ++i) {
            ULocAvailableType type;
            यदि (uprv_strcmp(साँचो, "InstalledLocales") == 0) {
                type = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा यदि (uprv_strcmp(साँचो, "AliasLocales") == 0) {
                type = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            } अन्यथा {
                // CLDRVersion, etc.
                जारी;
            }
            ResourceTable availableLocalesTable = value.getTable(status);
            यदि (U_FAILURE(status)) {
                फर्काऊ;
            }
            gAvailableLocaleCounts[type] = availableLocalesTable.getSize();
            gAvailableLocaleNames[type] = static_cast<अचर वर्ण**>(
                uprv_malloc(gAvailableLocaleCounts[type] * परिमाण(अचर वर्ण*)));
            यदि (gAvailableLocaleNames[type] == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
            लागि (int32_t j = 0; availableLocalesTable.get_साँचो_And_मान(j, साँचो, value); ++j) {
                gAvailableLocaleNames[type][j] = साँचो;
            }
        }
    }
};

वर्ग AvailableLocalesStringEnumeration : सार्वजनिक StringEnumeration {
  सार्वजनिक:
    AvailableLocalesStringEnumeration(ULocAvailableType type) : fType(type) {
    }

    अचर वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        यदि (U_FAILURE(status)) { फर्काऊ nullptr; }
        ULocAvailableType actualType = fType;
        int32_t actualIndex = fIndex++;

        // If the "combined" list was requested, resolve that now
        यदि (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_गन्ती = gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT];
            यदि (actualIndex < default_Locales_गन्ती) {
                actualType = ULOC_AVAILABLE_DEFAULT;
            } अन्यथा {
                actualIndex -= default_Locales_गन्ती;
                actualType = ULOC_AVAILABLE_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = gAvailableLocaleCounts[actualType];
        अचर वर्ण* result;
        यदि (actualIndex < count) {
            result = gAvailableLocaleNames[actualType][actualIndex];
            यदि (resultLength != nullptr) {
                *resultLength = static_cast<int32_t>(uprv_strlen(result));
            }
        } अन्यथा {
            result = nullptr;
            यदि (resultLength != nullptr) {
                *resultLength = 0;
            }
        }
        फर्काऊ result;
    }

    शून्य reset(UErrorCode &status) override {
        यदि (U_FAILURE(status)) { फर्काऊ; }
        fIndex = 0;
    }

    int32_t count(UErrorCode &status) अचर override {
        यदि (U_FAILURE(status)) { फर्काऊ 0; }
        यदि (fType == ULOC_AVAILABLE_WITH_LEGACY_ALIASES) {
            फर्काऊ gAvailableLocaleCounts[ULOC_AVAILABLE_DEFAULT]
                + gAvailableLocaleCounts[ULOC_AVAILABLE_ONLY_LEGACY_ALIASES];
        } अन्यथा {
            फर्काऊ gAvailableLocaleCounts[fType];
        }
    }

  गुप्त:
    ULocAvailableType fType;
    int32_t fIndex = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(gAvailableLocaleNames); i++) {
        uprv_free(gAvailableLocaleNames[i]);
        gAvailableLocaleNames[i] = nullptr;
        gAvailableLocaleCounts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    फर्काऊ सत्य;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

शून्य U_CALLCONV loadInstalledLocales(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::LocalUResourceBundlePointer rb(ures_openDirect(nullptr, "res_index", &status));
    AvailableLocalesSink sink;
    ures_get_All_वस्तुहरू_With_Fallback(rb.getAlias(), "", sink, status);
}

शून्य _load_installedLocales(UErrorCode& status) {
    umtx_initOnce(ginstalledLocalesInitOnce, &loadInstalledLocales, status);
}

} // namespace

U_CAPI अचर वर्ण* U_EXPORT2
uloc_getAvailable(int32_t offset) {
    icu::ErrorCode status;
    _load_installedLocales(status);
    यदि (status.isFailure()) {
        फर्काऊ nullptr;
    }
    यदि (offset > gAvailableLocaleCounts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ gAvailableLocaleNames[0][offset];
}

U_CAPI int32_t  U_EXPORT2
uloc_गन्ती_Available() {
    icu::ErrorCode status;
    _load_installedLocales(status);
    यदि (status.isFailure()) {
        फर्काऊ 0;
    }
    फर्काऊ gAvailableLocaleCounts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openAvailableByType(ULocAvailableType type, UErrorCode* status) {
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (type < 0 || type >= ULOC_AVAILABLE_गन्ती) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    _load_installedLocales(*status);
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    LocalPointer<AvailableLocalesStringEnumeration> result(
        नूतन AvailableLocalesStringEnumeration(type), *status);
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    फर्काऊ uenum_openFromStringEnumeration(result.orphan(), status);
}
