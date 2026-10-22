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

#অন্তর্ভাব <string_view>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/uenum.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "ureslocs.h"
#অন্তর্ভাব "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ধ্রুব
{
    প্রত্যাগম এতত্->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ধ্রুব Locale& displayLocale,
                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুব {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    যদি (buffer == nullptr) {
        result.truncate(0);
        প্রত্যাগম result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    যদি(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        যদি (buffer == nullptr) {
            result.truncate(0);
            প্রত্যাগম result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    প্রত্যাগম result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ধ্রুব
{
    প্রত্যাগম এতত্->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ধ্রুব Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) ধ্রুব {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    যদি (buffer == nullptr) {
        result.truncate(0);
        প্রত্যাগম result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    যদি(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        যদি (buffer == nullptr) {
            result.truncate(0);
            প্রত্যাগম result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    প্রত্যাগম result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ধ্রুব
{
    প্রত্যাগম এতত্->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ধ্রুব Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুব {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    যদি (buffer == nullptr) {
        result.truncate(0);
        প্রত্যাগম result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    যদি(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        যদি (buffer == nullptr) {
            result.truncate(0);
            প্রত্যাগম result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    প্রত্যাগম result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ধ্রুব
{
    প্রত্যাগম এতত্->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ধ্রুব Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুব {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    যদি (buffer == nullptr) {
        result.truncate(0);
        প্রত্যাগম result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    যদি(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        যদি (buffer == nullptr) {
            result.truncate(0);
            প্রত্যাগম result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    প্রত্যাগম result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ধ্রুব
{
    প্রত্যাগম এতত্->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ধ্রুব Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) ধ্রুব {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    যদি (buffer == nullptr) {
        result.truncate(0);
        প্রত্যাগম result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    যদি(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        যদি (buffer == nullptr) {
            result.truncate(0);
            প্রত্যাগম result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    প্রত্যাগম result;
}

#যদি !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ধ্রুব Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    প্রত্যাগম objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ধ্রুব Locale& objectLocale,
                              ধ্রুব Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    প্রত্যাগম objectLocale.getDisplayName(displayLocale, name);
}

#endif


U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

নামাকাশ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr বর্ণ _kLanguages[]       = "Languages";
constexpr বর্ণ _kScripts[]         = "Scripts";
constexpr বর্ণ _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr বর্ণ _kCountries[]       = "Countries";
constexpr বর্ণ _kVariants[]        = "Variants";
constexpr বর্ণ k_চাবিগুলি[]            = "Keys";
constexpr বর্ণ _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr বর্ণ _kCurrency[]        = "currency";
constexpr বর্ণ _kCurrencies[]      = "Currencies";
constexpr বর্ণ _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr বর্ণ _kPattern[]         = "pattern";
constexpr বর্ণ _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_চাবি(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *locale,
                    ধ্রুব বর্ণ *table_চাবি, 
                    ধ্রুব বর্ণ* sub_Table_চাবি,
                    ধ্রুব বর্ণ *item_চাবি,
                    ধ্রুব বর্ণ *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0; }
    ধ্রুব char16_t *s = nullptr;
    int32_t length = 0;

    যদি(item_চাবি==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        যদি(U_SUCCESS(errorCode)) {
            s=ures_get_String_By_চাবি(rb.getAlias(), table_চাবি, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } অন্যথা {
        তার্কিক isLanguageCode = (uprv_strncmp(table_চাবি, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        যদি (isLanguageCode && uprv_strtol(item_চাবি, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } অন্যথা {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               table_চাবি,
                                               sub_Table_চাবি,
                                               item_চাবি,
                                               &length,
                                               &errorCode);
            যদি (U_FAILURE(errorCode) && isLanguageCode && item_চাবি != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canon_চাবি = Locale::createCanonical(item_চাবি);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    table_চাবি,
                                                    sub_Table_চাবি,
                                                    canon_চাবি.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    যদি(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        যদি(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } অন্যথা {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    প্রত্যাগম u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(ধ্রুব বর্ণ *locale,
                            ধ্রুব বর্ণ *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            ধ্রুব বর্ণ *tag,
                            UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0; }
    UErrorCode localStatus;
    ধ্রুব বর্ণ* মূল = nullptr;

    যদি(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    যদি (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    যদি (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        যদি (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } অন্যথা {
            প্রত্যাগম u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    মূল = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    প্রত্যাগম get_String_Or_Copy_চাবি(মূল, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ধ্রুব বর্ণ *locale,
                        ধ্রুব বর্ণ *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    প্রত্যাগম _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ধ্রুব বর্ণ* locale,
                      ধ্রুব বর্ণ* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    যদি (U_FAILURE(*pErrorCode)) { প্রত্যাগম 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    যদি (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        প্রত্যাগম (fallback_res > res) ? fallback_res : res;
    }
    যদি ( err == U_USING_DEFAULT_WARNING ) {
        প্রত্যাগম _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } অন্যথা {
        *pErrorCode = err;
        প্রত্যাগম res;
    }
}

স্থির int32_t
uloc_getDisplayScriptInContext(ধ্রুব বর্ণ* locale,
                      ধ্রুব বর্ণ* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    প্রত্যাগম _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ধ্রুব বর্ণ *locale,
                       ধ্রুব বর্ণ *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    প্রত্যাগম _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ধ্রুব বর্ণ *locale,
                       ধ্রুব বর্ণ *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    প্রত্যাগম _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(ধ্রুব বর্ণ *locale,
                    ধ্রুব বর্ণ *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    স্থির ধ্রুব char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    স্থির ধ্রুব char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    স্থির ধ্রুব char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    স্থির ধ্রুব int32_t subLen = 3;
    স্থির ধ্রুব char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    স্থির ধ্রুব int32_t defaultPatLen = 9;
    স্থির ধ্রুব int32_t defaultSub0Pos = 0;
    স্থির ধ্রুব int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ধ্রুব char16_t *separator;
    int32_t sepLen = 0;
    ধ্রুব char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = সত্য; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = সত্য; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = অসত্য; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }

    যদি(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_get_By_চাবি_With_Fallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_get_String_By_চাবি_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_get_String_By_চাবি_With_Fallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    যদি(sepLen == 0) {
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
        যদি (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম 0;
        }
        separator = (ধ্রুব char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    যদি(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } অন্যথা { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        যদি (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        যদি (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        যদি (u_strchr(pattern, 0xFF08) != nullptr) {
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
    কুরু {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        যদি(sub0Pos) {
            যদি(destCapacity >= sub0Pos) {
                যাবত্ (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
                }
            } অন্যথা {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } অন্যথা {
            length=0;
        }

        প্রতি(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = অসত্য; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=destCapacity-length;
            যদি (cap <= 0) {
                cap=0;
            } অন্যথা {
                p=dest+length;
            }

            যদি (subi == langi) { /* {0}*/
                যদি(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=সত্য;
            } অন্যথা { /* {1} */
                যদি(!haveRest) {
                    subdone=সত্য;
                } অন্যথা {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    বিকল্প(resti++) {
                        পক্ষ 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            ভঙ্গ;
                        পক্ষ 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            ভঙ্গ;
                        পক্ষ 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            ভঙ্গ;
                        পক্ষ 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        স্বভাব: {
                            ধ্রুব বর্ণ* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            যদি (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=সত্য;
                            } অন্যথা {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                যদি(len) {
                                    যদি(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    যদি(cap <= 0) {
                                        cap=0;
                                    } অন্যথা {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                যদি(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_মান(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                যদি(len) {
                                    যদি(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    যদি(cap <= 0) {
                                        cap=0;
                                    } অন্যথা {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ভঙ্গ;
                    } /* end switch */

                    যদি (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        যদি(len+sepLen<=cap) {
                            ধ্রুব char16_t * plimit = p + len;
                            প্রতি (; p < plimit; p++) {
                                যদি (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } অন্যথা যদি (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            প্রতি(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } অন্যথা যদি(subdone) {
                        /* remove separator if we added it */
                        যদি (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            যদি(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            যদি(subdone) {
                যদি(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    যদি(length+padLen <= destCapacity) {
                        p=dest+length;
                        প্রতি(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } অন্যথা {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } অন্যথা যদি(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } অন্যথা যদি(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    যদি(dest && sub0Pos!=0) {
                        যদি (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } অন্যথা {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=সত্য;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } যাবত্(retry);

    প্রত্যাগম u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ধ্রুব বর্ণ* keyword,
                       ধ্রুব বর্ণ* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    যদি(status==nullptr || U_FAILURE(*status)) {
        প্রত্যাগম 0;
    }

    যদি(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    প্রত্যাগম get_String_Or_Copy_চাবি(U_ICUDATA_LANG, displayLocale,
                               k_চাবিগুলি, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_মান(   ধ্রুব বর্ণ* locale,
                               ধ্রুব বর্ণ* keyword,
                               ধ্রুব বর্ণ* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    যদি(status==nullptr || U_FAILURE(*status)) {
        প্রত্যাগম 0;
    }

    যদি(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    /* get the keyword value */
    CharString keyword_মান;
    যদি (keyword != nullptr && *keyword != '\0') {
        keyword_মান = ulocimp_get_Keyword_মান(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    যদি(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ধ্রুব char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_get_By_চাবি(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_get_By_চাবি_With_Fallback(currencies.getAlias(), keyword_মান.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        যদি(U_FAILURE(*status)){
            যদি(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }অন্যথা{
                প্রত্যাগম 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        যদি(dispName != nullptr){
            যদি(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                প্রত্যাগম u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }অন্যথা{
                *status = U_BUFFER_OVERFLOW_ERROR;
                প্রত্যাগম dispNameLen;
            }
        }অন্যথা{
            /* we have not found the display name for the value .. just copy over */
            যদি(keyword_মান.length() <= destCapacity){
                u_charsToUChars(keyword_মান.data(), dest, keyword_মান.length());
                প্রত্যাগম u_terminateUChars(dest, destCapacity, keyword_মান.length(), status);
            }অন্যথা{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                প্রত্যাগম keyword_মান.length();
            }
        }

        
    }অন্যথা{

        প্রত্যাগম get_String_Or_Copy_চাবি(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_মান.data(),
                                   keyword_মান.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
