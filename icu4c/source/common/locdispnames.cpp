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
                           UnicodeString& result U_LIFETIME_BOUND) ثابت {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    𐡄𐡍 (buffer == nullptr) {
        result.truncate(0);
        أرجع result;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    𐡄𐡍(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        𐡄𐡍 (buffer == nullptr) {
            result.truncate(0);
            أرجع result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       buffer, result.getCapacity(),
                                       &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    أرجع result;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ثابت Locale& displayLocale,
                         UnicodeString& result U_LIFETIME_BOUND) ثابت {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    𐡄𐡍 (buffer == nullptr) {
        result.truncate(0);
        أرجع result;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    𐡄𐡍(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        𐡄𐡍 (buffer == nullptr) {
            result.truncate(0);
            أرجع result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    أرجع result;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ثابت Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    𐡄𐡍 (buffer == nullptr) {
        result.truncate(0);
        أرجع result;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    𐡄𐡍(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        𐡄𐡍 (buffer == nullptr) {
            result.truncate(0);
            أرجع result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    أرجع result;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ثابت Locale& displayLocale,
                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    𐡄𐡍 (buffer == nullptr) {
        result.truncate(0);
        أرجع result;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  buffer, result.getCapacity(),
                                  &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    𐡄𐡍(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        𐡄𐡍 (buffer == nullptr) {
            result.truncate(0);
            أرجع result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      buffer, result.getCapacity(),
                                      &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    أرجع result;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ثابت
{
    أرجع هذا->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ثابت Locale& displayLocale,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت {
    char16_t *buffer;
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length;

    buffer=result.getBuffer(ULOC_FULLNAME_CAPACITY);
    𐡄𐡍 (buffer == nullptr) {
        result.truncate(0);
        أرجع result;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               buffer, result.getCapacity(),
                               &errorCode);
    result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);

    𐡄𐡍(errorCode==U_BUFFER_OVERFLOW_ERROR) {
        buffer=result.getBuffer(length);
        𐡄𐡍 (buffer == nullptr) {
            result.truncate(0);
            أرجع result;
        }
        errorCode=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   buffer, result.getCapacity(),
                                   &errorCode);
        result.releaseBuffer(U_SUCCESS(errorCode) ? length : 0);
    }

    أرجع result;
}

#𐡄𐡍 !UCONFIG_NO_BREAK_ITERATION

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

#endif


U_NAMESPACE_END

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
constexpr حرف _kKeys[]            = "Keys";
constexpr حرف _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr حرف _kCurrency[]        = "currency";
constexpr حرف _kCurrencies[]      = "Currencies";
constexpr حرف _kLocaleDisplayPattern[] = "localeDisplayPattern";
constexpr حرف _kPattern[]         = "pattern";
constexpr حرف _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
_getStringOrCopyKey(ثابت حرف *path, ثابت حرف *locale,
                    ثابت حرف *tableKey, 
                    ثابت حرف* subTableKey,
                    ثابت حرف *itemKey,
                    ثابت حرف *substitute,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع 0; }
    ثابت char16_t *s = nullptr;
    int32_t length = 0;

    𐡄𐡍(itemKey==nullptr) {
        /* top-level item: normal resource bundle access */
        icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

        𐡄𐡍(U_SUCCESS(errorCode)) {
            s=ures_getStringByKey(rb.getAlias(), tableKey, &length, &errorCode);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } وإلا {
        منطقي isLanguageCode = (uprv_strncmp(tableKey, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        𐡄𐡍 (isLanguageCode && uprv_strtol(itemKey, nullptr, 10)) {
            errorCode = U_MISSING_RESOURCE_ERROR;
        } وإلا {
            /* second-level item, use special fallback */
            s=uloc_getTableStringWithFallback(path, locale,
                                               tableKey,
                                               subTableKey,
                                               itemKey,
                                               &length,
                                               &errorCode);
            𐡄𐡍 (U_FAILURE(errorCode) && isLanguageCode && itemKey != nullptr) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                errorCode = U_ZERO_ERROR;
                Locale canonKey = Locale::createCanonical(itemKey);
                s=uloc_getTableStringWithFallback(path, locale,
                                                    tableKey,
                                                    subTableKey,
                                                    canonKey.getName(),
                                                    &length,
                                                    &errorCode);
            }
        }
    }

    𐡄𐡍(U_SUCCESS(errorCode)) {
        int32_t copyLength=uprv_min(length, destCapacity);
        𐡄𐡍(copyLength>0 && s != nullptr) {
            u_memcpy(dest, s, copyLength);
        }
    } وإلا {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_charsToUChars(substitute, dest, uprv_min(length, destCapacity));
        errorCode = U_USING_DEFAULT_WARNING;
    }

    أرجع u_terminateUChars(dest, destCapacity, length, &errorCode);
}

using UDisplayNameGetter = icu::CharString(std::string_view, UErrorCode&);

int32_t
_getDisplayNameForComponent(ثابت حرف *locale,
                            ثابت حرف *displayLocale,
                            char16_t *dest, int32_t destCapacity,
                            UDisplayNameGetter *getter,
                            ثابت حرف *tag,
                            UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع 0; }
    UErrorCode localStatus;
    ثابت حرف* root = nullptr;

    𐡄𐡍(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐡄𐡍 (locale == nullptr) {
        locale = uloc_getDefault();
    }

    localStatus = U_ZERO_ERROR;
    icu::CharString localeBuffer = (*getter)(locale, localStatus);
    𐡄𐡍 (U_FAILURE(localStatus)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    𐡄𐡍 (localeBuffer.isEmpty()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        𐡄𐡍 (getter == ulocimp_getLanguage) {
            localeBuffer.append("und", errorCode);
        } وإلا {
            أرجع u_terminateUChars(dest, destCapacity, 0, &errorCode);
        }
    }

    root = tag == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    أرجع _getStringOrCopyKey(root, displayLocale,
                               tag, nullptr, localeBuffer.data(),
                               localeBuffer.data(),
                               dest, destCapacity,
                               errorCode);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ثابت حرف *locale,
                        ثابت حرف *displayLocale,
                        char16_t *dest, int32_t destCapacity,
                        UErrorCode *pErrorCode) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getLanguage, _kLanguages, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    𐡄𐡍 (U_FAILURE(*pErrorCode)) { أرجع 0; }
    UErrorCode err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getScript, _kScriptsStandAlone, err);

    𐡄𐡍 (destCapacity == 0 && err == U_BUFFER_OVERFLOW_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                                           ulocimp_getScript, _kScripts, *pErrorCode);
        أرجع (fallback_res > res) ? fallback_res : res;
    }
    𐡄𐡍 ( err == U_USING_DEFAULT_WARNING ) {
        أرجع _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                                           ulocimp_getScript, _kScripts, *pErrorCode);
    } وإلا {
        *pErrorCode = err;
        أرجع res;
    }
}

ساكن int32_t
uloc_getDisplayScriptInContext(ثابت حرف* locale,
                      ثابت حرف* displayLocale,
                      char16_t *dest, int32_t destCapacity,
                      UErrorCode *pErrorCode)
{
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                    ulocimp_getScript, _kScripts, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ثابت حرف *locale,
                       ثابت حرف *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
                ulocimp_getRegion, _kCountries, *pErrorCode);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ثابت حرف *locale,
                       ثابت حرف *displayLocale,
                       char16_t *dest, int32_t destCapacity,
                       UErrorCode *pErrorCode) {
    أرجع _getDisplayNameForComponent(locale, displayLocale, dest, destCapacity,
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
uloc_getDisplayName(ثابت حرف *locale,
                    ثابت حرف *displayLocale,
                    char16_t *dest, int32_t destCapacity,
                    UErrorCode *pErrorCode)
{
    ساكن ثابت char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ساكن ثابت char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ساكن ثابت char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ساكن ثابت int32_t subLen = 3;
    ساكن ثابت char16_t defaultPattern[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ساكن ثابت int32_t defaultPatLen = 9;
    ساكن ثابت int32_t defaultSub0Pos = 0;
    ساكن ثابت int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ثابت char16_t *separator;
    int32_t sepLen = 0;
    ثابت char16_t *pattern;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = 𐡒𐡔𐡈; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = 𐡒𐡔𐡈; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = خطأ; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    𐡄𐡍(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    𐡄𐡍(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    {
        UErrorCode status = U_ZERO_ERROR;

        icu::LocalUResourceBundlePointer locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &status));
        icu::LocalUResourceBundlePointer dspbundle(
                ures_getByKeyWithFallback(locbundle.getAlias(), _kLocaleDisplayPattern, nullptr, &status));

        separator=ures_getStringByKeyWithFallback(dspbundle.getAlias(), _kSeparator, &sepLen, &status);
        pattern=ures_getStringByKeyWithFallback(dspbundle.getAlias(), _kPattern, &patLen, &status);
    }

    /* If we couldn't find any data, then use the defaults */
    𐡄𐡍(sepLen == 0) {
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
        𐡄𐡍 (p0==nullptr || p1==nullptr || p1<p0) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        separator = (ثابت char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    𐡄𐡍(patLen==0 || (patLen==defaultPatLen && !u_strncmp(pattern, defaultPattern, patLen))) {
        pattern=defaultPattern;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } وإلا { /* non-default pattern */
        char16_t *p0=u_strstr(pattern, sub0);
        char16_t *p1=u_strstr(pattern, sub1);
        𐡄𐡍 (p0==nullptr || p1==nullptr) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        sub0Pos = static_cast<int32_t>(p0-pattern);
        sub1Pos = static_cast<int32_t>(p1-pattern);
        𐡄𐡍 (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        𐡄𐡍 (u_strchr(pattern, 0xFF08) != nullptr) {
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
    𐡏𐡁𐡃 {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::LocalUEnumerationPointer kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        𐡄𐡍(sub0Pos) {
            𐡄𐡍(destCapacity >= sub0Pos) {
                طالما (patPos < sub0Pos) {
                    *p++ = pattern[patPos++];
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
            int32_t cap=destCapacity-length;
            𐡄𐡍 (cap <= 0) {
                cap=0;
            } وإلا {
                p=dest+length;
            }

            𐡄𐡍 (subi == langi) { /* {0}*/
                𐡄𐡍(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, pErrorCode);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=𐡒𐡔𐡈;
            } وإلا { /* {1} */
                𐡄𐡍(!haveRest) {
                    subdone=𐡒𐡔𐡈;
                } وإلا {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    بدل(resti++) {
                        حالة 0:
                            restPos=length;
                            len=uloc_getDisplayScriptInContext(locale, displayLocale, p, cap, pErrorCode);
                            اكسر;
                        حالة 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, pErrorCode);
                            اكسر;
                        حالة 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, pErrorCode);
                            اكسر;
                        حالة 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, pErrorCode));
                            U_FALLTHROUGH;
                        افتراضي: {
                            ثابت حرف* kw=uenum_next(kenum.getAlias(), &len, pErrorCode);
                            𐡄𐡍 (kw == nullptr) {
                                len=0; /* mark that we didn't add a component */
                                subdone=𐡒𐡔𐡈;
                            } وإلا {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, pErrorCode);
                                𐡄𐡍(len) {
                                    𐡄𐡍(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    𐡄𐡍(cap <= 0) {
                                        cap=0;
                                    } وإلا {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                𐡄𐡍(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                                    *pErrorCode=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_getDisplayKeywordValue(locale, kw, displayLocale,
                                                                           p, cap, pErrorCode);
                                𐡄𐡍(len) {
                                    𐡄𐡍(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=destCapacity-length;
                                    𐡄𐡍(cap <= 0) {
                                        cap=0;
                                    } وإلا {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } اكسر;
                    } /* end switch */

                    𐡄𐡍 (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        𐡄𐡍(len+sepLen<=cap) {
                            ثابت char16_t * plimit = p + len;
                            لكل (; p < plimit; p++) {
                                𐡄𐡍 (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } وإلا 𐡄𐡍 (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            لكل(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } وإلا 𐡄𐡍(subdone) {
                        /* remove separator if we added it */
                        𐡄𐡍 (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            𐡄𐡍(*pErrorCode == U_BUFFER_OVERFLOW_ERROR) {
                *pErrorCode=U_ZERO_ERROR;
            }

            𐡄𐡍(subdone) {
                𐡄𐡍(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    𐡄𐡍(length+padLen <= destCapacity) {
                        p=dest+length;
                        لكل(int32_t i=0;i<padLen;++i) {
                            *p++=pattern[patPos++];
                        }
                    } وإلا {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } وإلا 𐡄𐡍(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } وإلا 𐡄𐡍(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    𐡄𐡍(dest && sub0Pos!=0) {
                        𐡄𐡍 (sub0Pos+length<=destCapacity) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } وإلا {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=𐡒𐡔𐡈;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } طالما(retry);

    أرجع u_terminateUChars(dest, destCapacity, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ثابت حرف* keyword,
                       ثابت حرف* displayLocale,
                       char16_t* dest,
                       int32_t destCapacity,
                       UErrorCode* status){

    /* argument checking */
    𐡄𐡍(status==nullptr || U_FAILURE(*status)) {
        أرجع 0;
    }

    𐡄𐡍(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    أرجع _getStringOrCopyKey(U_ICUDATA_LANG, displayLocale,
                               _kKeys, nullptr,
                               keyword, 
                               keyword,      
                               dest, destCapacity,
                               *status);

}


#define UCURRENCY_DISPLAY_NAME_INDEX 1

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeywordValue(   ثابت حرف* locale,
                               ثابت حرف* keyword,
                               ثابت حرف* displayLocale,
                               char16_t* dest,
                               int32_t destCapacity,
                               UErrorCode* status){


    /* argument checking */
    𐡄𐡍(status==nullptr || U_FAILURE(*status)) {
        أرجع 0;
    }

    𐡄𐡍(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* get the keyword value */
    CharString keywordValue;
    𐡄𐡍 (keyword != nullptr && *keyword != '\0') {
        keywordValue = ulocimp_getKeywordValue(locale, keyword, *status);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    𐡄𐡍(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ثابت char16_t *dispName = nullptr;

        icu::LocalUResourceBundlePointer bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, status));
        icu::LocalUResourceBundlePointer currencies(
                ures_getByKey(bundle.getAlias(), _kCurrencies, nullptr, status));
        icu::LocalUResourceBundlePointer currency(
                ures_getByKeyWithFallback(currencies.getAlias(), keywordValue.data(), nullptr, status));

        dispName = ures_getStringByIndex(currency.getAlias(), UCURRENCY_DISPLAY_NAME_INDEX, &dispNameLen, status);

        𐡄𐡍(U_FAILURE(*status)){
            𐡄𐡍(*status == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *status = U_USING_DEFAULT_WARNING;
            }وإلا{
                أرجع 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        𐡄𐡍(dispName != nullptr){
            𐡄𐡍(dispNameLen <= destCapacity){
                u_memcpy(dest, dispName, dispNameLen);
                أرجع u_terminateUChars(dest, destCapacity, dispNameLen, status);
            }وإلا{
                *status = U_BUFFER_OVERFLOW_ERROR;
                أرجع dispNameLen;
            }
        }وإلا{
            /* we have not found the display name for the value .. just copy over */
            𐡄𐡍(keywordValue.length() <= destCapacity){
                u_charsToUChars(keywordValue.data(), dest, keywordValue.length());
                أرجع u_terminateUChars(dest, destCapacity, keywordValue.length(), status);
            }وإلا{
                 *status = U_BUFFER_OVERFLOW_ERROR;
                أرجع keywordValue.length();
            }
        }

        
    }وإلا{

        أرجع _getStringOrCopyKey(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keywordValue.data(),
                                   keywordValue.data(),
                                   dest, destCapacity,
                                   *status);
    }
}
