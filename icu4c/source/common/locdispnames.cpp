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
Locale::getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಇದು->getDisplayLanguage(getDefault(), dispLang);
}

/*We cannot make any assumptions on the size of the output display strings
* Yet, since we are calling through to a C API, we need to set limits on
* buffer size. For all the following getDisplay functions we first attempt
* to fill up a stack allocated buffer. If it is to small we heap allocated
* the exact buffer we need copy it to the UnicodeString and delete it*/

UnicodeString&
Locale::getDisplayLanguage(ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    char16_t *ಬಫರ್;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length;

    ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.truncate(0);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                   ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                   &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);

    ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(length);
        ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಫಲಿತಾಂಶ.truncate(0);
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        length=uloc_getDisplayLanguage(getName(), displayLocale.getName(),
                                       ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                       &error_ಸಂಕೇತ);
        ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale::getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಇದು->getDisplayScript(getDefault(), dispScript);
}

UnicodeString&
Locale::getDisplayScript(ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                         UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    char16_t *ಬಫರ್;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length;

    ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.truncate(0);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                  ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                  &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);

    ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(length);
        ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಫಲಿತಾಂಶ.truncate(0);
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        length=uloc_getDisplayScript(getName(), displayLocale.getName(),
                                      ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                      &error_ಸಂಕೇತ);
        ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale::getDisplayCountry(UnicodeString& dispCntry U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಇದು->getDisplayCountry(getDefault(), dispCntry);
}

UnicodeString&
Locale::getDisplayCountry(ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    char16_t *ಬಫರ್;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length;

    ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.truncate(0);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                  ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                  &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);

    ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(length);
        ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಫಲಿತಾಂಶ.truncate(0);
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        length=uloc_getDisplayCountry(getName(), displayLocale.getName(),
                                      ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                      &error_ಸಂಕೇತ);
        ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale::getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಇದು->getDisplayVariant(getDefault(), dispVar);
}

UnicodeString&
Locale::getDisplayVariant(ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    char16_t *ಬಫರ್;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length;

    ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.truncate(0);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                  ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                  &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);

    ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(length);
        ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಫಲಿತಾಂಶ.truncate(0);
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        length=uloc_getDisplayVariant(getName(), displayLocale.getName(),
                                      ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                      &error_ಸಂಕೇತ);
        ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale::getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಇದು->getDisplayName(getDefault(), name);
}

