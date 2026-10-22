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

#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uenum.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ureslocs.h"
#अन्तर्भाव "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ध्रुव
{
    प्रत्यागम एतत्->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ध्रुव Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    यदि (buffer == nullptr) {
        result.truncate(0);
        प्रत्यागम result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    यदि(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        यदि (buffer == nullptr) {
            result.truncate(0);
            प्रत्यागम result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    प्रत्यागम result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ध्रुव
{
    प्रत्यागम एतत्->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ध्रुव Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    यदि (buffer == nullptr) {
        result.truncate(0);
        प्रत्यागम result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    यदि(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        यदि (buffer == nullptr) {
            result.truncate(0);
            प्रत्यागम result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    प्रत्यागम result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ध्रुव
{
    प्रत्यागम एतत्->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ध्रुव Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    यदि (buffer == nullptr) {
        result.truncate(0);
        प्रत्यागम result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    यदि(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        यदि (buffer == nullptr) {
            result.truncate(0);
            प्रत्यागम result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    प्रत्यागम result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ध्रुव
{
    प्रत्यागम एतत्->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ध्रुव Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    यदि (buffer == nullptr) {
        result.truncate(0);
        प्रत्यागम result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    यदि(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        यदि (buffer == nullptr) {
            result.truncate(0);
            प्रत्यागम result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    प्रत्यागम result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ध्रुव
{
    प्रत्यागम एतत्->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ध्रुव Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    यदि (buffer == nullptr) {
        result.truncate(0);
        प्रत्यागम result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    यदि(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        यदि (buffer == nullptr) {
            result.truncate(0);
            प्रत्यागम result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    प्रत्यागम result;
}

#यदि !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ध्रुव Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    प्रत्यागम objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ध्रुव Locale& objectLocale,
                              ध्रुव Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    प्रत्यागम objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr वर्ण _kLanguages[]       = "Languages";
constexpr वर्ण _kScripts[]         = "Scripts";
constexpr वर्ण _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr वर्ण _kCountries[]       = "Countries";
constexpr वर्ण _kVariants[]        = "Variants";
constexpr वर्ण k_कुंजियाँ[]            = "Keys";
constexpr वर्ण _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr वर्ण _kCurrency[]        = "currency";
constexpr वर्ण _kCurrencies[]      = "Currencies";
constexpr वर्ण _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr वर्ण _kPattern[]         = "pattern";
constexpr वर्ण _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_कुंजी(ध्रुव वर्ण *path, ध्रुव वर्ण *locale,
                    ध्रुव वर्ण *table_कुंजी, 
                    ध्रुव वर्ण* sub_Table_कुंजी,
                    ध्रुव वर्ण *item_कुंजी,
                    ध्रुव वर्ण *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0; }
    ध्रुव char16_t *s = nullptr;
    int32_t length = 0;

    यदि(item_कुंजी==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        यदि(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_कुंजी(rb.getAlias(), table_कुंजी, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } अन्यथा {
        तार्किक isLanguageCode = (uprv_strncmp(table_कुंजी, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        यदि (isLanguageCode && uprv_strtol(item_कुंजी, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } अन्यथा {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_कुंजी,
                                               sub_Table_कुंजी,
                                               item_कुंजी,
                                               &length,
                                               &errorCode);
            यदि (U_FAILURE(errorCode) && isLanguageCode && item_कुंजी != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_कुंजी = Locale::createCanonical(item_कुंजी);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_कुंजी,
                                                    sub_Table_कुंजी,
                                                    canon_कुंजी.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    यदि(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        यदि(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } अन्यथा {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    प्रत्यागम u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(ध्रुव वर्ण *locale,
                            ध्रुव वर्ण *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            ध्रुव वर्ण *tag,
                            UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0; }
    UErrorCode localStatus;
    ध्रुव वर्ण* मूल = nullptr;

    यदि(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    यदि (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    यदि (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    यदि (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        यदि (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } अन्यथा {
            प्रत्यागम u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    मूल = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    प्रत्यागम get_String_Or_Copy_कुंजी(मूल, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ध्रुव वर्ण *locale,
                        ध्रुव वर्ण *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    प्रत्यागम _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ध्रुव वर्ण* locale,
                      ध्रुव वर्ण* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    यदि (U_FAILURE(*pErrorCode)) { प्रत्यागम 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    यदि (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        प्रत्यागम (fallback_res > res) ? fallback_res : res;
    }
    यदि ( err == U_USING_DEFAULT_WARNING ) {
        प्रत्यागम _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } अन्यथा {
        *pErrorCode = err;
        प्रत्यागम res;
    }
}

स्थिर int32_t
uloc_getDisplayScriptInContext(ध्रुव वर्ण* locale,
                      ध्रुव वर्ण* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    प्रत्यागम _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ध्रुव वर्ण *locale,
                       ध्रुव वर्ण *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    प्रत्यागम _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ध्रुव वर्ण *locale,
                       ध्रुव वर्ण *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    प्रत्यागम _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(ध्रुव वर्ण *locale,
                    ध्रुव वर्ण *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    स्थिर ध्रुव char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    स्थिर ध्रुव char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    स्थिर ध्रुव char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    स्थिर ध्रुव int32_t subLen = 3;
    स्थिर ध्रुव char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    स्थिर ध्रुव int32_t defaultPatLen = 9;
    स्थिर ध्रुव int32_t defaultSub0Pos = 0;
    स्थिर ध्रुव int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ध्रुव char16_t *separator;
    int32_t sepLen = 0;
    ध्रुव char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = सत्य; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = सत्य; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = असत्य; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    यदि(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_कुंजी_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_कुंजी_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_कुंजी_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    यदि(sepLen == 0) {
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
        यदि (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम 0;
        }
        separator = (ध्रुव char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    यदि(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } अन्यथा { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        यदि (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        यदि (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        यदि (u_strchr(pattern, 0xFF08) != nullptr) {
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
    कुरु {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        यदि(sub0Pos) {
            यदि(destCapacity >= sub0Pos) {
                यावत् (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } अन्यथा {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } अन्यथा {
            length=0;
        }

        प्रति(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = असत्य; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            यदि (cap <= 0) {
                cap=0;
            } अन्यथा {
                p=dest+length;
            }

            यदि (subi == langi) { /* {0}*/
                यदि(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=सत्य;
            } अन्यथा { /* {1} */
                यदि(!haveRest) {
                    subdone=सत्य;
                } अन्यथा {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    विकल्प(resti++) {
                        पक्ष 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            भङ्ग;
                        पक्ष 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            भङ्ग;
                        पक्ष 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            भङ्ग;
                        पक्ष 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        स्वभाव: {
                            ध्रुव वर्ण* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            यदि (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=सत्य;
                            } अन्यथा {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                यदि(len) {
                                    यदि(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    यदि(cap <= 0) {
                                        cap=0;
                                    } अन्यथा {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                यदि(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_मूल्य(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                यदि(len) {
                                    यदि(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    यदि(cap <= 0) {
                                        cap=0;
                                    } अन्यथा {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } भङ्ग;
                    } /* end switch */

                    यदि (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        यदि(len+sepLen<=cap) {
                            ध्रुव char16_t * plimit = p + len;
                            प्रति (; p < plimit; p++) {
                                यदि (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } अन्यथा यदि (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            प्रति(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } अन्यथा यदि(subdone) {
                        /* remove separator if we added it */
                        यदि (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            यदि(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            यदि(subdone) {
                यदि(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    यदि(length+padLen <= destCapacity) {
                        p=dest+length;
                        प्रति(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } अन्यथा {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } अन्यथा यदि(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } अन्यथा यदि(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    यदि(dest && sub0Pos!=0) {
                        यदि (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } अन्यथा {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=सत्य;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } यावत्(retry);

    प्रत्यागम u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ध्रुव वर्ण* keyword,
                       ध्रुव वर्ण* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    यदि(status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम 0;
    }

    यदि(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    प्रत्यागम get_String_Or_Copy_कुंजी(U_ICUDATA_LANG, displayLocale,
                               k_कुंजियाँ, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_मूल्य(   ध्रुव वर्ण* locale,
                               ध्रुव वर्ण* keyword,
                               ध्रुव वर्ण* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    यदि(status==nullptr || U_FAILURE(*status)) {
        प्रत्यागम 0;
    }

    यदि(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* get the keyword value */
    CharString keyword_मूल्य;
    यदि (keyword != nullptr && *keyword != '\0') {
        keyword_मूल्य = ulocimp_get_Keyword_मूल्य(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    यदि(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ध्रुव char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_कुंजी(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_कुंजी_With_Fallback(currencies.getAlias(), keyword_मूल्य.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        यदि(U_FAILURE(*status)){
            यदि(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }अन्यथा{
                प्रत्यागम 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        यदि(dispName != nullptr){
            यदि(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                प्रत्यागम u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }अन्यथा{
                *status = U_BUFFER_OVERFLOW_ERROR;
                प्रत्यागम dispNameLen;
            }
        }अन्यथा{
            /* we have not found the display name for the value .. just copy over */
            यदि(keyword_मूल्य.length() <= destCapacity){
                u_charsToUChars(keyword_मूल्य.data(), dest, keyword_मूल्य.length());
                प्रत्यागम u_terminateUChars(dest, destCapacity, keyword_मूल्य.length(), status);
            }अन्यथा{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                प्रत्यागम keyword_मूल्य.length();
            }
        }

        
    }अन्यथा{

        प्रत्यागम get_String_Or_Copy_कुंजी(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_मूल्य.data(),
                                   keyword_मूल्य.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
