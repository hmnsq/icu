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

#அந்தர்பாவ <string_view>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/uenum.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "ureslocs.h"
#அந்தர்பாவ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) மாறிலி
{
    திரும்பு இது->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(மாறிலி Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    எனில் (buffer == nullptr) {
        result.truncate(0);
        திரும்பு result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        எனில் (buffer == nullptr) {
            result.truncate(0);
            திரும்பு result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    திரும்பு result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) மாறிலி
{
    திரும்பு இது->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(மாறிலி Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    எனில் (buffer == nullptr) {
        result.truncate(0);
        திரும்பு result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        எனில் (buffer == nullptr) {
            result.truncate(0);
            திரும்பு result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    திரும்பு result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) மாறிலி
{
    திரும்பு இது->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(மாறிலி Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    எனில் (buffer == nullptr) {
        result.truncate(0);
        திரும்பு result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        எனில் (buffer == nullptr) {
            result.truncate(0);
            திரும்பு result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    திரும்பு result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) மாறிலி
{
    திரும்பு இது->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(மாறிலி Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    எனில் (buffer == nullptr) {
        result.truncate(0);
        திரும்பு result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        எனில் (buffer == nullptr) {
            result.truncate(0);
            திரும்பு result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    திரும்பு result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) மாறிலி
{
    திரும்பு இது->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(மாறிலி Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    எனில் (buffer == nullptr) {
        result.truncate(0);
        திரும்பு result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        எனில் (buffer == nullptr) {
            result.truncate(0);
            திரும்பு result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    திரும்பு result;
}

#எனில் !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(மாறிலி Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    திரும்பு objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(மாறிலி Locale& objectLocale,
                              மாறிலி Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    திரும்பு objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

நாமாகாஶ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr எழுத்து _kLanguages[]       = "Languages";
constexpr எழுத்து _kScripts[]         = "Scripts";
constexpr எழுத்து _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr எழுத்து _kCountries[]       = "Countries";
constexpr எழுத்து _kVariants[]        = "Variants";
constexpr எழுத்து k_சாவிகள்[]            = "Keys";
constexpr எழுத்து _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr எழுத்து _kCurrency[]        = "currency";
constexpr எழுத்து _kCurrencies[]      = "Currencies";
constexpr எழுத்து _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr எழுத்து _kPattern[]         = "pattern";
constexpr எழுத்து _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_சாவி(மாறிலி எழுத்து *path, மாறிலி எழுத்து *locale,
                    மாறிலி எழுத்து *table_சாவி, 
                    மாறிலி எழுத்து* sub_Table_சாவி,
                    மாறிலி எழுத்து *item_சாவி,
                    மாறிலி எழுத்து *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0; }
    மாறிலி char16_t *s = nullptr;
    int32_t length = 0;

    எனில்(item_சாவி==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        எனில்(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_சாவி(rb.getAlias(), table_சாவி, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } இல்லையெனில் {
        தருக்க isLanguageCode = (uprv_strncmp(table_சாவி, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        எனில் (isLanguageCode && uprv_strtol(item_சாவி, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } இல்லையெனில் {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_சாவி,
                                               sub_Table_சாவி,
                                               item_சாவி,
                                               &length,
                                               &errorCode);
            எனில் (U_FAILURE(errorCode) && isLanguageCode && item_சாவி != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_சாவி = Locale::createCanonical(item_சாவி);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_சாவி,
                                                    sub_Table_சாவி,
                                                    canon_சாவி.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    எனில்(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        எனில்(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } இல்லையெனில் {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    திரும்பு u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(மாறிலி எழுத்து *locale,
                            மாறிலி எழுத்து *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            மாறிலி எழுத்து *tag,
                            UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0; }
    UErrorCode localStatus;
    மாறிலி எழுத்து* வேர் = nullptr;

    எனில்(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    எனில் (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    எனில் (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        எனில் (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } இல்லையெனில் {
            திரும்பு u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    வேர் = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    திரும்பு get_String_Or_Copy_சாவி(வேர், displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(மாறிலி எழுத்து *locale,
                        மாறிலி எழுத்து *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    திரும்பு _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(மாறிலி எழுத்து* locale,
                      மாறிலி எழுத்து* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    எனில் (U_FAILURE(*pErrorCode)) { திரும்பு 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    எனில் (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        திரும்பு (fallback_res > res) ? fallback_res : res;
    }
    எனில் ( err == U_USING_DEFAULT_WARNING ) {
        திரும்பு _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } இல்லையெனில் {
        *pErrorCode = err;
        திரும்பு res;
    }
}

நிலையான int32_t
uloc_getDisplayScriptInContext(மாறிலி எழுத்து* locale,
                      மாறிலி எழுத்து* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    திரும்பு _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(மாறிலி எழுத்து *locale,
                       மாறிலி எழுத்து *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    திரும்பு _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(மாறிலி எழுத்து *locale,
                       மாறிலி எழுத்து *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    திரும்பு _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(மாறிலி எழுத்து *locale,
                    மாறிலி எழுத்து *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    நிலையான மாறிலி char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    நிலையான மாறிலி char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    நிலையான மாறிலி char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    நிலையான மாறிலி int32_t subLen = 3;
    நிலையான மாறிலி char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    நிலையான மாறிலி int32_t defaultPatLen = 9;
    நிலையான மாறிலி int32_t defaultSub0Pos = 0;
    நிலையான மாறிலி int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    மாறிலி char16_t *separator;
    int32_t sepLen = 0;
    மாறிலி char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = உண்மை; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = உண்மை; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = பொய்; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    எனில்(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_சாவி_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_சாவி_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_சாவி_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    எனில்(sepLen == 0) {
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
        எனில் (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு 0;
        }
        separator = (மாறிலி char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    எனில்(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } இல்லையெனில் { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        எனில் (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        எனில் (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        எனில் (u_strchr(pattern, 0xFF08) != nullptr) {
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
    செய் {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        எனில்(sub0Pos) {
            எனில்(destCapacity >= sub0Pos) {
                போது (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } இல்லையெனில் {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } இல்லையெனில் {
            length=0;
        }

        க்கு(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = பொய்; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            எனில் (cap <= 0) {
                cap=0;
            } இல்லையெனில் {
                p=dest+length;
            }

            எனில் (subi == langi) { /* {0}*/
                எனில்(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=உண்மை;
            } இல்லையெனில் { /* {1} */
                எனில்(!haveRest) {
                    subdone=உண்மை;
                } இல்லையெனில் {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    தேர்வு(resti++) {
                        நிலை 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            நிறுத்து;
                        நிலை 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            நிறுத்து;
                        நிலை 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            நிறுத்து;
                        நிலை 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        இயல்பு: {
                            மாறிலி எழுத்து* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            எனில் (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=உண்மை;
                            } இல்லையெனில் {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                எனில்(len) {
                                    எனில்(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    எனில்(cap <= 0) {
                                        cap=0;
                                    } இல்லையெனில் {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                எனில்(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_மதிப்பு(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                எனில்(len) {
                                    எனில்(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    எனில்(cap <= 0) {
                                        cap=0;
                                    } இல்லையெனில் {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } நிறுத்து;
                    } /* end switch */

                    எனில் (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        எனில்(len+sepLen<=cap) {
                            மாறிலி char16_t * plimit = p + len;
                            க்கு (; p < plimit; p++) {
                                எனில் (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } இல்லையெனில் எனில் (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            க்கு(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } இல்லையெனில் எனில்(subdone) {
                        /* remove separator if we added it */
                        எனில் (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            எனில்(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            எனில்(subdone) {
                எனில்(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    எனில்(length+padLen <= destCapacity) {
                        p=dest+length;
                        க்கு(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } இல்லையெனில் {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } இல்லையெனில் எனில்(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } இல்லையெனில் எனில்(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    எனில்(dest && sub0Pos!=0) {
                        எனில் (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } இல்லையெனில் {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=உண்மை;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } போது(retry);

    திரும்பு u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(மாறிலி எழுத்து* keyword,
                       மாறிலி எழுத்து* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    எனில்(status==nullptr || U_FAILURE(*status)) {
        திரும்பு 0;
    }

    எனில்(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    திரும்பு get_String_Or_Copy_சாவி(U_ICUDATA_LANG, displayLocale,
                               k_சாவிகள், nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_மதிப்பு(   மாறிலி எழுத்து* locale,
                               மாறிலி எழுத்து* keyword,
                               மாறிலி எழுத்து* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    எனில்(status==nullptr || U_FAILURE(*status)) {
        திரும்பு 0;
    }

    எனில்(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* get the keyword value */
    CharString keyword_மதிப்பு;
    எனில் (keyword != nullptr && *keyword != '\0') {
        keyword_மதிப்பு = ulocimp_get_Keyword_மதிப்பு(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    எனில்(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        மாறிலி char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_சாவி(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_சாவி_With_Fallback(currencies.getAlias(), keyword_மதிப்பு.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        எனில்(U_FAILURE(*status)){
            எனில்(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }இல்லையெனில்{
                திரும்பு 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        எனில்(dispName != nullptr){
            எனில்(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                திரும்பு u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }இல்லையெனில்{
                *status = U_BUFFER_OVERFLOW_ERROR;
                திரும்பு dispNameLen;
            }
        }இல்லையெனில்{
            /* we have not found the display name for the value .. just copy over */
            எனில்(keyword_மதிப்பு.length() <= destCapacity){
                u_charsToUChars(keyword_மதிப்பு.data(), dest, keyword_மதிப்பு.length());
                திரும்பு u_terminateUChars(dest, destCapacity, keyword_மதிப்பு.length(), status);
            }இல்லையெனில்{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                திரும்பு keyword_மதிப்பு.length();
            }
        }

        
    }இல்லையெனில்{

        திரும்பு get_String_Or_Copy_சாவி(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_மதிப்பு.data(),
                                   keyword_மதிப்பு.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
