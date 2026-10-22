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

#అన్తర్భావ "unicode/errorcode.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uresimp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

స్థిర icu::Locale*  అందుబాటు_Locale_List = శూన్యసూచిక;
స్థిర int32_t  అందుబాటు_Locale_List_లెక్క;
స్థిర icu::UInitOnce gInitOnceLocale {};

నామాకాశ {

UBool U_CALLCONV locale_అందుబాటు_cleanup()
{
    ఒకవేళ (అందుబాటు_Locale_List) {
        తొలగించు []అందుబాటు_Locale_List;
        అందుబాటు_Locale_List = శూన్యసూచిక;
    }
    అందుబాటు_Locale_List_లెక్క = 0;
    gInitOnceLocale.reset();

    తిరిగి నిజం;
}

}  // namespace

శూన్య U_CALLCONV locale_అందుబాటు_init() {
    // This function is a friend of class Locale.
    // This function is only invoked via umtx_initOnce().
    
    // for now, there is a hardcoded list, so just walk through that list and set it up.
    //  Note: this function is a friend of class Locale.
    అందుబాటు_Locale_List_లెక్క = uloc_లెక్క_అందుబాటు();
    ఒకవేళ(అందుబాటు_Locale_List_లెక్క) {
       అందుబాటు_Locale_List = కొత్త Locale[అందుబాటు_Locale_List_లెక్క];
    }
    ఒకవేళ (అందుబాటు_Locale_List == శూన్యసూచిక) {
        అందుబాటు_Locale_List_లెక్క= 0;
    }
    కొరకు (int32_t loc_లెక్క=అందుబాటు_Locale_List_లెక్క-1; loc_లెక్క>=0; --loc_లెక్క) {
        అందుబాటు_Locale_List[loc_లెక్క].సమితి_From_POSIXID(uloc_get_అందుబాటు(loc_లెక్క));
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE_అందుబాటు, locale_అందుబాటు_cleanup);
}

స్థిరరాశి Locale* U_EXPORT2
Locale::get_అందుబాటు_Locales(int32_t& count)
{
    umtx_initOnce(gInitOnceLocale, &locale_అందుబాటు_init);
    count = అందుబాటు_Locale_List_లెక్క;
    తిరిగి అందుబాటు_Locale_List;
}


U_NAMESPACE_చివర

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

/* ### Constants **************************************************/

నామాకాశ {

// Enough capacity for the two lists in the res_index.res file
స్థిరరాశి అక్షరం** g_అందుబాటు_Locale_పేర్లు[2] = {};
int32_t g_అందుబాటు_Locale_Counts[2] = {};
icu::UInitOnce ginstalledLocalesInitOnce {};

తరగతి అందుబాటు_Locales_Sink : బహిరంగ ResourceSink {
  బహిరంగ:
    శూన్య put(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &value, UBool /*noFallback*/, UError_సంకేతం &స్థితి_2) override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        Resource_పట్టిక res_సూచీ_పట్టిక = value.get_పట్టిక(స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        కొరకు (int32_t i = 0; res_సూచీ_పట్టిక.get_కీలకం_And_విలువ(i, కీలకం, value); ++i) {
            ULoc_అందుబాటు_Type type;
            ఒకవేళ (uprv_strcmp(కీలకం, "InstalledLocales") == 0) {
                type = ULOC_అందుబాటు_DEFAULT;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "AliasLocales") == 0) {
                type = ULOC_అందుబాటు_ONLY_LEGACY_ALIASES;
            } లేకపోతే {
                // CLDRVersion, etc.
                కొనసాగు;
            }
            Resource_పట్టిక అందుబాటు_Locales_పట్టిక = value.get_పట్టిక(స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి;
            }
            g_అందుబాటు_Locale_Counts[type] = అందుబాటు_Locales_పట్టిక.get_కొలత();
            g_అందుబాటు_Locale_పేర్లు[type] = static_cast<స్థిరరాశి అక్షరం**>(
                uprv_malloc(g_అందుబాటు_Locale_Counts[type] * పరిమాణం(స్థిరరాశి అక్షరం*)));
            ఒకవేళ (g_అందుబాటు_Locale_పేర్లు[type] == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            కొరకు (int32_t j = 0; అందుబాటు_Locales_పట్టిక.get_కీలకం_And_విలువ(j, కీలకం, value); ++j) {
                g_అందుబాటు_Locale_పేర్లు[type][j] = కీలకం;
            }
        }
    }
};

తరగతి అందుబాటు_Locales_String_Enumeration : బహిరంగ StringEnumeration {
  బహిరంగ:
    అందుబాటు_Locales_String_Enumeration(ULoc_అందుబాటు_Type type) : fType(type) {
    }

    స్థిరరాశి అక్షరం* next(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి శూన్యసూచిక; }
        ULoc_అందుబాటు_Type actualType = fType;
        int32_t actual_సూచీ = f_సూచీ++;

        // If the "combined" list was requested, resolve that now
        ఒకవేళ (fType == ULOC_అందుబాటు_WITH_LEGACY_ALIASES) {
            int32_t default_Locales_లెక్క = g_అందుబాటు_Locale_Counts[ULOC_అందుబాటు_DEFAULT];
            ఒకవేళ (actual_సూచీ < default_Locales_లెక్క) {
                actualType = ULOC_అందుబాటు_DEFAULT;
            } లేకపోతే {
                actual_సూచీ -= default_Locales_లెక్క;
                actualType = ULOC_అందుబాటు_ONLY_LEGACY_ALIASES;
            }
        }

        // Return the requested string
        int32_t count = g_అందుబాటు_Locale_Counts[actualType];
        స్థిరరాశి అక్షరం* ఫలితం;
        ఒకవేళ (actual_సూచీ < count) {
            ఫలితం = g_అందుబాటు_Locale_పేర్లు[actualType][actual_సూచీ];
            ఒకవేళ (ఫలితం_నిడివి != శూన్యసూచిక) {
                *ఫలితం_నిడివి = static_cast<int32_t>(uprv_strlen(ఫలితం));
            }
        } లేకపోతే {
            ఫలితం = శూన్యసూచిక;
            ఒకవేళ (ఫలితం_నిడివి != శూన్యసూచిక) {
                *ఫలితం_నిడివి = 0;
            }
        }
        తిరిగి ఫలితం;
    }

    శూన్య reset(UError_సంకేతం &స్థితి_2) override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        f_సూచీ = 0;
    }

    int32_t count(UError_సంకేతం &స్థితి_2) స్థిరరాశి override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }
        ఒకవేళ (fType == ULOC_అందుబాటు_WITH_LEGACY_ALIASES) {
            తిరిగి g_అందుబాటు_Locale_Counts[ULOC_అందుబాటు_DEFAULT]
                + g_అందుబాటు_Locale_Counts[ULOC_అందుబాటు_ONLY_LEGACY_ALIASES];
        } లేకపోతే {
            తిరిగి g_అందుబాటు_Locale_Counts[fType];
        }
    }

  అంతరంగ:
    ULoc_అందుబాటు_Type fType;
    int32_t f_సూచీ = 0;
};

