// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_FORMATTING

#ಅನ್ತರ್ಭಾವ "unicode/locdspnm.h"
#ಅನ್ತರ್ಭಾವ "unicode/simpleformatter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/udisplaycontext.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucurr.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ureslocs.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
ವರ್ಗ ICUData_ಕೋಷ್ಟಕ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಮಾರ್ಗ;
    Locale locale;

ಸಾರ್ವಜನಿಕ:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ Locale& locale);
    ~ICUData_ಕೋಷ್ಟಕ() = ಪೂರ್ವನಿಯೋಜಿತ;

    ಸ್ಥಿರಾಂಕ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
    UnicodeString& get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;

    UnicodeString& getNoFallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                                 UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
    UnicodeString& getNoFallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                                 UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
};

ಸಾಲೊಳಗೆ UnicodeString& ICUData_ಕೋಷ್ಟಕ::get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                                        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ get(ಕೋಷ್ಟಕ_ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, ಅಂಶ_ಕೀಲಿ, ಫಲಿತಾಂಶ);
}

ಸಾಲೊಳಗೆ UnicodeString& ICUData_ಕೋಷ್ಟಕ::getNoFallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                                                  UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ getNoFallback(ಕೋಷ್ಟಕ_ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, ಅಂಶ_ಕೀಲಿ, ಫಲಿತಾಂಶ);
}

ICUData_ಕೋಷ್ಟಕ::ICUData_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ Locale& locale)
    : ಮಾರ್ಗ(ಮಾರ್ಗ), locale(locale)
{
    U_ASSERT(ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ);
}

ಸ್ಥಿರಾಂಕ Locale&
ICUData_ಕೋಷ್ಟಕ::getLocale() U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ locale;
}

UnicodeString&
ICUData_ಕೋಷ್ಟಕ::get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                  UnicodeString &ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  int32_t len = 0;

  ಸ್ಥಿರಾಂಕ char16_t *s = uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(ಮಾರ್ಗ, locale.getName(),
                                                   ಕೋಷ್ಟಕ_ಕೀಲಿ, sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಅಂಶ_ಕೀಲಿ,
                                                   &len, &ಸ್ಥಿತಿ_2);
  ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && len > 0) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(s, len);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(UnicodeString(ಅಂಶ_ಕೀಲಿ, -1, US_INV));
}

UnicodeString&
ICUData_ಕೋಷ್ಟಕ::getNoFallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಅಂಶ_ಕೀಲಿ,
                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  int32_t len = 0;

  ಸ್ಥಿರಾಂಕ char16_t *s = uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(ಮಾರ್ಗ, locale.getName(),
                                                   ಕೋಷ್ಟಕ_ಕೀಲಿ, sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಅಂಶ_ಕೀಲಿ,
                                                   &len, &ಸ್ಥಿತಿ_2);
  ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(s, len);
  }

  ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ಹೆಸರುಗಳು::~Locale_Display_ಹೆಸರುಗಳು() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ಆದರೆ 0  // currently unused

