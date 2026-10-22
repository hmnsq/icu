// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locdspnm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/simpleformatter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udisplaycontext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucurr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ureslocs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
ਵਰਗ ICUData_ਸਾਰਣੀ {
    ਅਚਲ ਅੱਖਰ* ਅਚਲ ਰਸਤਾ;
    Locale locale;

ਜਨਤਕ:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_ਸਾਰਣੀ(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ Locale& locale);
    ~ICUData_ਸਾਰਣੀ() = ਮੂਲ;

    ਅਚਲ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
    UnicodeString& get(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;

    UnicodeString& getNoFallback(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                                 UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
    UnicodeString& getNoFallback(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                                 UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
};

ਸਤਰਅੰਦਰ UnicodeString& ICUData_ਸਾਰਣੀ::get(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                                        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ get(ਸਾਰਣੀ_ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, ਵਸਤੂ_ਕੁੰਜੀ, ਨਤੀਜਾ);
}

ਸਤਰਅੰਦਰ UnicodeString& ICUData_ਸਾਰਣੀ::getNoFallback(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                                                  UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ getNoFallback(ਸਾਰਣੀ_ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, ਵਸਤੂ_ਕੁੰਜੀ, ਨਤੀਜਾ);
}

ICUData_ਸਾਰਣੀ::ICUData_ਸਾਰਣੀ(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ Locale& locale)
    : ਰਸਤਾ(ਰਸਤਾ), locale(locale)
{
    U_ASSERT(ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ);
}

ਅਚਲ Locale&
ICUData_ਸਾਰਣੀ::getLocale() U_LIFETIME_BOUND {
  ਵਾਪਸ locale;
}

UnicodeString&
ICUData_ਸਾਰਣੀ::get(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                  UnicodeString &ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  int32_t len = 0;

  ਅਚਲ char16_t *s = uloc_get_ਸਾਰਣੀ_String_With_Fallback(ਰਸਤਾ, locale.getName(),
                                                   ਸਾਰਣੀ_ਕੁੰਜੀ, sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਵਸਤੂ_ਕੁੰਜੀ,
                                                   &len, &ਸਥਿਤੀ);
  ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && len > 0) {
    ਵਾਪਸ ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(s, len);
  }
  ਵਾਪਸ ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(UnicodeString(ਵਸਤੂ_ਕੁੰਜੀ, -1, US_INV));
}

UnicodeString&
ICUData_ਸਾਰਣੀ::getNoFallback(ਅਚਲ ਅੱਖਰ* ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਵਸਤੂ_ਕੁੰਜੀ,
                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  int32_t len = 0;

  ਅਚਲ char16_t *s = uloc_get_ਸਾਰਣੀ_String_With_Fallback(ਰਸਤਾ, locale.getName(),
                                                   ਸਾਰਣੀ_ਕੁੰਜੀ, sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਵਸਤੂ_ਕੁੰਜੀ,
                                                   &len, &ਸਥਿਤੀ);
  ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
    ਵਾਪਸ ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(s, len);
  }

  ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  ਵਾਪਸ ਨਤੀਜਾ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ਨਾਂ::~Locale_Display_ਨਾਂ() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ਜੇ 0  // currently unused

ਵਰਗ Default_Locale_Display_ਨਾਂ : ਜਨਤਕ Locale_Display_ਨਾਂ {
  UDialectHandling dialectHandling;

ਜਨਤਕ:
  // constructor
  Default_Locale_Display_ਨਾਂ(UDialectHandling dialectHandling);

  ਆਭਾਸੀ ~Default_Locale_Display_ਨਾਂ();

  ਆਭਾਸੀ ਅਚਲ Locale& getLocale() ਅਚਲ U_LIFETIME_BOUND;
  ਆਭਾਸੀ UDialectHandling getDialectHandling() ਅਚਲ;

  ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ Locale& locale,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ ਅੱਖਰ* localeId,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& languageDisplayName(ਅਚਲ ਅੱਖਰ* lang,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& scriptDisplayName(UScript_ਕੋਡ script_ਕੋਡ,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& variantDisplayName(ਅਚਲ ਅੱਖਰ* variant,
                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
  ਆਭਾਸੀ UnicodeString& ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                             ਅਚਲ ਅੱਖਰ* value,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
};

Default_Locale_Display_ਨਾਂ::Default_Locale_Display_ਨਾਂ(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_ਨਾਂ::~Default_Locale_Display_ਨਾਂ() {
}

ਅਚਲ Locale&
Default_Locale_Display_ਨਾਂ::getLocale() ਅਚਲ U_LIFETIME_BOUND {
  ਵਾਪਸ Locale::get_ਜੜ੍ਹ();
}

UDialectHandling
Default_Locale_Display_ਨਾਂ::getDialectHandling() ਅਚਲ {
  ਵਾਪਸ dialectHandling;
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::localeDisplayName(ਅਚਲ Locale& locale,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::localeDisplayName(ਅਚਲ ਅੱਖਰ* localeId,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::languageDisplayName(ਅਚਲ ਅੱਖਰ* lang,
                                               UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::scriptDisplayName(UScript_ਕੋਡ script_ਕੋਡ,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਅਚਲ ਅੱਖਰ* name = uscript_getName(script_ਕੋਡ);
  ਜੇ (name) {
    ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(name, -1, US_INV);
  }
  ਵਾਪਸ ਨਤੀਜਾ.remove();
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::variantDisplayName(ਅਚਲ ਅੱਖਰ* variant,
                                              UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(ਕੁੰਜੀ, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ਨਾਂ::ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* /*ਕੁੰਜੀ*/,
                                               ਅਚਲ ਅੱਖਰ* value,
                                               UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(value, -1, US_INV);
}

#ਜੇ_ਅੰਤ  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

ਵਰਗ Locale_Display_ਨਾਂ_Impl : ਜਨਤਕ Locale_Display_ਨਾਂ {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_ਸਾਰਣੀ lang_ਅੰਕੜੇ;
    ICUData_ਸਾਰਣੀ region_ਅੰਕੜੇ;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ਕੁੰਜੀ_Type_Format;
    UDisplayContext capitalizationContext;
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ਨਹੀਂਤਾਂ
    UObject* capitalizationBrkIter;
#ਜੇ_ਅੰਤ
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_ਲੰਮਾਈ;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ਗਿਣਤੀ CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_ਕੁੰਜੀ,
        k_Cap_Context_Usage_ਕੁੰਜੀ_ਮੁੱਲ,
        k_Cap_Context_Usage_ਗਿਣਤੀ
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     ਤਾਰਕਿਕ fCapitalization[k_Cap_Context_Usage_ਗਿਣਤੀ];

ਜਨਤਕ:
    // constructor
    Locale_Display_ਨਾਂ_Impl(ਅਚਲ Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_ਨਾਂ_Impl(ਅਚਲ Locale& locale, UDisplayContext *contexts, int32_t length);
    ਆਭਾਸੀ ~Locale_Display_ਨਾਂ_Impl();

    ਆਭਾਸੀ ਅਚਲ Locale& getLocale() ਅਚਲ U_LIFETIME_BOUND override;
    ਆਭਾਸੀ UDialectHandling getDialectHandling() ਅਚਲ override;
    ਆਭਾਸੀ UDisplayContext getContext(UDisplayContextType type) ਅਚਲ override;

    ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ Locale& locale,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ ਅੱਖਰ* localeId,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString&
    languageDisplayName(ਅਚਲ ਅੱਖਰ* lang, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString& scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString& scriptDisplayName(UScript_ਕੋਡ script_ਕੋਡ,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString& regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString&
    variantDisplayName(ਅਚਲ ਅੱਖਰ* variant, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString& ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString&
    ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* value,
                        UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ override;

ਨਿਜੀ:
    UnicodeString& localeIdName(ਅਚਲ ਅੱਖਰ* localeId, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                ਤਾਰਕਿਕ substitute) ਅਚਲ;
    UnicodeString& appendWithSep(UnicodeString& ਬਫਰ U_LIFETIME_BOUND,
                                 ਅਚਲ UnicodeString& src) ਅਚਲ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ;
    UnicodeString& scriptDisplayName(ਅਚਲ ਅੱਖਰ* script, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                     ਤਾਰਕਿਕ skipAdjust) ਅਚਲ;
    UnicodeString& regionDisplayName(ਅਚਲ ਅੱਖਰ* region, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                     ਤਾਰਕਿਕ skipAdjust) ਅਚਲ;
    UnicodeString& variantDisplayName(ਅਚਲ ਅੱਖਰ* variant, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                      ਤਾਰਕਿਕ skipAdjust) ਅਚਲ;
    UnicodeString& ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                  ਤਾਰਕਿਕ skipAdjust) ਅਚਲ;
    UnicodeString& ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* value,
                                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                       ਤਾਰਕਿਕ skipAdjust) ਅਚਲ;
    ਖਾਲੀ initialize();

    ਬਣਤਰ CapitalizationContextSink;
};

Locale_Display_ਨਾਂ_Impl::Locale_Display_ਨਾਂ_Impl(ਅਚਲ Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_ਅੰਕੜੇ(U_ICUDATA_LANG, locale)
    , region_ਅੰਕੜੇ(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(ਖਾਲੀ_ਸੂਚਕ)
    , name_ਲੰਮਾਈ(UDISPCTX_ਲੰਮਾਈ_ਭਰਿਆ)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_ਨਾਂ_Impl::Locale_Display_ਨਾਂ_Impl(ਅਚਲ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_ਨਾਂ)
    , lang_ਅੰਕੜੇ(U_ICUDATA_LANG, locale)
    , region_ਅੰਕੜੇ(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(ਖਾਲੀ_ਸੂਚਕ)
    , name_ਲੰਮਾਈ(UDISPCTX_ਲੰਮਾਈ_ਭਰਿਆ)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ਜਦੋਂ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ਚੋਣ (selector) {
            ਹਾਲਤ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ਰੋਕੋ;
            ਹਾਲਤ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ਰੋਕੋ;
            ਹਾਲਤ UDISPCTX_TYPE_DISPLAY_ਲੰਮਾਈ:
                name_ਲੰਮਾਈ = value;
                ਰੋਕੋ;
            ਹਾਲਤ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ਰੋਕੋ;
            ਮੂਲ:
                ਰੋਕੋ;
        }
    }
    initialize();
}

ਬਣਤਰ Locale_Display_ਨਾਂ_Impl::CapitalizationContextSink : ਜਨਤਕ ResourceSink {
    ਤਾਰਕਿਕ hasCapitalizationUsage;
    Locale_Display_ਨਾਂ_Impl& ਮਾਪਾ;

    CapitalizationContextSink(Locale_Display_ਨਾਂ_Impl& ਮਾਪਾ_2)
      : hasCapitalizationUsage(ਝੂਠ), ਮਾਪਾ(ਮਾਪਾ_2) {}
    ਆਭਾਸੀ ~CapitalizationContextSink();

    ਆਭਾਸੀ ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &value, UBool /*noFallback*/,
            UError_ਕੋਡ &error_ਕੋਡ) override {
        Resource_ਸਾਰਣੀ contexts = value.get_ਸਾਰਣੀ(error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਲਈ (ਪੂਰਨ i = 0; contexts.get_ਕੁੰਜੀ_And_ਮੁੱਲ(i, ਕੁੰਜੀ, value); ++i) {

            CapContextUsage usageEnum;
            ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ਕੁੰਜੀ;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ਕੁੰਜੀ_ਮੁੱਲ;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ਨਹੀਂਤਾਂ {
                ਜਾਰੀ;
            }

            int32_t len = 0;
            ਅਚਲ int32_t* int_ਸਦਿਸ਼ = value.get_Int_ਸਦਿਸ਼(len, error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
            ਜੇ (len < 2) { ਜਾਰੀ; }

            int32_t titlecaseInt = (ਮਾਪਾ.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_ਸਦਿਸ਼[0] : int_ਸਦਿਸ਼[1];
            ਜੇ (titlecaseInt == 0) { ਜਾਰੀ; }

            ਮਾਪਾ.fCapitalization[usageEnum] = ਸੱਚ;
            hasCapitalizationUsage = ਸੱਚ;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_ਨਾਂ_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

ਖਾਲੀ
Locale_Display_ਨਾਂ_Impl::initialize() {
    Locale_Display_ਨਾਂ_Impl* nonConstThis = ਇਹ;
    nonConstThis->locale = lang_ਅੰਕੜੇ.getLocale() == Locale::get_ਜੜ੍ਹ()
        ? region_ਅੰਕੜੇ.getLocale()
        : lang_ਅੰਕੜੇ.getLocale();

    UnicodeString sep;
    lang_ਅੰਕੜੇ.getNoFallback("localeDisplayPattern", "separator", sep);
    ਜੇ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    separatorFormat.apply_ਨਮੂਨਾ_Min_Max_Arguments(sep, 2, 2, ਸਥਿਤੀ);

    UnicodeString ਨਮੂਨਾ;
    lang_ਅੰਕੜੇ.getNoFallback("localeDisplayPattern", "pattern", ਨਮੂਨਾ);
    ਜੇ (ਨਮੂਨਾ.isBogus()) {
        ਨਮੂਨਾ = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_ਨਮੂਨਾ_Min_Max_Arguments(ਨਮੂਨਾ, 2, 2, ਸਥਿਤੀ);
    ਜੇ (ਨਮੂਨਾ.ਸੂਚਕਾਂਕ_ਦਾ(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ਨਹੀਂਤਾਂ {
        formatOpenParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.ਸੈੱਟ_ਮੰਜ਼ਿਲ(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_ਨਮੂਨਾ;
    lang_ਅੰਕੜੇ.get("localeDisplayPattern", "keyTypePattern", kt_ਨਮੂਨਾ);
    ਜੇ (kt_ਨਮੂਨਾ.isBogus()) {
        kt_ਨਮੂਨਾ = UnicodeString("{0}={1}", -1, US_INV);
    }
    ਕੁੰਜੀ_Type_Format.apply_ਨਮੂਨਾ_Min_Max_Arguments(kt_ਨਮੂਨਾ, 2, 2, ਸਥਿਤੀ);

    uprv_memset(fCapitalization, 0, ਆਕਾਰ(fCapitalization));
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    ਤਾਰਕਿਕ needBrkIter = ਝੂਠ;
    ਜੇ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_ਸੂਚਕ resource(ures_open(ਖਾਲੀ_ਸੂਚਕ, locale.getName(), &ਸਥਿਤੀ));
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        CapitalizationContextSink sink(*ਇਹ);
        ures_get_All_ਚੀਜ਼ਾਂ_With_Fallback(resource.getAlias(), "contextTransforms", sink, ਸਥਿਤੀ);
        ਜੇ (ਸਥਿਤੀ == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            ਸਥਿਤੀ = U_ZERO_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ਜੇ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_ਦਾ_SENTENCE) {
        ਸਥਿਤੀ = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਮਿਟਾਓ capitalizationBrkIter;
            capitalizationBrkIter = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
#ਜੇ_ਅੰਤ
}

Locale_Display_ਨਾਂ_Impl::~Locale_Display_ਨਾਂ_Impl() {
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    ਮਿਟਾਓ capitalizationBrkIter;
#ਜੇ_ਅੰਤ
}

ਅਚਲ Locale&
Locale_Display_ਨਾਂ_Impl::getLocale() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ locale;
}

UDialectHandling
Locale_Display_ਨਾਂ_Impl::getDialectHandling() ਅਚਲ {
    ਵਾਪਸ dialectHandling;
}

UDisplayContext
Locale_Display_ਨਾਂ_Impl::getContext(UDisplayContextType type) ਅਚਲ {
    ਚੋਣ (type) {
        ਹਾਲਤ UDISPCTX_TYPE_DIALECT_HANDLING:
            ਵਾਪਸ static_cast<UDisplayContext>(dialectHandling);
        ਹਾਲਤ UDISPCTX_TYPE_CAPITALIZATION:
            ਵਾਪਸ capitalizationContext;
        ਹਾਲਤ UDISPCTX_TYPE_DISPLAY_ਲੰਮਾਈ:
            ਵਾਪਸ name_ਲੰਮਾਈ;
        ਹਾਲਤ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ਵਾਪਸ substitute;
        ਮੂਲ:
            ਰੋਕੋ;
    }
    ਵਾਪਸ static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ਜੇ ( ਨਤੀਜਾ.length() > 0 && u_islower(ਨਤੀਜਾ.char32At(0)) && capitalizationBrkIter!= ਖਾਲੀ_ਸੂਚਕ &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_ਦਾ_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ਸਥਿਰ UMutex capitalization_Brk_Iter_ਤਾਲਾ;
        Mutex ਤਾਲਾ(&capitalization_Brk_Iter_ਤਾਲਾ);
        ਨਤੀਜਾ.ਮੰਜ਼ਿਲ_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::localeDisplayName(ਅਚਲ Locale& loc,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
  ਜੇ (loc.isBogus()) {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  UnicodeString ਨਤੀਜਾ_Name;

  ਅਚਲ ਅੱਖਰ* lang = loc.getLanguage();
  ਜੇ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ਅਚਲ ਅੱਖਰ* script = loc.getScript();
  ਅਚਲ ਅੱਖਰ* country = loc.getCountry();
  ਅਚਲ ਅੱਖਰ* variant = loc.getVariant();

  ਤਾਰਕਿਕ hasScript = uprv_strlen(script) > 0;
  ਤਾਰਕਿਕ hasCountry = uprv_strlen(country) > 0;
  ਤਾਰਕਿਕ hasVariant = uprv_strlen(variant) > 0;

  ਜੇ (dialectHandling == ULDN_DIALECT_ਨਾਂ) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    CharString ਬਫਰ;
    ਕਰੋ { // loop construct is so we can break early out of search
      ਜੇ (hasScript && hasCountry) {
        ਬਫਰ.append(lang, ਸਥਿਤੀ)
              .append('_', ਸਥਿਤੀ)
              .append(script, ਸਥਿਤੀ)
              .append('_', ਸਥਿਤੀ)
              .append(country, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
          localeIdName(ਬਫਰ.ਅੰਕੜੇ(), ਨਤੀਜਾ_Name, ਝੂਠ);
          ਜੇ (!ਨਤੀਜਾ_Name.isBogus()) {
            hasScript = ਝੂਠ;
            hasCountry = ਝੂਠ;
            ਰੋਕੋ;
          }
        }
      }
      ਜੇ (hasScript) {
        ਬਫਰ.append(lang, ਸਥਿਤੀ)
              .append('_', ਸਥਿਤੀ)
              .append(script, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
          localeIdName(ਬਫਰ.ਅੰਕੜੇ(), ਨਤੀਜਾ_Name, ਝੂਠ);
          ਜੇ (!ਨਤੀਜਾ_Name.isBogus()) {
            hasScript = ਝੂਠ;
            ਰੋਕੋ;
          }
        }
      }
      ਜੇ (hasCountry) {
        ਬਫਰ.append(lang, ਸਥਿਤੀ)
              .append('_', ਸਥਿਤੀ)
              .append(country, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
          localeIdName(ਬਫਰ.ਅੰਕੜੇ(), ਨਤੀਜਾ_Name, ਝੂਠ);
          ਜੇ (!ਨਤੀਜਾ_Name.isBogus()) {
            hasCountry = ਝੂਠ;
            ਰੋਕੋ;
          }
        }
      }
    } ਜਦੋਂ (ਝੂਠ);
  }
  ਜੇ (ਨਤੀਜਾ_Name.isBogus() || ਨਤੀਜਾ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    localeIdName(lang, ਨਤੀਜਾ_Name, substitute == UDISPCTX_SUBSTITUTE);
    ਜੇ (ਨਤੀਜਾ_Name.isBogus()) {
      ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  UnicodeString ਨਤੀਜਾ_Remainder;
  UnicodeString ਆਰਜ਼ੀ;
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

  ਜੇ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, ਆਰਜ਼ੀ, ਸੱਚ);
    ਜੇ (script_str.isBogus()) {
      ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਤੀਜਾ_Remainder.append(script_str);
  }
  ਜੇ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, ਆਰਜ਼ੀ, ਸੱਚ);
    ਜੇ (region_str.isBogus()) {
      ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    appendWithSep(ਨਤੀਜਾ_Remainder, region_str);
  }
  ਜੇ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, ਆਰਜ਼ੀ, ਸੱਚ);
    ਜੇ (variant_str.isBogus()) {
      ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    appendWithSep(ਨਤੀਜਾ_Remainder, variant_str);
  }
  ਨਤੀਜਾ_Remainder.ਲੱਭੋ_And_Replace(formatOpenParen, formatReplaceOpenParen);
  ਨਤੀਜਾ_Remainder.ਲੱਭੋ_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_ਸੂਚਕ<StringEnumeration> e(loc.createKeywords(ਸਥਿਤੀ));
  ਜੇ (e.isValid() && U_SUCCESS(ਸਥਿਤੀ)) {
    UnicodeString temp2;
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ;
    ਜਦੋਂ ((ਕੁੰਜੀ = e->next((int32_t*)ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਸਵੈਚਾਲਿਤ value = loc.get_Keyword_ਮੁੱਲ<CharString>(ਕੁੰਜੀ, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਕੁੰਜੀ_Display_Name(ਕੁੰਜੀ, ਆਰਜ਼ੀ, ਸੱਚ);
      ਆਰਜ਼ੀ.ਲੱਭੋ_And_Replace(formatOpenParen, formatReplaceOpenParen);
      ਆਰਜ਼ੀ.ਲੱਭੋ_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਕੁੰਜੀ, value.ਅੰਕੜੇ(), temp2, ਸੱਚ);
      temp2.ਲੱਭੋ_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.ਲੱਭੋ_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ਜੇ (temp2 != UnicodeString(value.ਅੰਕੜੇ(), -1, US_INV)) {
        appendWithSep(ਨਤੀਜਾ_Remainder, temp2);
      } ਨਹੀਂਤਾਂ ਜੇ (ਆਰਜ਼ੀ != UnicodeString(ਕੁੰਜੀ, -1, US_INV)) {
        UnicodeString temp3;
        ਕੁੰਜੀ_Type_Format.format(ਆਰਜ਼ੀ, temp2, temp3, ਸਥਿਤੀ);
        appendWithSep(ਨਤੀਜਾ_Remainder, temp3);
      } ਨਹੀਂਤਾਂ {
        appendWithSep(ਨਤੀਜਾ_Remainder, ਆਰਜ਼ੀ)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ਜੇ (!ਨਤੀਜਾ_Remainder.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    format.format(ਨਤੀਜਾ_Name, ਨਤੀਜਾ_Remainder, ਨਤੀਜਾ.remove(), ਸਥਿਤੀ);
    ਵਾਪਸ adjustForUsageAndContext(kCapContextUsageLanguage, ਨਤੀਜਾ);
  }

  ਨਤੀਜਾ = ਨਤੀਜਾ_Name;
  ਵਾਪਸ adjustForUsageAndContext(kCapContextUsageLanguage, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::appendWithSep(UnicodeString& ਬਫਰ U_LIFETIME_BOUND, ਅਚਲ UnicodeString& src) ਅਚਲ {
    ਜੇ (ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਬਫਰ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(src);
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ UnicodeString *ਮੁੱਲਾਂ[2] = { &ਬਫਰ, &src };
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(ਮੁੱਲਾਂ, 2, ਬਫਰ, ਖਾਲੀ_ਸੂਚਕ, 0, ਸਥਿਤੀ);
    }
    ਵਾਪਸ ਬਫਰ;
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::localeDisplayName(ਅਚਲ ਅੱਖਰ* localeId,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ localeDisplayName(Locale(localeId), ਨਤੀਜਾ);
}

//ਨਿੱਜੀ
UnicodeString&
Locale_Display_ਨਾਂ_Impl::localeIdName(ਅਚਲ ਅੱਖਰ* localeId,
                                     UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND, ਤਾਰਕਿਕ substitute) ਅਚਲ {
    ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
        lang_ਅੰਕੜੇ.getNoFallback("Languages%short", localeId, ਨਤੀਜਾ);
        ਜੇ (!ਨਤੀਜਾ.isBogus()) {
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }
    lang_ਅੰਕੜੇ.getNoFallback("Languages", localeId, ਨਤੀਜਾ);
    ਜੇ (ਨਤੀਜਾ.isBogus() && uprv_strchr(localeId, '_') == ਖਾਲੀ_ਸੂਚਕ) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ਅਚਲ ਅੱਖਰ* canonLocId = canonLocale.getName();
        ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
            lang_ਅੰਕੜੇ.getNoFallback("Languages%short", canonLocId, ਨਤੀਜਾ);
            ਜੇ (!ਨਤੀਜਾ.isBogus()) {
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        }
        lang_ਅੰਕੜੇ.getNoFallback("Languages", canonLocId, ਨਤੀਜਾ);
    }
    ਜੇ (ਨਤੀਜਾ.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::languageDisplayName(ਅਚਲ ਅੱਖਰ* lang,
                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਜੇ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਨਤੀਜਾ = UnicodeString(lang, -1, US_INV);
    }
    ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
        lang_ਅੰਕੜੇ.getNoFallback("Languages%short", lang, ਨਤੀਜਾ);
        ਜੇ (!ਨਤੀਜਾ.isBogus()) {
            ਵਾਪਸ adjustForUsageAndContext(kCapContextUsageLanguage, ਨਤੀਜਾ);
        }
    }
    lang_ਅੰਕੜੇ.getNoFallback("Languages", lang, ਨਤੀਜਾ);
    ਜੇ (ਨਤੀਜਾ.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ਅਚਲ ਅੱਖਰ* canonLocId = canonLocale.getName();
        ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
            lang_ਅੰਕੜੇ.getNoFallback("Languages%short", canonLocId, ਨਤੀਜਾ);
            ਜੇ (!ਨਤੀਜਾ.isBogus()) {
                ਵਾਪਸ adjustForUsageAndContext(kCapContextUsageLanguage, ਨਤੀਜਾ);
            }
        }
        lang_ਅੰਕੜੇ.getNoFallback("Languages", canonLocId, ਨਤੀਜਾ);
    }
    ਜੇ (ਨਤੀਜਾ.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ਵਾਪਸ adjustForUsageAndContext(kCapContextUsageLanguage, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                          ਤਾਰਕਿਕ skipAdjust) ਅਚਲ {
    ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
        lang_ਅੰਕੜੇ.getNoFallback("Scripts%short", script, ਨਤੀਜਾ);
        ਜੇ (!ਨਤੀਜਾ.isBogus()) {
            ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(kCapContextUsageScript, ਨਤੀਜਾ);
        }
    }
    ਜੇ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ਅੰਕੜੇ.get("Scripts", script, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        lang_ਅੰਕੜੇ.getNoFallback("Scripts", script, ਨਤੀਜਾ);
    }
    ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(kCapContextUsageScript, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ scriptDisplayName(script, ਨਤੀਜਾ, ਝੂਠ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::scriptDisplayName(UScript_ਕੋਡ script_ਕੋਡ,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ scriptDisplayName(uscript_getName(script_ਕੋਡ), ਨਤੀਜਾ, ਝੂਠ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                          ਤਾਰਕਿਕ skipAdjust) ਅਚਲ {
    ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
         region_ਅੰਕੜੇ.getNoFallback("Countries%short", region, ਨਤੀਜਾ);
        ਜੇ (!ਨਤੀਜਾ.isBogus()) {
            ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(kCapContextUsageTerritory, ਨਤੀਜਾ);
        }
    }
    ਜੇ (substitute == UDISPCTX_SUBSTITUTE) {
        region_ਅੰਕੜੇ.get("Countries", region, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        region_ਅੰਕੜੇ.getNoFallback("Countries", region, ਨਤੀਜਾ);
    }
    ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(kCapContextUsageTerritory, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ regionDisplayName(region, ਨਤੀਜਾ, ਝੂਠ);
}


UnicodeString&
Locale_Display_ਨਾਂ_Impl::variantDisplayName(ਅਚਲ ਅੱਖਰ* variant,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                           ਤਾਰਕਿਕ skipAdjust) ਅਚਲ {
    // don't have a resource for short variant names
    ਜੇ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ਅੰਕੜੇ.get("Variants", variant, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        lang_ਅੰਕੜੇ.getNoFallback("Variants", variant, ਨਤੀਜਾ);
    }
    ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(kCapContextUsageVariant, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::variantDisplayName(ਅਚਲ ਅੱਖਰ* variant,
                                           UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ variantDisplayName(variant, ਨਤੀਜਾ, ਝੂਠ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                       ਤਾਰਕਿਕ skipAdjust) ਅਚਲ {
    // don't have a resource for short key names
    ਜੇ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ਅੰਕੜੇ.get("Keys", ਕੁੰਜੀ, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        lang_ਅੰਕੜੇ.getNoFallback("Keys", ਕੁੰਜੀ, ਨਤੀਜਾ);
    }
    ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(k_Cap_Context_Usage_ਕੁੰਜੀ, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                       UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ ਕੁੰਜੀ_Display_Name(ਕੁੰਜੀ, ਨਤੀਜਾ, ਝੂਠ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                            ਅਚਲ ਅੱਖਰ* value,
                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                            ਤਾਰਕਿਕ skipAdjust) ਅਚਲ {
    ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_ਕੋਡ sts = U_ZERO_ERROR;
        UnicodeString ustr_ਮੁੱਲ(value, -1, US_INV);
        int32_t len;
        ਅਚਲ char16_t *currencyName = ucurr_getName(ustr_ਮੁੱਲ.get_Terminated_ਬਫਰ(),
            locale.get_ਅਧਾਰ_Name(), UCURR_LONG_NAME, ਖਾਲੀ_ਸੂਚਕ /* isChoiceFormat */, &len, &sts);
        ਜੇ (U_FAILURE(sts)) {
            // Return the value as is on failure
            ਨਤੀਜਾ = ustr_ਮੁੱਲ;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(currencyName, len);
        ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(k_Cap_Context_Usage_ਕੁੰਜੀ_ਮੁੱਲ, ਨਤੀਜਾ);
    }

    ਜੇ (name_ਲੰਮਾਈ == UDISPCTX_ਲੰਮਾਈ_SHORT) {
        lang_ਅੰਕੜੇ.getNoFallback("Types%short", ਕੁੰਜੀ, value, ਨਤੀਜਾ);
        ਜੇ (!ਨਤੀਜਾ.isBogus()) {
            ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(k_Cap_Context_Usage_ਕੁੰਜੀ_ਮੁੱਲ, ਨਤੀਜਾ);
        }
    }
    ਜੇ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ਅੰਕੜੇ.get("Types", ਕੁੰਜੀ, value, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        lang_ਅੰਕੜੇ.getNoFallback("Types", ਕੁੰਜੀ, value, ਨਤੀਜਾ);
    }
    ਵਾਪਸ skipAdjust? ਨਤੀਜਾ: adjustForUsageAndContext(k_Cap_Context_Usage_ਕੁੰਜੀ_ਮੁੱਲ, ਨਤੀਜਾ);
}

UnicodeString&
Locale_Display_ਨਾਂ_Impl::ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                            ਅਚਲ ਅੱਖਰ* value,
                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ {
    ਵਾਪਸ ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਕੁੰਜੀ, value, ਨਤੀਜਾ, ਝੂਠ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ਨਾਂ*
Locale_Display_ਨਾਂ::createInstance(ਅਚਲ Locale& locale,
                                   UDialectHandling dialectHandling) {
    ਵਾਪਸ ਨਵਾਂ Locale_Display_ਨਾਂ_Impl(locale, dialectHandling);
}

Locale_Display_ਨਾਂ*
Locale_Display_ਨਾਂ::createInstance(ਅਚਲ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ਜੇ (contexts == ਖਾਲੀ_ਸੂਚਕ) {
        length = 0;
    }
    ਵਾਪਸ ਨਵਾਂ Locale_Display_ਨਾਂ_Impl(locale, contexts, length);
}

U_NAMESPACE_ਸਿਰਾ

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_ਨਾਂ * U_EXPORT2
uldn_open(ਅਚਲ ਅੱਖਰ * locale,
          UDialectHandling dialectHandling,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
    locale = uloc_getDefault();
  }
  ਵਾਪਸ (ULocale_Display_ਨਾਂ *)Locale_Display_ਨਾਂ::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_ਨਾਂ * U_EXPORT2
uldn_openForContext(ਅਚਲ ਅੱਖਰ * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
    locale = uloc_getDefault();
  }
  ਵਾਪਸ (ULocale_Display_ਨਾਂ *)Locale_Display_ਨਾਂ::createInstance(Locale(locale), contexts, length);
}


U_CAPI ਖਾਲੀ U_EXPORT2
uldn_close(ULocale_Display_ਨਾਂ *ldn) {
  ਮਿਟਾਓ (Locale_Display_ਨਾਂ *)ldn;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uldn_getLocale(ਅਚਲ ULocale_Display_ਨਾਂ* ldn U_LIFETIME_BOUND) {
  ਜੇ (ldn) {
    ਵਾਪਸ ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->getLocale().getName();
  }
  ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ਅਚਲ ULocale_Display_ਨਾਂ *ldn) {
  ਜੇ (ldn) {
    ਵਾਪਸ ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->getDialectHandling();
  }
  ਵਾਪਸ ULDN_STANDARD_ਨਾਂ;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
              UDisplayContextType type,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ (UDisplayContext)0;
  }
  ਵਾਪਸ ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *locale,
                       char16_t *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || locale == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->localeDisplayName(locale, ਆਰਜ਼ੀ);
  ਜੇ (ਆਰਜ਼ੀ.isBogus()) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                         ਅਚਲ ਅੱਖਰ *lang,
                         char16_t *ਨਤੀਜਾ,
                         int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || lang == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->languageDisplayName(lang, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *script,
                       char16_t *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || script == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->scriptDisplayName(script, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_script_ਕੋਡ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                           UScript_ਕੋਡ script_ਕੋਡ,
                           char16_t *ਨਤੀਜਾ,
                           int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਵਾਪਸ uldn_scriptDisplayName(ldn, uscript_getName(script_ਕੋਡ), ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *region,
                       char16_t *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || region == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->regionDisplayName(region, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                        ਅਚਲ ਅੱਖਰ *variant,
                        char16_t *ਨਤੀਜਾ,
                        int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || variant == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->variantDisplayName(variant, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_ਕੁੰਜੀ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                    char16_t *ਨਤੀਜਾ,
                    int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || ਕੁੰਜੀ == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0) || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->ਕੁੰਜੀ_Display_Name(ਕੁੰਜੀ, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uldn_ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                         ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                         ਅਚਲ ਅੱਖਰ *value,
                         char16_t *ਨਤੀਜਾ,
                         int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ) {
  ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
    ਵਾਪਸ 0;
  }
  ਜੇ (ldn == ਖਾਲੀ_ਸੂਚਕ || ਕੁੰਜੀ == ਖਾਲੀ_ਸੂਚਕ || value == ਖਾਲੀ_ਸੂਚਕ || (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ && max_ਨਤੀਜਾ_ਮਾਪ > 0)
      || max_ਨਤੀਜਾ_ਮਾਪ < 0) {
    *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  UnicodeString ਆਰਜ਼ੀ(ਨਤੀਜਾ, 0, max_ਨਤੀਜਾ_ਮਾਪ);
  ((ਅਚਲ Locale_Display_ਨਾਂ *)ldn)->ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਕੁੰਜੀ, value, ਆਰਜ਼ੀ);
  ਵਾਪਸ ਆਰਜ਼ੀ.extract(ਨਤੀਜਾ, max_ਨਤੀਜਾ_ਮਾਪ, *p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ
