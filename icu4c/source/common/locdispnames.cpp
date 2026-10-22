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

#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ureslocs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

// C++ API ----------------------------------------------------------------- ***

U_NAMESPACE_BEGIN

UnicodeString&
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ਅਚਲ
{
    ਵਾਪਸ ਇਹ->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ਅਚਲ Locale& displayLocale,
                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    char16_t *ਬਫਰ;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length;

    ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(ULOC_FULLNAME_ਸਮਰੱਥਾ);
    ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.truncate(0);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                   &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);

    ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(length);
        ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਨਤੀਜਾ.truncate(0);
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        error_ਕੋਡ=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                       &error_ਕੋਡ);
        ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ਅਚਲ
{
    ਵਾਪਸ ਇਹ->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ਅਚਲ Locale& displayLocale,
                         UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    char16_t *ਬਫਰ;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length;

    ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(ULOC_FULLNAME_ਸਮਰੱਥਾ);
    ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.truncate(0);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                  &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);

    ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(length);
        ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਨਤੀਜਾ.truncate(0);
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        error_ਕੋਡ=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                      &error_ਕੋਡ);
        ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ਅਚਲ
{
    ਵਾਪਸ ਇਹ->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ਅਚਲ Locale& displayLocale,
                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    char16_t *ਬਫਰ;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length;

    ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(ULOC_FULLNAME_ਸਮਰੱਥਾ);
    ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.truncate(0);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                  &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);

    ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(length);
        ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਨਤੀਜਾ.truncate(0);
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        error_ਕੋਡ=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                      &error_ਕੋਡ);
        ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ਅਚਲ
{
    ਵਾਪਸ ਇਹ->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ਅਚਲ Locale& displayLocale,
                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    char16_t *ਬਫਰ;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length;

    ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(ULOC_FULLNAME_ਸਮਰੱਥਾ);
    ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.truncate(0);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                  &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);

    ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(length);
        ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਨਤੀਜਾ.truncate(0);
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        error_ਕੋਡ=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                      &error_ਕੋਡ);
        ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ਅਚਲ
{
    ਵਾਪਸ ਇਹ->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ਅਚਲ Locale& displayLocale,
                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    char16_t *ਬਫਰ;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    int32_t length;

    ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(ULOC_FULLNAME_ਸਮਰੱਥਾ);
    ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ.truncate(0);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                               &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);

    ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਬਫਰ=ਨਤੀਜਾ.get_ਬਫਰ(length);
        ਜੇ (ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਨਤੀਜਾ.truncate(0);
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        error_ਕੋਡ=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   ਬਫਰ, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(),
                                   &error_ਕੋਡ);
        ਨਤੀਜਾ.release_ਬਫਰ(U_SUCCESS(error_ਕੋਡ) ? length : 0);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ਅਚਲ Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ਵਾਪਸ objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ਅਚਲ Locale& objectLocale,
                              ਅਚਲ Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ਵਾਪਸ objectLocale.getDisplayName(displayLocale, name);
}

#ਜੇ_ਅੰਤ


U_NAMESPACE_ਸਿਰਾ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

