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
                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    char16_t *میانگیر;
    UError_کد error_کد=U_ZERO_ERROR;
    int32_t length;

    میانگیر=نتیجه.get_میانگیر(ULOC_FULLNAME_ظرفیت);
    إذا (میانگیر == اشاره_تهی) {
        نتیجه.truncate(0);
        أرجع نتیجه;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   میانگیر, نتیجه.get_ظرفیت(),
                                   &error_کد);
    نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);

    إذا(error_کد==U_میانگیر_سرریز_ERROR) {
        میانگیر=نتیجه.get_میانگیر(length);
        إذا (میانگیر == اشاره_تهی) {
            نتیجه.truncate(0);
            أرجع نتیجه;
        }
        error_کد=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       میانگیر, نتیجه.get_ظرفیت(),
                                       &error_کد);
        نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);
    }

    أرجع نتیجه;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ثابت Locale& displayLocale,
                         UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    char16_t *میانگیر;
    UError_کد error_کد=U_ZERO_ERROR;
    int32_t length;

    میانگیر=نتیجه.get_میانگیر(ULOC_FULLNAME_ظرفیت);
    إذا (میانگیر == اشاره_تهی) {
        نتیجه.truncate(0);
        أرجع نتیجه;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  میانگیر, نتیجه.get_ظرفیت(),
                                  &error_کد);
    نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);

    إذا(error_کد==U_میانگیر_سرریز_ERROR) {
        میانگیر=نتیجه.get_میانگیر(length);
        إذا (میانگیر == اشاره_تهی) {
            نتیجه.truncate(0);
            أرجع نتیجه;
        }
        error_کد=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      میانگیر, نتیجه.get_ظرفیت(),
                                      &error_کد);
        نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);
    }

    أرجع نتیجه;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ثابت Locale& displayLocale,
                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    char16_t *میانگیر;
    UError_کد error_کد=U_ZERO_ERROR;
    int32_t length;

    میانگیر=نتیجه.get_میانگیر(ULOC_FULLNAME_ظرفیت);
    إذا (میانگیر == اشاره_تهی) {
        نتیجه.truncate(0);
        أرجع نتیجه;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  میانگیر, نتیجه.get_ظرفیت(),
                                  &error_کد);
    نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);

    إذا(error_کد==U_میانگیر_سرریز_ERROR) {
        میانگیر=نتیجه.get_میانگیر(length);
        إذا (میانگیر == اشاره_تهی) {
            نتیجه.truncate(0);
            أرجع نتیجه;
        }
        error_کد=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      میانگیر, نتیجه.get_ظرفیت(),
                                      &error_کد);
        نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);
    }

    أرجع نتیجه;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ثابت Locale& displayLocale,
                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    char16_t *میانگیر;
    UError_کد error_کد=U_ZERO_ERROR;
    int32_t length;

    میانگیر=نتیجه.get_میانگیر(ULOC_FULLNAME_ظرفیت);
    إذا (میانگیر == اشاره_تهی) {
        نتیجه.truncate(0);
        أرجع نتیجه;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  میانگیر, نتیجه.get_ظرفیت(),
                                  &error_کد);
    نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);

    إذا(error_کد==U_میانگیر_سرریز_ERROR) {
        میانگیر=نتیجه.get_میانگیر(length);
        إذا (میانگیر == اشاره_تهی) {
            نتیجه.truncate(0);
            أرجع نتیجه;
        }
        error_کد=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      میانگیر, نتیجه.get_ظرفیت(),
                                      &error_کد);
        نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);
    }

    أرجع نتیجه;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ثابت Locale& displayLocale,
                       UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    char16_t *میانگیر;
    UError_کد error_کد=U_ZERO_ERROR;
    int32_t length;

    میانگیر=نتیجه.get_میانگیر(ULOC_FULLNAME_ظرفیت);
    إذا (میانگیر == اشاره_تهی) {
        نتیجه.truncate(0);
        أرجع نتیجه;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               میانگیر, نتیجه.get_ظرفیت(),
                               &error_کد);
    نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);

    إذا(error_کد==U_میانگیر_سرریز_ERROR) {
        میانگیر=نتیجه.get_میانگیر(length);
        إذا (میانگیر == اشاره_تهی) {
            نتیجه.truncate(0);
            أرجع نتیجه;
        }
        error_کد=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   میانگیر, نتیجه.get_ظرفیت(),
                                   &error_کد);
        نتیجه.release_میانگیر(U_SUCCESS(error_کد) ? length : 0);
    }

    أرجع نتیجه;
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

