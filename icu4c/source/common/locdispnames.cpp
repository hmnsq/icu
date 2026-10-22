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

#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ureslocs.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ಏತತ್->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ಧ್ರುವ Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ಯದಿ (buffer == nullptr) {
        result.truncate(0);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ಯದಿ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ಯದಿ (buffer == nullptr) {
            result.truncate(0);
            ಪ್ರತ್ಯಾಗಮ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ಏತತ್->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ಧ್ರುವ Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ಯದಿ (buffer == nullptr) {
        result.truncate(0);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ಯದಿ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ಯದಿ (buffer == nullptr) {
            result.truncate(0);
            ಪ್ರತ್ಯಾಗಮ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ಏತತ್->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ಧ್ರುವ Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ಯದಿ (buffer == nullptr) {
        result.truncate(0);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ಯದಿ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ಯದಿ (buffer == nullptr) {
            result.truncate(0);
            ಪ್ರತ್ಯಾಗಮ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ಏತತ್->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ಧ್ರುವ Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ಯದಿ (buffer == nullptr) {
        result.truncate(0);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ಯದಿ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ಯದಿ (buffer == nullptr) {
            result.truncate(0);
            ಪ್ರತ್ಯಾಗಮ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ಏತತ್->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ಧ್ರುವ Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    ಯದಿ (buffer == nullptr) {
        result.truncate(0);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    ಯದಿ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        ಯದಿ (buffer == nullptr) {
            result.truncate(0);
            ಪ್ರತ್ಯಾಗಮ result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ಧ್ರುವ Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ಪ್ರತ್ಯಾಗಮ objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ಧ್ರುವ Locale& objectLocale,
                              ಧ್ರುವ Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ಪ್ರತ್ಯಾಗಮ objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

ನಾಮಾಕಾಶ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr ವರ್ಣ _kLanguages[]       = "Languages";
constexpr ವರ್ಣ _kScripts[]         = "Scripts";
constexpr ವರ್ಣ _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr ವರ್ಣ _kCountries[]       = "Countries";
constexpr ವರ್ಣ _kVariants[]        = "Variants";
constexpr ವರ್ಣ k_ಕೀಲಿಗಳು[]            = "Keys";
constexpr ವರ್ಣ _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr ವರ್ಣ _kCurrency[]        = "currency";
constexpr ವರ್ಣ _kCurrencies[]      = "Currencies";
constexpr ವರ್ಣ _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr ವರ್ಣ _kPattern[]         = "pattern";
constexpr ವರ್ಣ _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *locale,
                    ಧ್ರುವ ವರ್ಣ *table_ಕೀಲಿ, 
                    ಧ್ರುವ ವರ್ಣ* sub_Table_ಕೀಲಿ,
                    ಧ್ರುವ ವರ್ಣ *item_ಕೀಲಿ,
                    ಧ್ರುವ ವರ್ಣ *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಧ್ರುವ char16_t *s = nullptr;
    int32_t length = 0;

    ಯದಿ(item_ಕೀಲಿ==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        ಯದಿ(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_ಕೀಲಿ(rb.getAlias(), table_ಕೀಲಿ, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } ಅನ್ಯಥಾ {
        ತಾರ್ಕಿಕ isLanguageCode = (uprv_strncmp(table_ಕೀಲಿ, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        ಯದಿ (isLanguageCode && uprv_strtol(item_ಕೀಲಿ, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } ಅನ್ಯಥಾ {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_ಕೀಲಿ,
                                               sub_Table_ಕೀಲಿ,
                                               item_ಕೀಲಿ,
                                               &length,
                                               &errorCode);
            ಯದಿ (U_FAILURE(errorCode) && isLanguageCode && item_ಕೀಲಿ != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_ಕೀಲಿ = Locale::createCanonical(item_ಕೀಲಿ);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_ಕೀಲಿ,
                                                    sub_Table_ಕೀಲಿ,
                                                    canon_ಕೀಲಿ.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    ಯದಿ(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        ಯದಿ(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } ಅನ್ಯಥಾ {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(ಧ್ರುವ ವರ್ಣ *locale,
                            ಧ್ರುವ ವರ್ಣ *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            ಧ್ರುವ ವರ್ಣ *tag,
                            UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    UErrorCode localStatus;
    ಧ್ರುವ ವರ್ಣ* ಬೇರು = nullptr;

    ಯದಿ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    ಯದಿ (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        ಯದಿ (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    ಬೇರು = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    ಪ್ರತ್ಯಾಗಮ get_String_Or_Copy_ಕೀಲಿ(ಬೇರು, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ಧ್ರುವ ವರ್ಣ *locale,
                        ಧ್ರುವ ವರ್ಣ *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ಧ್ರುವ ವರ್ಣ* locale,
                      ಧ್ರುವ ವರ್ಣ* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    ಯದಿ (U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    ಯದಿ (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        ಪ್ರತ್ಯಾಗಮ (fallback_res > res) ? fallback_res : res;
    }
    ಯದಿ ( err == U_USING_DEFAULT_WARNING ) {
        ಪ್ರತ್ಯಾಗಮ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } ಅನ್ಯಥಾ {
        *pErrorCode = err;
        ಪ್ರತ್ಯಾಗಮ res;
    }
}

ಸ್ಥಿರ int32_t
uloc_getDisplayScriptInContext(ಧ್ರುವ ವರ್ಣ* locale,
                      ಧ್ರುವ ವರ್ಣ* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    ಪ್ರತ್ಯಾಗಮ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ಧ್ರುವ ವರ್ಣ *locale,
                       ಧ್ರುವ ವರ್ಣ *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ಧ್ರುವ ವರ್ಣ *locale,
                       ಧ್ರುವ ವರ್ಣ *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(ಧ್ರುವ ವರ್ಣ *locale,
                    ಧ್ರುವ ವರ್ಣ *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    ಸ್ಥಿರ ಧ್ರುವ char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ಸ್ಥಿರ ಧ್ರುವ char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ಸ್ಥಿರ ಧ್ರುವ char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ಸ್ಥಿರ ಧ್ರುವ int32_t subLen = 3;
    ಸ್ಥಿರ ಧ್ರುವ char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ಸ್ಥಿರ ಧ್ರುವ int32_t defaultPatLen = 9;
    ಸ್ಥಿರ ಧ್ರುವ int32_t defaultSub0Pos = 0;
    ಸ್ಥಿರ ಧ್ರುವ int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ಧ್ರುವ char16_t *separator;
    int32_t sepLen = 0;
    ಧ್ರುವ char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = ಸತ್ಯ; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = ಸತ್ಯ; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = ಅಸತ್ಯ; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_ಕೀಲಿ_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_ಕೀಲಿ_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_ಕೀಲಿ_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    ಯದಿ(sepLen == 0) {
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
        ಯದಿ (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        separator = (ಧ್ರುವ char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    ಯದಿ(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } ಅನ್ಯಥಾ { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        ಯದಿ (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        ಯದಿ (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        ಯದಿ (u_strchr(pattern, 0xFF08) != nullptr) {
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
    ಕುರು {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        ಯದಿ(sub0Pos) {
            ಯದಿ(destCapacity >= sub0Pos) {
                ಯಾವತ್ (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } ಅನ್ಯಥಾ {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } ಅನ್ಯಥಾ {
            length=0;
        }

        ಪ್ರತಿ(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = ಅಸತ್ಯ; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            ಯದಿ (cap <= 0) {
                cap=0;
            } ಅನ್ಯಥಾ {
                p=dest+length;
            }

            ಯದಿ (subi == langi) { /* {0}*/
                ಯದಿ(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=ಸತ್ಯ;
            } ಅನ್ಯಥಾ { /* {1} */
                ಯದಿ(!haveRest) {
                    subdone=ಸತ್ಯ;
                } ಅನ್ಯಥಾ {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    ವಿಕಲ್ಪ(resti++) {
                        ಪಕ್ಷ 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            ಭಙ್ಗ;
                        ಪಕ್ಷ 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            ಭಙ್ಗ;
                        ಪಕ್ಷ 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            ಭಙ್ಗ;
                        ಪಕ್ಷ 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        ಸ್ವಭಾವ: {
                            ಧ್ರುವ ವರ್ಣ* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            ಯದಿ (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=ಸತ್ಯ;
                            } ಅನ್ಯಥಾ {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                ಯದಿ(len) {
                                    ಯದಿ(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    ಯದಿ(cap <= 0) {
                                        cap=0;
                                    } ಅನ್ಯಥಾ {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                ಯದಿ(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_ಮೌಲ್ಯ(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                ಯದಿ(len) {
                                    ಯದಿ(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    ಯದಿ(cap <= 0) {
                                        cap=0;
                                    } ಅನ್ಯಥಾ {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ಭಙ್ಗ;
                    } /* end switch */

                    ಯದಿ (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        ಯದಿ(len+sepLen<=cap) {
                            ಧ್ರುವ char16_t * plimit = p + len;
                            ಪ್ರತಿ (; p < plimit; p++) {
                                ಯದಿ (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } ಅನ್ಯಥಾ ಯದಿ (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            ಪ್ರತಿ(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } ಅನ್ಯಥಾ ಯದಿ(subdone) {
                        /* remove separator if we added it */
                        ಯದಿ (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            ಯದಿ(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            ಯದಿ(subdone) {
                ಯದಿ(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    ಯದಿ(length+padLen <= destCapacity) {
                        p=dest+length;
                        ಪ್ರತಿ(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } ಅನ್ಯಥಾ {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } ಅನ್ಯಥಾ ಯದಿ(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } ಅನ್ಯಥಾ ಯದಿ(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    ಯದಿ(dest && sub0Pos!=0) {
                        ಯದಿ (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } ಅನ್ಯಥಾ {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=ಸತ್ಯ;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } ಯಾವತ್(retry);

    ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ಧ್ರುವ ವರ್ಣ* keyword,
                       ಧ್ರುವ ವರ್ಣ* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    ಯದಿ(status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    ಪ್ರತ್ಯಾಗಮ get_String_Or_Copy_ಕೀಲಿ(U_ICUDATA_LANG, displayLocale,
                               k_ಕೀಲಿಗಳು, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_ಮೌಲ್ಯ(   ಧ್ರುವ ವರ್ಣ* locale,
                               ಧ್ರುವ ವರ್ಣ* keyword,
                               ಧ್ರುವ ವರ್ಣ* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    ಯದಿ(status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* get the keyword value */
    CharString keyword_ಮೌಲ್ಯ;
    ಯದಿ (keyword != nullptr && *keyword != '\0') {
        keyword_ಮೌಲ್ಯ = ulocimp_get_Keyword_ಮೌಲ್ಯ(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    ಯದಿ(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ಧ್ರುವ char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_ಕೀಲಿ(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_ಕೀಲಿ_With_Fallback(currencies.getAlias(), keyword_ಮೌಲ್ಯ.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        ಯದಿ(U_FAILURE(*status)){
            ಯದಿ(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }ಅನ್ಯಥಾ{
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        ಯದಿ(dispName != nullptr){
            ಯದಿ(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }ಅನ್ಯಥಾ{
                *status = U_BUFFER_OVERFLOW_ERROR;
                ಪ್ರತ್ಯಾಗಮ dispNameLen;
            }
        }ಅನ್ಯಥಾ{
            /* we have not found the display name for the value .. just copy over */
            ಯದಿ(keyword_ಮೌಲ್ಯ.length() <= destCapacity){
                u_charsToUChars(keyword_ಮೌಲ್ಯ.data(), dest, keyword_ಮೌಲ್ಯ.length());
                ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, keyword_ಮೌಲ್ಯ.length(), status);
            }ಅನ್ಯಥಾ{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                ಪ್ರತ್ಯಾಗಮ keyword_ಮೌಲ್ಯ.length();
            }
        }

        
    }ಅನ್ಯಥಾ{

        ಪ್ರತ್ಯಾಗಮ get_String_Or_Copy_ಕೀಲಿ(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_ಮೌಲ್ಯ.data(),
                                   keyword_ಮೌಲ್ಯ.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
