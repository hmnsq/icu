// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/locdspnm.h"
#अन्तर्भाव "unicode/simpleformatter.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/udisplaycontext.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/ucurr.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ureslocs.h"
#अन्तर्भाव "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
वर्ग ICUData_सारणी {
    ध्रुव वर्ण* ध्रुव मार्गः;
    Locale locale;

सार्वजनिक:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_सारणी(ध्रुव वर्ण* मार्गः, ध्रुव Locale& locale);
    ~ICUData_सारणी() = स्वभाव;

    ध्रुव Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                       UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& get(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* sub_सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                       UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;

    UnicodeString& getNoFallback(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                 UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& getNoFallback(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* sub_सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                 UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
};

अन्तःस्थ UnicodeString& ICUData_सारणी::get(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                        UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम get(सारणी_कुञ्चिका, शून्यसूचकः, item_कुञ्चिका, परिणाम);
}

अन्तःस्थ UnicodeString& ICUData_सारणी::getNoFallback(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                                  UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम getNoFallback(सारणी_कुञ्चिका, शून्यसूचकः, item_कुञ्चिका, परिणाम);
}

ICUData_सारणी::ICUData_सारणी(ध्रुव वर्ण* मार्गः, ध्रुव Locale& locale)
    : मार्गः(मार्गः), locale(locale)
{
    U_ASSERT(मार्गः != शून्यसूचकः);
}

ध्रुव Locale&
ICUData_सारणी::getLocale() U_LIFETIME_BOUND {
  प्रत्यागम locale;
}

UnicodeString&
ICUData_सारणी::get(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* sub_सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                  UnicodeString &परिणाम U_LIFETIME_BOUND) ध्रुव {
  UError_संहिता स्थितिः = U_ZERO_ERROR;
  int32_t len = 0;

  ध्रुव char16_t *s = uloc_get_सारणी_String_With_Fallback(मार्गः, locale.getName(),
                                                   सारणी_कुञ्चिका, sub_सारणी_कुञ्चिका, item_कुञ्चिका,
                                                   &len, &स्थितिः);
  यदि (U_SUCCESS(स्थितिः) && len > 0) {
    प्रत्यागम परिणाम.समुच्चयः_गन्तव्यम्(s, len);
  }
  प्रत्यागम परिणाम.समुच्चयः_गन्तव्यम्(UnicodeString(item_कुञ्चिका, -1, US_INV));
}

UnicodeString&
ICUData_सारणी::getNoFallback(ध्रुव वर्ण* सारणी_कुञ्चिका, ध्रुव वर्ण* sub_सारणी_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                            UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  UError_संहिता स्थितिः = U_ZERO_ERROR;
  int32_t len = 0;

  ध्रुव char16_t *s = uloc_get_सारणी_String_With_Fallback(मार्गः, locale.getName(),
                                                   सारणी_कुञ्चिका, sub_सारणी_कुञ्चिका, item_कुञ्चिका,
                                                   &len, &स्थितिः);
  यदि (U_SUCCESS(स्थितिः)) {
    प्रत्यागम परिणाम.समुच्चयः_गन्तव्यम्(s, len);
  }

  परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
  प्रत्यागम परिणाम;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_नामानि::~Locale_Display_नामानि() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#यदि 0  // currently unused

वर्ग Default_Locale_Display_नामानि : सार्वजनिक Locale_Display_नामानि {
  UDialectHandling dialectHandling;

सार्वजनिक:
  // constructor
  Default_Locale_Display_नामानि(UDialectHandling dialectHandling);

  आभास ~Default_Locale_Display_नामानि();

  आभास ध्रुव Locale& getLocale() ध्रुव U_LIFETIME_BOUND;
  आभास UDialectHandling getDialectHandling() ध्रुव;

  आभास UnicodeString& localeDisplayName(ध्रुव Locale& locale,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& localeDisplayName(ध्रुव वर्ण* localeId,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& languageDisplayName(ध्रुव वर्ण* lang,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& scriptDisplayName(ध्रुव वर्ण* script,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& scriptDisplayName(UScript_संहिता script_संहिता,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& regionDisplayName(ध्रुव वर्ण* region,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& variantDisplayName(ध्रुव वर्ण* variant,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                        UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                             ध्रुव वर्ण* value,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
};

Default_Locale_Display_नामानि::Default_Locale_Display_नामानि(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_नामानि::~Default_Locale_Display_नामानि() {
}

ध्रुव Locale&
Default_Locale_Display_नामानि::getLocale() ध्रुव U_LIFETIME_BOUND {
  प्रत्यागम Locale::get_मूलम्();
}

UDialectHandling
Default_Locale_Display_नामानि::getDialectHandling() ध्रुव {
  प्रत्यागम dialectHandling;
}

UnicodeString&
Default_Locale_Display_नामानि::localeDisplayName(ध्रुव Locale& locale,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::localeDisplayName(ध्रुव वर्ण* localeId,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::languageDisplayName(ध्रुव वर्ण* lang,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::scriptDisplayName(ध्रुव वर्ण* script,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::scriptDisplayName(UScript_संहिता script_संहिता,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  ध्रुव वर्ण* name = uscript_getName(script_संहिता);
  यदि (name) {
    प्रत्यागम परिणाम = UnicodeString(name, -1, US_INV);
  }
  प्रत्यागम परिणाम.remove();
}

UnicodeString&
Default_Locale_Display_नामानि::regionDisplayName(ध्रुव वर्ण* region,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::variantDisplayName(ध्रुव वर्ण* variant,
                                              UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(कुञ्चिका, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_नामानि::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* /*कुञ्चिका*/,
                                               ध्रुव वर्ण* value,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम परिणाम = UnicodeString(value, -1, US_INV);
}

#यदि_अन्तः  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

वर्ग Locale_Display_नामानि_Impl : सार्वजनिक Locale_Display_नामानि {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_सारणी lang_दत्त;
    ICUData_सारणी region_दत्त;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter कुञ्चिका_Type_Format;
    UDisplayContext capitalizationContext;
#यदि !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#अन्यथा
    UObject* capitalizationBrkIter;
#यदि_अन्तः
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_दीर्घता;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    गणना CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_कुञ्चिका,
        k_Cap_Context_Usage_कुञ्चिका_मानम्,
        k_Cap_Context_Usage_गणनम्
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     तार्किक fCapitalization[k_Cap_Context_Usage_गणनम्];

सार्वजनिक:
    // constructor
    Locale_Display_नामानि_Impl(ध्रुव Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_नामानि_Impl(ध्रुव Locale& locale, UDisplayContext *contexts, int32_t length);
    आभास ~Locale_Display_नामानि_Impl();

    आभास ध्रुव Locale& getLocale() ध्रुव U_LIFETIME_BOUND override;
    आभास UDialectHandling getDialectHandling() ध्रुव override;
    आभास UDisplayContext getContext(UDisplayContextType type) ध्रुव override;

    आभास UnicodeString& localeDisplayName(ध्रुव Locale& locale,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& localeDisplayName(ध्रुव वर्ण* localeId,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    languageDisplayName(ध्रुव वर्ण* lang, UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& scriptDisplayName(ध्रुव वर्ण* script,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& scriptDisplayName(UScript_संहिता script_संहिता,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& regionDisplayName(ध्रुव वर्ण* region,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    variantDisplayName(ध्रुव वर्ण* variant, UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value,
                        UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव override;

गुप्त:
    UnicodeString& localeIdName(ध्रुव वर्ण* localeId, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                तार्किक substitute) ध्रुव;
    UnicodeString& appendWithSep(UnicodeString& बफर U_LIFETIME_BOUND,
                                 ध्रुव UnicodeString& src) ध्रुव;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& scriptDisplayName(ध्रुव वर्ण* script, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) ध्रुव;
    UnicodeString& regionDisplayName(ध्रुव वर्ण* region, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) ध्रुव;
    UnicodeString& variantDisplayName(ध्रुव वर्ण* variant, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      तार्किक skipAdjust) ध्रुव;
    UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                  तार्किक skipAdjust) ध्रुव;
    UnicodeString& कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) ध्रुव;
    शून्य initialize();

    संरचना CapitalizationContextSink;
};

Locale_Display_नामानि_Impl::Locale_Display_नामानि_Impl(ध्रुव Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_दत्त(U_ICUDATA_LANG, locale)
    , region_दत्त(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(शून्यसूचकः)
    , name_दीर्घता(UDISPCTX_दीर्घता_पूर्णम्)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_नामानि_Impl::Locale_Display_नामानि_Impl(ध्रुव Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_नामानि)
    , lang_दत्त(U_ICUDATA_LANG, locale)
    , region_दत्त(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(शून्यसूचकः)
    , name_दीर्घता(UDISPCTX_दीर्घता_पूर्णम्)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    यावत् (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        विकल्प (selector) {
            पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                भङ्ग;
            पक्ष UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                भङ्ग;
            पक्ष UDISPCTX_TYPE_DISPLAY_दीर्घता:
                name_दीर्घता = value;
                भङ्ग;
            पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                भङ्ग;
            स्वभाव:
                भङ्ग;
        }
    }
    initialize();
}

संरचना Locale_Display_नामानि_Impl::CapitalizationContextSink : सार्वजनिक ResourceSink {
    तार्किक hasCapitalizationUsage;
    Locale_Display_नामानि_Impl& parent;

    CapitalizationContextSink(Locale_Display_नामानि_Impl& _parent)
      : hasCapitalizationUsage(असत्य), parent(_parent) {}
    आभास ~CapitalizationContextSink();

    आभास शून्य put(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value, UBool /*noFallback*/,
            UError_संहिता &error_संहिता) override {
        Resource_सारणी contexts = value.get_सारणी(error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        प्रति (पूर्णाङ्क i = 0; contexts.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {

            CapContextUsage usageEnum;
            यदि (uprv_strcmp(कुञ्चिका, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_कुञ्चिका;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_कुञ्चिका_मानम्;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } अन्यथा यदि (uprv_strcmp(कुञ्चिका, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } अन्यथा {
                अनुवर्त;
            }

            int32_t len = 0;
            ध्रुव int32_t* int_सदिशम् = value.get_Int_सदिशम्(len, error_संहिता);
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
            यदि (len < 2) { अनुवर्त; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_सदिशम्[0] : int_सदिशम्[1];
            यदि (titlecaseInt == 0) { अनुवर्त; }

            parent.fCapitalization[usageEnum] = सत्य;
            hasCapitalizationUsage = सत्य;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_नामानि_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

शून्य
Locale_Display_नामानि_Impl::initialize() {
    Locale_Display_नामानि_Impl* nonConstThis = एतत्;
    nonConstThis->locale = lang_दत्त.getLocale() == Locale::get_मूलम्()
        ? region_दत्त.getLocale()
        : lang_दत्त.getLocale();

    UnicodeString sep;
    lang_दत्त.getNoFallback("localeDisplayPattern", "separator", sep);
    यदि (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    separatorFormat.apply_आदर्शः_Min_Max_Arguments(sep, 2, 2, स्थितिः);

    UnicodeString आदर्शः;
    lang_दत्त.getNoFallback("localeDisplayPattern", "pattern", आदर्शः);
    यदि (आदर्शः.isBogus()) {
        आदर्शः = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_आदर्शः_Min_Max_Arguments(आदर्शः, 2, 2, स्थितिः);
    यदि (आदर्शः.सूचकांक_सम्बन्धि(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } अन्यथा {
        formatOpenParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_आदर्शः;
    lang_दत्त.get("localeDisplayPattern", "keyTypePattern", kt_आदर्शः);
    यदि (kt_आदर्शः.isBogus()) {
        kt_आदर्शः = UnicodeString("{0}={1}", -1, US_INV);
    }
    कुञ्चिका_Type_Format.apply_आदर्शः_Min_Max_Arguments(kt_आदर्शः, 2, 2, स्थितिः);

    uprv_memset(fCapitalization, 0, परिमाण(fCapitalization));
#यदि !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    तार्किक needBrkIter = असत्य;
    यदि (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(शून्यसूचकः, locale.getName(), &स्थितिः));
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        CapitalizationContextSink sink(*एतत्);
        ures_get_All_वस्तूनि_With_Fallback(resource.getAlias(), "contextTransforms", sink, स्थितिः);
        यदि (स्थितिः == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            स्थितिः = U_ZERO_ERROR;
        } अन्यथा यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    यदि (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_सम्बन्धि_SENTENCE) {
        स्थितिः = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            विलोपय capitalizationBrkIter;
            capitalizationBrkIter = शून्यसूचकः;
        }
    }
#यदि_अन्तः
}

Locale_Display_नामानि_Impl::~Locale_Display_नामानि_Impl() {
#यदि !UCONFIG_NO_BREAK_ITERATION
    विलोपय capitalizationBrkIter;
#यदि_अन्तः
}

ध्रुव Locale&
Locale_Display_नामानि_Impl::getLocale() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम locale;
}

UDialectHandling
Locale_Display_नामानि_Impl::getDialectHandling() ध्रुव {
    प्रत्यागम dialectHandling;
}

UDisplayContext
Locale_Display_नामानि_Impl::getContext(UDisplayContextType type) ध्रुव {
    विकल्प (type) {
        पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
            प्रत्यागम static_cast<UDisplayContext>(dialectHandling);
        पक्ष UDISPCTX_TYPE_CAPITALIZATION:
            प्रत्यागम capitalizationContext;
        पक्ष UDISPCTX_TYPE_DISPLAY_दीर्घता:
            प्रत्यागम name_दीर्घता;
        पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            प्रत्यागम substitute;
        स्वभाव:
            भङ्ग;
    }
    प्रत्यागम static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_नामानि_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
#यदि !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    यदि ( परिणाम.length() > 0 && u_islower(परिणाम.char32At(0)) && capitalizationBrkIter!= शून्यसूचकः &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_सम्बन्धि_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        स्थिर UMutex capitalization_Brk_Iter_तालकम्;
        Mutex तालकम्(&capitalization_Brk_Iter_तालकम्);
        परिणाम.गन्तव्यम्_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#यदि_अन्तः
    प्रत्यागम परिणाम;
}

UnicodeString&
Locale_Display_नामानि_Impl::localeDisplayName(ध्रुव Locale& loc,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
  यदि (loc.isBogus()) {
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम परिणाम;
  }
  UnicodeString परिणाम_Name;

  ध्रुव वर्ण* lang = loc.getLanguage();
  यदि (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ध्रुव वर्ण* script = loc.getScript();
  ध्रुव वर्ण* country = loc.getCountry();
  ध्रुव वर्ण* variant = loc.getVariant();

  तार्किक hasScript = uprv_strlen(script) > 0;
  तार्किक hasCountry = uprv_strlen(country) > 0;
  तार्किक hasVariant = uprv_strlen(variant) > 0;

  यदि (dialectHandling == ULDN_DIALECT_नामानि) {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    CharString बफर;
    कुरु { // loop construct is so we can break early out of search
      यदि (hasScript && hasCountry) {
        बफर.append(lang, स्थितिः)
              .append('_', स्थितिः)
              .append(script, स्थितिः)
              .append('_', स्थितिः)
              .append(country, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
          localeIdName(बफर.दत्त(), परिणाम_Name, असत्य);
          यदि (!परिणाम_Name.isBogus()) {
            hasScript = असत्य;
            hasCountry = असत्य;
            भङ्ग;
          }
        }
      }
      यदि (hasScript) {
        बफर.append(lang, स्थितिः)
              .append('_', स्थितिः)
              .append(script, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
          localeIdName(बफर.दत्त(), परिणाम_Name, असत्य);
          यदि (!परिणाम_Name.isBogus()) {
            hasScript = असत्य;
            भङ्ग;
          }
        }
      }
      यदि (hasCountry) {
        बफर.append(lang, स्थितिः)
              .append('_', स्थितिः)
              .append(country, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
          localeIdName(बफर.दत्त(), परिणाम_Name, असत्य);
          यदि (!परिणाम_Name.isBogus()) {
            hasCountry = असत्य;
            भङ्ग;
          }
        }
      }
    } यावत् (असत्य);
  }
  यदि (परिणाम_Name.isBogus() || परिणाम_Name.is_रिक्तानि()) {
    localeIdName(lang, परिणाम_Name, substitute == UDISPCTX_SUBSTITUTE);
    यदि (परिणाम_Name.isBogus()) {
      परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम परिणाम;
    }
  }

  UnicodeString परिणाम_Remainder;
  UnicodeString अस्थायी;
  UError_संहिता स्थितिः = U_ZERO_ERROR;

  यदि (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, अस्थायी, सत्य);
    यदि (script_str.isBogus()) {
      परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम परिणाम;
    }
    परिणाम_Remainder.append(script_str);
  }
  यदि (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, अस्थायी, सत्य);
    यदि (region_str.isBogus()) {
      परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम परिणाम;
    }
    appendWithSep(परिणाम_Remainder, region_str);
  }
  यदि (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, अस्थायी, सत्य);
    यदि (variant_str.isBogus()) {
      परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम परिणाम;
    }
    appendWithSep(परिणाम_Remainder, variant_str);
  }
  परिणाम_Remainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  परिणाम_Remainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(स्थितिः));
  यदि (e.isValid() && U_SUCCESS(स्थितिः)) {
    UnicodeString temp2;
    ध्रुव वर्ण* कुञ्चिका;
    यावत् ((कुञ्चिका = e->next((int32_t*)शून्यसूचकः, स्थितिः)) != शून्यसूचकः) {
        स्वचालित value = loc.get_Keyword_मानम्<CharString>(कुञ्चिका, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम परिणाम;
      }
      कुञ्चिका_Display_Name(कुञ्चिका, अस्थायी, सत्य);
      अस्थायी.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      अस्थायी.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value.दत्त(), temp2, सत्य);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      यदि (temp2 != UnicodeString(value.दत्त(), -1, US_INV)) {
        appendWithSep(परिणाम_Remainder, temp2);
      } अन्यथा यदि (अस्थायी != UnicodeString(कुञ्चिका, -1, US_INV)) {
        UnicodeString temp3;
        कुञ्चिका_Type_Format.format(अस्थायी, temp2, temp3, स्थितिः);
        appendWithSep(परिणाम_Remainder, temp3);
      } अन्यथा {
        appendWithSep(परिणाम_Remainder, अस्थायी)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  यदि (!परिणाम_Remainder.is_रिक्तानि()) {
    format.format(परिणाम_Name, परिणाम_Remainder, परिणाम.remove(), स्थितिः);
    प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
  }

  परिणाम = परिणाम_Name;
  प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::appendWithSep(UnicodeString& बफर U_LIFETIME_BOUND, ध्रुव UnicodeString& src) ध्रुव {
    यदि (बफर.is_रिक्तानि()) {
        बफर.समुच्चयः_गन्तव्यम्(src);
    } अन्यथा {
        ध्रुव UnicodeString *मानानि[2] = { &बफर, &src };
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मानानि, 2, बफर, शून्यसूचकः, 0, स्थितिः);
    }
    प्रत्यागम बफर;
}

UnicodeString&
Locale_Display_नामानि_Impl::localeDisplayName(ध्रुव वर्ण* localeId,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम localeDisplayName(Locale(localeId), परिणाम);
}

//वैयक्तिकम्
UnicodeString&
Locale_Display_नामानि_Impl::localeIdName(ध्रुव वर्ण* localeId,
                                     UnicodeString& परिणाम U_LIFETIME_BOUND, तार्किक substitute) ध्रुव {
    यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
        lang_दत्त.getNoFallback("Languages%short", localeId, परिणाम);
        यदि (!परिणाम.isBogus()) {
            प्रत्यागम परिणाम;
        }
    }
    lang_दत्त.getNoFallback("Languages", localeId, परिणाम);
    यदि (परिणाम.isBogus() && uprv_strchr(localeId, '_') == शून्यसूचकः) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ध्रुव वर्ण* canonLocId = canonLocale.getName();
        यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
            lang_दत्त.getNoFallback("Languages%short", canonLocId, परिणाम);
            यदि (!परिणाम.isBogus()) {
                प्रत्यागम परिणाम;
            }
        }
        lang_दत्त.getNoFallback("Languages", canonLocId, परिणाम);
    }
    यदि (परिणाम.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        परिणाम.समुच्चयः_गन्तव्यम्(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    प्रत्यागम परिणाम;
}

UnicodeString&
Locale_Display_नामानि_Impl::languageDisplayName(ध्रुव वर्ण* lang,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    यदि (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != शून्यसूचकः) {
        प्रत्यागम परिणाम = UnicodeString(lang, -1, US_INV);
    }
    यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
        lang_दत्त.getNoFallback("Languages%short", lang, परिणाम);
        यदि (!परिणाम.isBogus()) {
            प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
        }
    }
    lang_दत्त.getNoFallback("Languages", lang, परिणाम);
    यदि (परिणाम.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ध्रुव वर्ण* canonLocId = canonLocale.getName();
        यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
            lang_दत्त.getNoFallback("Languages%short", canonLocId, परिणाम);
            यदि (!परिणाम.isBogus()) {
                प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
            }
        }
        lang_दत्त.getNoFallback("Languages", canonLocId, परिणाम);
    }
    यदि (परिणाम.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        परिणाम.समुच्चयः_गन्तव्यम्(UnicodeString(lang, -1, US_INV)); // use key (
    }
    प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::scriptDisplayName(ध्रुव वर्ण* script,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) ध्रुव {
    यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
        lang_दत्त.getNoFallback("Scripts%short", script, परिणाम);
        यदि (!परिणाम.isBogus()) {
            प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageScript, परिणाम);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        lang_दत्त.get("Scripts", script, परिणाम);
    } अन्यथा {
        lang_दत्त.getNoFallback("Scripts", script, परिणाम);
    }
    प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageScript, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::scriptDisplayName(ध्रुव वर्ण* script,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम scriptDisplayName(script, परिणाम, असत्य);
}

UnicodeString&
Locale_Display_नामानि_Impl::scriptDisplayName(UScript_संहिता script_संहिता,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम scriptDisplayName(uscript_getName(script_संहिता), परिणाम, असत्य);
}

UnicodeString&
Locale_Display_नामानि_Impl::regionDisplayName(ध्रुव वर्ण* region,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) ध्रुव {
    यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
         region_दत्त.getNoFallback("Countries%short", region, परिणाम);
        यदि (!परिणाम.isBogus()) {
            प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageTerritory, परिणाम);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        region_दत्त.get("Countries", region, परिणाम);
    } अन्यथा {
        region_दत्त.getNoFallback("Countries", region, परिणाम);
    }
    प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageTerritory, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::regionDisplayName(ध्रुव वर्ण* region,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम regionDisplayName(region, परिणाम, असत्य);
}


UnicodeString&
Locale_Display_नामानि_Impl::variantDisplayName(ध्रुव वर्ण* variant,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND,
                                           तार्किक skipAdjust) ध्रुव {
    // don't have a resource for short variant names
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        lang_दत्त.get("Variants", variant, परिणाम);
    } अन्यथा {
        lang_दत्त.getNoFallback("Variants", variant, परिणाम);
    }
    प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageVariant, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::variantDisplayName(ध्रुव वर्ण* variant,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम variantDisplayName(variant, परिणाम, असत्य);
}

UnicodeString&
Locale_Display_नामानि_Impl::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) ध्रुव {
    // don't have a resource for short key names
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        lang_दत्त.get("Keys", कुञ्चिका, परिणाम);
    } अन्यथा {
        lang_दत्त.getNoFallback("Keys", कुञ्चिका, परिणाम);
    }
    प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम कुञ्चिका_Display_Name(कुञ्चिका, परिणाम, असत्य);
}

UnicodeString&
Locale_Display_नामानि_Impl::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                            ध्रुव वर्ण* value,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND,
                                            तार्किक skipAdjust) ध्रुव {
    यदि (uprv_strcmp(कुञ्चिका, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_संहिता sts = U_ZERO_ERROR;
        UnicodeString ustr_मानम्(value, -1, US_INV);
        int32_t len;
        ध्रुव char16_t *currencyName = ucurr_getName(ustr_मानम्.get_Terminated_बफर(),
            locale.get_आधारः_Name(), UCURR_LONG_NAME, शून्यसूचकः /* isChoiceFormat */, &len, &sts);
        यदि (U_FAILURE(sts)) {
            // Return the value as is on failure
            परिणाम = ustr_मानम्;
            प्रत्यागम परिणाम;
        }
        परिणाम.समुच्चयः_गन्तव्यम्(currencyName, len);
        प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, परिणाम);
    }

    यदि (name_दीर्घता == UDISPCTX_दीर्घता_SHORT) {
        lang_दत्त.getNoFallback("Types%short", कुञ्चिका, value, परिणाम);
        यदि (!परिणाम.isBogus()) {
            प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, परिणाम);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        lang_दत्त.get("Types", कुञ्चिका, value, परिणाम);
    } अन्यथा {
        lang_दत्त.getNoFallback("Types", कुञ्चिका, value, परिणाम);
    }
    प्रत्यागम skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, परिणाम);
}

UnicodeString&
Locale_Display_नामानि_Impl::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                            ध्रुव वर्ण* value,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value, परिणाम, असत्य);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_नामानि*
Locale_Display_नामानि::createInstance(ध्रुव Locale& locale,
                                   UDialectHandling dialectHandling) {
    प्रत्यागम नूतन Locale_Display_नामानि_Impl(locale, dialectHandling);
}

Locale_Display_नामानि*
Locale_Display_नामानि::createInstance(ध्रुव Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    यदि (contexts == शून्यसूचकः) {
        length = 0;
    }
    प्रत्यागम नूतन Locale_Display_नामानि_Impl(locale, contexts, length);
}

U_NAMESPACE_अन्तः

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_नामानि * U_EXPORT2
uldn_open(ध्रुव वर्ण * locale,
          UDialectHandling dialectHandling,
          UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि (locale == शून्यसूचकः) {
    locale = uloc_getDefault();
  }
  प्रत्यागम (ULocale_Display_नामानि *)Locale_Display_नामानि::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_नामानि * U_EXPORT2
uldn_openForContext(ध्रुव वर्ण * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि (locale == शून्यसूचकः) {
    locale = uloc_getDefault();
  }
  प्रत्यागम (ULocale_Display_नामानि *)Locale_Display_नामानि::createInstance(Locale(locale), contexts, length);
}


U_CAPI शून्य U_EXPORT2
uldn_close(ULocale_Display_नामानि *ldn) {
  विलोपय (Locale_Display_नामानि *)ldn;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uldn_getLocale(ध्रुव ULocale_Display_नामानि* ldn U_LIFETIME_BOUND) {
  यदि (ldn) {
    प्रत्यागम ((ध्रुव Locale_Display_नामानि *)ldn)->getLocale().getName();
  }
  प्रत्यागम शून्यसूचकः;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ध्रुव ULocale_Display_नामानि *ldn) {
  यदि (ldn) {
    प्रत्यागम ((ध्रुव Locale_Display_नामानि *)ldn)->getDialectHandling();
  }
  प्रत्यागम ULDN_STANDARD_नामानि;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ध्रुव ULocale_Display_नामानि *ldn,
              UDisplayContextType type,
              UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम (UDisplayContext)0;
  }
  प्रत्यागम ((ध्रुव Locale_Display_नामानि *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ध्रुव ULocale_Display_नामानि *ldn,
                       ध्रुव वर्ण *locale,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || locale == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->localeDisplayName(locale, अस्थायी);
  यदि (अस्थायी.isBogus()) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ध्रुव ULocale_Display_नामानि *ldn,
                         ध्रुव वर्ण *lang,
                         char16_t *परिणाम,
                         int32_t max_परिणाम_आकार,
                         UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || lang == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->languageDisplayName(lang, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ध्रुव ULocale_Display_नामानि *ldn,
                       ध्रुव वर्ण *script,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || script == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->scriptDisplayName(script, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_script_संहिता_Display_Name(ध्रुव ULocale_Display_नामानि *ldn,
                           UScript_संहिता script_संहिता,
                           char16_t *परिणाम,
                           int32_t max_परिणाम_आकार,
                           UError_संहिता *p_Error_संहिता) {
  प्रत्यागम uldn_scriptDisplayName(ldn, uscript_getName(script_संहिता), परिणाम, max_परिणाम_आकार, p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ध्रुव ULocale_Display_नामानि *ldn,
                       ध्रुव वर्ण *region,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || region == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->regionDisplayName(region, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ध्रुव ULocale_Display_नामानि *ldn,
                        ध्रुव वर्ण *variant,
                        char16_t *परिणाम,
                        int32_t max_परिणाम_आकार,
                        UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || variant == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->variantDisplayName(variant, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_कुञ्चिका_Display_Name(ध्रुव ULocale_Display_नामानि *ldn,
                    ध्रुव वर्ण *कुञ्चिका,
                    char16_t *परिणाम,
                    int32_t max_परिणाम_आकार,
                    UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || कुञ्चिका == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->कुञ्चिका_Display_Name(कुञ्चिका, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uldn_कुञ्चिका_मानम्_Display_Name(ध्रुव ULocale_Display_नामानि *ldn,
                         ध्रुव वर्ण *कुञ्चिका,
                         ध्रुव वर्ण *value,
                         char16_t *परिणाम,
                         int32_t max_परिणाम_आकार,
                         UError_संहिता *p_Error_संहिता) {
  यदि (U_FAILURE(*p_Error_संहिता)) {
    प्रत्यागम 0;
  }
  यदि (ldn == शून्यसूचकः || कुञ्चिका == शून्यसूचकः || value == शून्यसूचकः || (परिणाम == शून्यसूचकः && max_परिणाम_आकार > 0)
      || max_परिणाम_आकार < 0) {
    *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((ध्रुव Locale_Display_नामानि *)ldn)->कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value, अस्थायी);
  प्रत्यागम अस्थायी.extract(परिणाम, max_परिणाम_आकार, *p_Error_संहिता);
}

#यदि_अन्तः
