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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/errorcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

ਸਥਿਰ icu::Locale*  ਉਪਲਬਧ_Locale_List = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ int32_t  ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ;
ਸਥਿਰ icu::UInitOnce gInitOnceLocale {};

ਨਾਮਾਕਾਸ਼ {

UBool U_CALLCONV locale_ਉਪਲਬਧ_cleanup()
{
    ਜੇ (ਉਪਲਬਧ_Locale_List) {
        ਮਿਟਾਓ []ਉਪਲਬਧ_Locale_List;
        ਉਪਲਬਧ_Locale_List = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ = 0;
    gInitOnceLocale.reset();

    ਵਾਪਸ ਸੱਚ;
}

}  // namespace

ਖਾਲੀ U_CALLCONV locale_ਉਪਲਬਧ_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ = uloc_ਗਿਣਤੀ_ਉਪਲਬਧ();
    ਜੇ(ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ) {
       ਉਪਲਬਧ_Locale_List = ਨਵਾਂ Locale[ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ];
    }
    ਜੇ (ਉਪਲਬਧ_Locale_List == ਖਾਲੀ_ਸੂਚਕ) {
        ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ= 0;
    }
    ਲਈ (int32_t loc_ਗਿਣਤੀ=ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ-1; loc_ਗਿਣਤੀ>=0; --loc_ਗਿਣਤੀ) {
        ਉਪਲਬਧ_Locale_List[loc_ਗਿਣਤੀ].ਸੈੱਟ_From_POSIXID(uloc_get_ਉਪਲਬਧ(loc_ਗਿਣਤੀ));
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE_ਉਪਲਬਧ, locale_ਉਪਲਬਧ_cleanup);
}

ਅਚਲ Locale* U_EXPORT2
Locale::get_ਉਪਲਬਧ_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_ਉਪਲਬਧ_init);
    count = ਉਪਲਬਧ_Locale_List_ਗਿਣਤੀ;
    ਵਾਪਸ ਉਪਲਬਧ_Locale_List;
}


U_NAMESPACE_ਸਿਰਾ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

ਨਾਮਾਕਾਸ਼ {

// Enough capacity for the two lists in the res_index.res file
ਅਚਲ ਅੱਖਰ** g_ਉਪਲਬਧ_Locale_ਨਾਂ[2] = {};
int32_t g_ਉਪਲਬਧ_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

ਵਰਗ ਉਪਲਬਧ_Locales_Sink : ਜਨਤਕ ResourceSink {
  ਜਨਤਕ:
    ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &value, UBool /*noFallback*/, UError_ਕੋਡ &ਸਥਿਤੀ) override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        Resource_ਸਾਰਣੀ res_ਸੂਚਕਾਂਕ_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        ਲਈ (int32_t i = 0; res_ਸੂਚਕਾਂਕ_ਸਾਰਣੀ.get_ਕੁੰਜੀ_And_ਮੁੱਲ(i, ਕੁੰਜੀ, value); ++i) {
            ULoc_ਉਪਲਬਧ_Type type;
            ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "InstalledLocales") == 0) {
                type = ULOC_ਉਪਲਬਧ_DEFAULT;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "AliasLocales") == 0) {
                type = ULOC_ਉਪਲਬਧ_ONLY_LEGACY_ALIASES;
            } ਨਹੀਂਤਾਂ {
                // CLDRVersion, etc.
                ਜਾਰੀ;
            }
            Resource_ਸਾਰਣੀ ਉਪਲਬਧ_Locales_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ;
            }
            g_ਉਪਲਬਧ_Locale_Counts[type] = ਉਪਲਬਧ_Locales_ਸਾਰਣੀ.get_ਮਾਪ();
            g_ਉਪਲਬਧ_Locale_ਨਾਂ[type] = static_cast<ਅਚਲ ਅੱਖਰ**>(
                uprv_malloc(g_ਉਪਲਬਧ_Locale_Counts[type] * ਆਕਾਰ(ਅਚਲ ਅੱਖਰ*)));
            ਜੇ (g_ਉਪਲਬਧ_Locale_ਨਾਂ[type] == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            ਲਈ (int32_t j = 0; ਉਪਲਬਧ_Locales_ਸਾਰਣੀ.get_ਕੁੰਜੀ_And_ਮੁੱਲ(j, ਕੁੰਜੀ, value); ++j) {
                g_ਉਪਲਬਧ_Locale_ਨਾਂ[type][j] = ਕੁੰਜੀ;
            }
        }
    }
};