UnicodeString&
Locale::getDisplayName(ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    char16_t *ಬಫರ್;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    int32_t length;

    ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ.truncate(0);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    length=uloc_getDisplayName(getName(), displayLocale.getName(),
                               ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                               &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);

    ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಬಫರ್=ಫಲಿತಾಂಶ.get_ಬಫರ್(length);
        ಆದರೆ (ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಫಲಿತಾಂಶ.truncate(0);
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        error_ಸಂಕೇತ=U_ZERO_ERROR;
        length=uloc_getDisplayName(getName(), displayLocale.getName(),
                                   ಬಫರ್, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(),
                                   &error_ಸಂಕೇತ);
        ಫಲಿತಾಂಶ.release_ಬಫರ್(U_SUCCESS(error_ಸಂಕೇತ) ? length : 0);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

// -------------------------------------
// Gets the objectLocale display name in the default locale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ಸ್ಥಿರಾಂಕ Locale& objectLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ಹಿಂತಿರುಗಿ objectLocale.getDisplayName(name);
}

// -------------------------------------
// Gets the objectLocale display name in the displayLocale language.
UnicodeString& U_EXPORT2
BreakIterator::getDisplayName(ಸ್ಥಿರಾಂಕ Locale& objectLocale,
                              ಸ್ಥಿರಾಂಕ Locale& displayLocale,
                              UnicodeString& name U_LIFETIME_BOUND)
{
    ಹಿಂತಿರುಗಿ objectLocale.getDisplayName(displayLocale, name);
}

#ಆದರೆ_ಅಂತ್ಯ


U_NAMESPACE_ಕೊನೆ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

ನಾಮಾಕಾಶ {

/* ### Constants **************************************************/

/* These strings describe the resources we attempt to load from
 the locale ResourceBundle data file.*/
constexpr ಅಕ್ಷರ _kLanguages[]       = "Languages";
constexpr ಅಕ್ಷರ _kScripts[]         = "Scripts";
constexpr ಅಕ್ಷರ _kScriptsStandAlone[] = "Scripts%stand-alone";
constexpr ಅಕ್ಷರ _kCountries[]       = "Countries";
constexpr ಅಕ್ಷರ _kVariants[]        = "Variants";
constexpr ಅಕ್ಷರ k_ಕೀಲಿಗಳು[]            = "Keys";
constexpr ಅಕ್ಷರ _kTypes[]           = "Types";
//constexpr char _kRootName[]        = "root";
constexpr ಅಕ್ಷರ _kCurrency[]        = "currency";
constexpr ಅಕ್ಷರ _kCurrencies[]      = "Currencies";
constexpr ಅಕ್ಷರ k_Locale_Display_ನಮೂನೆ[] = "localeDisplayPattern";
constexpr ಅಕ್ಷರ k_ನಮೂನೆ[]         = "pattern";
constexpr ಅಕ್ಷರ _kSeparator[]       = "separator";

/* ### Display name **************************************************/

int32_t
get_String_Or_Copy_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ_ಕೀಲಿ, 
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sub_ಕೋಷ್ಟಕ_ಕೀಲಿ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ_ಕೀಲಿ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *substitute,
                    char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                    UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    ಸ್ಥಿರಾಂಕ char16_t *s = ಶೂನ್ಯಸೂಚಕ;
    int32_t length = 0;

    ಆದರೆ(ಅಂಶ_ಕೀಲಿ==ಶೂನ್ಯಸೂಚಕ) {
        /* top-level item: normal resource bundle access */
        icu::Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(ಮಾರ್ಗ, locale, &error_ಸಂಕೇತ));

        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            s=ures_get_String_By_ಕೀಲಿ(rb.getAlias(), ಕೋಷ್ಟಕ_ಕೀಲಿ, &length, &error_ಸಂಕೇತ);
            /* see comment about closing rb near "return item;" in _res_getTableStringWithFallback() */
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತಾರ್ಕಿಕ is_Language_ಸಂಕೇತ = (uprv_strncmp(ಕೋಷ್ಟಕ_ಕೀಲಿ, _kLanguages, 9) == 0);
        /* Language code should not be a number. If it is, set the error code. */
        ಆದರೆ (is_Language_ಸಂಕೇತ && uprv_strtol(ಅಂಶ_ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, 10)) {
            error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* second-level item, use special fallback */
            s=uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(ಮಾರ್ಗ, locale,
                                               ಕೋಷ್ಟಕ_ಕೀಲಿ,
                                               sub_ಕೋಷ್ಟಕ_ಕೀಲಿ,
                                               ಅಂಶ_ಕೀಲಿ,
                                               &length,
                                               &error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) && is_Language_ಸಂಕೇತ && ಅಂಶ_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
                // convert itemKey locale code to canonical form and try again, ICU-20870
                error_ಸಂಕೇತ = U_ZERO_ERROR;
                Locale canon_ಕೀಲಿ = Locale::createCanonical(ಅಂಶ_ಕೀಲಿ);
                s=uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(ಮಾರ್ಗ, locale,
                                                    ಕೋಷ್ಟಕ_ಕೀಲಿ,
                                                    sub_ಕೋಷ್ಟಕ_ಕೀಲಿ,
                                                    canon_ಕೀಲಿ.getName(),
                                                    &length,
                                                    &error_ಸಂಕೇತ);
            }
        }
    }

    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        int32_t copy_ಉದ್ದಳತೆ=uprv_min(length, dest_ಸಾಮರ್ಥ್ಯ);
        ಆದರೆ(copy_ಉದ್ದಳತೆ>0 && s != ಶೂನ್ಯಸೂಚಕ) {
            u_memcpy(dest, s, copy_ಉದ್ದಳತೆ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* no string from a resource bundle: convert the substitute */
        length = static_cast<int32_t>(uprv_strlen(substitute));
        u_chars_ಗಮ್ಯ_UChars(substitute, dest, uprv_min(length, dest_ಸಾಮರ್ಥ್ಯ));
        error_ಸಂಕೇತ = U_USING_DEFAULT_WARNING;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, length, &error_ಸಂಕೇತ);
}

