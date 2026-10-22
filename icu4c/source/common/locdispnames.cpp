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

#ضمن <string_view>

#ضمن "unicode/utypes.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uenum.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/ustring.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "putilimp.h"
#ضمن "ulocimp.h"
#ضمن "uresimp.h"
#ضمن "ureslocs.h"
#ضمن "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ثابت Locale& displayLocale,
                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    char16_t *بفر;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t length;

    بفر=پایله.get_بفر(ULOC_FULLNAME_ظرفیت);
    إذا (بفر == تش_اشاره) {
        پایله.truncate(0);
        أرجع پایله;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   بفر, پایله.get_ظرفیت(),
                                   &error_کوډ);
    پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);

    إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
        بفر=پایله.get_بفر(length);
        إذا (بفر == تش_اشاره) {
            پایله.truncate(0);
            أرجع پایله;
        }
        error_کوډ=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       بفر, پایله.get_ظرفیت(),
                                       &error_کوډ);
        پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);
    }

    أرجع پایله;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ثابت Locale& displayLocale,
                         UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    char16_t *بفر;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t length;

    بفر=پایله.get_بفر(ULOC_FULLNAME_ظرفیت);
    إذا (بفر == تش_اشاره) {
        پایله.truncate(0);
        أرجع پایله;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  بفر, پایله.get_ظرفیت(),
                                  &error_کوډ);
    پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);

    إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
        بفر=پایله.get_بفر(length);
        إذا (بفر == تش_اشاره) {
            پایله.truncate(0);
            أرجع پایله;
        }
        error_کوډ=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      بفر, پایله.get_ظرفیت(),
                                      &error_کوډ);
        پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);
    }

    أرجع پایله;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ثابت Locale& displayLocale,
                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    char16_t *بفر;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t length;

    بفر=پایله.get_بفر(ULOC_FULLNAME_ظرفیت);
    إذا (بفر == تش_اشاره) {
        پایله.truncate(0);
        أرجع پایله;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  بفر, پایله.get_ظرفیت(),
                                  &error_کوډ);
    پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);

    إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
        بفر=پایله.get_بفر(length);
        إذا (بفر == تش_اشاره) {
            پایله.truncate(0);
            أرجع پایله;
        }
        error_کوډ=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      بفر, پایله.get_ظرفیت(),
                                      &error_کوډ);
        پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);
    }

    أرجع پایله;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ثابت Locale& displayLocale,
                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    char16_t *بفر;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t length;

    بفر=پایله.get_بفر(ULOC_FULLNAME_ظرفیت);
    إذا (بفر == تش_اشاره) {
        پایله.truncate(0);
        أرجع پایله;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  بفر, پایله.get_ظرفیت(),
                                  &error_کوډ);
    پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);

    إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
        بفر=پایله.get_بفر(length);
        إذا (بفر == تش_اشاره) {
            پایله.truncate(0);
            أرجع پایله;
        }
        error_کوډ=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      بفر, پایله.get_ظرفیت(),
                                      &error_کوډ);
        پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);
    }

    أرجع پایله;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ثابت Locale& displayLocale,
                       UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    char16_t *بفر;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    int32_t length;

    بفر=پایله.get_بفر(ULOC_FULLNAME_ظرفیت);
    إذا (بفر == تش_اشاره) {
        پایله.truncate(0);
        أرجع پایله;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               بفر, پایله.get_ظرفیت(),
                               &error_کوډ);
    پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);

    إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
        بفر=پایله.get_بفر(length);
        إذا (بفر == تش_اشاره) {
            پایله.truncate(0);
            أرجع پایله;
        }
        error_کوډ=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   بفر, پایله.get_ظرفیت(),
                                   &error_کوډ);
        پایله.release_بفر(U_SUCCESS(error_کوډ) ? length : 0);
    }

    أرجع پایله;
}

#إذا !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ثابت Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    أرجع objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ثابت Locale& objectLocale,
                              ثابت Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    أرجع objectLocale.getDisplayName(displayLocale, name);
}

#د_که_پای


