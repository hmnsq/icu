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

ساكن icu::Locale*  موجود_Locale_List = اشاره_تهی;
ساكن int32_t  موجود_Locale_List_شمار;
ساكن icu::UInitOnce gInitOnceLocale {};

نطاق {

UBool U_CALLCONV locale_موجود_cleanup()
{
    إذا (موجود_Locale_List) {
        حذف []موجود_Locale_List;
        موجود_Locale_List = اشاره_تهی;
    }
    موجود_Locale_List_شمار = 0;
    gInitOnceLocale.reset();

    أرجع درست;
}

}  // namespace

فراغ U_CALLCONV locale_موجود_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    موجود_Locale_List_شمار = uloc_شمار_موجود();
    إذا(موجود_Locale_List_شمار) {
       موجود_Locale_List = جديد Locale[موجود_Locale_List_شمار];
    }
    إذا (موجود_Locale_List == اشاره_تهی) {
        موجود_Locale_List_شمار= 0;
    }
    لكل (int32_t loc_شمار=موجود_Locale_List_شمار-1; loc_شمار>=0; --loc_شمار) {
        موجود_Locale_List[loc_شمار].مجموعه_From_POSIXID(uloc_get_موجود(loc_شمار));
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE_موجود, locale_موجود_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::get_موجود_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_موجود_init);
    count = موجود_Locale_List_شمار;
    أرجع موجود_Locale_List;
}


U_NAMESPACE_انتها

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نطاق {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** g_موجود_Locale_نامها[2] = {};
int32_t g_موجود_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

صنف موجود_Locales_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف *کلید, Resource_مقدار &value, UBool /*noFallback*/, UError_کد &وضعیت) override {
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        Resource_جدول res_شاخص_جدول = value.get_جدول(وضعیت);
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        لكل (int32_t i = 0; res_شاخص_جدول.get_کلید_And_مقدار(i, کلید, value); ++i) {
            ULoc_موجود_Type type;
            إذا (uprv_strcmp(کلید, "InstalledLocales") == 0) {
                type = ULOC_موجود_DEFAULT;
            } وإلا إذا (uprv_strcmp(کلید, "AliasLocales") == 0) {
                type = ULOC_موجود_ONLY_LEGACY_ALIASES;
            } وإلا {
                // CLDRVersion, etc.
                استمر;
            }
            Resource_جدول موجود_Locales_جدول = value.get_جدول(وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع;
            }
            g_موجود_Locale_Counts[type] = موجود_Locales_جدول.get_اندازه();
            g_موجود_Locale_نامها[type] = static_cast<ثابت حرف**>(
                uprv_malloc(g_موجود_Locale_Counts[type] * حجم(ثابت حرف*)));
            إذا (g_موجود_Locale_نامها[type] == اشاره_تهی) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            لكل (int32_t j = 0; موجود_Locales_جدول.get_کلید_And_مقدار(j, کلید, value); ++j) {
                g_موجود_Locale_نامها[type][j] = کلید;
            }
        }
    }
};

صنف موجود_Locales_String_Enumeration : عام StringEnumeration {
  عام:
    موجود_Locales_String_Enumeration(ULoc_موجود_Type type) : fType(type) {
    }

    ثابت حرف* next(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(وضعیت)) { أرجع اشاره_تهی; }
        ULoc_موجود_Type actualType = fType;
        int32_t actual_شاخص = f_شاخص++;

        // If the "combined" list was requested, resolve that now
        إذا (fType == ULOC_موجود_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_شمار = g_موجود_Locale_Counts[ULOC_موجود_DEFAULT];
            إذا (actual_شاخص < default_Locales_شمار) {
                actualType = ULOC_موجود_DEFAULT;
            } وإلا {
                actual_شاخص -= default_Locales_شمار;
                actualType = ULOC_موجود_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_موجود_Locale_Counts[actualType];
        ثابت حرف* نتیجه;
        إذا (actual_شاخص < count) {
            نتیجه = g_موجود_Locale_نامها[actualType][actual_شاخص];
            إذا (نتیجه_درازی != اشاره_تهی) {
                *نتیجه_درازی = static_cast<int32_t>(uprv_strlen(نتیجه));
            }
        } وإلا {
            نتیجه = اشاره_تهی;
            إذا (نتیجه_درازی != اشاره_تهی) {
                *نتیجه_درازی = 0;
            }
        }
        أرجع نتیجه;
    }

    فراغ reset(UError_کد &وضعیت) override {
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        f_شاخص = 0;
    }

    int32_t count(UError_کد &وضعیت) ثابت override {
        إذا (U_FAILURE(وضعیت)) { أرجع 0; }
        إذا (fType == ULOC_موجود_WITH_LEGACY_ALIASES) {
            أرجع g_موجود_Locale_Counts[ULOC_موجود_DEFAULT]
                + g_موجود_Locale_Counts[ULOC_موجود_ONLY_LEGACY_ALIASES];
        } وإلا {
            أرجع g_موجود_Locale_Counts[fType];
        }
    }

  خاص:
    ULoc_موجود_Type fType;
    int32_t f_شاخص = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(g_موجود_Locale_نامها); i++) {
        uprv_free(g_موجود_Locale_نامها[i]);
        g_موجود_Locale_نامها[i] = اشاره_تهی;
        g_موجود_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    أرجع درست;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

فراغ U_CALLCONV بارگیری_Installed_Locales(UError_کد& وضعیت) {
    ucln_common_ثبات_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_اشاره_گر rb(ures_open_مستقیم(اشاره_تهی, "res_index", &وضعیت));
    موجود_Locales_Sink sink;
    ures_get_All_اقلام_With_Fallback(rb.getAlias(), "", sink, وضعیت);
}

فراغ بارگیری_installed_Locales(UError_کد& وضعیت) {
    umtx_initOnce(ginstalledLocalesInitOnce, &بارگیری_Installed_Locales, وضعیت);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_get_موجود(int32_t جابه_جایی) {
    icu::Error_کد وضعیت;
    بارگیری_installed_Locales(وضعیت);
    إذا (وضعیت.isFailure()) {
        أرجع اشاره_تهی;
    }
    إذا (جابه_جایی > g_موجود_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع g_موجود_Locale_نامها[0][جابه_جایی];
}

U_CAPI int32_t  U_EXPORT2
uloc_شمار_موجود() {
    icu::Error_کد وضعیت;
    بارگیری_installed_Locales(وضعیت);
    إذا (وضعیت.isFailure()) {
        أرجع 0;
    }
    أرجع g_موجود_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_موجود_By_Type(ULoc_موجود_Type type, UError_کد* وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (type < 0 || type >= ULOC_موجود_شمار) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    بارگیری_installed_Locales(*وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    Local_اشاره_گر<موجود_Locales_String_Enumeration> نتیجه(
        جديد موجود_Locales_String_Enumeration(type), *وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    أرجع uenum_openFromStringEnumeration(نتیجه.orphan(), وضعیت);
}
