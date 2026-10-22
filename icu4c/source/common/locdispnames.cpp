// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  locdispnames.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for locale display names, separated out from other .cpp files
*   that then do not depend on resource bundle code and display name data.
*/

#అన్తర్భావ <string_view>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uenum.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "ureslocs.h"
#అన్తర్భావ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) స్థిరరాశి
{
    తిరిగి ఇది->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(స్థిరరాశి Locale& displayLocale,
                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    char16_t *బఫరు;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length;

    బఫరు=ఫలితం.get_బఫరు(ULOC_FULLNAME_సామర్థ్యం);
    ఒకవేళ (బఫరు == శూన్యసూచిక) {
        ఫలితం.truncate(0);
        తిరిగి ఫలితం;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   బఫరు, ఫలితం.get_సామర్థ్యం(),
                                   &error_సంకేతం);
    ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);

    ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        బఫరు=ఫలితం.get_బఫరు(length);
        ఒకవేళ (బఫరు == శూన్యసూచిక) {
            ఫలితం.truncate(0);
            తిరిగి ఫలితం;
        }
        error_సంకేతం=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       బఫరు, ఫలితం.get_సామర్థ్యం(),
                                       &error_సంకేతం);
        ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);
    }

    తిరిగి ఫలితం;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) స్థిరరాశి
{
    తిరిగి ఇది->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(స్థిరరాశి Locale& displayLocale,
                         UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    char16_t *బఫరు;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length;

    బఫరు=ఫలితం.get_బఫరు(ULOC_FULLNAME_సామర్థ్యం);
    ఒకవేళ (బఫరు == శూన్యసూచిక) {
        ఫలితం.truncate(0);
        తిరిగి ఫలితం;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  బఫరు, ఫలితం.get_సామర్థ్యం(),
                                  &error_సంకేతం);
    ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);

    ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        బఫరు=ఫలితం.get_బఫరు(length);
        ఒకవేళ (బఫరు == శూన్యసూచిక) {
            ఫలితం.truncate(0);
            తిరిగి ఫలితం;
        }
        error_సంకేతం=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      బఫరు, ఫలితం.get_సామర్థ్యం(),
                                      &error_సంకేతం);
        ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);
    }

    తిరిగి ఫలితం;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) స్థిరరాశి
{
    తిరిగి ఇది->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(స్థిరరాశి Locale& displayLocale,
                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    char16_t *బఫరు;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length;

    బఫరు=ఫలితం.get_బఫరు(ULOC_FULLNAME_సామర్థ్యం);
    ఒకవేళ (బఫరు == శూన్యసూచిక) {
        ఫలితం.truncate(0);
        తిరిగి ఫలితం;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  బఫరు, ఫలితం.get_సామర్థ్యం(),
                                  &error_సంకేతం);
    ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);

    ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        బఫరు=ఫలితం.get_బఫరు(length);
        ఒకవేళ (బఫరు == శూన్యసూచిక) {
            ఫలితం.truncate(0);
            తిరిగి ఫలితం;
        }
        error_సంకేతం=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      బఫరు, ఫలితం.get_సామర్థ్యం(),
                                      &error_సంకేతం);
        ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);
    }

    తిరిగి ఫలితం;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) స్థిరరాశి
{
    తిరిగి ఇది->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(స్థిరరాశి Locale& displayLocale,
                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    char16_t *బఫరు;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length;

    బఫరు=ఫలితం.get_బఫరు(ULOC_FULLNAME_సామర్థ్యం);
    ఒకవేళ (బఫరు == శూన్యసూచిక) {
        ఫలితం.truncate(0);
        తిరిగి ఫలితం;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  బఫరు, ఫలితం.get_సామర్థ్యం(),
                                  &error_సంకేతం);
    ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);

    ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        బఫరు=ఫలితం.get_బఫరు(length);
        ఒకవేళ (బఫరు == శూన్యసూచిక) {
            ఫలితం.truncate(0);
            తిరిగి ఫలితం;
        }
        error_సంకేతం=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      బఫరు, ఫలితం.get_సామర్థ్యం(),
                                      &error_సంకేతం);
        ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);
    }

    తిరిగి ఫలితం;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) స్థిరరాశి
{
    తిరిగి ఇది->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(స్థిరరాశి Locale& displayLocale,
                       UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    char16_t *బఫరు;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t length;

    బఫరు=ఫలితం.get_బఫరు(ULOC_FULLNAME_సామర్థ్యం);
    ఒకవేళ (బఫరు == శూన్యసూచిక) {
        ఫలితం.truncate(0);
        తిరిగి ఫలితం;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               బఫరు, ఫలితం.get_సామర్థ్యం(),
                               &error_సంకేతం);
    ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);

    ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
        బఫరు=ఫలితం.get_బఫరు(length);
        ఒకవేళ (బఫరు == శూన్యసూచిక) {
            ఫలితం.truncate(0);
            తిరిగి ఫలితం;
        }
        error_సంకేతం=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   బఫరు, ఫలితం.get_సామర్థ్యం(),
                                   &error_సంకేతం);
        ఫలితం.release_బఫరు(U_SUCCESS(error_సంకేతం) ? length : 0);
    }

    తిరిగి ఫలితం;
}

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(స్థిరరాశి Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    తిరిగి objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(స్థిరరాశి Locale& objectLocale,
                              స్థిరరాశి Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    తిరిగి objectLocale.getDisplayName(displayLocale, name);
}

#అయితే_ముగింపు


U_NAMESPACE_చివర

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

నామాకాశ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr అక్షరం _kLanguages[]       = "Languages";
constexpr అక్షరం _kScripts[]         = "Scripts";
constexpr అక్షరం _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr అక్షరం _kCountries[]       = "Countries";
constexpr అక్షరం _kVariants[]        = "Variants";
constexpr అక్షరం k_కీలకాలు[]            = "Keys";
constexpr అక్షరం _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr అక్షరం _kCurrency[]        = "currency";
constexpr అక్షరం _kCurrencies[]      = "Currencies";
constexpr అక్షరం k_Locale_Display_నమూనా[] = "localeDisplayPattern";
constexpr అక్షరం k_నమూనా[]         = "pattern";
constexpr అక్షరం _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_కీలకం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *locale,
                    స్థిరరాశి అక్షరం *పట్టిక_కీలకం, 
                    స్థిరరాశి అక్షరం* sub_పట్టిక_కీలకం,
                    స్థిరరాశి అక్షరం *అంశం_కీలకం,
                    స్థిరరాశి అక్షరం *substitute,
                    char16_t *dest, int32_t dest_సామర్థ్యం,
                    UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0; }
    స్థిరరాశి char16_t *s = శూన్యసూచిక;
    int32_t length = 0;

    ఒకవేళ(అంశం_కీలకం==శూన్యసూచిక) {
        /* top-level item: normal resource bundle access */
        icu::Local_UResource_Bundle_సూచిక rb(ures_open(మార్గం, locale, &error_సంకేతం));

        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            s=ures_get_String_By_కీలకం(rb.getAlias(), పట్టిక_కీలకం, &length, &error_సంకేతం);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } లేకపోతే {
        తార్కిక is_Language_సంకేతం = (uprv_strncmp(పట్టిక_కీలకం, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        ఒకవేళ (is_Language_సంకేతం && uprv_strtol(అంశం_కీలకం, శూన్యసూచిక, 10)) {
            error_సంకేతం = U_MISSING_RESOURCE_ERROR;
        } లేకపోతే {
            /* second-level item, use special fallback */
            s=uloc_get_పట్టిక_String_With_Fallback(మార్గం, locale,
                                               పట్టిక_కీలకం,
                                               sub_పట్టిక_కీలకం,
                                               అంశం_కీలకం,
                                               &length,
                                               &error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం) && is_Language_సంకేతం && అంశం_కీలకం != శూన్యసూచిక) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                error_సంకేతం = U_ZERO_ERROR;
                Locale canon_కీలకం = Locale::createCanonical(అంశం_కీలకం);
                s=uloc_get_పట్టిక_String_With_Fallback(మార్గం, locale,
                                                    పట్టిక_కీలకం,
                                                    sub_పట్టిక_కీలకం,
                                                    canon_కీలకం.getName(),
                                                    &length,
                                                    &error_సంకేతం);
            }
        }
    }

    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        int32_t copy_నిడివి=uprv_min(length, dest_సామర్థ్యం);
        ఒకవేళ(copy_నిడివి>0 && s != శూన్యసూచిక) {
            u_memcpy(dest, s, copy_నిడివి);
        }
    } లేకపోతే {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_chars_గమ్యం_UChars(substitute, dest, uprv_min(length, dest_సామర్థ్యం));
        error_సంకేతం = U_USING_DEFAULT_WARNING;
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, length, &error_సంకేతం);
}