#پایان_اگر


U_NAMESPACE_انتها

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
constexpr حرف k_کلیدها[]            = "Keys";
constexpr حرف _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr حرف _kCurrency[]        = "currency";
constexpr حرف _kCurrencies[]      = "Currencies";
constexpr حرف k_Locale_Display_الگو[] = "localeDisplayPattern";
constexpr حرف k_الگو[]         = "pattern";
constexpr حرف _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_کلید(ثابت حرف *مسیر, ثابت حرف *locale,
                    ثابت حرف *جدول_کلید, 
                    ثابت حرف* sub_جدول_کلید,
                    ثابت حرف *قلم_کلید,
                    ثابت حرف *substitute,
                    char16_t *dest, int32_t dest_ظرفیت,
                    UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع 0; }
    ثابت char16_t *s = اشاره_تهی;
    int32_t length = 0;

    إذا(قلم_کلید==اشاره_تهی) {
        /* top-level item: normal resource bundle access */
        icu::Local_UResource_Bundle_اشاره_گر rb(ures_open(مسیر, locale, &error_کد));

        إذا(U_SUCCESS(error_کد)) {
            s=ures_get_String_By_کلید(rb.getAlias(), جدول_کلید, &length, &error_کد);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } وإلا {
        منطقي is_Language_کد = (uprv_strncmp(جدول_کلید, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        إذا (is_Language_کد && uprv_strtol(قلم_کلید, اشاره_تهی, 10)) {
            error_کد = U_MISSING_RESOURCE_ERROR;
        } وإلا {
            /* second-level item, use special fallback */
            s=uloc_get_جدول_String_With_Fallback(مسیر, locale,
                                               جدول_کلید,
                                               sub_جدول_کلید,
                                               قلم_کلید,
                                               &length,
                                               &error_کد);
            إذا (U_FAILURE(error_کد) && is_Language_کد && قلم_کلید != اشاره_تهی) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                error_کد = U_ZERO_ERROR;
                Locale canon_کلید = Locale::createCanonical(قلم_کلید);
                s=uloc_get_جدول_String_With_Fallback(مسیر, locale,
                                                    جدول_کلید,
                                                    sub_جدول_کلید,
                                                    canon_کلید.getName(),
                                                    &length,
                                                    &error_کد);
            }
        }
    }

    إذا(U_SUCCESS(error_کد)) {
        int32_t copy_درازی=uprv_min(length, dest_ظرفیت);
        إذا(copy_درازی>0 && s != اشاره_تهی) {
            u_memcpy(dest, s, copy_درازی);
        }
    } وإلا {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_chars_مقصد_UChars(substitute, dest, uprv_min(length, dest_ظرفیت));
        error_کد = U_USING_DEFAULT_WARNING;
    }

    أرجع u_terminateUChars(dest, dest_ظرفیت, length, &error_کد);
}

استفاده UDisplayNameGetter = icu::CharString(std::string_view, UError_کد&);

int32_t
_getDisplayNameForComponent(ثابت حرف *locale,
                            ثابت حرف *displayLocale,
                            char16_t *dest, int32_t dest_ظرفیت,
                            UDisplayNameGetter *getter,
                            ثابت حرف *برچسب,
                            UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع 0; }
    UError_کد local_وضعیت;
    ثابت حرف* ریشه = اشاره_تهی;

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا (locale == اشاره_تهی) {
        locale = uloc_getDefault();
    }

    local_وضعیت = U_ZERO_ERROR;
    icu::CharString locale_میانگیر = (*getter)(locale, local_وضعیت);
    إذا (U_FAILURE(local_وضعیت)) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    إذا (locale_میانگیر.is_خالی()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        إذا (getter == ulocimp_getLanguage) {
            locale_میانگیر.append("und", error_کد);
        } وإلا {
            أرجع u_terminateUChars(dest, dest_ظرفیت, 0, &error_کد);
        }
    }

    ریشه = برچسب == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    أرجع get_String_Or_Copy_کلید(ریشه, displayLocale,
                               برچسب, اشاره_تهی, locale_میانگیر.داده(),
                               locale_میانگیر.داده(),
                               dest, dest_ظرفیت,
                               error_کد);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ثابت حرف *locale,
                        ثابت حرف *displayLocale,
                        char16_t *dest, int32_t dest_ظرفیت,
                        UError_کد *p_Error_کد) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getLanguage, _kLanguages, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t dest_ظرفیت,
                      UError_کد *p_Error_کد)
{
    إذا (U_FAILURE(*p_Error_کد)) { أرجع 0; }
    UError_کد err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getScript, _kScriptsStandAlone, err);

    إذا (dest_ظرفیت == 0 && err == U_میانگیر_سرریز_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                                                           ulocimp_getScript, _kScripts, *p_Error_کد);
        أرجع (fallback_res > res) ? fallback_res : res;
    }
    إذا ( err == U_USING_DEFAULT_WARNING ) {
        أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                                           ulocimp_getScript, _kScripts, *p_Error_کد);
    } وإلا {
        *p_Error_کد = err;
        أرجع res;
    }
}