ਨਾਮਾਕਾਸ਼ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr ਅੱਖਰ _kLanguages[]       = "Languages";
constexpr ਅੱਖਰ _kScripts[]         = "Scripts";
constexpr ਅੱਖਰ _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr ਅੱਖਰ _kCountries[]       = "Countries";
constexpr ਅੱਖਰ _kVariants[]        = "Variants";
constexpr ਅੱਖਰ k_ਕੁੰਜੀਆਂ[]            = "Keys";
constexpr ਅੱਖਰ _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr ਅੱਖਰ _kCurrency[]        = "currency";
constexpr ਅੱਖਰ _kCurrencies[]      = "Currencies";
constexpr ਅੱਖਰ k_Locale_Display_ਨਮੂਨਾ[] = "localeDisplayPattern";
constexpr ਅੱਖਰ k_ਨਮੂਨਾ[]         = "pattern";
constexpr ਅੱਖਰ _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *locale,
                    ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ_ਕੁੰਜੀ, 
                    ਅਚਲ ਅੱਖਰ* sub_ਸਾਰਣੀ_ਕੁੰਜੀ,
                    ਅਚਲ ਅੱਖਰ *ਵਸਤੂ_ਕੁੰਜੀ,
                    ਅਚਲ ਅੱਖਰ *substitute,
                    char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                    UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }
    ਅਚਲ char16_t *s = ਖਾਲੀ_ਸੂਚਕ;
    int32_t length = 0;

    ਜੇ(ਵਸਤੂ_ਕੁੰਜੀ==ਖਾਲੀ_ਸੂਚਕ) {
        /* top-level item: normal resource bundle access */
        icu::Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(ਰਸਤਾ, locale, &error_ਕੋਡ));

        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            s=ures_get_String_By_ਕੁੰਜੀ(rb.getAlias(), ਸਾਰਣੀ_ਕੁੰਜੀ, &length, &error_ਕੋਡ);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } ਨਹੀਂਤਾਂ {
        ਤਾਰਕਿਕ is_Language_ਕੋਡ = (uprv_strncmp(ਸਾਰਣੀ_ਕੁੰਜੀ, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        ਜੇ (is_Language_ਕੋਡ && uprv_strtol(ਵਸਤੂ_ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, 10)) {
            error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
        } ਨਹੀਂਤਾਂ {
            /* second-level item, use special fallback */
            s=uloc_get_ਸਾਰਣੀ_String_With_Fallback(ਰਸਤਾ, locale,
                                               ਸਾਰਣੀ_ਕੁੰਜੀ,
                                               sub_ਸਾਰਣੀ_ਕੁੰਜੀ,
                                               ਵਸਤੂ_ਕੁੰਜੀ,
                                               &length,
                                               &error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ) && is_Language_ਕੋਡ && ਵਸਤੂ_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                error_ਕੋਡ = U_ZERO_ERROR;
                Locale canon_ਕੁੰਜੀ = Locale::createCanonical(ਵਸਤੂ_ਕੁੰਜੀ);
                s=uloc_get_ਸਾਰਣੀ_String_With_Fallback(ਰਸਤਾ, locale,
                                                    ਸਾਰਣੀ_ਕੁੰਜੀ,
                                                    sub_ਸਾਰਣੀ_ਕੁੰਜੀ,
                                                    canon_ਕੁੰਜੀ.getName(),
                                                    &length,
                                                    &error_ਕੋਡ);
            }
        }
    }

    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        int32_t copy_ਲੰਮਾਈ=uprv_min(length, dest_ਸਮਰੱਥਾ);
        ਜੇ(copy_ਲੰਮਾਈ>0 && s != ਖਾਲੀ_ਸੂਚਕ) {
            u_memcpy(dest, s, copy_ਲੰਮਾਈ);
        }
    } ਨਹੀਂਤਾਂ {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_chars_ਮੰਜ਼ਿਲ_UChars(substitute, dest, uprv_min(length, dest_ਸਮਰੱਥਾ));
        error_ਕੋਡ = U_USING_DEFAULT_WARNING;
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, length, &error_ਕੋਡ);
}

ਵਰਤੋਂ UDisplayNameGetter = icu::CharString(std::string_view, UError_ਕੋਡ&);

int32_t
_getDisplayNameForComponent(ਅਚਲ ਅੱਖਰ *locale,
                            ਅਚਲ ਅੱਖਰ *displayLocale,
                            char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                            UDisplayNameGetter *getter,
                            ਅਚਲ ਅੱਖਰ *ਟੈਗ,
                            UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }
    UError_ਕੋਡ local_ਸਥਿਤੀ;
    ਅਚਲ ਅੱਖਰ* ਜੜ੍ਹ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }

    local_ਸਥਿਤੀ = U_ZERO_ERROR;
    icu::CharString locale_ਬਫਰ = (*getter)(locale, local_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(local_ਸਥਿਤੀ)) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ (locale_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        ਜੇ (getter == ulocimp_getLanguage) {
            locale_ਬਫਰ.append("und", error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, 0, &error_ਕੋਡ);
        }
    }

    ਜੜ੍ਹ = ਟੈਗ == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    ਵਾਪਸ get_String_Or_Copy_ਕੁੰਜੀ(ਜੜ੍ਹ, displayLocale,
                               ਟੈਗ, ਖਾਲੀ_ਸੂਚਕ, locale_ਬਫਰ.ਅੰਕੜੇ(),
                               locale_ਬਫਰ.ਅੰਕੜੇ(),
                               dest, dest_ਸਮਰੱਥਾ,
                               error_ਕੋਡ);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ਅਚਲ ਅੱਖਰ *locale,
                        ਅਚਲ ਅੱਖਰ *displayLocale,
                        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                ulocimp_getLanguage, _kLanguages, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ਅਚਲ ਅੱਖਰ* locale,
                      ਅਚਲ ਅੱਖਰ* displayLocale,
                      char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                      UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ 0; }
    UError_ਕੋਡ err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                ulocimp_getScript, _kScriptsStandAlone, err);

    ਜੇ (dest_ਸਮਰੱਥਾ == 0 && err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                                                           ulocimp_getScript, _kScripts, *p_Error_ਕੋਡ);
        ਵਾਪਸ (fallback_res > res) ? fallback_res : res;
    }
    ਜੇ ( err == U_USING_DEFAULT_WARNING ) {
        ਵਾਪਸ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                                           ulocimp_getScript, _kScripts, *p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ = err;
        ਵਾਪਸ res;
    }
}