/* ### Get available **************************************************/

UBool U_CALLCONV uloc_cleanup() {
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(g_అందుబాటు_Locale_పేర్లు); i++) {
        uprv_free(g_అందుబాటు_Locale_పేర్లు[i]);
        g_అందుబాటు_Locale_పేర్లు[i] = శూన్యసూచిక;
        g_అందుబాటు_Locale_Counts[i] = 0;
    }
    ginstalledLocalesInitOnce.reset();
    తిరిగి నిజం;
}

// Load Installed Locales. This function will be called exactly once
//   via the initOnce mechanism.

శూన్య U_CALLCONV ఎక్కించు_Installed_Locales(UError_సంకేతం& స్థితి_2) {
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_ULOC, uloc_cleanup);

    icu::Local_UResource_Bundle_సూచిక rb(ures_open_ప్రత్యక్ష(శూన్యసూచిక, "res_index", &స్థితి_2));
    అందుబాటు_Locales_Sink sink;
    ures_get_All_వస్తువులు_With_Fallback(rb.getAlias(), "", sink, స్థితి_2);
}

శూన్య ఎక్కించు_installed_Locales(UError_సంకేతం& స్థితి_2) {
    umtx_initOnce(ginstalledLocalesInitOnce, &ఎక్కించు_Installed_Locales, స్థితి_2);
}

} // namespace

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uloc_get_అందుబాటు(int32_t స్థానభ్రంశం) {
    icu::Error_సంకేతం స్థితి_2;
    ఎక్కించు_installed_Locales(స్థితి_2);
    ఒకవేళ (స్థితి_2.isFailure()) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (స్థానభ్రంశం > g_అందుబాటు_Locale_Counts[0]) {
        // *status = U_ILLEGAL_ARGUMENT_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి g_అందుబాటు_Locale_పేర్లు[0][స్థానభ్రంశం];
}

U_CAPI int32_t  U_EXPORT2
uloc_లెక్క_అందుబాటు() {
    icu::Error_సంకేతం స్థితి_2;
    ఎక్కించు_installed_Locales(స్థితి_2);
    ఒకవేళ (స్థితి_2.isFailure()) {
        తిరిగి 0;
    }
    తిరిగి g_అందుబాటు_Locale_Counts[0];
}

U_CAPI UEnumeration* U_EXPORT2
uloc_open_అందుబాటు_By_Type(ULoc_అందుబాటు_Type type, UError_సంకేతం* స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (type < 0 || type >= ULOC_అందుబాటు_లెక్క) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఎక్కించు_installed_Locales(*స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    Local_సూచిక<అందుబాటు_Locales_String_Enumeration> ఫలితం(
        కొత్త అందుబాటు_Locales_String_Enumeration(type), *స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి uenum_openFromStringEnumeration(ఫలితం.orphan(), స్థితి_2);
}
