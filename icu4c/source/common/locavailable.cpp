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

ساکن icu::Locale*  دستیاب_Locale_List = خالی_اشاریہ;
ساکن int32_t  دستیاب_Locale_List_گنتی;
ساکن icu::UInitOnce gInitOnceLocale {};

نامخلا {

UBool U_CALLCONV locale_دستیاب_cleanup()
{
    اگر (دستیاب_Locale_List) {
        حذف []دستیاب_Locale_List;
        دستیاب_Locale_List = خالی_اشاریہ;
    }
    دستیاب_Locale_List_گنتی = 0;
    gInitOnceLocale.reset();

    واپس سچ;
}

}  // namespace

خالی U_CALLCONV locale_دستیاب_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    دستیاب_Locale_List_گنتی = uloc_گنتی_دستیاب();
    اگر(دستیاب_Locale_List_گنتی) {
       دستیاب_Locale_List = نیا Locale[دستیاب_Locale_List_گنتی];
    }
    اگر (دستیاب_Locale_List == خالی_اشاریہ) {
        دستیاب_Locale_List_گنتی= 0;
    }
    برائے (int32_t loc_گنتی=دستیاب_Locale_List_گنتی-1; loc_گنتی>=0; --loc_گنتی) {
        دستیاب_Locale_List[loc_گنتی].سیٹ_From_POSIXID(uloc_get_دستیاب(loc_گنتی));
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_دستیاب, locale_دستیاب_cleanup);
}

ثابت Locale* U_EXPORT2
Locale::get_دستیاب_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_دستیاب_init);
    count = دستیاب_Locale_List_گنتی;
    واپس دستیاب_Locale_List;
}


U_NAMESPACE_انتہا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

