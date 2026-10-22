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

ساكن icu::Locale*  دستیاب_Locale_List = خالی_سوچک;
ساكن int32_t  دستیاب_Locale_List_گنتی;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_دستیاب_cleanup()
{
    إذا (دستیاب_Locale_List) {
        مٹاؤ []دستیاب_Locale_List;
        دستیاب_Locale_List = خالی_سوچک;
    }
    دستیاب_Locale_List_گنتی = 0;
    gInitOnceLocale.reset();

    أرجع سچ;
}

}  // namespace

فراغ U_CALLCONV locale_دستیاب_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    دستیاب_Locale_List_گنتی = uloc_گنتی_دستیاب();
    إذا(دستیاب_Locale_List_گنتی) {
       دستیاب_Locale_List = جديد Locale[دستیاب_Locale_List_گنتی];
    }
    إذا (دستیاب_Locale_List == خالی_سوچک) {
        دستیاب_Locale_List_گنتی= 0;
    }
    لكل (int32_t loc_گنتی=دستیاب_Locale_List_گنتی-1; loc_گنتی>=0; --loc_گنتی) {
        دستیاب_Locale_List[loc_گنتی].سیٹ_From_POSIXID(uloc_get_دستیاب(loc_گنتی));
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_دستیاب, locale_دستیاب_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::get_دستیاب_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_دستیاب_init);
    count = دستیاب_Locale_List_گنتی;
    أرجع دستیاب_Locale_List;
}


U_NAMESPACE_سرا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نطاق {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** g_دستیاب_Locale_ناں[2] = {};
int32_t g_دستیاب_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

صنف دستیاب_Locales_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف *کلید, Resource_قدر &value, UBool /*noFallback*/, UError_کوڈ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        Resource_جدول res_اشاریہ_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (int32_t i = 0; res_اشاریہ_جدول.get_کلید_And_قدر(i, کلید, value); ++i) {
            ULoc_دستیاب_Type type;
            إذا (uprv_strcmp(کلید, "InstalledLocales") == 0) {
                type = ULOC_دستیاب_DEFAULT;
            } وإلا إذا (uprv_strcmp(کلید, "AliasLocales") == 0) {
                type = ULOC_دستیاب_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            Resource_جدول دستیاب_Locales_جدول = value.get_جدول(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع;
            }
            g_دستیاب_Locale_Counts[type] = دستیاب_Locales_جدول.get_ناپ();
            g_دستیاب_Locale_ناں[type] = static_cast<ثابت حرف**>(
                uprv_malloc(g_دستیاب_Locale_Counts[type] * حجم(ثابت حرف*)));
            إذا (g_دستیاب_Locale_ناں[type] == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; دستیاب_Locales_جدول.get_کلید_And_قدر(j, کلید, value); ++j) {
                g_دستیاب_Locale_ناں[type][j] = کلید;
            }
        }
    }
};

صنف دستیاب_Locales_String_Enumeration : عام StringEnumeration {
  عام:
    دستیاب_Locales_String_Enumeration(ULoc_دستیاب_Type type) : fType(type) {
    }

    ثابت حرف* next(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع خالی_سوچک; }
        ULoc_دستیاب_Type actualType = fType;
        int32_t actual_اشاریہ = f_اشاریہ++;

        // If the "combined" list was requested, resolve that now
        إذا (fType == ULOC_دستیاب_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_گنتی = g_دستیاب_Locale_Counts[ULOC_دستیاب_DEFAULT];
            إذا (actual_اشاریہ < default_Locales_گنتی) {
                actualType = ULOC_دستیاب_DEFAULT;
            } وإلا {
                actual_اشاریہ -= default_Locales_گنتی;
                actualType = ULOC_دستیاب_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_دستیاب_Locale_Counts[actualType];
        ثابت حرف* نتیجہ;
        إذا (actual_اشاریہ < count) {
            نتیجہ = g_دستیاب_Locale_ناں[actualType][actual_اشاریہ];
            إذا (نتیجہ_لمائی != خالی_سوچک) {
                *نتیجہ_لمائی = static_cast<int32_t>(uprv_strlen(نتیجہ));
            }
        } وإلا {
            نتیجہ = خالی_سوچک;
            إذا (نتیجہ_لمائی != خالی_سوچک) {
                *نتیجہ_لمائی = 0;
            }
        }
        أرجع نتیجہ;
    }

    فراغ reset(UError_کوڈ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        f_اشاریہ = 0;
    }

    int32_t count(UError_کوڈ &حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        إذا (fType == ULOC_دستیاب_WITH_LEGACY_ALIASES) {
            أرجع g_دستیاب_Locale_Counts[ULOC_دستیاب_DEFAULT]
                + g_دستیاب_Locale_Counts[ULOC_دستیاب_ONLY_LEGACY_ALIASES];
        } وإلا {
            أرجع g_دستیاب_Locale_Counts[fType];
        }
    }

  خاص:
    ULoc_دستیاب_Type fType;
    int32_t f_اشاریہ = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(g_دستیاب_Locale_ناں); i++) {
        uprv_free(g_دستیاب_Locale_ناں[i]);
        g_دستیاب_Locale_ناں[i] = خالی_سوچک;
        g_دستیاب_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    أرجع سچ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

فراغ U_CALLCONV لدو_Installed_Locales(UError_کوڈ& حالت) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_اشارہ_گر rb(ures_open_سدھا(خالی_سوچک, "res_index", &حالت));
    دستیاب_Locales_Sink sink;
    ures_get_All_اشیاء_With_Fallback(rb.getAlias(), "", sink, حالت);
}

فراغ لدو_installed_Locales(UError_کوڈ& حالت) {
    umtx_initOnce(ginstalledLocalesInitOnce, &لدو_Installed_Locales, حالت);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_get_دستیاب(int32_t وتھ) {
    icu::Error_کوڈ حالت;
    لدو_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع خالی_سوچک;
    }
    إذا (وتھ > g_دستیاب_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع g_دستیاب_Locale_ناں[0][وتھ];
}

U_CAPI int32_t  U_EXPORT2
uloc_گنتی_دستیاب() {
    icu::Error_کوڈ حالت;
    لدو_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع 0;
    }
    أرجع g_دستیاب_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_دستیاب_By_Type(ULoc_دستیاب_Type type, UError_کوڈ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (type < 0 || type >= ULOC_دستیاب_گنتی) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    لدو_installed_Locales(*حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    Local_اشارہ_گر<دستیاب_Locales_String_Enumeration> نتیجہ(
        جديد دستیاب_Locales_String_Enumeration(type), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    أرجع uenum_openFromStringEnumeration(نتیجہ.orphan(), حالت);
}
