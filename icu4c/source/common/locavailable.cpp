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

ساكن icu::Locale*  موجود_Locale_List = خالي_اشارو;
ساكن int32_t  موجود_Locale_List_ڳڻپ;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_موجود_cleanup()
{
    إذا (موجود_Locale_List) {
        ميٽيو []موجود_Locale_List;
        موجود_Locale_List = خالي_اشارو;
    }
    موجود_Locale_List_ڳڻپ = 0;
    gInitOnceLocale.reset();

    أرجع سچ;
}

}  // namespace

فراغ U_CALLCONV locale_موجود_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    موجود_Locale_List_ڳڻپ = uloc_ڳڻپ_موجود();
    إذا(موجود_Locale_List_ڳڻپ) {
       موجود_Locale_List = جديد Locale[موجود_Locale_List_ڳڻپ];
    }
    إذا (موجود_Locale_List == خالي_اشارو) {
        موجود_Locale_List_ڳڻپ= 0;
    }
    لكل (int32_t loc_ڳڻپ=موجود_Locale_List_ڳڻپ-1; loc_ڳڻپ>=0; --loc_ڳڻپ) {
        موجود_Locale_List[loc_ڳڻپ].سيٽ_From_POSIXID(uloc_get_موجود(loc_ڳڻپ));
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE_موجود, locale_موجود_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::get_موجود_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_موجود_init);
    count = موجود_Locale_List_ڳڻپ;
    أرجع موجود_Locale_List;
}


U_NAMESPACE_پڇاڙي

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نطاق {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** g_موجود_Locale_نالا[2] = {};
int32_t g_موجود_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

صنف موجود_Locales_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف *ڪنجي, Resource_قدر &value, UBool /*noFallback*/, UError_ڪوڊ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        Resource_جدول res_اشاريو_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (int32_t i = 0; res_اشاريو_جدول.get_ڪنجي_And_قدر(i, ڪنجي, value); ++i) {
            ULoc_موجود_Type type;
            إذا (uprv_strcmp(ڪنجي, "InstalledLocales") == 0) {
                type = ULOC_موجود_DEFAULT;
            } وإلا إذا (uprv_strcmp(ڪنجي, "AliasLocales") == 0) {
                type = ULOC_موجود_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            Resource_جدول موجود_Locales_جدول = value.get_جدول(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع;
            }
            g_موجود_Locale_Counts[type] = موجود_Locales_جدول.get_سائيز();
            g_موجود_Locale_نالا[type] = static_cast<ثابت حرف**>(
                uprv_malloc(g_موجود_Locale_Counts[type] * حجم(ثابت حرف*)));
            إذا (g_موجود_Locale_نالا[type] == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; موجود_Locales_جدول.get_ڪنجي_And_قدر(j, ڪنجي, value); ++j) {
                g_موجود_Locale_نالا[type][j] = ڪنجي;
            }
        }
    }
};

صنف موجود_Locales_String_Enumeration : عام StringEnumeration {
  عام:
    موجود_Locales_String_Enumeration(ULoc_موجود_Type type) : fType(type) {
    }

    ثابت حرف* next(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع خالي_اشارو; }
        ULoc_موجود_Type actualType = fType;
        int32_t actual_اشاريو = f_اشاريو++;

        // If the "combined" list was requested, resolve that now
        إذا (fType == ULOC_موجود_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ڳڻپ = g_موجود_Locale_Counts[ULOC_موجود_DEFAULT];
            إذا (actual_اشاريو < default_Locales_ڳڻپ) {
                actualType = ULOC_موجود_DEFAULT;
            } وإلا {
                actual_اشاريو -= default_Locales_ڳڻپ;
                actualType = ULOC_موجود_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_موجود_Locale_Counts[actualType];
        ثابت حرف* نتيجو;
        إذا (actual_اشاريو < count) {
            نتيجو = g_موجود_Locale_نالا[actualType][actual_اشاريو];
            إذا (نتيجو_ڊيگهائي != خالي_اشارو) {
                *نتيجو_ڊيگهائي = static_cast<int32_t>(uprv_strlen(نتيجو));
            }
        } وإلا {
            نتيجو = خالي_اشارو;
            إذا (نتيجو_ڊيگهائي != خالي_اشارو) {
                *نتيجو_ڊيگهائي = 0;
            }
        }
        أرجع نتيجو;
    }

    فراغ reset(UError_ڪوڊ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        f_اشاريو = 0;
    }

    int32_t count(UError_ڪوڊ &حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        إذا (fType == ULOC_موجود_WITH_LEGACY_ALIASES) {
            أرجع g_موجود_Locale_Counts[ULOC_موجود_DEFAULT]
                + g_موجود_Locale_Counts[ULOC_موجود_ONLY_LEGACY_ALIASES];
        } وإلا {
            أرجع g_موجود_Locale_Counts[fType];
        }
    }

  خاص:
    ULoc_موجود_Type fType;
    int32_t f_اشاريو = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(g_موجود_Locale_نالا); i++) {
        uprv_free(g_موجود_Locale_نالا[i]);
        g_موجود_Locale_نالا[i] = خالي_اشارو;
        g_موجود_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    أرجع سچ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

فراغ U_CALLCONV ڀريو_Installed_Locales(UError_ڪوڊ& حالت) {
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_اشارو rb(ures_open_سڌو(خالي_اشارو, "res_index", &حالت));
    موجود_Locales_Sink sink;
    ures_get_All_شيون_With_Fallback(rb.getAlias(), "", sink, حالت);
}

فراغ ڀريو_installed_Locales(UError_ڪوڊ& حالت) {
    umtx_initOnce(ginstalledLocalesInitOnce, &ڀريو_Installed_Locales, حالت);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_get_موجود(int32_t وٿي) {
    icu::Error_ڪوڊ حالت;
    ڀريو_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع خالي_اشارو;
    }
    إذا (وٿي > g_موجود_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع g_موجود_Locale_نالا[0][وٿي];
}

U_CAPI int32_t  U_EXPORT2
uloc_ڳڻپ_موجود() {
    icu::Error_ڪوڊ حالت;
    ڀريو_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع 0;
    }
    أرجع g_موجود_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_موجود_By_Type(ULoc_موجود_Type type, UError_ڪوڊ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (type < 0 || type >= ULOC_موجود_ڳڻپ) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    ڀريو_installed_Locales(*حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    Local_اشارو<موجود_Locales_String_Enumeration> نتيجو(
        جديد موجود_Locales_String_Enumeration(type), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    أرجع uenum_openFromStringEnumeration(نتيجو.orphan(), حالت);
}