ವರ್ಗ Default_Locale_Display_ಹೆಸರುಗಳು : ಸಾರ್ವಜನಿಕ Locale_Display_ಹೆಸರುಗಳು {
  UDialectHandling dialectHandling;

ಸಾರ್ವಜನಿಕ:
  // constructor
  Default_Locale_Display_ಹೆಸರುಗಳು(UDialectHandling dialectHandling);

  ವಾಸ್ತವಿಕ ~Default_Locale_Display_ಹೆಸರುಗಳು();

  ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ Locale& getLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;
  ವಾಸ್ತವಿಕ UDialectHandling getDialectHandling() ಸ್ಥಿರಾಂಕ;

  ವಾಸ್ತವಿಕ UnicodeString& localeDisplayName(ಸ್ಥಿರಾಂಕ Locale& locale,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& localeDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& languageDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& scriptDisplayName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
  ವಾಸ್ತವಿಕ UnicodeString& ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
};

Default_Locale_Display_ಹೆಸರುಗಳು::Default_Locale_Display_ಹೆಸರುಗಳು(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_ಹೆಸರುಗಳು::~Default_Locale_Display_ಹೆಸರುಗಳು() {
}

ಸ್ಥಿರಾಂಕ Locale&
Default_Locale_Display_ಹೆಸರುಗಳು::getLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ Locale::get_ಬೇರು();
}

UDialectHandling
Default_Locale_Display_ಹೆಸರುಗಳು::getDialectHandling() ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ dialectHandling;
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::localeDisplayName(ಸ್ಥಿರಾಂಕ Locale& locale,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::localeDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::languageDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang,
                                               UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::scriptDisplayName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name = uscript_getName(script_ಸಂಕೇತ);
  ಆದರೆ (name) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(name, -1, US_INV);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.remove();
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
                                              UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(ಕೀಲಿ, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ಹೆಸರುಗಳು::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* /*ಕೀಲಿ*/,
                                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                               UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(value, -1, US_INV);
}

#ಆದರೆ_ಅಂತ್ಯ  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

ವರ್ಗ Locale_Display_ಹೆಸರುಗಳು_Impl : ಸಾರ್ವಜನಿಕ Locale_Display_ಹೆಸರುಗಳು {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_ಕೋಷ್ಟಕ lang_ದತ್ತಾಂಶ;
    ICUData_ಕೋಷ್ಟಕ region_ದತ್ತಾಂಶ;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ಕೀಲಿ_Type_Format;
    UDisplayContext capitalizationContext;
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ಇಲ್ಲದಿದ್ದರೆ
    UObject* capitalizationBrkIter;
#ಆದರೆ_ಅಂತ್ಯ
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_ಉದ್ದಳತೆ;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ಎಣಿಕೆ CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_ಕೀಲಿ,
        k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ,
        k_Cap_Context_Usage_ಎಣಿಕೆ
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     ತಾರ್ಕಿಕ fCapitalization[k_Cap_Context_Usage_ಎಣಿಕೆ];

ಸಾರ್ವಜನಿಕ:
    // constructor
    Locale_Display_ಹೆಸರುಗಳು_Impl(ಸ್ಥಿರಾಂಕ Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_ಹೆಸರುಗಳು_Impl(ಸ್ಥಿರಾಂಕ Locale& locale, UDisplayContext *contexts, int32_t length);
    ವಾಸ್ತವಿಕ ~Locale_Display_ಹೆಸರುಗಳು_Impl();

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ Locale& getLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND override;
    ವಾಸ್ತವಿಕ UDialectHandling getDialectHandling() ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UDisplayContext getContext(UDisplayContextType type) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UnicodeString& localeDisplayName(ಸ್ಥಿರಾಂಕ Locale& locale,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString& localeDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString&
    languageDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString& scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString& scriptDisplayName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString& regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                                             UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString&
    variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString& ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString&
    ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                        UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    UnicodeString& localeIdName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                ತಾರ್ಕಿಕ substitute) ಸ್ಥಿರಾಂಕ;
    UnicodeString& appendWithSep(UnicodeString& ಬಫರ್ U_LIFETIME_BOUND,
                                 ಸ್ಥಿರಾಂಕ UnicodeString& src) ಸ್ಥಿರಾಂಕ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ;
    UnicodeString& scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                     ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ;
    UnicodeString& regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                     ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ;
    UnicodeString& variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                      ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ;
    UnicodeString& ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                  ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ;
    UnicodeString& ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                       ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ;
    ಶೂನ್ಯ initialize();

    ರಚನೆ CapitalizationContextSink;
};

Locale_Display_ಹೆಸರುಗಳು_Impl::Locale_Display_ಹೆಸರುಗಳು_Impl(ಸ್ಥಿರಾಂಕ Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_ದತ್ತಾಂಶ(U_ICUDATA_LANG, locale)
    , region_ದತ್ತಾಂಶ(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(ಶೂನ್ಯಸೂಚಕ)
    , name_ಉದ್ದಳತೆ(UDISPCTX_ಉದ್ದಳತೆ_ತುಂಬಿದ)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_ಹೆಸರುಗಳು_Impl::Locale_Display_ಹೆಸರುಗಳು_Impl(ಸ್ಥಿರಾಂಕ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_ಹೆಸರುಗಳು)
    , lang_ದತ್ತಾಂಶ(U_ICUDATA_LANG, locale)
    , region_ದತ್ತಾಂಶ(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(ಶೂನ್ಯಸೂಚಕ)
    , name_ಉದ್ದಳತೆ(UDISPCTX_ಉದ್ದಳತೆ_ತುಂಬಿದ)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ಆಗ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ಆಯ್ಕೆ (selector) {
            ಸ್ಥಿತಿ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UDISPCTX_TYPE_DISPLAY_ಉದ್ದಳತೆ:
                name_ಉದ್ದಳತೆ = value;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                ನಿಲ್ಲಿಸು;
        }
    }
    initialize();
}

ರಚನೆ Locale_Display_ಹೆಸರುಗಳು_Impl::CapitalizationContextSink : ಸಾರ್ವಜನಿಕ ResourceSink {
    ತಾರ್ಕಿಕ hasCapitalizationUsage;
    Locale_Display_ಹೆಸರುಗಳು_Impl& ಪೋಷಕ;

    CapitalizationContextSink(Locale_Display_ಹೆಸರುಗಳು_Impl& ಪೋಷಕ_2)
      : hasCapitalizationUsage(ಸುಳ್ಳು), ಪೋಷಕ(ಪೋಷಕ_2) {}
    ವಾಸ್ತವಿಕ ~CapitalizationContextSink();

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool /*noFallback*/,
            UError_ಸಂಕೇತ &error_ಸಂಕೇತ) override {
        Resource_ಕೋಷ್ಟಕ contexts = value.get_ಕೋಷ್ಟಕ(error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; contexts.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {

            CapContextUsage usageEnum;
            ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ಕೀಲಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಮುಂದುವರಿ;
            }

            int32_t len = 0;
            ಸ್ಥಿರಾಂಕ int32_t* int_ಸದಿಶ = value.get_Int_ಸದಿಶ(len, error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
            ಆದರೆ (len < 2) { ಮುಂದುವರಿ; }

            int32_t titlecaseInt = (ಪೋಷಕ.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_ಸದಿಶ[0] : int_ಸದಿಶ[1];
            ಆದರೆ (titlecaseInt == 0) { ಮುಂದುವರಿ; }

            ಪೋಷಕ.fCapitalization[usageEnum] = ನಿಜ;
            hasCapitalizationUsage = ನಿಜ;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_ಹೆಸರುಗಳು_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

ಶೂನ್ಯ
Locale_Display_ಹೆಸರುಗಳು_Impl::initialize() {
    Locale_Display_ಹೆಸರುಗಳು_Impl* nonConstThis = ಇದು;
    nonConstThis->locale = lang_ದತ್ತಾಂಶ.getLocale() == Locale::get_ಬೇರು()
        ? region_ದತ್ತಾಂಶ.getLocale()
        : lang_ದತ್ತಾಂಶ.getLocale();

    UnicodeString sep;
    lang_ದತ್ತಾಂಶ.getNoFallback("localeDisplayPattern", "separator", sep);
    ಆದರೆ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    separatorFormat.apply_ನಮೂನೆ_Min_Max_Arguments(sep, 2, 2, ಸ್ಥಿತಿ_2);

    UnicodeString ನಮೂನೆ;
    lang_ದತ್ತಾಂಶ.getNoFallback("localeDisplayPattern", "pattern", ನಮೂನೆ);
    ಆದರೆ (ನಮೂನೆ.isBogus()) {
        ನಮೂನೆ = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_ನಮೂನೆ_Min_Max_Arguments(ನಮೂನೆ, 2, 2, ಸ್ಥಿತಿ_2);
    ಆದರೆ (ನಮೂನೆ.ಸೂಚ್ಯಂಕ_ದ(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ಇಲ್ಲದಿದ್ದರೆ {
        formatOpenParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.ಗಣ_ಗಮ್ಯ(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_ನಮೂನೆ;
    lang_ದತ್ತಾಂಶ.get("localeDisplayPattern", "keyTypePattern", kt_ನಮೂನೆ);
    ಆದರೆ (kt_ನಮೂನೆ.isBogus()) {
        kt_ನಮೂನೆ = UnicodeString("{0}={1}", -1, US_INV);
    }
    ಕೀಲಿ_Type_Format.apply_ನಮೂನೆ_Min_Max_Arguments(kt_ನಮೂನೆ, 2, 2, ಸ್ಥಿತಿ_2);

    uprv_memset(fCapitalization, 0, ಗಾತ್ರ(fCapitalization));
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    ತಾರ್ಕಿಕ needBrkIter = ಸುಳ್ಳು;
    ಆದರೆ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_ಸೂಚಕ resource(ures_open(ಶೂನ್ಯಸೂಚಕ, locale.getName(), &ಸ್ಥಿತಿ_2));
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        CapitalizationContextSink sink(*ಇದು);
        ures_get_All_ವಸ್ತುಗಳು_With_Fallback(resource.getAlias(), "contextTransforms", sink, ಸ್ಥಿತಿ_2);
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ಆದರೆ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_ದ_SENTENCE) {
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಅಳಿಸು capitalizationBrkIter;
            capitalizationBrkIter = ಶೂನ್ಯಸೂಚಕ;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

Locale_Display_ಹೆಸರುಗಳು_Impl::~Locale_Display_ಹೆಸರುಗಳು_Impl() {
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    ಅಳಿಸು capitalizationBrkIter;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ Locale&
Locale_Display_ಹೆಸರುಗಳು_Impl::getLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ locale;
}

UDialectHandling
Locale_Display_ಹೆಸರುಗಳು_Impl::getDialectHandling() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ dialectHandling;
}

UDisplayContext
Locale_Display_ಹೆಸರುಗಳು_Impl::getContext(UDisplayContextType type) ಸ್ಥಿರಾಂಕ {
    ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ UDISPCTX_TYPE_DIALECT_HANDLING:
            ಹಿಂತಿರುಗಿ static_cast<UDisplayContext>(dialectHandling);
        ಸ್ಥಿತಿ UDISPCTX_TYPE_CAPITALIZATION:
            ಹಿಂತಿರುಗಿ capitalizationContext;
        ಸ್ಥಿತಿ UDISPCTX_TYPE_DISPLAY_ಉದ್ದಳತೆ:
            ಹಿಂತಿರುಗಿ name_ಉದ್ದಳತೆ;
        ಸ್ಥಿತಿ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ಹಿಂತಿರುಗಿ substitute;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ಆದರೆ ( ಫಲಿತಾಂಶ.length() > 0 && u_islower(ಫಲಿತಾಂಶ.char32At(0)) && capitalizationBrkIter!= ಶೂನ್ಯಸೂಚಕ &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_ದ_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ಸ್ಥಿರ UMutex capitalization_Brk_Iter_ಬೀಗ;
        Mutex ಬೀಗ(&capitalization_Brk_Iter_ಬೀಗ);
        ಫಲಿತಾಂಶ.ಗಮ್ಯ_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::localeDisplayName(ಸ್ಥಿರಾಂಕ Locale& loc,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
  ಆದರೆ (loc.isBogus()) {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  UnicodeString ಫಲಿತಾಂಶ_Name;

  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang = loc.getLanguage();
  ಆದರೆ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script = loc.getScript();
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* country = loc.getCountry();
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant = loc.getVariant();

  ತಾರ್ಕಿಕ hasScript = uprv_strlen(script) > 0;
  ತಾರ್ಕಿಕ hasCountry = uprv_strlen(country) > 0;
  ತಾರ್ಕಿಕ hasVariant = uprv_strlen(variant) > 0;

  ಆದರೆ (dialectHandling == ULDN_DIALECT_ಹೆಸರುಗಳು) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    CharString ಬಫರ್;
    ಮಾಡು { // loop construct is so we can break early out of search
      ಆದರೆ (hasScript && hasCountry) {
        ಬಫರ್.append(lang, ಸ್ಥಿತಿ_2)
              .append('_', ಸ್ಥಿತಿ_2)
              .append(script, ಸ್ಥಿತಿ_2)
              .append('_', ಸ್ಥಿತಿ_2)
              .append(country, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
          localeIdName(ಬಫರ್.ದತ್ತಾಂಶ(), ಫಲಿತಾಂಶ_Name, ಸುಳ್ಳು);
          ಆದರೆ (!ಫಲಿತಾಂಶ_Name.isBogus()) {
            hasScript = ಸುಳ್ಳು;
            hasCountry = ಸುಳ್ಳು;
            ನಿಲ್ಲಿಸು;
          }
        }
      }
      ಆದರೆ (hasScript) {
        ಬಫರ್.append(lang, ಸ್ಥಿತಿ_2)
              .append('_', ಸ್ಥಿತಿ_2)
              .append(script, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
          localeIdName(ಬಫರ್.ದತ್ತಾಂಶ(), ಫಲಿತಾಂಶ_Name, ಸುಳ್ಳು);
          ಆದರೆ (!ಫಲಿತಾಂಶ_Name.isBogus()) {
            hasScript = ಸುಳ್ಳು;
            ನಿಲ್ಲಿಸು;
          }
        }
      }
      ಆದರೆ (hasCountry) {
        ಬಫರ್.append(lang, ಸ್ಥಿತಿ_2)
              .append('_', ಸ್ಥಿತಿ_2)
              .append(country, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
          localeIdName(ಬಫರ್.ದತ್ತಾಂಶ(), ಫಲಿತಾಂಶ_Name, ಸುಳ್ಳು);
          ಆದರೆ (!ಫಲಿತಾಂಶ_Name.isBogus()) {
            hasCountry = ಸುಳ್ಳು;
            ನಿಲ್ಲಿಸು;
          }
        }
      }
    } ಆಗ (ಸುಳ್ಳು);
  }
  ಆದರೆ (ಫಲಿತಾಂಶ_Name.isBogus() || ಫಲಿತಾಂಶ_Name.is_ಖಾಲಿಗಳು()) {
    localeIdName(lang, ಫಲಿತಾಂಶ_Name, substitute == UDISPCTX_SUBSTITUTE);
    ಆದರೆ (ಫಲಿತಾಂಶ_Name.isBogus()) {
      ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  UnicodeString ಫಲಿತಾಂಶ_Remainder;
  UnicodeString ತಾತ್ಕಾಲಿಕ;
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

  ಆದರೆ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, ತಾತ್ಕಾಲಿಕ, ನಿಜ);
    ಆದರೆ (script_str.isBogus()) {
      ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಫಲಿತಾಂಶ_Remainder.append(script_str);
  }
  ಆದರೆ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, ತಾತ್ಕಾಲಿಕ, ನಿಜ);
    ಆದರೆ (region_str.isBogus()) {
      ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    appendWithSep(ಫಲಿತಾಂಶ_Remainder, region_str);
  }
  ಆದರೆ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, ತಾತ್ಕಾಲಿಕ, ನಿಜ);
    ಆದರೆ (variant_str.isBogus()) {
      ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    appendWithSep(ಫಲಿತಾಂಶ_Remainder, variant_str);
  }
  ಫಲಿತಾಂಶ_Remainder.ಕಂಡುಹಿಡಿ_And_Replace(formatOpenParen, formatReplaceOpenParen);
  ಫಲಿತಾಂಶ_Remainder.ಕಂಡುಹಿಡಿ_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_ಸೂಚಕ<StringEnumeration> e(loc.createKeywords(ಸ್ಥಿತಿ_2));
  ಆದರೆ (e.isValid() && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
    UnicodeString temp2;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ;
    ಆಗ ((ಕೀಲಿ = e->next((int32_t*)ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)) != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ವಯಂಚಾಲಿತ value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಕೀಲಿ_Display_Name(ಕೀಲಿ, ತಾತ್ಕಾಲಿಕ, ನಿಜ);
      ತಾತ್ಕಾಲಿಕ.ಕಂಡುಹಿಡಿ_And_Replace(formatOpenParen, formatReplaceOpenParen);
      ತಾತ್ಕಾಲಿಕ.ಕಂಡುಹಿಡಿ_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value.ದತ್ತಾಂಶ(), temp2, ನಿಜ);
      temp2.ಕಂಡುಹಿಡಿ_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.ಕಂಡುಹಿಡಿ_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ಆದರೆ (temp2 != UnicodeString(value.ದತ್ತಾಂಶ(), -1, US_INV)) {
        appendWithSep(ಫಲಿತಾಂಶ_Remainder, temp2);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತಾತ್ಕಾಲಿಕ != UnicodeString(ಕೀಲಿ, -1, US_INV)) {
        UnicodeString temp3;
        ಕೀಲಿ_Type_Format.format(ತಾತ್ಕಾಲಿಕ, temp2, temp3, ಸ್ಥಿತಿ_2);
        appendWithSep(ಫಲಿತಾಂಶ_Remainder, temp3);
      } ಇಲ್ಲದಿದ್ದರೆ {
        appendWithSep(ಫಲಿತಾಂಶ_Remainder, ತಾತ್ಕಾಲಿಕ)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ಆದರೆ (!ಫಲಿತಾಂಶ_Remainder.is_ಖಾಲಿಗಳು()) {
    format.format(ಫಲಿತಾಂಶ_Name, ಫಲಿತಾಂಶ_Remainder, ಫಲಿತಾಂಶ.remove(), ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ adjustForUsageAndContext(kCapContextUsageLanguage, ಫಲಿತಾಂಶ);
  }

  ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ_Name;
  ಹಿಂತಿರುಗಿ adjustForUsageAndContext(kCapContextUsageLanguage, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::appendWithSep(UnicodeString& ಬಫರ್ U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ UnicodeString& src) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಬಫರ್.is_ಖಾಲಿಗಳು()) {
        ಬಫರ್.ಗಣ_ಗಮ್ಯ(src);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ UnicodeString *ಮೌಲ್ಯಗಳು[2] = { &ಬಫರ್, &src };
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(ಮೌಲ್ಯಗಳು, 2, ಬಫರ್, ಶೂನ್ಯಸೂಚಕ, 0, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::localeDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ localeDisplayName(Locale(localeId), ಫಲಿತಾಂಶ);
}

//ಖಾಸಗಿ
UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::localeIdName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                                     UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND, ತಾರ್ಕಿಕ substitute) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
        lang_ದತ್ತಾಂಶ.getNoFallback("Languages%short", localeId, ಫಲಿತಾಂಶ);
        ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }
    lang_ದತ್ತಾಂಶ.getNoFallback("Languages", localeId, ಫಲಿತಾಂಶ);
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus() && uprv_strchr(localeId, '_') == ಶೂನ್ಯಸೂಚಕ) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* canonLocId = canonLocale.getName();
        ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
            lang_ದತ್ತಾಂಶ.getNoFallback("Languages%short", canonLocId, ಫಲಿತಾಂಶ);
            ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        }
        lang_ದತ್ತಾಂಶ.getNoFallback("Languages", canonLocId, ಫಲಿತಾಂಶ);
    }
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::languageDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang,
                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ = UnicodeString(lang, -1, US_INV);
    }
    ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
        lang_ದತ್ತಾಂಶ.getNoFallback("Languages%short", lang, ಫಲಿತಾಂಶ);
        ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
            ಹಿಂತಿರುಗಿ adjustForUsageAndContext(kCapContextUsageLanguage, ಫಲಿತಾಂಶ);
        }
    }
    lang_ದತ್ತಾಂಶ.getNoFallback("Languages", lang, ಫಲಿತಾಂಶ);
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* canonLocId = canonLocale.getName();
        ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
            lang_ದತ್ತಾಂಶ.getNoFallback("Languages%short", canonLocId, ಫಲಿತಾಂಶ);
            ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
                ಹಿಂತಿರುಗಿ adjustForUsageAndContext(kCapContextUsageLanguage, ಫಲಿತಾಂಶ);
            }
        }
        lang_ದತ್ತಾಂಶ.getNoFallback("Languages", canonLocId, ಫಲಿತಾಂಶ);
    }
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ಹಿಂತಿರುಗಿ adjustForUsageAndContext(kCapContextUsageLanguage, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                          ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
        lang_ದತ್ತಾಂಶ.getNoFallback("Scripts%short", script, ಫಲಿತಾಂಶ);
        ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
            ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(kCapContextUsageScript, ಫಲಿತಾಂಶ);
        }
    }
    ಆದರೆ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ದತ್ತಾಂಶ.get("Scripts", script, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        lang_ದತ್ತಾಂಶ.getNoFallback("Scripts", script, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(kCapContextUsageScript, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::scriptDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ scriptDisplayName(script, ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::scriptDisplayName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ scriptDisplayName(uscript_getName(script_ಸಂಕೇತ), ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                          ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
         region_ದತ್ತಾಂಶ.getNoFallback("Countries%short", region, ಫಲಿತಾಂಶ);
        ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
            ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(kCapContextUsageTerritory, ಫಲಿತಾಂಶ);
        }
    }
    ಆದರೆ (substitute == UDISPCTX_SUBSTITUTE) {
        region_ದತ್ತಾಂಶ.get("Countries", region, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        region_ದತ್ತಾಂಶ.getNoFallback("Countries", region, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(kCapContextUsageTerritory, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::regionDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                                          UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ regionDisplayName(region, ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}


UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                           ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ {
    // don't have a resource for short variant names
    ಆದರೆ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ದತ್ತಾಂಶ.get("Variants", variant, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        lang_ದತ್ತಾಂಶ.getNoFallback("Variants", variant, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(kCapContextUsageVariant, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::variantDisplayName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
                                           UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ variantDisplayName(variant, ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                       ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ {
    // don't have a resource for short key names
    ಆದರೆ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ದತ್ತಾಂಶ.get("Keys", ಕೀಲಿ, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        lang_ದತ್ತಾಂಶ.getNoFallback("Keys", ಕೀಲಿ, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                       UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಕೀಲಿ_Display_Name(ಕೀಲಿ, ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                            ತಾರ್ಕಿಕ skipAdjust) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_ಸಂಕೇತ sts = U_ZERO_ERROR;
        UnicodeString ustr_ಮೌಲ್ಯ(value, -1, US_INV);
        int32_t len;
        ಸ್ಥಿರಾಂಕ char16_t *currencyName = ucurr_getName(ustr_ಮೌಲ್ಯ.get_Terminated_ಬಫರ್(),
            locale.get_ತಳ_Name(), UCURR_LONG_NAME, ಶೂನ್ಯಸೂಚಕ /* isChoiceFormat */, &len, &sts);
        ಆದರೆ (U_FAILURE(sts)) {
            // Return the value as is on failure
            ಫಲಿತಾಂಶ = ustr_ಮೌಲ್ಯ;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(currencyName, len);
        ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, ಫಲಿತಾಂಶ);
    }

    ಆದರೆ (name_ಉದ್ದಳತೆ == UDISPCTX_ಉದ್ದಳತೆ_SHORT) {
        lang_ದತ್ತಾಂಶ.getNoFallback("Types%short", ಕೀಲಿ, value, ಫಲಿತಾಂಶ);
        ಆದರೆ (!ಫಲಿತಾಂಶ.isBogus()) {
            ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, ಫಲಿತಾಂಶ);
        }
    }
    ಆದರೆ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ದತ್ತಾಂಶ.get("Types", ಕೀಲಿ, value, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        lang_ದತ್ತಾಂಶ.getNoFallback("Types", ಕೀಲಿ, value, ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ skipAdjust? ಫಲಿತಾಂಶ: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, ಫಲಿತಾಂಶ);
}

UnicodeString&
Locale_Display_ಹೆಸರುಗಳು_Impl::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value, ಫಲಿತಾಂಶ, ಸುಳ್ಳು);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ಹೆಸರುಗಳು*
Locale_Display_ಹೆಸರುಗಳು::createInstance(ಸ್ಥಿರಾಂಕ Locale& locale,
                                   UDialectHandling dialectHandling) {
    ಹಿಂತಿರುಗಿ ಹೊಸ Locale_Display_ಹೆಸರುಗಳು_Impl(locale, dialectHandling);
}

Locale_Display_ಹೆಸರುಗಳು*
Locale_Display_ಹೆಸರುಗಳು::createInstance(ಸ್ಥಿರಾಂಕ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ಆದರೆ (contexts == ಶೂನ್ಯಸೂಚಕ) {
        length = 0;
    }
    ಹಿಂತಿರುಗಿ ಹೊಸ Locale_Display_ಹೆಸರುಗಳು_Impl(locale, contexts, length);
}

U_NAMESPACE_ಕೊನೆ

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_ಹೆಸರುಗಳು * U_EXPORT2
uldn_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * locale,
          UDialectHandling dialectHandling,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
    locale = uloc_getDefault();
  }
  ಹಿಂತಿರುಗಿ (ULocale_Display_ಹೆಸರುಗಳು *)Locale_Display_ಹೆಸರುಗಳು::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_ಹೆಸರುಗಳು * U_EXPORT2
uldn_openForContext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
    locale = uloc_getDefault();
  }
  ಹಿಂತಿರುಗಿ (ULocale_Display_ಹೆಸರುಗಳು *)Locale_Display_ಹೆಸರುಗಳು::createInstance(Locale(locale), contexts, length);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uldn_close(ULocale_Display_ಹೆಸರುಗಳು *ldn) {
  ಅಳಿಸು (Locale_Display_ಹೆಸರುಗಳು *)ldn;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uldn_getLocale(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು* ldn U_LIFETIME_BOUND) {
  ಆದರೆ (ldn) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->getLocale().getName();
  }
  ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn) {
  ಆದರೆ (ldn) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->getDialectHandling();
  }
  ಹಿಂತಿರುಗಿ ULDN_STANDARD_ಹೆಸರುಗಳು;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
              UDisplayContextType type,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ (UDisplayContext)0;
  }
  ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                       char16_t *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || locale == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->localeDisplayName(locale, ತಾತ್ಕಾಲಿಕ);
  ಆದರೆ (ತಾತ್ಕಾಲಿಕ.isBogus()) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lang,
                         char16_t *ಫಲಿತಾಂಶ,
                         int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || lang == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->languageDisplayName(lang, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script,
                       char16_t *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || script == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->scriptDisplayName(script, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_script_ಸಂಕೇತ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                           UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                           char16_t *ಫಲಿತಾಂಶ,
                           int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಹಿಂತಿರುಗಿ uldn_scriptDisplayName(ldn, uscript_getName(script_ಸಂಕೇತ), ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                       char16_t *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || region == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->regionDisplayName(region, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variant,
                        char16_t *ಫಲಿತಾಂಶ,
                        int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || variant == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->variantDisplayName(variant, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                    char16_t *ಫಲಿತಾಂಶ,
                    int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || ಕೀಲಿ == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0) || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->ಕೀಲಿ_Display_Name(ಕೀಲಿ, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uldn_ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *value,
                         char16_t *ಫಲಿತಾಂಶ,
                         int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
  ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ (ldn == ಶೂನ್ಯಸೂಚಕ || ಕೀಲಿ == ಶೂನ್ಯಸೂಚಕ || value == ಶೂನ್ಯಸೂಚಕ || (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ && max_ಫಲಿತಾಂಶ_ಗಾತ್ರ > 0)
      || max_ಫಲಿತಾಂಶ_ಗಾತ್ರ < 0) {
    *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  UnicodeString ತಾತ್ಕಾಲಿಕ(ಫಲಿತಾಂಶ, 0, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ);
  ((ಸ್ಥಿರಾಂಕ Locale_Display_ಹೆಸರುಗಳು *)ldn)->ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value, ತಾತ್ಕಾಲಿಕ);
  ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.extract(ಫಲಿತಾಂಶ, max_ಫಲಿತಾಂಶ_ಗಾತ್ರ, *p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ
