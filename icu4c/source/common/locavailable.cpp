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

ساكن icu::Locale*  شته_Locale_List = تش_اشاره;
ساكن int32_t  شته_Locale_List_شمېر;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_شته_cleanup()
{
    إذا (شته_Locale_List) {
        ړنګول []شته_Locale_List;
        شته_Locale_List = تش_اشاره;
    }
    شته_Locale_List_شمېر = 0;
    gInitOnceLocale.reset();

    أرجع ریښتیا;
}

}  // namespace

فراغ U_CALLCONV locale_شته_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    شته_Locale_List_شمېر = uloc_شمېر_شته();
    إذا(شته_Locale_List_شمېر) {
       شته_Locale_List = جديد Locale[شته_Locale_List_شمېر];
    }
    إذا (شته_Locale_List == تش_اشاره) {
        شته_Locale_List_شمېر= 0;
    }
    لكل (int32_t loc_شمېر=شته_Locale_List_شمېر-1; loc_شمېر>=0; --loc_شمېر) {
        شته_Locale_List[loc_شمېر].سېټ_From_POSIXID(uloc_get_شته(loc_شمېر));
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE_شته, locale_شته_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::get_شته_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_شته_init);
    count = شته_Locale_List_شمېر;
    أرجع شته_Locale_List;
}


U_NAMESPACE_پای_ټکی

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نطاق {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** g_شته_Locale_نومونه[2] = {};
int32_t g_شته_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

صنف شته_Locales_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف *کلی, Resource_ارزښت &value, UBool /*noFallback*/, UError_کوډ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        Resource_جدول res_شاخص_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (int32_t i = 0; res_شاخص_جدول.get_کلی_And_ارزښت(i, کلی, value); ++i) {
            ULoc_شته_Type type;
            إذا (uprv_strcmp(کلی, "InstalledLocales") == 0) {
                type = ULOC_شته_DEFAULT;
            } وإلا إذا (uprv_strcmp(کلی, "AliasLocales") == 0) {
                type = ULOC_شته_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            Resource_جدول شته_Locales_جدول = value.get_جدول(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع;
            }
            g_شته_Locale_Counts[type] = شته_Locales_جدول.get_اندازه();
            g_شته_Locale_نومونه[type] = static_cast<ثابت حرف**>(
                uprv_malloc(g_شته_Locale_Counts[type] * حجم(ثابت حرف*)));
            إذا (g_شته_Locale_نومونه[type] == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; شته_Locales_جدول.get_کلی_And_ارزښت(j, کلی, value); ++j) {
                g_شته_Locale_نومونه[type][j] = کلی;
            }
        }
    }
};

صنف شته_Locales_String_Enumeration : عام StringEnumeration {
  عام:
    شته_Locales_String_Enumeration(ULoc_شته_Type type) : fType(type) {
    }

    ثابت حرف* next(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع تش_اشاره; }
        ULoc_شته_Type actualType = fType;
        int32_t actual_شاخص = f_شاخص++;

        // If the "combined" list was requested, resolve that now
        إذا (fType == ULOC_شته_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_شمېر = g_شته_Locale_Counts[ULOC_شته_DEFAULT];
            إذا (actual_شاخص < default_Locales_شمېر) {
                actualType = ULOC_شته_DEFAULT;
            } وإلا {
                actual_شاخص -= default_Locales_شمېر;
                actualType = ULOC_شته_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_شته_Locale_Counts[actualType];
        ثابت حرف* پایله;
        إذا (actual_شاخص < count) {
            پایله = g_شته_Locale_نومونه[actualType][actual_شاخص];
            إذا (پایله_اوږدتیا != تش_اشاره) {
                *پایله_اوږدتیا = static_cast<int32_t>(uprv_strlen(پایله));
            }
        } وإلا {
            پایله = تش_اشاره;
            إذا (پایله_اوږدتیا != تش_اشاره) {
                *پایله_اوږدتیا = 0;
            }
        }
        أرجع پایله;
    }

    فراغ reset(UError_کوډ &حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        f_شاخص = 0;
    }

    int32_t count(UError_کوډ &حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        إذا (fType == ULOC_شته_WITH_LEGACY_ALIASES) {
            أرجع g_شته_Locale_Counts[ULOC_شته_DEFAULT]
                + g_شته_Locale_Counts[ULOC_شته_ONLY_LEGACY_ALIASES];
        } وإلا {
            أرجع g_شته_Locale_Counts[fType];
        }
    }

  خاص:
    ULoc_شته_Type fType;
    int32_t f_شاخص = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(g_شته_Locale_نومونه); i++) {
        uprv_free(g_شته_Locale_نومونه[i]);
        g_شته_Locale_نومونه[i] = تش_اشاره;
        g_شته_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    أرجع ریښتیا;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

فراغ U_CALLCONV بارول_Installed_Locales(UError_کوډ& حالت) {
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_اشاره_ګر rb(ures_open_مستقیم(تش_اشاره, "res_index", &حالت));
    شته_Locales_Sink sink;
    ures_get_All_توکي_With_Fallback(rb.getAlias(), "", sink, حالت);
}

فراغ بارول_installed_Locales(UError_کوډ& حالت) {
    umtx_initOnce(ginstalledLocalesInitOnce, &بارول_Installed_Locales, حالت);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_get_شته(int32_t دننه_واټن) {
    icu::Error_کوډ حالت;
    بارول_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع تش_اشاره;
    }
    إذا (دننه_واټن > g_شته_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع تش_اشاره;
    }
    أرجع g_شته_Locale_نومونه[0][دننه_واټن];
}

U_CAPI int32_t  U_EXPORT2
uloc_شمېر_شته() {
    icu::Error_کوډ حالت;
    بارول_installed_Locales(حالت);
    إذا (حالت.isFailure()) {
        أرجع 0;
    }
    أرجع g_شته_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_شته_By_Type(ULoc_شته_Type type, UError_کوډ* حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (type < 0 || type >= ULOC_شته_شمېر) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    بارول_installed_Locales(*حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    Local_اشاره_ګر<شته_Locales_String_Enumeration> پایله(
        جديد شته_Locales_String_Enumeration(type), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    أرجع uenum_openFromStringEnumeration(پایله.orphan(), حالت);
}