ಬಳಸಿ UDisplayNameGetter = icu::CharString(std::string_view, UError_ಸಂಕೇತ&);

int32_t
_getDisplayNameForComponent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                            char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                            UDisplayNameGetter *getter,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಟ್ಯಾಗ್,
                            UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬೇರು = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }

    local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    icu::CharString locale_ಬಫರ್ = (*getter)(locale, local_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(local_ಸ್ಥಿತಿ)) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (locale_ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        // For the display name, we treat this as unknown language (ICU-20273).
        ಆದರೆ (getter == ulocimp_getLanguage) {
            locale_ಬಫರ್.append("und", error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, &error_ಸಂಕೇತ);
        }
    }

    ಬೇರು = ಟ್ಯಾಗ್ == _kCountries ? U_ICUDATA_REGION : U_ICUDATA_LANG;

    ಹಿಂತಿರುಗಿ get_String_Or_Copy_ಕೀಲಿ(ಬೇರು, displayLocale,
                               ಟ್ಯಾಗ್, ಶೂನ್ಯಸೂಚಕ, locale_ಬಫರ್.ದತ್ತಾಂಶ(),
                               locale_ಬಫರ್.ದತ್ತಾಂಶ(),
                               dest, dest_ಸಾಮರ್ಥ್ಯ,
                               error_ಸಂಕೇತ);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_getDisplayLanguage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                ulocimp_getLanguage, _kLanguages, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayScript(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* displayLocale,
                      char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    int32_t res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                ulocimp_getScript, _kScriptsStandAlone, err);

    ಆದರೆ (dest_ಸಾಮರ್ಥ್ಯ == 0 && err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        // For preflight, return the max of the value and the fallback.
        int32_t fallback_res = _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                                                           ulocimp_getScript, _kScripts, *p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ (fallback_res > res) ? fallback_res : res;
    }
    ಆದರೆ ( err == U_USING_DEFAULT_WARNING ) {
        ಹಿಂತಿರುಗಿ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                                           ulocimp_getScript, _kScripts, *p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ = err;
        ಹಿಂತಿರುಗಿ res;
    }
}

ಸ್ಥಿರ int32_t
uloc_get_Display_Script_ಒಳ_Context(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* displayLocale,
                      char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಹಿಂತಿರುಗಿ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                    ulocimp_getScript, _kScripts, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayCountry(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                       char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                ulocimp_getRegion, _kCountries, *p_Error_ಸಂಕೇತ);
}

/*
 * TODO separate variant1_variant2_variant3...
 * by getting each tag's display string and concatenating them with ", "
 * in between - similar to uloc_getDisplayName()
 */