ఉపయోగించి UDisplayNameGetter = icu::CharString(std::string_view, UError_సంకేతం&);

int32_t
_getDisplayNameForComponent(స్థిరరాశి అక్షరం *locale,
                            స్థిరరాశి అక్షరం *displayLocale,
                            char16_t *dest, int32_t dest_సామర్థ్యం,
                            UDisplayNameGetter *getter,
                            స్థిరరాశి అక్షరం *ట్యాగ్,
                            UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0; }
    UError_సంకేతం local_స్థితి;
    స్థిరరాశి అక్షరం* మూలం = శూన్యసూచిక;

    ఒకవేళ(dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }

    local_స్థితి = U_ZERO_ERROR;
    icu::CharString locale_బఫరు = (*getter)(locale, local_స్థితి);
    ఒకవేళ (U_FAILURE(local_స్థితి)) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ (locale_బఫరు.is_ఖాళీలు()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        ఒకవేళ (getter == ulocimp_getLanguage) {
            locale_బఫరు.append("und", error_సంకేతం);
        } లేకపోతే {
            తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, 0, &error_సంకేతం);
        }
    }

    మూలం = ట్యాగ్ == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    తిరిగి get_String_Or_Copy_కీలకం(మూలం, displayLocale,
                               ట్యాగ్, శూన్యసూచిక, locale_బఫరు.దత్తాంశం(),
                               locale_బఫరు.దత్తాంశం(),
                               dest, dest_సామర్థ్యం,
                               error_సంకేతం);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(స్థిరరాశి అక్షరం *locale,
                        స్థిరరాశి అక్షరం *displayLocale,
                        char16_t *dest, int32_t dest_సామర్థ్యం,
                        UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                ulocimp_getLanguage, _kLanguages, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(స్థిరరాశి అక్షరం* locale,
                      స్థిరరాశి అక్షరం* displayLocale,
                      char16_t *dest, int32_t dest_సామర్థ్యం,
                      UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) { తిరిగి 0; }
    UError_సంకేతం err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                ulocimp_getScript, _kScriptsStandAlone, err);

    ఒకవేళ (dest_సామర్థ్యం == 0 && err == U_బఫరు_అధికప్రవాహం_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                                                           ulocimp_getScript, _kScripts, *p_Error_సంకేతం);
        తిరిగి (fallback_res > res) ? fallback_res : res;
    }
    ఒకవేళ ( err == U_USING_DEFAULT_WARNING ) {
        తిరిగి _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                                           ulocimp_getScript, _kScripts, *p_Error_సంకేతం);
    } లేకపోతే {
        *p_Error_సంకేతం = err;
        తిరిగి res;
    }
}