ساكن int32_t
uloc_get_Display_Script_درون_Context(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t dest_ظرفیت,
                      UError_کد *p_Error_کد)
{
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                    ulocimp_getScript, _kScripts, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ثابت حرف *locale,
                       ثابت حرف *displayLocale,
                       char16_t *dest, int32_t dest_ظرفیت,
                       UError_کد *p_Error_کد) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getRegion, _kCountries, *p_Error_کد);
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
                       UError_کد *p_Error_کد) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, dest_ظرفیت,
                ulocimp_getVariant, _kVariants, *p_Error_کد);
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
                    UError_کد *p_Error_کد)
{
    ساكن ثابت char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ساكن ثابت char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ساكن ثابت char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ساكن ثابت int32_t subLen = 3;
    ساكن ثابت char16_t default_الگو[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ساكن ثابت int32_t defaultPatLen = 9;
    ساكن ثابت int32_t defaultSub0Pos = 0;
    ساكن ثابت int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ثابت char16_t *separator;
    int32_t sepLen = 0;
    ثابت char16_t *الگو;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = درست; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = درست; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = خطأ; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    {
        UError_کد وضعیت = U_ZERO_ERROR;

        icu::Local_UResource_Bundle_اشاره_گر locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &وضعیت));
        icu::Local_UResource_Bundle_اشاره_گر dspbundle(
                ures_get_By_کلید_With_Fallback(locbundle.getAlias(), k_Locale_Display_الگو, اشاره_تهی, &وضعیت));

        separator=ures_get_String_By_کلید_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &وضعیت);
        الگو=ures_get_String_By_کلید_With_Fallback(dspbundle.getAlias(), k_الگو, &patLen, &وضعیت);
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
        إذا (p0==اشاره_تهی || p1==اشاره_تهی || p1<p0) {
            *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع 0;
        }
        separator = (ثابت char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    إذا(patLen==0 || (patLen==defaultPatLen && !u_strncmp(الگو, default_الگو, patLen))) {
        الگو=default_الگو;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } وإلا { /* non-default pattern */
        char16_t *p0=u_strstr(الگو, sub0);
        char16_t *p1=u_strstr(الگو, sub1);
        إذا (p0==اشاره_تهی || p1==اشاره_تهی) {
            *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع 0;
        }
        sub0Pos = static_cast<int32_t>(p0-الگو);
        sub1Pos = static_cast<int32_t>(p1-الگو);
        إذا (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        إذا (u_strchr(الگو, 0xFF08) != اشاره_تهی) {
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
        icu::Local_UEnumeration_اشاره_گر kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        إذا(sub0Pos) {
            إذا(dest_ظرفیت >= sub0Pos) {
                طالما (patPos < sub0Pos) {
                    *p++ = الگو[patPos++];
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
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, p_Error_کد);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=درست;
            } وإلا { /* {1} */
                إذا(!haveRest) {
                    subdone=درست;
                } وإلا {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    بدل(resti++) {
                        حالة 0:
                            restPos=length;
                            len=uloc_get_Display_Script_درون_Context(locale, displayLocale, p, cap, p_Error_کد);
                            اكسر;
                        حالة 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, p_Error_کد);
                            اكسر;
                        حالة 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, p_Error_کد);
                            اكسر;
                        حالة 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, p_Error_کد));
                            U_FALLTHROUGH;
                        افتراضي: {
                            ثابت حرف* kw=uenum_بعدی(kenum.getAlias(), &len, p_Error_کد);
                            إذا (kw == اشاره_تهی) {
                                len=0; /* mark that we didn't add a component */
                                subdone=درست;
                            } وإلا {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, p_Error_کد);
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
                                إذا(*p_Error_کد == U_میانگیر_سرریز_ERROR) {
                                    *p_Error_کد=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_مقدار(locale, kw, displayLocale,
                                                                           p, cap, p_Error_کد);
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

            إذا(*p_Error_کد == U_میانگیر_سرریز_ERROR) {
                *p_Error_کد=U_ZERO_ERROR;
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
                            *p++=الگو[patPos++];
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
                            retry=درست;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } طالما(retry);

    أرجع u_terminateUChars(dest, dest_ظرفیت, length, p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ثابت حرف* keyword,
                       ثابت حرف* displayLocale,
                       char16_t* dest,
                       int32_t dest_ظرفیت,
                       UError_کد* وضعیت){

    /* argument checking */
    إذا(وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    أرجع get_String_Or_Copy_کلید(U_ICUDATA_LANG, displayLocale,
                               k_کلیدها, اشاره_تهی,
                               keyword, 
                               keyword,      
                               dest, dest_ظرفیت,
                               *وضعیت);

}


#تعریف_کن UCURRENCY_DISPLAY_NAME_شاخص 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_مقدار(   ثابت حرف* locale,
                               ثابت حرف* keyword,
                               ثابت حرف* displayLocale,
                               char16_t* dest,
                               int32_t dest_ظرفیت,
                               UError_کد* وضعیت){


    /* argument checking */
    إذا(وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع 0;
    }

    إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* get the keyword value */
    CharString keyword_مقدار;
    إذا (keyword != اشاره_تهی && *keyword != '\0') {
        keyword_مقدار = ulocimp_get_Keyword_مقدار(locale, keyword, *وضعیت);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    إذا(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ثابت char16_t *dispName = اشاره_تهی;

        icu::Local_UResource_Bundle_اشاره_گر bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, وضعیت));
        icu::Local_UResource_Bundle_اشاره_گر currencies(
                ures_get_By_کلید(bundle.getAlias(), _kCurrencies, اشاره_تهی, وضعیت));
        icu::Local_UResource_Bundle_اشاره_گر currency(
                ures_get_By_کلید_With_Fallback(currencies.getAlias(), keyword_مقدار.داده(), اشاره_تهی, وضعیت));

        dispName = ures_get_String_By_شاخص(currency.getAlias(), UCURRENCY_DISPLAY_NAME_شاخص, &dispNameLen, وضعیت);

        إذا(U_FAILURE(*وضعیت)){
            إذا(*وضعیت == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *وضعیت = U_USING_DEFAULT_WARNING;
            }وإلا{
                أرجع 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        إذا(dispName != اشاره_تهی){
            إذا(dispNameLen <= dest_ظرفیت){
                u_memcpy(dest, dispName, dispNameLen);
                أرجع u_terminateUChars(dest, dest_ظرفیت, dispNameLen, وضعیت);
            }وإلا{
                *وضعیت = U_میانگیر_سرریز_ERROR;
                أرجع dispNameLen;
            }
        }وإلا{
            /* we have not found the display name for the value .. just copy over */
            إذا(keyword_مقدار.length() <= dest_ظرفیت){
                u_chars_مقصد_UChars(keyword_مقدار.داده(), dest, keyword_مقدار.length());
                أرجع u_terminateUChars(dest, dest_ظرفیت, keyword_مقدار.length(), وضعیت);
            }وإلا{
                 *وضعیت = U_میانگیر_سرریز_ERROR;
                أرجع keyword_مقدار.length();
            }
        }

        
    }وإلا{

        أرجع get_String_Or_Copy_کلید(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_مقدار.داده(),
                                   keyword_مقدار.داده(),
                                   dest, dest_ظرفیت,
                                   *وضعیت);
    }
}
