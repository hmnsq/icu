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

#ଅନ୍ତର୍ଭାଵ <string_view>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uenum.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "ureslocs.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ଅଚଳ
{
    ଫେରନ୍ତୁ ଏହି->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ଅଚଳ Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ଯଦି (buffer == nullptr) {
        result.truncate(0);
        ଫେରନ୍ତୁ result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ଯଦି (buffer == nullptr) {
            result.truncate(0);
            ଫେରନ୍ତୁ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ଅଚଳ
{
    ଫେରନ୍ତୁ ଏହି->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ଅଚଳ Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ଯଦି (buffer == nullptr) {
        result.truncate(0);
        ଫେରନ୍ତୁ result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ଯଦି (buffer == nullptr) {
            result.truncate(0);
            ଫେରନ୍ତୁ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ଅଚଳ
{
    ଫେରନ୍ତୁ ଏହି->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ଅଚଳ Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ଯଦି (buffer == nullptr) {
        result.truncate(0);
        ଫେରନ୍ତୁ result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ଯଦି (buffer == nullptr) {
            result.truncate(0);
            ଫେରନ୍ତୁ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ଅଚଳ
{
    ଫେରନ୍ତୁ ଏହି->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ଅଚଳ Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ଯଦି (buffer == nullptr) {
        result.truncate(0);
        ଫେରନ୍ତୁ result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ଯଦି (buffer == nullptr) {
            result.truncate(0);
            ଫେରନ୍ତୁ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ଅଚଳ
{
    ଫେରନ୍ତୁ ଏହି->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ଅଚଳ Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ଯଦି (buffer == nullptr) {
        result.truncate(0);
        ଫେରନ୍ତୁ result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ଯଦି (buffer == nullptr) {
            result.truncate(0);
            ଫେରନ୍ତୁ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ଫେରନ୍ତୁ result;
}

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ଅଚଳ Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ଫେରନ୍ତୁ objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ଅଚଳ Locale& objectLocale,
                              ଅଚଳ Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ଫେରନ୍ତୁ objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

ନାମାକାଶ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr ଅକ୍ଷର _kLanguages[]       = "Languages";
constexpr ଅକ୍ଷର _kScripts[]         = "Scripts";
constexpr ଅକ୍ଷର _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr ଅକ୍ଷର _kCountries[]       = "Countries";
constexpr ଅକ୍ଷର _kVariants[]        = "Variants";
constexpr ଅକ୍ଷର k_ଚାବିଗୁଡ଼ିକ[]            = "Keys";
constexpr ଅକ୍ଷର _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr ଅକ୍ଷର _kCurrency[]        = "currency";
constexpr ଅକ୍ଷର _kCurrencies[]      = "Currencies";
constexpr ଅକ୍ଷର _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr ଅକ୍ଷର _kPattern[]         = "pattern";
constexpr ଅକ୍ଷର _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_ଚାବି(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *locale,
                    ଅଚଳ ଅକ୍ଷର *table_ଚାବି, 
                    ଅଚଳ ଅକ୍ଷର* sub_Table_ଚାବି,
                    ଅଚଳ ଅକ୍ଷର *item_ଚାବି,
                    ଅଚଳ ଅକ୍ଷର *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0; }
    ଅଚଳ char16_t *s = nullptr;
    int32_t length = 0;

    ଯଦି(item_ଚାବି==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        ଯଦି(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_ଚାବି(rb.getAlias(), table_ଚାବି, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } ଅନ୍ୟଥା {
        ତାର୍କିକ isLanguageCode = (uprv_strncmp(table_ଚାବି, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        ଯଦି (isLanguageCode && uprv_strtol(item_ଚାବି, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } ଅନ୍ୟଥା {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_ଚାବି,
                                               sub_Table_ଚାବି,
                                               item_ଚାବି,
                                               &length,
                                               &errorCode);
            ଯଦି (U_FAILURE(errorCode) && isLanguageCode && item_ଚାବି != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_ଚାବି = Locale::createCanonical(item_ଚାବି);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_ଚାବି,
                                                    sub_Table_ଚାବି,
                                                    canon_ଚାବି.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    ଯଦି(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        ଯଦି(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } ଅନ୍ୟଥା {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(ଅଚଳ ଅକ୍ଷର *locale,
                            ଅଚଳ ଅକ୍ଷର *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            ଅଚଳ ଅକ୍ଷର *tag,
                            UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0; }
    UErrorCode localStatus;
    ଅଚଳ ଅକ୍ଷର* ମୂଳ = nullptr;

    ଯଦି(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    ଯଦି (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        ଯଦି (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    ମୂଳ = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    ଫେରନ୍ତୁ get_String_Or_Copy_ଚାବି(ମୂଳ, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ଅଚଳ ଅକ୍ଷର *locale,
                        ଅଚଳ ଅକ୍ଷର *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ଅଚଳ ଅକ୍ଷର* locale,
                      ଅଚଳ ଅକ୍ଷର* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    ଯଦି (U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    ଯଦି (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        ଫେରନ୍ତୁ (fallback_res > res) ? fallback_res : res;
    }
    ଯଦି ( err == U_USING_DEFAULT_WARNING ) {
        ଫେରନ୍ତୁ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } ଅନ୍ୟଥା {
        *pErrorCode = err;
        ଫେରନ୍ତୁ res;
    }
}

ସ୍ଥିର int32_t
uloc_getDisplayScriptInContext(ଅଚଳ ଅକ୍ଷର* locale,
                      ଅଚଳ ଅକ୍ଷର* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    ଫେରନ୍ତୁ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ଅଚଳ ଅକ୍ଷର *locale,
                       ଅଚଳ ଅକ୍ଷର *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ଅଚଳ ଅକ୍ଷର *locale,
                       ଅଚଳ ଅକ୍ଷର *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getVariant, _kVariants, *pErrorCode);
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
uloc_getDisplayName(ଅଚଳ ଅକ୍ଷର *locale,
                    ଅଚଳ ଅକ୍ଷର *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    ସ୍ଥିର ଅଚଳ char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ସ୍ଥିର ଅଚଳ char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ସ୍ଥିର ଅଚଳ char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ସ୍ଥିର ଅଚଳ int32_t subLen = 3;
    ସ୍ଥିର ଅଚଳ char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ସ୍ଥିର ଅଚଳ int32_t defaultPatLen = 9;
    ସ୍ଥିର ଅଚଳ int32_t defaultSub0Pos = 0;
    ସ୍ଥିର ଅଚଳ int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ଅଚଳ char16_t *separator;
    int32_t sepLen = 0;
    ଅଚଳ char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = ସତ୍ୟ; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = ସତ୍ୟ; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = ମିଥ୍ୟା; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_ଚାବି_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_ଚାବି_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_ଚାବି_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    ଯଦି(sepLen == 0) {
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
        ଯଦି (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        separator = (ଅଚଳ char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    ଯଦି(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } ଅନ୍ୟଥା { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        ଯଦି (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        ଯଦି (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        ଯଦି (u_strchr(pattern, 0xFF08) != nullptr) {
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
    କର {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        ଯଦି(sub0Pos) {
            ଯଦି(destCapacity >= sub0Pos) {
                ଯେତେବେଳେ (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } ଅନ୍ୟଥା {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } ଅନ୍ୟଥା {
            length=0;
        }

        ପାଇଁ(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = ମିଥ୍ୟା; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            ଯଦି (cap <= 0) {
                cap=0;
            } ଅନ୍ୟଥା {
                p=dest+length;
            }

            ଯଦି (subi == langi) { /* {0}*/
                ଯଦି(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=ସତ୍ୟ;
            } ଅନ୍ୟଥା { /* {1} */
                ଯଦି(!haveRest) {
                    subdone=ସତ୍ୟ;
                } ଅନ୍ୟଥା {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    ବାଛ(resti++) {
                        ସ୍ଥିତି 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
                            ଅଚଳ ଅକ୍ଷର* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            ଯଦି (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=ସତ୍ୟ;
                            } ଅନ୍ୟଥା {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                ଯଦି(len) {
                                    ଯଦି(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    ଯଦି(cap <= 0) {
                                        cap=0;
                                    } ଅନ୍ୟଥା {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                ଯଦି(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_ମୂଲ୍ୟ(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                ଯଦି(len) {
                                    ଯଦି(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    ଯଦି(cap <= 0) {
                                        cap=0;
                                    } ଅନ୍ୟଥା {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ଭାଙ୍ଗ;
                    } /* end switch */

                    ଯଦି (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        ଯଦି(len+sepLen<=cap) {
                            ଅଚଳ char16_t * plimit = p + len;
                            ପାଇଁ (; p < plimit; p++) {
                                ଯଦି (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } ଅନ୍ୟଥା ଯଦି (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            ପାଇଁ(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } ଅନ୍ୟଥା ଯଦି(subdone) {
                        /* remove separator if we added it */
                        ଯଦି (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            ଯଦି(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            ଯଦି(subdone) {
                ଯଦି(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    ଯଦି(length+padLen <= destCapacity) {
                        p=dest+length;
                        ପାଇଁ(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } ଅନ୍ୟଥା {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } ଅନ୍ୟଥା ଯଦି(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } ଅନ୍ୟଥା ଯଦି(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    ଯଦି(dest && sub0Pos!=0) {
                        ଯଦି (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } ଅନ୍ୟଥା {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=ସତ୍ୟ;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } ଯେତେବେଳେ(retry);

    ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ଅଚଳ ଅକ୍ଷର* keyword,
                       ଅଚଳ ଅକ୍ଷର* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    ଯଦି(status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    ଫେରନ୍ତୁ get_String_Or_Copy_ଚାବି(U_ICUDATA_LANG, displayLocale,
                               k_ଚାବିଗୁଡ଼ିକ, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_ମୂଲ୍ୟ(   ଅଚଳ ଅକ୍ଷର* locale,
                               ଅଚଳ ଅକ୍ଷର* keyword,
                               ଅଚଳ ଅକ୍ଷର* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    ଯଦି(status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* get the keyword value */
    CharString keyword_ମୂଲ୍ୟ;
    ଯଦି (keyword != nullptr && *keyword != '\0') {
        keyword_ମୂଲ୍ୟ = ulocimp_get_Keyword_ମୂଲ୍ୟ(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    ଯଦି(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ଅଚଳ char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_ଚାବି(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_ଚାବି_With_Fallback(currencies.getAlias(), keyword_ମୂଲ୍ୟ.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        ଯଦି(U_FAILURE(*status)){
            ଯଦି(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }ଅନ୍ୟଥା{
                ଫେରନ୍ତୁ 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        ଯଦି(dispName != nullptr){
            ଯଦି(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }ଅନ୍ୟଥା{
                *status = U_BUFFER_OVERFLOW_ERROR;
                ଫେରନ୍ତୁ dispNameLen;
            }
        }ଅନ୍ୟଥା{
            /* we have not found the display name for the value .. just copy over */
            ଯଦି(keyword_ମୂଲ୍ୟ.length() <= destCapacity){
                u_charsToUChars(keyword_ମୂଲ୍ୟ.data(), dest, keyword_ମୂଲ୍ୟ.length());
                ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, keyword_ମୂଲ୍ୟ.length(), status);
            }ଅନ୍ୟଥା{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                ଫେରନ୍ତୁ keyword_ମୂଲ୍ୟ.length();
            }
        }

        
    }ଅନ୍ୟଥା{

        ଫେରନ୍ତୁ get_String_Or_Copy_ଚାବି(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_ମୂଲ୍ୟ.data(),
                                   keyword_ମୂଲ୍ୟ.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