ਸਥਿਰ int32_t
uloc_get_Display_Script_ਵਿੱਚ_Context(ਅਚਲ ਅੱਖਰ* locale,
                      ਅਚਲ ਅੱਖਰ* displayLocale,
                      char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                      UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਵਾਪਸ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                    ulocimp_getScript, _kScripts, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ਅਚਲ ਅੱਖਰ *locale,
                       ਅਚਲ ਅੱਖਰ *displayLocale,
                       char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                ulocimp_getRegion, _kCountries, *p_Error_ਕੋਡ);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ਅਚਲ ਅੱਖਰ *locale,
                       ਅਚਲ ਅੱਖਰ *displayLocale,
                       char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ਸਮਰੱਥਾ,
                ulocimp_getVariant, _kVariants, *p_Error_ਕੋਡ);
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
uloc_getDisplayName(ਅਚਲ ਅੱਖਰ *locale,
                    ਅਚਲ ਅੱਖਰ *displayLocale,
                    char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਸਥਿਰ ਅਚਲ char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ਸਥਿਰ ਅਚਲ char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ਸਥਿਰ ਅਚਲ char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ਸਥਿਰ ਅਚਲ int32_t subLen = 3;
    ਸਥਿਰ ਅਚਲ char16_t default_ਨਮੂਨਾ[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ਸਥਿਰ ਅਚਲ int32_t defaultPatLen = 9;
    ਸਥਿਰ ਅਚਲ int32_t defaultSub0Pos = 0;
    ਸਥਿਰ ਅਚਲ int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ਅਚਲ char16_t *separator;
    int32_t sepLen = 0;
    ਅਚਲ char16_t *ਨਮੂਨਾ;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = ਸੱਚ; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = ਸੱਚ; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = ਝੂਠ; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

        icu::Local_UResource_Bundle_ਸੂਚਕ locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &ਸਥਿਤੀ));
        icu::Local_UResource_Bundle_ਸੂਚਕ dspbundle(
                ures_get_By_ਕੁੰਜੀ_With_Fallback(locbundle.getAlias(), k_Locale_Display_ਨਮੂਨਾ, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));

        separator=ures_get_String_By_ਕੁੰਜੀ_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &ਸਥਿਤੀ);
        ਨਮੂਨਾ=ures_get_String_By_ਕੁੰਜੀ_With_Fallback(dspbundle.getAlias(), k_ਨਮੂਨਾ, &patLen, &ਸਥਿਤੀ);
    }

    /* If we couldn't find any data, then use the defaults */
    ਜੇ(sepLen == 0) {
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
        ਜੇ (p0==ਖਾਲੀ_ਸੂਚਕ || p1==ਖਾਲੀ_ਸੂਚਕ || p1<p0) {
            *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        separator = (ਅਚਲ char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    ਜੇ(patLen==0 || (patLen==defaultPatLen && !u_strncmp(ਨਮੂਨਾ, default_ਨਮੂਨਾ, patLen))) {
        ਨਮੂਨਾ=default_ਨਮੂਨਾ;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } ਨਹੀਂਤਾਂ { /* non-default pattern */
        char16_t *p0=u_strstr(ਨਮੂਨਾ, sub0);
        char16_t *p1=u_strstr(ਨਮੂਨਾ, sub1);
        ਜੇ (p0==ਖਾਲੀ_ਸੂਚਕ || p1==ਖਾਲੀ_ਸੂਚਕ) {
            *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        sub0Pos = static_cast<int32_t>(p0-ਨਮੂਨਾ);
        sub1Pos = static_cast<int32_t>(p1-ਨਮੂਨਾ);
        ਜੇ (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        ਜੇ (u_strchr(ਨਮੂਨਾ, 0xFF08) != ਖਾਲੀ_ਸੂਚਕ) {
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
    ਕਰੋ {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::Local_UEnumeration_ਸੂਚਕ kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        ਜੇ(sub0Pos) {
            ਜੇ(dest_ਸਮਰੱਥਾ >= sub0Pos) {
                ਜਦੋਂ (patPos < sub0Pos) {
                    *p++ = ਨਮੂਨਾ[patPos++];
                }
            } ਨਹੀਂਤਾਂ {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } ਨਹੀਂਤਾਂ {
            length=0;
        }

        ਲਈ(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = ਝੂਠ; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=dest_ਸਮਰੱਥਾ-length;
            ਜੇ (cap <= 0) {
                cap=0;
            } ਨਹੀਂਤਾਂ {
                p=dest+length;
            }

            ਜੇ (subi == langi) { /* {0}*/
                ਜੇ(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, p_Error_ਕੋਡ);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=ਸੱਚ;
            } ਨਹੀਂਤਾਂ { /* {1} */
                ਜੇ(!haveRest) {
                    subdone=ਸੱਚ;
                } ਨਹੀਂਤਾਂ {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    ਚੋਣ(resti++) {
                        ਹਾਲਤ 0:
                            restPos=length;
                            len=uloc_get_Display_Script_ਵਿੱਚ_Context(locale, displayLocale, p, cap, p_Error_ਕੋਡ);
                            ਰੋਕੋ;
                        ਹਾਲਤ 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, p_Error_ਕੋਡ);
                            ਰੋਕੋ;
                        ਹਾਲਤ 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, p_Error_ਕੋਡ);
                            ਰੋਕੋ;
                        ਹਾਲਤ 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, p_Error_ਕੋਡ));
                            U_FALLTHROUGH;
                        ਮੂਲ: {
                            ਅਚਲ ਅੱਖਰ* kw=uenum_ਅਗਲਾ(kenum.getAlias(), &len, p_Error_ਕੋਡ);
                            ਜੇ (kw == ਖਾਲੀ_ਸੂਚਕ) {
                                len=0; /* mark that we didn't add a component */
                                subdone=ਸੱਚ;
                            } ਨਹੀਂਤਾਂ {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, p_Error_ਕੋਡ);
                                ਜੇ(len) {
                                    ਜੇ(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    ਜੇ(cap <= 0) {
                                        cap=0;
                                    } ਨਹੀਂਤਾਂ {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                ਜੇ(*p_Error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                                    *p_Error_ਕੋਡ=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_ਮੁੱਲ(locale, kw, displayLocale,
                                                                           p, cap, p_Error_ਕੋਡ);
                                ਜੇ(len) {
                                    ਜੇ(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=dest_ਸਮਰੱਥਾ-length;
                                    ਜੇ(cap <= 0) {
                                        cap=0;
                                    } ਨਹੀਂਤਾਂ {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ਰੋਕੋ;
                    } /* end switch */

                    ਜੇ (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        ਜੇ(len+sepLen<=cap) {
                            ਅਚਲ char16_t * plimit = p + len;
                            ਲਈ (; p < plimit; p++) {
                                ਜੇ (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } ਨਹੀਂਤਾਂ ਜੇ (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            ਲਈ(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } ਨਹੀਂਤਾਂ ਜੇ(subdone) {
                        /* remove separator if we added it */
                        ਜੇ (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            ਜੇ(*p_Error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                *p_Error_ਕੋਡ=U_ZERO_ERROR;
            }

            ਜੇ(subdone) {
                ਜੇ(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    ਜੇ(length+padLen <= dest_ਸਮਰੱਥਾ) {
                        p=dest+length;
                        ਲਈ(int32_t i=0;i<padLen;++i) {
                            *p++=ਨਮੂਨਾ[patPos++];
                        }
                    } ਨਹੀਂਤਾਂ {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } ਨਹੀਂਤਾਂ ਜੇ(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } ਨਹੀਂਤਾਂ ਜੇ(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    ਜੇ(dest && sub0Pos!=0) {
                        ਜੇ (sub0Pos+length<=dest_ਸਮਰੱਥਾ) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } ਨਹੀਂਤਾਂ {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=ਸੱਚ;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } ਜਦੋਂ(retry);

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, length, p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ਅਚਲ ਅੱਖਰ* keyword,
                       ਅਚਲ ਅੱਖਰ* displayLocale,
                       char16_t* dest,
                       int32_t dest_ਸਮਰੱਥਾ,
                       UError_ਕੋਡ* ਸਥਿਤੀ){

    /* argument checking */
    ਜੇ(ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    ਵਾਪਸ get_String_Or_Copy_ਕੁੰਜੀ(U_ICUDATA_LANG, displayLocale,
                               k_ਕੁੰਜੀਆਂ, ਖਾਲੀ_ਸੂਚਕ,
                               keyword, 
                               keyword,      
                               dest, dest_ਸਮਰੱਥਾ,
                               *ਸਥਿਤੀ);

}


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCURRENCY_DISPLAY_NAME_ਸੂਚਕਾਂਕ 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_ਮੁੱਲ(   ਅਚਲ ਅੱਖਰ* locale,
                               ਅਚਲ ਅੱਖਰ* keyword,
                               ਅਚਲ ਅੱਖਰ* displayLocale,
                               char16_t* dest,
                               int32_t dest_ਸਮਰੱਥਾ,
                               UError_ਕੋਡ* ਸਥਿਤੀ){


    /* argument checking */
    ਜੇ(ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* get the keyword value */
    CharString keyword_ਮੁੱਲ;
    ਜੇ (keyword != ਖਾਲੀ_ਸੂਚਕ && *keyword != '\0') {
        keyword_ਮੁੱਲ = ulocimp_get_Keyword_ਮੁੱਲ(locale, keyword, *ਸਥਿਤੀ);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    ਜੇ(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ਅਚਲ char16_t *dispName = ਖਾਲੀ_ਸੂਚਕ;

        icu::Local_UResource_Bundle_ਸੂਚਕ bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, ਸਥਿਤੀ));
        icu::Local_UResource_Bundle_ਸੂਚਕ currencies(
                ures_get_By_ਕੁੰਜੀ(bundle.getAlias(), _kCurrencies, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ));
        icu::Local_UResource_Bundle_ਸੂਚਕ currency(
                ures_get_By_ਕੁੰਜੀ_With_Fallback(currencies.getAlias(), keyword_ਮੁੱਲ.ਅੰਕੜੇ(), ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ));

        dispName = ures_get_String_By_ਸੂਚਕਾਂਕ(currency.getAlias(), UCURRENCY_DISPLAY_NAME_ਸੂਚਕਾਂਕ, &dispNameLen, ਸਥਿਤੀ);

        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
            ਜੇ(*ਸਥਿਤੀ == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *ਸਥਿਤੀ = U_USING_DEFAULT_WARNING;
            }ਨਹੀਂਤਾਂ{
                ਵਾਪਸ 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        ਜੇ(dispName != ਖਾਲੀ_ਸੂਚਕ){
            ਜੇ(dispNameLen <= dest_ਸਮਰੱਥਾ){
                u_memcpy(dest, dispName, dispNameLen);
                ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dispNameLen, ਸਥਿਤੀ);
            }ਨਹੀਂਤਾਂ{
                *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਵਾਪਸ dispNameLen;
            }
        }ਨਹੀਂਤਾਂ{
            /* we have not found the display name for the value .. just copy over */
            ਜੇ(keyword_ਮੁੱਲ.length() <= dest_ਸਮਰੱਥਾ){
                u_chars_ਮੰਜ਼ਿਲ_UChars(keyword_ਮੁੱਲ.ਅੰਕੜੇ(), dest, keyword_ਮੁੱਲ.length());
                ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, keyword_ਮੁੱਲ.length(), ਸਥਿਤੀ);
            }ਨਹੀਂਤਾਂ{
                 *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਵਾਪਸ keyword_ਮੁੱਲ.length();
            }
        }

        
    }ਨਹੀਂਤਾਂ{

        ਵਾਪਸ get_String_Or_Copy_ਕੁੰਜੀ(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_ਮੁੱਲ.ਅੰਕੜੇ(),
                                   keyword_ਮੁੱਲ.ਅੰਕੜੇ(),
                                   dest, dest_ਸਮਰੱਥਾ,
                                   *ਸਥਿਤੀ);
    }
}