స్థిర int32_t
uloc_get_Display_Script_లోపల_Context(స్థిరరాశి అక్షరం* locale,
                      స్థిరరాశి అక్షరం* displayLocale,
                      char16_t *dest, int32_t dest_సామర్థ్యం,
                      UError_సంకేతం *p_Error_సంకేతం)
{
    తిరిగి _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                    ulocimp_getScript, _kScripts, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(స్థిరరాశి అక్షరం *locale,
                       స్థిరరాశి అక్షరం *displayLocale,
                       char16_t *dest, int32_t dest_సామర్థ్యం,
                       UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                ulocimp_getRegion, _kCountries, *p_Error_సంకేతం);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(స్థిరరాశి అక్షరం *locale,
                       స్థిరరాశి అక్షరం *displayLocale,
                       char16_t *dest, int32_t dest_సామర్థ్యం,
                       UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి _getDisplayNameForComponent(locale, displayLocale, dest, dest_సామర్థ్యం,
                ulocimp_getVariant, _kVariants, *p_Error_సంకేతం);
}

/* Instead of having a separate pass for 'special' patterns, reintegrate the two
 * so we don't get bitten by preflight bugs again.  We can be reasonably efficient
 * without two separate code paths, this code isn't that performance-critical.
 *
 * This code is general enough to deal with patterns that have a prefix or swap the
 * language and remainder components, since we gave developers enough rope to do such
 * things if they futz with the pattern data.  But since we don't give them a way to
 * specify a pattern for arbitrary combinations of components, there's not much use in
 * that.  I don't think our data includes such patterns, the only variable I know if is
 * whether there is a space before the open paren, or not.  Oh, and zh uses different
 * chars than the standard open/close paren (which ja and ko use, btw).
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayName(స్థిరరాశి అక్షరం *locale,
                    స్థిరరాశి అక్షరం *displayLocale,
                    char16_t *dest, int32_t dest_సామర్థ్యం,
                    UError_సంకేతం *p_Error_సంకేతం)
{
    స్థిర స్థిరరాశి char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    స్థిర స్థిరరాశి char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    స్థిర స్థిరరాశి char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    స్థిర స్థిరరాశి int32_t subLen = 3;
    స్థిర స్థిరరాశి char16_t default_నమూనా[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    స్థిర స్థిరరాశి int32_t defaultPatLen = 9;
    స్థిర స్థిరరాశి int32_t defaultSub0Pos = 0;
    స్థిర స్థిరరాశి int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    స్థిరరాశి char16_t *separator;
    int32_t sepLen = 0;
    స్థిరరాశి char16_t *నమూనా;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = నిజం; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = నిజం; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = అబద్ధం; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ(dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

        icu::Local_UResource_Bundle_సూచిక locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &స్థితి_2));
        icu::Local_UResource_Bundle_సూచిక dspbundle(
                ures_get_By_కీలకం_With_Fallback(locbundle.getAlias(), k_Locale_Display_నమూనా, శూన్యసూచిక, &స్థితి_2));

        separator=ures_get_String_By_కీలకం_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &స్థితి_2);
        నమూనా=ures_get_String_By_కీలకం_With_Fallback(dspbundle.getAlias(), k_నమూనా, &patLen, &స్థితి_2);
    }

    /* If we couldn't find any data, then use the defaults */
    ఒకవేళ(sepLen == 0) {
       separator = defaultSeparator;
    }
    /* #10244: Even though separator is now a pattern, it is awkward to handle it as such
     * here since we are trying to build the display string in place in the dest buffer,
     * and to handle it as a pattern would entail having separate storage for the
     * substrings that need to be combined (the first of which may be the result of
     * previous such combinations). So for now we continue to treat the portion between
     * {0} and {1} as a string to be appended when joining substrings, ignoring anything
     * that is before {0} or after {1} (no existing separator pattern has any such thing).
     * This is similar to how pattern is handled below.
     */
    {
        char16_t *p0=u_strstr(separator, sub0);
        char16_t *p1=u_strstr(separator, sub1);
        ఒకవేళ (p0==శూన్యసూచిక || p1==శూన్యసూచిక || p1<p0) {
            *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        separator = (స్థిరరాశి char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    ఒకవేళ(patLen==0 || (patLen==defaultPatLen && !u_strncmp(నమూనా, default_నమూనా, patLen))) {
        నమూనా=default_నమూనా;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } లేకపోతే { /* non-default pattern */
        char16_t *p0=u_strstr(నమూనా, sub0);
        char16_t *p1=u_strstr(నమూనా, sub1);
        ఒకవేళ (p0==శూన్యసూచిక || p1==శూన్యసూచిక) {
            *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        sub0Pos = static_cast<int32_t>(p0-నమూనా);
        sub1Pos = static_cast<int32_t>(p1-నమూనా);
        ఒకవేళ (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        ఒకవేళ (u_strchr(నమూనా, 0xFF08) != శూన్యసూచిక) {
            formatOpenParen         = 0xFF08; // fullwidth (
            formatReplaceOpenParen  = 0xFF3B; // fullwidth [
            formatCloseParen        = 0xFF09; // fullwidth )
            formatReplaceCloseParen = 0xFF3D; // fullwidth ]
        }
    }

    /* We loop here because there is one case in which after the first pass we could need to
     * reextract the data.  If there's initial padding before the first element, we put in
     * the padding and then write that element.  If it turns out there's no second element,
     * we didn't need the padding.  If we do need the data (no preflight), and the first element
     * would have fit but for the padding, we need to reextract.  In this case (only) we
     * adjust the parameters so padding is not added, and repeat.
     */
    చేయి {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::Local_UEnumeration_సూచిక kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        ఒకవేళ(sub0Pos) {
            ఒకవేళ(dest_సామర్థ్యం >= sub0Pos) {
                అయితే (patPos < sub0Pos) {
                    *p++ = నమూనా[patPos++];
                }
            } లేకపోతే {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } లేకపోతే {
            length=0;
        }

        కొరకు(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = అబద్ధం; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=dest_సామర్థ్యం-length;
            ఒకవేళ (cap <= 0) {
                cap=0;
            } లేకపోతే {
                p=dest+length;
            }

            ఒకవేళ (subi == langi) { /* {0}*/
                ఒకవేళ(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, p_Error_సంకేతం);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=నిజం;
            } లేకపోతే { /* {1} */
                ఒకవేళ(!haveRest) {
                    subdone=నిజం;
                } లేకపోతే {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    ఎంపిక(resti++) {
                        స్థితి 0:
                            restPos=length;
                            len=uloc_get_Display_Script_లోపల_Context(locale, displayLocale, p, cap, p_Error_సంకేతం);
                            ఆపు;
                        స్థితి 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, p_Error_సంకేతం);
                            ఆపు;
                        స్థితి 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, p_Error_సంకేతం);
                            ఆపు;
                        స్థితి 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, p_Error_సంకేతం));
                            U_FALLTHROUGH;
                        అప్రమేయ: {
                            స్థిరరాశి అక్షరం* kw=uenum_తదుపరి(kenum.getAlias(), &len, p_Error_సంకేతం);
                            ఒకవేళ (kw == శూన్యసూచిక) {
                                len=0; /* mark that we didn't add a component */
                                subdone=నిజం;
                            } లేకపోతే {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, p_Error_సంకేతం);
                                ఒకవేళ(len) {
                                    ఒకవేళ(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    ఒకవేళ(cap <= 0) {
                                        cap=0;
                                    } లేకపోతే {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                ఒకవేళ(*p_Error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
                                    *p_Error_సంకేతం=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_విలువ(locale, kw, displayLocale,
                                                                           p, cap, p_Error_సంకేతం);
                                ఒకవేళ(len) {
                                    ఒకవేళ(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=dest_సామర్థ్యం-length;
                                    ఒకవేళ(cap <= 0) {
                                        cap=0;
                                    } లేకపోతే {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ఆపు;
                    } /* end switch */

                    ఒకవేళ (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        ఒకవేళ(len+sepLen<=cap) {
                            స్థిరరాశి char16_t * plimit = p + len;
                            కొరకు (; p < plimit; p++) {
                                ఒకవేళ (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } లేకపోతే ఒకవేళ (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            కొరకు(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } లేకపోతే ఒకవేళ(subdone) {
                        /* remove separator if we added it */
                        ఒకవేళ (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            ఒకవేళ(*p_Error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
                *p_Error_సంకేతం=U_ZERO_ERROR;
            }

            ఒకవేళ(subdone) {
                ఒకవేళ(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    ఒకవేళ(length+padLen <= dest_సామర్థ్యం) {
                        p=dest+length;
                        కొరకు(int32_t i=0;i<padLen;++i) {
                            *p++=నమూనా[patPos++];
                        }
                    } లేకపోతే {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } లేకపోతే ఒకవేళ(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } లేకపోతే ఒకవేళ(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    ఒకవేళ(dest && sub0Pos!=0) {
                        ఒకవేళ (sub0Pos+length<=dest_సామర్థ్యం) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } లేకపోతే {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=నిజం;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } అయితే(retry);

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, length, p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(స్థిరరాశి అక్షరం* keyword,
                       స్థిరరాశి అక్షరం* displayLocale,
                       char16_t* dest,
                       int32_t dest_సామర్థ్యం,
                       UError_సంకేతం* స్థితి_2){

    /* argument checking */
    ఒకవేళ(స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }

    ఒకవేళ(dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    తిరిగి get_String_Or_Copy_కీలకం(U_ICUDATA_LANG, displayLocale,
                               k_కీలకాలు, శూన్యసూచిక,
                               keyword, 
                               keyword,      
                               dest, dest_సామర్థ్యం,
                               *స్థితి_2);

}


#నిర్వచించు UCURRENCY_DISPLAY_NAME_సూచీ 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_విలువ(   స్థిరరాశి అక్షరం* locale,
                               స్థిరరాశి అక్షరం* keyword,
                               స్థిరరాశి అక్షరం* displayLocale,
                               char16_t* dest,
                               int32_t dest_సామర్థ్యం,
                               UError_సంకేతం* స్థితి_2){


    /* argument checking */
    ఒకవేళ(స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }

    ఒకవేళ(dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* get the keyword value */
    CharString keyword_విలువ;
    ఒకవేళ (keyword != శూన్యసూచిక && *keyword != '\0') {
        keyword_విలువ = ulocimp_get_Keyword_విలువ(locale, keyword, *స్థితి_2);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    ఒకవేళ(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        స్థిరరాశి char16_t *dispName = శూన్యసూచిక;

        icu::Local_UResource_Bundle_సూచిక bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, స్థితి_2));
        icu::Local_UResource_Bundle_సూచిక currencies(
                ures_get_By_కీలకం(bundle.getAlias(), _kCurrencies, శూన్యసూచిక, స్థితి_2));
        icu::Local_UResource_Bundle_సూచిక currency(
                ures_get_By_కీలకం_With_Fallback(currencies.getAlias(), keyword_విలువ.దత్తాంశం(), శూన్యసూచిక, స్థితి_2));

        dispName = ures_get_String_By_సూచీ(currency.getAlias(), UCURRENCY_DISPLAY_NAME_సూచీ, &dispNameLen, స్థితి_2);

        ఒకవేళ(U_FAILURE(*స్థితి_2)){
            ఒకవేళ(*స్థితి_2 == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *స్థితి_2 = U_USING_DEFAULT_WARNING;
            }లేకపోతే{
                తిరిగి 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        ఒకవేళ(dispName != శూన్యసూచిక){
            ఒకవేళ(dispNameLen <= dest_సామర్థ్యం){
                u_memcpy(dest, dispName, dispNameLen);
                తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dispNameLen, స్థితి_2);
            }లేకపోతే{
                *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
                తిరిగి dispNameLen;
            }
        }లేకపోతే{
            /* we have not found the display name for the value .. just copy over */
            ఒకవేళ(keyword_విలువ.length() <= dest_సామర్థ్యం){
                u_chars_గమ్యం_UChars(keyword_విలువ.దత్తాంశం(), dest, keyword_విలువ.length());
                తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, keyword_విలువ.length(), స్థితి_2);
            }లేకపోతే{
                 *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
                తిరిగి keyword_విలువ.length();
            }
        }

        
    }లేకపోతే{

        తిరిగి get_String_Or_Copy_కీలకం(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_విలువ.దత్తాంశం(),
                                   keyword_విలువ.దత్తాంశం(),
                                   dest, dest_సామర్థ్యం,
                                   *స్థితి_2);
    }
}