نامخلا {

// Enough capacity for the two lists in the res_index.res file
ثابت حرف** g_دستیاب_Locale_نام[2] = {};
int32_t g_دستیاب_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

جماعت دستیاب_Locales_Sink : عوامی ResourceSink {
  عوامی:
    خالی put(ثابت حرف *کلید, Resource_قدر &value, UBool /*noFallback*/, UError_رمز &حالت_2) override {
        اگر (U_FAILURE(حالت_2)) { واپس; }
        Resource_جدول res_اشاریہ_جدول = value.get_جدول(حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس; }
        برائے (int32_t i = 0; res_اشاریہ_جدول.get_کلید_And_قدر(i, کلید, value); ++i) {
            ULoc_دستیاب_Type type;
            اگر (uprv_strcmp(کلید, "InstalledLocales") == 0) {
                type = ULOC_دستیاب_DEFAULT;
            } ورنہ اگر (uprv_strcmp(کلید, "AliasLocales") == 0) {
                type = ULOC_دستیاب_ONLY_LEGACY_ALIASES;
            } ورنہ {
                // CLDRVersion, etc.
                جاری;
            }
            Resource_جدول دستیاب_Locales_جدول = value.get_جدول(حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس;
            }
            g_دستیاب_Locale_Counts[type] = دستیاب_Locales_جدول.get_سائز();
            g_دستیاب_Locale_نام[type] = static_cast<ثابت حرف**>(
                uprv_malloc(g_دستیاب_Locale_Counts[type] * حجم(ثابت حرف*)));
            اگر (g_دستیاب_Locale_نام[type] == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            برائے (int32_t j = 0; دستیاب_Locales_جدول.get_کلید_And_قدر(j, کلید, value); ++j) {
                g_دستیاب_Locale_نام[type][j] = کلید;
            }
        }
    }
};

جماعت دستیاب_Locales_String_Enumeration : عوامی StringEnumeration {
  عوامی:
    دستیاب_Locales_String_Enumeration(ULoc_دستیاب_Type type) : fType(type) {
    }

    ثابت حرف* next(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND override {
        اگر (U_FAILURE(حالت_2)) { واپس خالی_اشاریہ; }
        ULoc_دستیاب_Type actualType = fType;
        int32_t actual_اشاریہ = f_اشاریہ++;

        // If the "combined" list was requested, resolve that now
        اگر (fType == ULOC_دستیاب_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_گنتی = g_دستیاب_Locale_Counts[ULOC_دستیاب_DEFAULT];
            اگر (actual_اشاریہ < default_Locales_گنتی) {
                actualType = ULOC_دستیاب_DEFAULT;
            } ورنہ {
                actual_اشاریہ -= default_Locales_گنتی;
                actualType = ULOC_دستیاب_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_دستیاب_Locale_Counts[actualType];
        ثابت حرف* نتیجہ;
        اگر (actual_اشاریہ < count) {
            نتیجہ = g_دستیاب_Locale_نام[actualType][actual_اشاریہ];
            اگر (نتیجہ_لمبائی != خالی_اشاریہ) {
                *نتیجہ_لمبائی = static_cast<int32_t>(uprv_strlen(نتیجہ));
            }
        } ورنہ {
            نتیجہ = خالی_اشاریہ;
            اگر (نتیجہ_لمبائی != خالی_اشاریہ) {
                *نتیجہ_لمبائی = 0;
            }
        }
        واپس نتیجہ;
    }

    خالی reset(UError_رمز &حالت_2) override {
        اگر (U_FAILURE(حالت_2)) { واپس; }
        f_اشاریہ = 0;
    }

    int32_t count(UError_رمز &حالت_2) ثابت override {
        اگر (U_FAILURE(حالت_2)) { واپس 0; }
        اگر (fType == ULOC_دستیاب_WITH_LEGACY_ALIASES) {
            واپس g_دستیاب_Locale_Counts[ULOC_دستیاب_DEFAULT]
                + g_دستیاب_Locale_Counts[ULOC_دستیاب_ONLY_LEGACY_ALIASES];
        } ورنہ {
            واپس g_دستیاب_Locale_Counts[fType];
        }
    }

  نجی:
    ULoc_دستیاب_Type fType;
    int32_t f_اشاریہ = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(g_دستیاب_Locale_نام); i++) {
        uprv_free(g_دستیاب_Locale_نام[i]);
        g_دستیاب_Locale_نام[i] = خالی_اشاریہ;
        g_دستیاب_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    واپس سچ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

خالی U_CALLCONV بھرو_Installed_Locales(UError_رمز& حالت_2) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_اشارہ_گر rb(ures_open_براہ_راست(خالی_اشاریہ, "res_index", &حالت_2));
    دستیاب_Locales_Sink sink;
    ures_get_All_اشیاء_With_Fallback(rb.getAlias(), "", sink, حالت_2);
}

خالی بھرو_installed_Locales(UError_رمز& حالت_2) {
    umtx_initOnce(ginstalledLocalesInitOnce, &بھرو_Installed_Locales, حالت_2);
}

} // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_get_دستیاب(int32_t انحراف) {
    icu::Error_رمز حالت_2;
    بھرو_installed_Locales(حالت_2);
    اگر (حالت_2.isFailure()) {
        واپس خالی_اشاریہ;
    }
    اگر (انحراف > g_دستیاب_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس g_دستیاب_Locale_نام[0][انحراف];
}

U_CAPI int32_t  U_EXPORT2
uloc_گنتی_دستیاب() {
    icu::Error_رمز حالت_2;
    بھرو_installed_Locales(حالت_2);
    اگر (حالت_2.isFailure()) {
        واپس 0;
    }
    واپس g_دستیاب_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_دستیاب_By_Type(ULoc_دستیاب_Type type, UError_رمز* حالت_2) {
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (type < 0 || type >= ULOC_دستیاب_گنتی) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    بھرو_installed_Locales(*حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    Local_اشارہ_گر<دستیاب_Locales_String_Enumeration> نتیجہ(
        نیا دستیاب_Locales_String_Enumeration(type), *حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    واپس uenum_openFromStringEnumeration(نتیجہ.orphan(), حالت_2);
}
