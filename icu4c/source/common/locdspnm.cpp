// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_FORMATTING

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
ವರ್ಗ ICUDataTable {
    ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ path;
    Locale locale;

ಸಾರ್ವಜನಿಕ:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ Locale& locale);
    ~ICUDataTable() = ಸ್ವಭಾವ;

    ಧ್ರುವ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                       UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
    UnicodeString& get(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* sub_Table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                       UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;

    UnicodeString& getNoFallback(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                                 UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
    UnicodeString& getNoFallback(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* sub_Table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                                 UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
};

ಅನ್ತಃಸ್ಥ UnicodeString& ICUDataTable::get(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                                        UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ get(table_ಕೀಲಿ, nullptr, item_ಕೀಲಿ, result);
}

ಅನ್ತಃಸ್ಥ UnicodeString& ICUDataTable::getNoFallback(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                                                  UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ getNoFallback(table_ಕೀಲಿ, nullptr, item_ಕೀಲಿ, result);
}

ICUDataTable::ICUDataTable(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ಧ್ರುವ Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ locale;
}

UnicodeString&
ICUDataTable::get(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* sub_Table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                  UnicodeString &result U_LIFETIME_BOUND) ಧ್ರುವ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ಧ್ರುವ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ಕೀಲಿ, sub_Table_ಕೀಲಿ, item_ಕೀಲಿ,
                                                   &len, &status);
  ಯದಿ (U_SUCCESS(status) && len > 0) {
    ಪ್ರತ್ಯಾಗಮ result.setTo(s, len);
  }
  ಪ್ರತ್ಯಾಗಮ result.setTo(UnicodeString(item_ಕೀಲಿ, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ಧ್ರುವ ವರ್ಣ* table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* sub_Table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* item_ಕೀಲಿ,
                            UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ಧ್ರುವ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ಕೀಲಿ, sub_Table_ಕೀಲಿ, item_ಕೀಲಿ,
                                                   &len, &status);
  ಯದಿ (U_SUCCESS(status)) {
    ಪ್ರತ್ಯಾಗಮ result.setTo(s, len);
  }

  result.setToBogus();
  ಪ್ರತ್ಯಾಗಮ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ಯದಿ 0  // currently unused

ವರ್ಗ DefaultLocaleDisplayNames : ಸಾರ್ವಜನಿಕ LocaleDisplayNames {
  UDialectHandling dialectHandling;

ಸಾರ್ವಜನಿಕ:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  ಆಭಾಸ ~DefaultLocaleDisplayNames();

  ಆಭಾಸ ಧ್ರುವ Locale& getLocale() ಧ್ರುವ U_LIFETIME_BOUND;
  ಆಭಾಸ UDialectHandling getDialectHandling() ಧ್ರುವ;

  ಆಭಾಸ UnicodeString& localeDisplayName(ಧ್ರುವ Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& localeDisplayName(ಧ್ರುವ ವರ್ಣ* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& languageDisplayName(ಧ್ರುವ ವರ್ಣ* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& regionDisplayName(ಧ್ರುವ ವರ್ಣ* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                        UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
  ಆಭಾಸ UnicodeString& ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                             ಧ್ರುವ ವರ್ಣ* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ಧ್ರುವ Locale&
DefaultLocaleDisplayNames::getLocale() ಧ್ರುವ U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ Locale::get_ಬೇರು();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ಧ್ರುವ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ಧ್ರುವ ವರ್ಣ* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ಧ್ರುವ ವರ್ಣ* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಧ್ರುವ ವರ್ಣ* name = uscript_getName(scriptCode);
  ಯದಿ (name) {
    ಪ್ರತ್ಯಾಗಮ result = UnicodeString(name, -1, US_INV);
  }
  ಪ್ರತ್ಯಾಗಮ result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ಧ್ರುವ ವರ್ಣ* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(ಕೀಲಿ, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* /*ಕೀಲಿ*/,
                                               ಧ್ರುವ ವರ್ಣ* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

ವರ್ಗ LocaleDisplayNamesImpl : ಸಾರ್ವಜನಿಕ LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ಕೀಲಿ_Type_Format;
    UDisplayContext capitalizationContext;
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ಅನ್ಯಥಾ
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ಗಣನಾ CapContextUsage {
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
    LocaleDisplayNamesImpl(ಧ್ರುವ Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ಧ್ರುವ Locale& locale, UDisplayContext *contexts, int32_t length);
    ಆಭಾಸ ~LocaleDisplayNamesImpl();

    ಆಭಾಸ ಧ್ರುವ Locale& getLocale() ಧ್ರುವ U_LIFETIME_BOUND override;
    ಆಭಾಸ UDialectHandling getDialectHandling() ಧ್ರುವ override;
    ಆಭಾಸ UDisplayContext getContext(UDisplayContextType type) ಧ್ರುವ override;

    ಆಭಾಸ UnicodeString& localeDisplayName(ಧ್ರುವ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString& localeDisplayName(ಧ್ರುವ ವರ್ಣ* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString&
    languageDisplayName(ಧ್ರುವ ವರ್ಣ* lang, UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString& scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString& regionDisplayName(ಧ್ರುವ ವರ್ಣ* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString&
    variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant, UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString& ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString&
    ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* value,
                        UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ override;

ಗುಪ್ತ:
    UnicodeString& localeIdName(ಧ್ರುವ ವರ್ಣ* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                ತಾರ್ಕಿಕ substitute) ಧ್ರುವ;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ಧ್ರುವ UnicodeString& src) ಧ್ರುವ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ;
    UnicodeString& scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script, UnicodeString& result U_LIFETIME_BOUND,
                                     ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ;
    UnicodeString& regionDisplayName(ಧ್ರುವ ವರ್ಣ* region, UnicodeString& result U_LIFETIME_BOUND,
                                     ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ;
    UnicodeString& variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ;
    UnicodeString& ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, UnicodeString& result U_LIFETIME_BOUND,
                                  ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ;
    UnicodeString& ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ;
    ಶೂನ್ಯ initialize();

    ಸಂರಚನಾ CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ಧ್ರುವ Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ಧ್ರುವ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ಯಾವತ್ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ವಿಕಲ್ಪ (selector) {
            ಪಕ್ಷ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ಭಙ್ಗ;
            ಪಕ್ಷ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ಭಙ್ಗ;
            ಪಕ್ಷ UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                ಭಙ್ಗ;
            ಪಕ್ಷ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                ಭಙ್ಗ;
        }
    }
    initialize();
}

ಸಂರಚನಾ LocaleDisplayNamesImpl::CapitalizationContextSink : ಸಾರ್ವಜನಿಕ ResourceSink {
    ತಾರ್ಕಿಕ hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(ಅಸತ್ಯ), parent(_parent) {}
    ಆಭಾಸ ~CapitalizationContextSink();

    ಆಭಾಸ ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; contexts.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); ++i) {

            CapContextUsage usageEnum;
            ಯದಿ (uprv_strcmp(ಕೀಲಿ, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ಕೀಲಿ;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ಅನ್ಯಥಾ {
                ಅನುವರ್ತ;
            }

            int32_t len = 0;
            ಧ್ರುವ int32_t* intVector = value.getIntVector(len, errorCode);
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
            ಯದಿ (len < 2) { ಅನುವರ್ತ; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            ಯದಿ (titlecaseInt == 0) { ಅನುವರ್ತ; }

            parent.fCapitalization[usageEnum] = ಸತ್ಯ;
            hasCapitalizationUsage = ಸತ್ಯ;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

ಶೂನ್ಯ
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = ಏತತ್;
    nonConstThis->locale = langData.getLocale() == Locale::get_ಬೇರು()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    ಯದಿ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    ಯದಿ (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    ಯದಿ (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ಅನ್ಯಥಾ {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    ಯದಿ (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    ಕೀಲಿ_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, ಪರಿಮಾಣ(fCapitalization));
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    ತಾರ್ಕಿಕ needBrkIter = ಅಸತ್ಯ;
    ಯದಿ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        CapitalizationContextSink sink(*ಏತತ್);
        ures_get_All_ವಸ್ತುಗಳು_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        ಯದಿ (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } ಅನ್ಯಥಾ ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ಯದಿ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        ಯದಿ (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ಧ್ರುವ Locale&
LocaleDisplayNamesImpl::getLocale() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ಧ್ರುವ {
    ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ UDISPCTX_TYPE_DIALECT_HANDLING:
            ಪ್ರತ್ಯಾಗಮ static_cast<UDisplayContext>(dialectHandling);
        ಪಕ್ಷ UDISPCTX_TYPE_CAPITALIZATION:
            ಪ್ರತ್ಯಾಗಮ capitalizationContext;
        ಪಕ್ಷ UDISPCTX_TYPE_DISPLAY_LENGTH:
            ಪ್ರತ್ಯಾಗಮ nameLength;
        ಪಕ್ಷ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ಪ್ರತ್ಯಾಗಮ substitute;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ಯದಿ ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ಸ್ಥಿರ UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ಧ್ರುವ Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
  ಯದಿ (loc.isBogus()) {
    result.setToBogus();
    ಪ್ರತ್ಯಾಗಮ result;
  }
  UnicodeString resultName;

  ಧ್ರುವ ವರ್ಣ* lang = loc.getLanguage();
  ಯದಿ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ಧ್ರುವ ವರ್ಣ* script = loc.getScript();
  ಧ್ರುವ ವರ್ಣ* country = loc.getCountry();
  ಧ್ರುವ ವರ್ಣ* variant = loc.getVariant();

  ತಾರ್ಕಿಕ hasScript = uprv_strlen(script) > 0;
  ತಾರ್ಕಿಕ hasCountry = uprv_strlen(country) > 0;
  ತಾರ್ಕಿಕ hasVariant = uprv_strlen(variant) > 0;

  ಯದಿ (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    ಕುರು { // loop construct is so we can break early out of search
      ಯದಿ (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        ಯದಿ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ಅಸತ್ಯ);
          ಯದಿ (!resultName.isBogus()) {
            hasScript = ಅಸತ್ಯ;
            hasCountry = ಅಸತ್ಯ;
            ಭಙ್ಗ;
          }
        }
      }
      ಯದಿ (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        ಯದಿ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ಅಸತ್ಯ);
          ಯದಿ (!resultName.isBogus()) {
            hasScript = ಅಸತ್ಯ;
            ಭಙ್ಗ;
          }
        }
      }
      ಯದಿ (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        ಯದಿ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ಅಸತ್ಯ);
          ಯದಿ (!resultName.isBogus()) {
            hasCountry = ಅಸತ್ಯ;
            ಭಙ್ಗ;
          }
        }
      }
    } ಯಾವತ್ (ಅಸತ್ಯ);
  }
  ಯದಿ (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    ಯದಿ (resultName.isBogus()) {
      result.setToBogus();
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  ಯದಿ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, ಸತ್ಯ);
    ಯದಿ (script_str.isBogus()) {
      result.setToBogus();
      ಪ್ರತ್ಯಾಗಮ result;
    }
    resultRemainder.append(script_str);
  }
  ಯದಿ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, ಸತ್ಯ);
    ಯದಿ (region_str.isBogus()) {
      result.setToBogus();
      ಪ್ರತ್ಯಾಗಮ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  ಯದಿ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, ಸತ್ಯ);
    ಯದಿ (variant_str.isBogus()) {
      result.setToBogus();
      ಪ್ರತ್ಯಾಗಮ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  ಯದಿ (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ;
    ಯಾವತ್ ((ಕೀಲಿ = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ result;
      }
      ಕೀಲಿ_Display_Name(ಕೀಲಿ, temp, ಸತ್ಯ);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value.data(), temp2, ಸತ್ಯ);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ಯದಿ (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } ಅನ್ಯಥಾ ಯದಿ (temp != UnicodeString(ಕೀಲಿ, -1, US_INV)) {
        UnicodeString temp3;
        ಕೀಲಿ_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } ಅನ್ಯಥಾ {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ಯದಿ (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    ಪ್ರತ್ಯಾಗಮ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  ಪ್ರತ್ಯಾಗಮ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ಧ್ರುವ UnicodeString& src) ಧ್ರುವ {
    ಯದಿ (buffer.isEmpty()) {
        buffer.setTo(src);
    } ಅನ್ಯಥಾ {
        ಧ್ರುವ UnicodeString *ಮೌಲ್ಯಗಳು[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(ಮೌಲ್ಯಗಳು, 2, buffer, nullptr, 0, status);
    }
    ಪ್ರತ್ಯಾಗಮ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ಧ್ರುವ ವರ್ಣ* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ಧ್ರುವ ವರ್ಣ* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, ತಾರ್ಕಿಕ substitute) ಧ್ರುವ {
    ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        ಯದಿ (!result.isBogus()) {
            ಪ್ರತ್ಯಾಗಮ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    ಯದಿ (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ಧ್ರುವ ವರ್ಣ* canonLocId = canonLocale.getName();
        ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ಯದಿ (!result.isBogus()) {
                ಪ್ರತ್ಯಾಗಮ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ಯದಿ (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ಧ್ರುವ ವರ್ಣ* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಯದಿ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ಪ್ರತ್ಯಾಗಮ result = UnicodeString(lang, -1, US_INV);
    }
    ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        ಯದಿ (!result.isBogus()) {
            ಪ್ರತ್ಯಾಗಮ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    ಯದಿ (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ಧ್ರುವ ವರ್ಣ* canonLocId = canonLocale.getName();
        ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ಯದಿ (!result.isBogus()) {
                ಪ್ರತ್ಯಾಗಮ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ಯದಿ (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ಪ್ರತ್ಯಾಗಮ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ {
    ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        ಯದಿ (!result.isBogus()) {
            ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    ಯದಿ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } ಅನ್ಯಥಾ {
        langData.getNoFallback("Scripts", script, result);
    }
    ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ಧ್ರುವ ವರ್ಣ* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ scriptDisplayName(script, result, ಅಸತ್ಯ);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ scriptDisplayName(uscript_getName(scriptCode), result, ಅಸತ್ಯ);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ಧ್ರುವ ವರ್ಣ* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ {
    ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        ಯದಿ (!result.isBogus()) {
            ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    ಯದಿ (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } ಅನ್ಯಥಾ {
        regionData.getNoFallback("Countries", region, result);
    }
    ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ಧ್ರುವ ವರ್ಣ* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ regionDisplayName(region, result, ಅಸತ್ಯ);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ {
    // don't have a resource for short variant names
    ಯದಿ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } ಅನ್ಯಥಾ {
        langData.getNoFallback("Variants", variant, result);
    }
    ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ಧ್ರುವ ವರ್ಣ* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ variantDisplayName(variant, result, ಅಸತ್ಯ);
}

UnicodeString&
LocaleDisplayNamesImpl::ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ {
    // don't have a resource for short key names
    ಯದಿ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", ಕೀಲಿ, result);
    } ಅನ್ಯಥಾ {
        langData.getNoFallback("Keys", ಕೀಲಿ, result);
    }
    ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ಕೀಲಿ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                       UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_Display_Name(ಕೀಲಿ, result, ಅಸತ್ಯ);
}

UnicodeString&
LocaleDisplayNamesImpl::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                            ಧ್ರುವ ವರ್ಣ* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            ತಾರ್ಕಿಕ skipAdjust) ಧ್ರುವ {
    ಯದಿ (uprv_strcmp(ಕೀಲಿ, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_ಮೌಲ್ಯ(value, -1, US_INV);
        int32_t len;
        ಧ್ರುವ char16_t *currencyName = ucurr_getName(ustr_ಮೌಲ್ಯ.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        ಯದಿ (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_ಮೌಲ್ಯ;
            ಪ್ರತ್ಯಾಗಮ result;
        }
        result.setTo(currencyName, len);
        ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, result);
    }

    ಯದಿ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", ಕೀಲಿ, value, result);
        ಯದಿ (!result.isBogus()) {
            ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, result);
        }
    }
    ಯದಿ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", ಕೀಲಿ, value, result);
    } ಅನ್ಯಥಾ {
        langData.getNoFallback("Types", ಕೀಲಿ, value, result);
    }
    ಪ್ರತ್ಯಾಗಮ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ಕೀಲಿ_ಮೌಲ್ಯ, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                                            ಧ್ರುವ ವರ್ಣ* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value, result, ಅಸತ್ಯ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ಧ್ರುವ Locale& locale,
                                   UDialectHandling dialectHandling) {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ಧ್ರುವ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ಯದಿ (contexts == nullptr) {
        length = 0;
    }
    ಪ್ರತ್ಯಾಗಮ ನೂತನ LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ಧ್ರುವ ವರ್ಣ * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ಪ್ರತ್ಯಾಗಮ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ಧ್ರುವ ವರ್ಣ * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ಪ್ರತ್ಯಾಗಮ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uldn_getLocale(ಧ್ರುವ ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  ಯದಿ (ldn) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ಧ್ರುವ ULocaleDisplayNames *ldn) {
  ಯದಿ (ldn) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ಪ್ರತ್ಯಾಗಮ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ಧ್ರುವ ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ (UDisplayContext)0;
  }
  ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                       ಧ್ರುವ ವರ್ಣ *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  ಯದಿ (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                         ಧ್ರುವ ವರ್ಣ *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                       ಧ್ರುವ ವರ್ಣ *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  ಪ್ರತ್ಯಾಗಮ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                       ಧ್ರುವ ವರ್ಣ *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ಧ್ರುವ ULocaleDisplayNames *ldn,
                        ಧ್ರುವ ವರ್ಣ *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ಕೀಲಿ_Display_Name(ಧ್ರುವ ULocaleDisplayNames *ldn,
                    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || ಕೀಲಿ == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->ಕೀಲಿ_Display_Name(ಕೀಲಿ, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಧ್ರುವ ULocaleDisplayNames *ldn,
                         ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ,
                         ಧ್ರುವ ವರ್ಣ *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ಯದಿ (U_FAILURE(*pErrorCode)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (ldn == nullptr || ಕೀಲಿ == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ಧ್ರುವ LocaleDisplayNames *)ldn)->ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಕೀಲಿ, value, temp);
  ಪ್ರತ್ಯಾಗಮ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
