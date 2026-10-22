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

#അന്തര്ഭാവ <string_view>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/uenum.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "ureslocs.h"
#അന്തര്ഭാവ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) സ്ഥിരം
{
    തിരികെ ഇത്->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(സ്ഥിരം Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    എങ്കിൽ (buffer == nullptr) {
        result.truncate(0);
        തിരികെ result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        എങ്കിൽ (buffer == nullptr) {
            result.truncate(0);
            തിരികെ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    തിരികെ result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) സ്ഥിരം
{
    തിരികെ ഇത്->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(സ്ഥിരം Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    എങ്കിൽ (buffer == nullptr) {
        result.truncate(0);
        തിരികെ result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        എങ്കിൽ (buffer == nullptr) {
            result.truncate(0);
            തിരികെ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    തിരികെ result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) സ്ഥിരം
{
    തിരികെ ഇത്->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(സ്ഥിരം Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    എങ്കിൽ (buffer == nullptr) {
        result.truncate(0);
        തിരികെ result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        എങ്കിൽ (buffer == nullptr) {
            result.truncate(0);
            തിരികെ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    തിരികെ result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) സ്ഥിരം
{
    തിരികെ ഇത്->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(സ്ഥിരം Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    എങ്കിൽ (buffer == nullptr) {
        result.truncate(0);
        തിരികെ result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        എങ്കിൽ (buffer == nullptr) {
            result.truncate(0);
            തിരികെ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    തിരികെ result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) സ്ഥിരം
{
    തിരികെ ഇത്->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(സ്ഥിരം Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    എങ്കിൽ (buffer == nullptr) {
        result.truncate(0);
        തിരികെ result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        എങ്കിൽ (buffer == nullptr) {
            result.truncate(0);
            തിരികെ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    തിരികെ result;
}

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(സ്ഥിരം Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    തിരികെ objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(സ്ഥിരം Locale& objectLocale,
                              സ്ഥിരം Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    തിരികെ objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

നാമാകാശ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr അക്ഷരം _kLanguages[]       = "Languages";
constexpr അക്ഷരം _kScripts[]         = "Scripts";
constexpr അക്ഷരം _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr അക്ഷരം _kCountries[]       = "Countries";
constexpr അക്ഷരം _kVariants[]        = "Variants";
constexpr അക്ഷരം k_താക്കോലുകൾ[]            = "Keys";
constexpr അക്ഷരം _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr അക്ഷരം _kCurrency[]        = "currency";
constexpr അക്ഷരം _kCurrencies[]      = "Currencies";
constexpr അക്ഷരം _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr അക്ഷരം _kPattern[]         = "pattern";
constexpr അക്ഷരം _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_താക്കോൽ(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *locale,
                    സ്ഥിരം അക്ഷരം *table_താക്കോൽ, 
                    സ്ഥിരം അക്ഷരം* sub_Table_താക്കോൽ,
                    സ്ഥിരം അക്ഷരം *item_താക്കോൽ,
                    സ്ഥിരം അക്ഷരം *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0; }
    സ്ഥിരം char16_t *s = nullptr;
    int32_t length = 0;

    എങ്കിൽ(item_താക്കോൽ==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        എങ്കിൽ(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_താക്കോൽ(rb.getAlias(), table_താക്കോൽ, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } അല്ലെങ്കിൽ {
        തർക്കം isLanguageCode = (uprv_strncmp(table_താക്കോൽ, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        എങ്കിൽ (isLanguageCode && uprv_strtol(item_താക്കോൽ, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } അല്ലെങ്കിൽ {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_താക്കോൽ,
                                               sub_Table_താക്കോൽ,
                                               item_താക്കോൽ,
                                               &length,
                                               &errorCode);
            എങ്കിൽ (U_FAILURE(errorCode) && isLanguageCode && item_താക്കോൽ != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_താക്കോൽ = Locale::createCanonical(item_താക്കോൽ);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_താക്കോൽ,
                                                    sub_Table_താക്കോൽ,
                                                    canon_താക്കോൽ.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    എങ്കിൽ(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        എങ്കിൽ(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } അല്ലെങ്കിൽ {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    തിരികെ u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(സ്ഥിരം അക്ഷരം *locale,
                            സ്ഥിരം അക്ഷരം *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            സ്ഥിരം അക്ഷരം *tag,
                            UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0; }
    UErrorCode localStatus;
    സ്ഥിരം അക്ഷരം* വേര് = nullptr;

    എങ്കിൽ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    എങ്കിൽ (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    എങ്കിൽ (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        എങ്കിൽ (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } അല്ലെങ്കിൽ {
            തിരികെ u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    വേര് = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    തിരികെ get_String_Or_Copy_താക്കോൽ(വേര്, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(സ്ഥിരം അക്ഷരം *locale,
                        സ്ഥിരം അക്ഷരം *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    തിരികെ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(സ്ഥിരം അക്ഷരം* locale,
                      സ്ഥിരം അക്ഷരം* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    എങ്കിൽ (U_FAILURE(*pErrorCode)) { തിരികെ 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    എങ്കിൽ (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        തിരികെ (fallback_res > res) ? fallback_res : res;
    }
    എങ്കിൽ ( err == U_USING_DEFAULT_WARNING ) {
        തിരികെ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } അല്ലെങ്കിൽ {
        *pErrorCode = err;
        തിരികെ res;
    }
}

സ്ഥിതം int32_t
uloc_getDisplayScriptInContext(സ്ഥിരം അക്ഷരം* locale,
                      സ്ഥിരം അക്ഷരം* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    തിരികെ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(സ്ഥിരം അക്ഷരം *locale,
                       സ്ഥിരം അക്ഷരം *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    തിരികെ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(സ്ഥിരം അക്ഷരം *locale,
                       സ്ഥിരം അക്ഷരം *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    തിരികെ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(സ്ഥിരം അക്ഷരം *locale,
                    സ്ഥിരം അക്ഷരം *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    സ്ഥിതം സ്ഥിരം char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    സ്ഥിതം സ്ഥിരം char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    സ്ഥിതം സ്ഥിരം char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    സ്ഥിതം സ്ഥിരം int32_t subLen = 3;
    സ്ഥിതം സ്ഥിരം char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    സ്ഥിതം സ്ഥിരം int32_t defaultPatLen = 9;
    സ്ഥിതം സ്ഥിരം int32_t defaultSub0Pos = 0;
    സ്ഥിതം സ്ഥിരം int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    സ്ഥിരം char16_t *separator;
    int32_t sepLen = 0;
    സ്ഥിരം char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = സത്യം; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = സത്യം; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = അസത്യം; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    എങ്കിൽ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_താക്കോൽ_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_താക്കോൽ_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_താക്കോൽ_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    എങ്കിൽ(sepLen == 0) {
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
        എങ്കിൽ (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ 0;
        }
        separator = (സ്ഥിരം char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    എങ്കിൽ(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } അല്ലെങ്കിൽ { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        എങ്കിൽ (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        എങ്കിൽ (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        എങ്കിൽ (u_strchr(pattern, 0xFF08) != nullptr) {
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
    ചെയ്യുക {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        എങ്കിൽ(sub0Pos) {
            എങ്കിൽ(destCapacity >= sub0Pos) {
                ആകുമ്പോൾ (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } അല്ലെങ്കിൽ {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } അല്ലെങ്കിൽ {
            length=0;
        }

        വേണ്ടി(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = അസത്യം; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            എങ്കിൽ (cap <= 0) {
                cap=0;
            } അല്ലെങ്കിൽ {
                p=dest+length;
            }

            എങ്കിൽ (subi == langi) { /* {0}*/
                എങ്കിൽ(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=സത്യം;
            } അല്ലെങ്കിൽ { /* {1} */
                എങ്കിൽ(!haveRest) {
                    subdone=സത്യം;
                } അല്ലെങ്കിൽ {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    തിരഞ്ഞെടുപ്പ്(resti++) {
                        അവസ്ഥ 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            നിർത്തുക;
                        അവസ്ഥ 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            നിർത്തുക;
                        അവസ്ഥ 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            നിർത്തുക;
                        അവസ്ഥ 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        സ്ഥിരസ്ഥിതി: {
                            സ്ഥിരം അക്ഷരം* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            എങ്കിൽ (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=സത്യം;
                            } അല്ലെങ്കിൽ {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                എങ്കിൽ(len) {
                                    എങ്കിൽ(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    എങ്കിൽ(cap <= 0) {
                                        cap=0;
                                    } അല്ലെങ്കിൽ {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                എങ്കിൽ(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_മൂല്യം(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                എങ്കിൽ(len) {
                                    എങ്കിൽ(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    എങ്കിൽ(cap <= 0) {
                                        cap=0;
                                    } അല്ലെങ്കിൽ {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } നിർത്തുക;
                    } /* end switch */

                    എങ്കിൽ (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        എങ്കിൽ(len+sepLen<=cap) {
                            സ്ഥിരം char16_t * plimit = p + len;
                            വേണ്ടി (; p < plimit; p++) {
                                എങ്കിൽ (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } അല്ലെങ്കിൽ എങ്കിൽ (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            വേണ്ടി(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } അല്ലെങ്കിൽ എങ്കിൽ(subdone) {
                        /* remove separator if we added it */
                        എങ്കിൽ (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            എങ്കിൽ(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            എങ്കിൽ(subdone) {
                എങ്കിൽ(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    എങ്കിൽ(length+padLen <= destCapacity) {
                        p=dest+length;
                        വേണ്ടി(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } അല്ലെങ്കിൽ {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } അല്ലെങ്കിൽ എങ്കിൽ(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } അല്ലെങ്കിൽ എങ്കിൽ(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    എങ്കിൽ(dest && sub0Pos!=0) {
                        എങ്കിൽ (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } അല്ലെങ്കിൽ {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=സത്യം;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } ആകുമ്പോൾ(retry);

    തിരികെ u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(സ്ഥിരം അക്ഷരം* keyword,
                       സ്ഥിരം അക്ഷരം* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    എങ്കിൽ(status==nullptr || U_FAILURE(*status)) {
        തിരികെ 0;
    }

    എങ്കിൽ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    തിരികെ get_String_Or_Copy_താക്കോൽ(U_ICUDATA_LANG, displayLocale,
                               k_താക്കോലുകൾ, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_മൂല്യം(   സ്ഥിരം അക്ഷരം* locale,
                               സ്ഥിരം അക്ഷരം* keyword,
                               സ്ഥിരം അക്ഷരം* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    എങ്കിൽ(status==nullptr || U_FAILURE(*status)) {
        തിരികെ 0;
    }

    എങ്കിൽ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* get the keyword value */
    CharString keyword_മൂല്യം;
    എങ്കിൽ (keyword != nullptr && *keyword != '\0') {
        keyword_മൂല്യം = ulocimp_get_Keyword_മൂല്യം(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    എങ്കിൽ(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        സ്ഥിരം char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_താക്കോൽ(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_താക്കോൽ_With_Fallback(currencies.getAlias(), keyword_മൂല്യം.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        എങ്കിൽ(U_FAILURE(*status)){
            എങ്കിൽ(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }അല്ലെങ്കിൽ{
                തിരികെ 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        എങ്കിൽ(dispName != nullptr){
            എങ്കിൽ(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                തിരികെ u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }അല്ലെങ്കിൽ{
                *status = U_BUFFER_OVERFLOW_ERROR;
                തിരികെ dispNameLen;
            }
        }അല്ലെങ്കിൽ{
            /* we have not found the display name for the value .. just copy over */
            എങ്കിൽ(keyword_മൂല്യം.length() <= destCapacity){
                u_charsToUChars(keyword_മൂല്യം.data(), dest, keyword_മൂല്യം.length());
                തിരികെ u_terminateUChars(dest, destCapacity, keyword_മൂല്യം.length(), status);
            }അല്ലെങ്കിൽ{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                തിരികെ keyword_മൂല്യം.length();
            }
        }

        
    }അല്ലെങ്കിൽ{

        തിരികെ get_String_Or_Copy_താക്കോൽ(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_മൂല്യം.data(),
                                   keyword_മൂല്യം.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