U_NAMESPACE_پای_ټکی

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr حرف _kLanguages[]       = "Languages";
constexpr حرف _kScripts[]         = "Scripts";
constexpr حرف _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr حرف _kCountries[]       = "Countries";
constexpr حرف _kVariants[]        = "Variants";
constexpr حرف k_کلیګانې[]            = "Keys";
constexpr حرف _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr حرف _kCurrency[]        = "currency";
constexpr حرف _kCurrencies[]      = "Currencies";
constexpr حرف k_Locale_Display_نمونه[] = "localeDisplayPattern";
constexpr حرف k_نمونه[]         = "pattern";
constexpr حرف _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_کلی(ثابت حرف *لار, ثابت حرف *locale,
                    ثابت حرف *جدول_کلی, 
                    ثابت حرف* sub_جدول_کلی,
                    ثابت حرف *توکی_کلی,
                    ثابت حرف *substitute,
                    char16_t *dest, int32_t dest_ظرفیت,
                    UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع 0; }
    ثابت char16_t *s = تش_اشاره;
    int32_t length = 0;

    إذا(توکی_کلی==تش_اشاره) {
        /* top-level item: normal resource bundle access */
        icu::Local_UResource_Bundle_اشاره_ګر rb(ures_open(لار, locale, &error_کوډ));

        إذا(U_SUCCESS(error_کوډ)) {
            s=ures_get_String_By_کلی(rb.getAlias(), جدول_کلی, &length, &error_کوډ);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } وإلا {
        منطقي is_Language_کوډ = (uprv_strncmp(جدول_کلی, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        إذا (is_Language_کوډ && uprv_strtol(توکی_کلی, تش_اشاره, 10)) {
            error_کوډ = U_MISSING_RESOURCE_ERROR;
        } وإلا {
            /* second-level item, use special fallback */
            s=uloc_get_جدول_String_With_Fallback(لار, locale,
                                               جدول_کلی,
                                               sub_جدول_کلی,
                                               توکی_کلی,
                                               &length,
                                               &error_کوډ);
            إذا (U_FAILURE(error_کوډ) && is_Language_کوډ && توکی_کلی != تش_اشاره) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                error_کوډ = U_ZERO_ERROR;
                Locale canon_کلی = Locale::createCanonical(توکی_کلی);
                s=uloc_get_جدول_String_With_Fallback(لار, locale,
                                                    جدول_کلی,
                                                    sub_جدول_کلی,
                                                    canon_کلی.getName(),
                                                    &length,
                                                    &error_کوډ);
            }
        }
    }

    إذا(U_SUCCESS(error_کوډ)) {
        int32_t copy_اوږدتیا=uprv_min(length, dest_ظرفیت);
        إذا(copy_اوږدتیا>0 && s != تش_اشاره) {
            u_memcpy(dest, s, copy_اوږدتیا);
        }
    } وإلا {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_chars_منزل_UChars(substitute, dest, uprv_min(length, dest_ظرفیت));
        error_کوډ = U_USING_DEFAULT_WARNING;
    }

    أرجع u_terminateUChars(dest, dest_ظرفیت, length, &error_کوډ);
}

کارول UDisplayNameGetter = icu::CharString(std::string_view, UError_کوډ&);

int32_t
_getDisplayNameForComponent(ثابت حرف *locale,
                            ثابت حرف *displayLocale,
                            char16_t *dest, int32_t dest_ظرفیت,
                            UDisplayNameGetter *getter,
                            ثابت حرف *نښان,
                            UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع 0; }
    UError_کوډ local_حالت;
    ثابت حرف* ریښه = تش_اشاره;

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }

    local_حالت = U_ZERO_ERROR;
    icu::CharString locale_بفر = (*getter)(locale, local_حالت);
    إذا (U_FAILURE(local_حالت)) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا (locale_بفر.is_تشه()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        إذا (getter == ulocimp_getLanguage) {
            locale_بفر.append("und", error_کوډ);
        } وإلا {
            أرجع u_terminateUChars(dest, dest_ظرفیت, 0, &error_کوډ);
        }
    }

    ریښه = نښان == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    أرجع get_String_Or_Copy_کلی(ریښه, displayLocale,
                               نښان, تش_اشاره, locale_بفر.معلومات(),
                               locale_بفر.معلومات(),
                               dest, dest_ظرفیت,
                               error_کوډ);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ثابت حرف *locale,
                        ثابت حرف *displayLocale,
                        char16_t *dest, int32_t dest_ظرفیت,
                        UError_کوډ *p_Error_کوډ) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getLanguage, _kLanguages, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t dest_ظرفیت,
                      UError_کوډ *p_Error_کوډ)
{
    إذا (U_FAILURE(*p_Error_کوډ)) { أرجع 0; }
    UError_کوډ err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getScript, _kScriptsStandAlone, err);

    إذا (dest_ظرفیت == 0 && err == U_بفر_ډکېدنه_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                                                           ulocimp_getScript, _kScripts, *p_Error_کوډ);
        أرجع (fallback_res > res) ? fallback_res : res;
    }
    إذا ( err == U_USING_DEFAULT_WARNING ) {
        أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                                           ulocimp_getScript, _kScripts, *p_Error_کوډ);
    } وإلا {
        *p_Error_کوډ = err;
        أرجع res;
    }
}