ਵਰਗ ਉਪਲਬਧ_Locales_String_Enumeration : ਜਨਤਕ StringEnumeration {
  ਜਨਤਕ:
    ਉਪਲਬਧ_Locales_String_Enumeration(ULoc_ਉਪਲਬਧ_Type type) : fType(type) {
    }

    ਅਚਲ ਅੱਖਰ* next(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
        ULoc_ਉਪਲਬਧ_Type actualType = fType;
        int32_t actual_ਸੂਚਕਾਂਕ = f_ਸੂਚਕਾਂਕ++;

        // If the "combined" list was requested, resolve that now
        ਜੇ (fType == ULOC_ਉਪਲਬਧ_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_ਗਿਣਤੀ = g_ਉਪਲਬਧ_Locale_Counts[ULOC_ਉਪਲਬਧ_DEFAULT];
            ਜੇ (actual_ਸੂਚਕਾਂਕ < default_Locales_ਗਿਣਤੀ) {
                actualType = ULOC_ਉਪਲਬਧ_DEFAULT;
            } ਨਹੀਂਤਾਂ {
                actual_ਸੂਚਕਾਂਕ -= default_Locales_ਗਿਣਤੀ;
                actualType = ULOC_ਉਪਲਬਧ_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_ਉਪਲਬਧ_Locale_Counts[actualType];
        ਅਚਲ ਅੱਖਰ* ਨਤੀਜਾ;
        ਜੇ (actual_ਸੂਚਕਾਂਕ < count) {
            ਨਤੀਜਾ = g_ਉਪਲਬਧ_Locale_ਨਾਂ[actualType][actual_ਸੂਚਕਾਂਕ];
            ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(ਨਤੀਜਾ));
            }
        } ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
            ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
            }
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਖਾਲੀ reset(UError_ਕੋਡ &ਸਥਿਤੀ) override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        f_ਸੂਚਕਾਂਕ = 0;
    }

    int32_t count(UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
        ਜੇ (fType == ULOC_ਉਪਲਬਧ_WITH_LEGACY_ALIASES) {
            ਵਾਪਸ g_ਉਪਲਬਧ_Locale_Counts[ULOC_ਉਪਲਬਧ_DEFAULT]
                + g_ਉਪਲਬਧ_Locale_Counts[ULOC_ਉਪਲਬਧ_ONLY_LEGACY_ALIASES];
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ g_ਉਪਲਬਧ_Locale_Counts[fType];
        }
    }

  ਨਿਜੀ:
    ULoc_ਉਪਲਬਧ_Type fType;
    int32_t f_ਸੂਚਕਾਂਕ = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(g_ਉਪਲਬਧ_Locale_ਨਾਂ); i++) {
        uprv_free(g_ਉਪਲਬਧ_Locale_ਨਾਂ[i]);
        g_ਉਪਲਬਧ_Locale_ਨਾਂ[i] = ਖਾਲੀ_ਸੂਚਕ;
        g_ਉਪਲਬਧ_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

ਖਾਲੀ U_CALLCONV ਲੱਦੋ_Installed_Locales(UError_ਕੋਡ& ਸਥਿਤੀ) {
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_ਸੂਚਕ rb(ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, "res_index", &ਸਥਿਤੀ));
    ਉਪਲਬਧ_Locales_Sink sink;
    ures_get_All_ਚੀਜ਼ਾਂ_With_Fallback(rb.getAlias(), "", sink, ਸਥਿਤੀ);
}

ਖਾਲੀ ਲੱਦੋ_installed_Locales(UError_ਕੋਡ& ਸਥਿਤੀ) {
    umtx_initOnce(ginstalledLocalesInitOnce, &ਲੱਦੋ_Installed_Locales, ਸਥਿਤੀ);
}

} // namespace

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uloc_get_ਉਪਲਬਧ(int32_t ਵਿੱਥ) {
    icu::Error_ਕੋਡ ਸਥਿਤੀ;
    ਲੱਦੋ_installed_Locales(ਸਥਿਤੀ);
    ਜੇ (ਸਥਿਤੀ.isFailure()) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (ਵਿੱਥ > g_ਉਪਲਬਧ_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ g_ਉਪਲਬਧ_Locale_ਨਾਂ[0][ਵਿੱਥ];
}

U_CAPI int32_t  U_EXPORT2
uloc_ਗਿਣਤੀ_ਉਪਲਬਧ() {
    icu::Error_ਕੋਡ ਸਥਿਤੀ;
    ਲੱਦੋ_installed_Locales(ਸਥਿਤੀ);
    ਜੇ (ਸਥਿਤੀ.isFailure()) {
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ g_ਉਪਲਬਧ_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_ਉਪਲਬਧ_By_Type(ULoc_ਉਪਲਬਧ_Type type, UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (type < 0 || type >= ULOC_ਉਪਲਬਧ_ਗਿਣਤੀ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਲੱਦੋ_installed_Locales(*ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Local_ਸੂਚਕ<ਉਪਲਬਧ_Locales_String_Enumeration> ਨਤੀਜਾ(
        ਨਵਾਂ ਉਪਲਬਧ_Locales_String_Enumeration(type), *ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ uenum_openFromStringEnumeration(ਨਤੀਜਾ.orphan(), ਸਥਿਤੀ);
}