U_CAPI int32_t U_EXPORT2
uloc_getDisplayVariant(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                       char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ _getDisplayNameForComponent(locale, displayLocale, dest, dest_ಸಾಮರ್ಥ್ಯ,
                ulocimp_getVariant, _kVariants, *p_Error_ಸಂಕೇತ);
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
uloc_getDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *displayLocale,
                    char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t defaultSeparator[9] = { 0x007b, 0x0030, 0x007d, 0x002c, 0x0020, 0x007b, 0x0031, 0x007d, 0x0000 }; /* "{0}, {1}" */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t sub0[4] = { 0x007b, 0x0030, 0x007d , 0x0000 } ; /* {0} */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t sub1[4] = { 0x007b, 0x0031, 0x007d , 0x0000 } ; /* {1} */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t subLen = 3;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t default_ನಮೂನೆ[10] = {
        0x007b, 0x0030, 0x007d, 0x0020, 0x0028, 0x007b, 0x0031, 0x007d, 0x0029, 0x0000
    }; /* {0} ({1}) */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t defaultPatLen = 9;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t defaultSub0Pos = 0;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t defaultSub1Pos = 5;

    int32_t length; /* of formatted result */

    ಸ್ಥಿರಾಂಕ char16_t *separator;
    int32_t sepLen = 0;
    ಸ್ಥಿರಾಂಕ char16_t *ನಮೂನೆ;
    int32_t patLen = 0;
    int32_t sub0Pos, sub1Pos;
    
    char16_t formatOpenParen         = 0x0028; // (
    char16_t formatReplaceOpenParen  = 0x005B; // [
    char16_t formatCloseParen        = 0x0029; // )
    char16_t formatReplaceCloseParen = 0x005D; // ]

    UBool haveLang = ನಿಜ; /* assume true, set false if we find we don't have
                              a lang component in the locale */
    UBool haveRest = ನಿಜ; /* assume true, set false if we find we don't have
                              any other component in the locale */
    UBool retry = ಸುಳ್ಳು; /* set true if we need to retry, see below */

    int32_t langi = 0; /* index of the language substitution (0 or 1), virtually always 0 */

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

        icu::Local_UResource_Bundle_ಸೂಚಕ locbundle(
                ures_open(U_ICUDATA_LANG, displayLocale, &ಸ್ಥಿತಿ_2));
        icu::Local_UResource_Bundle_ಸೂಚಕ dspbundle(
                ures_get_By_ಕೀಲಿ_With_Fallback(locbundle.getAlias(), k_Locale_Display_ನಮೂನೆ, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));

        separator=ures_get_String_By_ಕೀಲಿ_With_Fallback(dspbundle.getAlias(), _kSeparator, &sepLen, &ಸ್ಥಿತಿ_2);
        ನಮೂನೆ=ures_get_String_By_ಕೀಲಿ_With_Fallback(dspbundle.getAlias(), k_ನಮೂನೆ, &patLen, &ಸ್ಥಿತಿ_2);
    }

    /* If we couldn't find any data, then use the defaults */
    ಆದರೆ(sepLen == 0) {
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
        ಆದರೆ (p0==ಶೂನ್ಯಸೂಚಕ || p1==ಶೂನ್ಯಸೂಚಕ || p1<p0) {
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        separator = (ಸ್ಥಿರಾಂಕ char16_t *)p0 + subLen;
        sepLen = static_cast<int32_t>(p1 - separator);
    }

    ಆದರೆ(patLen==0 || (patLen==defaultPatLen && !u_strncmp(ನಮೂನೆ, default_ನಮೂನೆ, patLen))) {
        ನಮೂನೆ=default_ನಮೂನೆ;
        patLen=defaultPatLen;
        sub0Pos=defaultSub0Pos;
        sub1Pos=defaultSub1Pos;
        // use default formatOpenParen etc. set above
    } ಇಲ್ಲದಿದ್ದರೆ { /* non-default pattern */
        char16_t *p0=u_strstr(ನಮೂನೆ, sub0);
        char16_t *p1=u_strstr(ನಮೂನೆ, sub1);
        ಆದರೆ (p0==ಶೂನ್ಯಸೂಚಕ || p1==ಶೂನ್ಯಸೂಚಕ) {
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        sub0Pos = static_cast<int32_t>(p0-ನಮೂನೆ);
        sub1Pos = static_cast<int32_t>(p1-ನಮೂನೆ);
        ಆದರೆ (sub1Pos < sub0Pos) { /* a very odd pattern */
            int32_t t=sub0Pos; sub0Pos=sub1Pos; sub1Pos=t;
            langi=1;
        }
        ಆದರೆ (u_strchr(ನಮೂನೆ, 0xFF08) != ಶೂನ್ಯಸೂಚಕ) {
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
    ಮಾಡು {
        char16_t* p=dest;
        int32_t patPos=0; /* position in the pattern, used for non-substitution portions */
        int32_t langLen=0; /* length of language substitution */
        int32_t langPos=0; /* position in output of language substitution */
        int32_t restLen=0; /* length of 'everything else' substitution */
        int32_t restPos=0; /* position in output of 'everything else' substitution */
        icu::Local_UEnumeration_ಸೂಚಕ kenum; /* keyword enumeration */

        /* prefix of pattern, extremely likely to be empty */
        ಆದರೆ(sub0Pos) {
            ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ >= sub0Pos) {
                ಆಗ (patPos < sub0Pos) {
                    *p++ = ನಮೂನೆ[patPos++];
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                patPos=sub0Pos;
            }
            length=sub0Pos;
        } ಇಲ್ಲದಿದ್ದರೆ {
            length=0;
        }

        ಗಾಗಿ(int32_t subi=0,resti=0;subi<2;) { /* iterate through patterns 0 and 1*/
            UBool subdone = ಸುಳ್ಳು; /* set true when ready to move to next substitution */

            /* prep p and cap for calls to get display components, pin cap to 0 since
               they complain if cap is negative */
            int32_t cap=dest_ಸಾಮರ್ಥ್ಯ-length;
            ಆದರೆ (cap <= 0) {
                cap=0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                p=dest+length;
            }

            ಆದರೆ (subi == langi) { /* {0}*/
                ಆದರೆ(haveLang) {
                    langPos=length;
                    langLen=uloc_getDisplayLanguage(locale, displayLocale, p, cap, p_Error_ಸಂಕೇತ);
                    length+=langLen;
                    haveLang=langLen>0;
                }
                subdone=ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ { /* {1} */
                ಆದರೆ(!haveRest) {
                    subdone=ನಿಜ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    int32_t len; /* length of component (plus other stuff) we just fetched */
                    ಆಯ್ಕೆ(resti++) {
                        ಸ್ಥಿತಿ 0:
                            restPos=length;
                            len=uloc_get_Display_Script_ಒಳ_Context(locale, displayLocale, p, cap, p_Error_ಸಂಕೇತ);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 1:
                            len=uloc_getDisplayCountry(locale, displayLocale, p, cap, p_Error_ಸಂಕೇತ);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 2:
                            len=uloc_getDisplayVariant(locale, displayLocale, p, cap, p_Error_ಸಂಕೇತ);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 3:
                            kenum.adoptInstead(uloc_openKeywords(locale, p_Error_ಸಂಕೇತ));
                            U_FALLTHROUGH;
                        ಪೂರ್ವನಿಯೋಜಿತ: {
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* kw=uenum_ಮುಂದಿನ(kenum.getAlias(), &len, p_Error_ಸಂಕೇತ);
                            ಆದರೆ (kw == ಶೂನ್ಯಸೂಚಕ) {
                                len=0; /* mark that we didn't add a component */
                                subdone=ನಿಜ;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* incorporating this behavior into the loop made it even more complex,
                                   so just special case it here */
                                len = uloc_getDisplayKeyword(kw, displayLocale, p, cap, p_Error_ಸಂಕೇತ);
                                ಆದರೆ(len) {
                                    ಆದರೆ(len < cap) {
                                        p[len]=0x3d; /* '=', assume we'll need it */
                                    }
                                    len+=1;

                                    /* adjust for call to get keyword */
                                    cap-=len;
                                    ಆದರೆ(cap <= 0) {
                                        cap=0;
                                    } ಇಲ್ಲದಿದ್ದರೆ {
                                        p+=len;
                                    }
                                }
                                /* reset for call below */
                                ಆದರೆ(*p_Error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                                    *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
                                }
                                int32_t vlen = uloc_get_Display_Keyword_ಮೌಲ್ಯ(locale, kw, displayLocale,
                                                                           p, cap, p_Error_ಸಂಕೇತ);
                                ಆದರೆ(len) {
                                    ಆದರೆ(vlen==0) {
                                        --len; /* remove unneeded '=' */
                                    }
                                    /* restore cap and p to what they were at start */
                                    cap=dest_ಸಾಮರ್ಥ್ಯ-length;
                                    ಆದರೆ(cap <= 0) {
                                        cap=0;
                                    } ಇಲ್ಲದಿದ್ದರೆ {
                                        p=dest+length;
                                    }
                                }
                                len+=vlen; /* total we added for key + '=' + value */
                            }
                        } ನಿಲ್ಲಿಸು;
                    } /* end switch */

                    ಆದರೆ (len>0) {
                        /* we added a component, so add separator and write it if there's room. */
                        ಆದರೆ(len+sepLen<=cap) {
                            ಸ್ಥಿರಾಂಕ char16_t * plimit = p + len;
                            ಗಾಗಿ (; p < plimit; p++) {
                                ಆದರೆ (*p == formatOpenParen) {
                                    *p = formatReplaceOpenParen;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == formatCloseParen) {
                                    *p = formatReplaceCloseParen;
                                }
                            }
                            ಗಾಗಿ(int32_t i=0;i<sepLen;++i) {
                                *p++=separator[i];
                            }
                        }
                        length+=len+sepLen;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subdone) {
                        /* remove separator if we added it */
                        ಆದರೆ (length!=restPos) {
                            length-=sepLen;
                        }
                        restLen=length-restPos;
                        haveRest=restLen>0;
                    }
                }
            }

            ಆದರೆ(*p_Error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            }

            ಆದರೆ(subdone) {
                ಆದರೆ(haveLang && haveRest) {
                    /* append internal portion of pattern, the first time,
                       or last portion of pattern the second time */
                    int32_t padLen;
                    patPos+=subLen;
                    padLen=(subi==0 ? sub1Pos : patLen)-patPos;
                    ಆದರೆ(length+padLen <= dest_ಸಾಮರ್ಥ್ಯ) {
                        p=dest+length;
                        ಗಾಗಿ(int32_t i=0;i<padLen;++i) {
                            *p++=ನಮೂನೆ[patPos++];
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        patPos+=padLen;
                    }
                    length+=padLen;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(subi==0) {
                    /* don't have first component, reset for second component */
                    sub0Pos=0;
                    length=0;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length>0) {
                    /* true length is the length of just the component we got. */
                    length=haveLang?langLen:restLen;
                    ಆದರೆ(dest && sub0Pos!=0) {
                        ಆದರೆ (sub0Pos+length<=dest_ಸಾಮರ್ಥ್ಯ) {
                            /* first component not at start of result,
                               but we have full component in buffer. */
                            u_memmove(dest, dest+(haveLang?langPos:restPos), length);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* would have fit, but didn't because of pattern prefix. */
                            sub0Pos=0; /* stops initial padding (and a second retry,
                                          so we won't end up here again) */
                            retry=ನಿಜ;
                        }
                    }
                }

                ++subi; /* move on to next substitution */
            }
        }
    } ಆಗ(retry);

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, length, p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uloc_getDisplayKeyword(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* displayLocale,
                       char16_t* dest,
                       int32_t dest_ಸಾಮರ್ಥ್ಯ,
                       UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){

    /* argument checking */
    ಆದರೆ(ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }


    /* pass itemKey=nullptr to look for a top-level item */
    ಹಿಂತಿರುಗಿ get_String_Or_Copy_ಕೀಲಿ(U_ICUDATA_LANG, displayLocale,
                               k_ಕೀಲಿಗಳು, ಶೂನ್ಯಸೂಚಕ,
                               keyword, 
                               keyword,      
                               dest, dest_ಸಾಮರ್ಥ್ಯ,
                               *ಸ್ಥಿತಿ_2);

}


#ವ್ಯಾಖ್ಯಾನಿಸು UCURRENCY_DISPLAY_NAME_ಸೂಚ್ಯಂಕ 1

U_CAPI int32_t U_EXPORT2
uloc_get_Display_Keyword_ಮೌಲ್ಯ(   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* displayLocale,
                               char16_t* dest,
                               int32_t dest_ಸಾಮರ್ಥ್ಯ,
                               UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){


    /* argument checking */
    ಆದರೆ(ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* get the keyword value */
    CharString keyword_ಮೌಲ್ಯ;
    ಆದರೆ (keyword != ಶೂನ್ಯಸೂಚಕ && *keyword != '\0') {
        keyword_ಮೌಲ್ಯ = ulocimp_get_Keyword_ಮೌಲ್ಯ(locale, keyword, *ಸ್ಥಿತಿ_2);
    }

    /* 
     * if the keyword is equal to currency .. then to get the display name 
     * we need to do the fallback ourselves
     */
    ಆದರೆ(uprv_stricmp(keyword, _kCurrency)==0){

        int32_t dispNameLen = 0;
        ಸ್ಥಿರಾಂಕ char16_t *dispName = ಶೂನ್ಯಸೂಚಕ;

        icu::Local_UResource_Bundle_ಸೂಚಕ bundle(
                ures_open(U_ICUDATA_CURR, displayLocale, ಸ್ಥಿತಿ_2));
        icu::Local_UResource_Bundle_ಸೂಚಕ currencies(
                ures_get_By_ಕೀಲಿ(bundle.getAlias(), _kCurrencies, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2));
        icu::Local_UResource_Bundle_ಸೂಚಕ currency(
                ures_get_By_ಕೀಲಿ_With_Fallback(currencies.getAlias(), keyword_ಮೌಲ್ಯ.ದತ್ತಾಂಶ(), ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2));

        dispName = ures_get_String_By_ಸೂಚ್ಯಂಕ(currency.getAlias(), UCURRENCY_DISPLAY_NAME_ಸೂಚ್ಯಂಕ, &dispNameLen, ಸ್ಥಿತಿ_2);

        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
            ಆದರೆ(*ಸ್ಥಿತಿ_2 == U_MISSING_RESOURCE_ERROR){
                /* we just want to write the value over if nothing is available */
                *ಸ್ಥಿತಿ_2 = U_USING_DEFAULT_WARNING;
            }ಇಲ್ಲದಿದ್ದರೆ{
                ಹಿಂತಿರುಗಿ 0;
            }
        }

        /* now copy the dispName over if not nullptr */
        ಆದರೆ(dispName != ಶೂನ್ಯಸೂಚಕ){
            ಆದರೆ(dispNameLen <= dest_ಸಾಮರ್ಥ್ಯ){
                u_memcpy(dest, dispName, dispNameLen);
                ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dispNameLen, ಸ್ಥಿತಿ_2);
            }ಇಲ್ಲದಿದ್ದರೆ{
                *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ಹಿಂತಿರುಗಿ dispNameLen;
            }
        }ಇಲ್ಲದಿದ್ದರೆ{
            /* we have not found the display name for the value .. just copy over */
            ಆದರೆ(keyword_ಮೌಲ್ಯ.length() <= dest_ಸಾಮರ್ಥ್ಯ){
                u_chars_ಗಮ್ಯ_UChars(keyword_ಮೌಲ್ಯ.ದತ್ತಾಂಶ(), dest, keyword_ಮೌಲ್ಯ.length());
                ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, keyword_ಮೌಲ್ಯ.length(), ಸ್ಥಿತಿ_2);
            }ಇಲ್ಲದಿದ್ದರೆ{
                 *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ಹಿಂತಿರುಗಿ keyword_ಮೌಲ್ಯ.length();
            }
        }

        
    }ಇಲ್ಲದಿದ್ದರೆ{

        ಹಿಂತಿರುಗಿ get_String_Or_Copy_ಕೀಲಿ(U_ICUDATA_LANG, displayLocale,
                                   _kTypes, keyword, 
                                   keyword_ಮೌಲ್ಯ.ದತ್ತಾಂಶ(),
                                   keyword_ಮೌಲ್ಯ.ದತ್ತಾಂಶ(),
                                   dest, dest_ಸಾಮರ್ಥ್ಯ,
                                   *ಸ್ಥಿತಿ_2);
    }
}