ساكن int32_t
uloc_get_Display_Script_دننه_Context(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t dest_ظرفیت,
                      UError_کوډ *p_Error_کوډ)
{
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                    ulocimp_getScript, _kScripts, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ثابت حرف *locale,
                       ثابت حرف *displayLocale,
                       char16_t *dest, int32_t dest_ظرفیت,
                       UError_کوډ *p_Error_کوډ) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getRegion, _kCountries, *p_Error_کوډ);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ثابت حرف *locale,
                       ثابت حرف *displayLocale,
                       char16_t *dest, int32_t dest_ظرفیت,
                       UError_کوډ *p_Error_کوډ) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getVariant, _kVariants, *p_Error_کوډ);
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
uloc_getDisplayName(ثابت حرف *locale,
                    ثابت حرف *displayLocale,
                    char16_t *dest, int32_t dest_ظرفیت,
                    UError_کوډ *p_Error_کوډ)
{
    ساكن ثابت char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ساكن ثابت char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ساكن ثابت char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ساكن ثابت int32_t subLen = 3;
    ساكن ثابت char16_t default_نمونه[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ساكن ثابت int32_t defaultPatLen = 9;
    ساكن ثابت int32_t defaultSub0Pos = 0;
    ساكن ثابت int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ثابت char16_t *separator;
    int32_t sepLen = 0;
    ثابت char16_t *نمونه;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = ریښتیا; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = ریښتیا; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = خطأ; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    {
        UError_کوډ حالت = U_ZERO_ERROR;

        icu::Local_UResource_Bundle_اشاره_ګر locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &حالت));
        icu::Local_UResource_Bundle_اشاره_ګر dspbundle(
                ures_get_By_کلی_With_Fallback(locbundle.getAlias(), k_Locale_Display_نمونه, تش_اشاره, &حالت));

        separator=ures_get_String_By_کلی_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &حالت);
        نمونه=ures_get_String_By_کلی_With_Fallback(dspbundle.getAlias(), k_نمونه, &patLen, &حالت);
    }

    /* If we couldn't find any data, then use the defaults */
    إذا(sepLen == 0) {
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
        إذا (p0==تش_اشاره || p1==تش_اشاره || p1<p0) {
            *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        separator = (ثابت char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    إذا(patLen==0 || (patLen==defaultPatLen && !u_strncmp(نمونه, default_نمونه, patLen))) {
        نمونه=default_نمونه;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } وإلا { /* non-default pattern */
        char16_t *p0=u_strstr(نمونه, sub0);
        char16_t *p1=u_strstr(نمونه, sub1);
        إذا (p0==تش_اشاره || p1==تش_اشاره) {
            *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        sub0Pos = static_cast<int32_t>(p0-نمونه);
        sub1Pos = static_cast<int32_t>(p1-نمونه);
        إذا (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        إذا (u_strchr(نمونه, 0xFF08) != تش_اشاره) {
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
    افعل {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::Local_UEnumeration_اشاره_ګر kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        إذا(sub0Pos) {
            إذا(dest_ظرفیت >= sub0Pos) {
                طالما (patPos < sub0Pos) {
                    *p++ = نمونه[patPos++];
                }
            } وإلا {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } وإلا {
            length=0;
        }

        لكل(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = خطأ; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=dest_ظرفیت-length;
            إذا (cap <= 0) {
                cap=0;
            } وإلا {
                p=dest+length;
            }

            إذا (subi == langi) { /* {0}*/
                إذا(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, p_Error_کوډ);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=ریښتیا;
            } وإلا { /* {1} */
                إذا(!haveRest) {
                    subdone=ریښتیا;
                } وإلا {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    بدل(resti++) {
                        حالة 0:
                            restPos=length;
                            len=uloc_get_Display_Script_دننه_Context(locale, displayLocale, p, cap, p_Error_کوډ);
                            اكسر;
                        حالة 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, p_Error_کوډ);
                            اكسر;
                        حالة 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, p_Error_کوډ);
                            اكسر;
                        حالة 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, p_Error_کوډ));
                            U_FALLTHROUGH;
                        افتراضي: {
                            ثابت حرف* kw=uenum_بل(kenum.getAlias(), &len, p_Error_کوډ);
                            إذا (kw == تش_اشاره) {
                                len=0; /* mark that we didn't add a component */
                                subdone=ریښتیا;
                            } وإلا {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, p_Error_کوډ);
                                إذا(len) {
                                    إذا(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    إذا(cap <= 0) {
                                        cap=0;
                                    } وإلا {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                إذا(*p_Error_کوډ == U_بفر_ډکېدنه_ERROR) {
                                    *p_Error_کوډ=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_ارزښت(locale, kw, displayLocale,
                                                                           p, cap, p_Error_کوډ);
                                إذا(len) {
                                    إذا(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=dest_ظرفیت-length;
                                    إذا(cap <= 0) {
                                        cap=0;
                                    } وإلا {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } اكسر;
                    } /* end switch */

                    إذا (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        إذا(len+sepLen<=cap) {
                            ثابت char16_t * plimit = p + len;
                            لكل (; p < plimit; p++) {
                                إذا (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } وإلا إذا (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            لكل(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } وإلا إذا(subdone) {
                        /* remove separator if we added it */
                        إذا (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            إذا(*p_Error_کوډ == U_بفر_ډکېدنه_ERROR) {
                *p_Error_کوډ=U_ZERO_ERROR;
            }

            إذا(subdone) {
                إذا(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    إذا(length+padLen <= dest_ظرفیت) {
                        p=dest+length;
                        لكل(int32_t i=0;i<padLen;++i) {
                            *p++=نمونه[patPos++];
                        }
                    } وإلا {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } وإلا إذا(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } وإلا إذا(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    إذا(dest && sub0Pos!=0) {
                        إذا (sub0Pos+length<=dest_ظرفیت) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } وإلا {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=ریښتیا;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } طالما(retry);

    أرجع u_terminateUChars(dest, dest_ظرفیت, length, p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ثابت حرف* keyword,
                       ثابت حرف* displayLocale,
                       char16_t* dest,
                       int32_t dest_ظرفیت,
                       UError_کوډ* حالت){

    /* argument checking */
    إذا(حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    أرجع get_String_Or_Copy_کلی(U_ICUDATA_LANG, displayLocale,
                               k_کلیګانې, تش_اشاره,
                               keyword, 
                               keyword,      
                               dest, dest_ظرفیت,
                               *حالت);

}


#تعریف_کړه UCURRENCY_DISPLAY_NAME_شاخص 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_ارزښت(   ثابت حرف* locale,
                               ثابت حرف* keyword,
                               ثابت حرف* displayLocale,
                               char16_t* dest,
                               int32_t dest_ظرفیت,
                               UError_کوډ* حالت){


    /* argument checking */
    إذا(حالت==تش_اشاره || U_FAILURE(*حالت)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* get the keyword value */
    CharString keyword_ارزښت;
    إذا (keyword != تش_اشاره && *keyword != '\0') {
        keyword_ارزښت = ulocimp_get_Keyword_ارزښت(locale, keyword, *حالت);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    إذا(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ثابت char16_t *dispName = تش_اشاره;

        icu::Local_UResource_Bundle_اشاره_ګر bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, حالت));
        icu::Local_UResource_Bundle_اشاره_ګر currencies(
                ures_get_By_کلی(bundle.getAlias(), _kCurrencies, تش_اشاره, حالت));
        icu::Local_UResource_Bundle_اشاره_ګر currency(
                ures_get_By_کلی_With_Fallback(currencies.getAlias(), keyword_ارزښت.معلومات(), تش_اشاره, حالت));

        dispName = ures_get_String_By_شاخص(currency.getAlias(), UCURRENCY_DISPLAY_NAME_شاخص, &dispNameLen, حالت);

        إذا(U_FAILURE(*حالت)){
            إذا(*حالت == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *حالت = U_USING_DEFAULT_WARNING;
            }وإلا{
                أرجع 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        إذا(dispName != تش_اشاره){
            إذا(dispNameLen <= dest_ظرفیت){
                u_memcpy(dest, dispName, dispNameLen);
                أرجع u_terminateUChars(dest, dest_ظرفیت, dispNameLen, حالت);
            }وإلا{
                *حالت = U_بفر_ډکېدنه_ERROR;
                أرجع dispNameLen;
            }
        }وإلا{
            /* we have not found the display name for the value .. just copy over */
            إذا(keyword_ارزښت.length() <= dest_ظرفیت){
                u_chars_منزل_UChars(keyword_ارزښت.معلومات(), dest, keyword_ارزښت.length());
                أرجع u_terminateUChars(dest, dest_ظرفیت, keyword_ارزښت.length(), حالت);
            }وإلا{
                 *حالت = U_بفر_ډکېدنه_ERROR;
                أرجع keyword_ارزښت.length();
            }
        }

        
    }وإلا{

        أرجع get_String_Or_Copy_کلی(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_ارزښت.معلومات(),
                                   keyword_ارزښت.معلومات(),
                                   dest, dest_ظرفیت,
                                   *حالت);
    }
}
