// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_FORMATTING

#ଅନ୍ତର୍ଭାଵ "unicode/locdspnm.h"
#ଅନ୍ତର୍ଭାଵ "unicode/simpleformatter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucasemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udisplaycontext.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucurr.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "ureslocs.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
ଶ୍ରେଣୀ ICUDataTable {
    ଅଚଳ ଅକ୍ଷର* ଅଚଳ path;
    Locale locale;

ସର୍ବସାଧାରଣ:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ Locale& locale);
    ~ICUDataTable() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

    ଅଚଳ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                       UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
    UnicodeString& get(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* sub_Table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                       UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;

    UnicodeString& getNoFallback(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                                 UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
    UnicodeString& getNoFallback(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* sub_Table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                                 UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
};

ଧାଡ଼ିଭିତରେ UnicodeString& ICUDataTable::get(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                                        UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ get(table_ଚାବି, nullptr, item_ଚାବି, result);
}

ଧାଡ଼ିଭିତରେ UnicodeString& ICUDataTable::getNoFallback(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                                                  UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ getNoFallback(table_ଚାବି, nullptr, item_ଚାବି, result);
}

ICUDataTable::ICUDataTable(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ଅଚଳ Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ଫେରନ୍ତୁ locale;
}

UnicodeString&
ICUDataTable::get(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* sub_Table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                  UnicodeString &result U_LIFETIME_BOUND) ଅଚଳ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ଅଚଳ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ଚାବି, sub_Table_ଚାବି, item_ଚାବି,
                                                   &len, &status);
  ଯଦି (U_SUCCESS(status) && len > 0) {
    ଫେରନ୍ତୁ result.setTo(s, len);
  }
  ଫେରନ୍ତୁ result.setTo(UnicodeString(item_ଚାବି, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ଅଚଳ ଅକ୍ଷର* table_ଚାବି, ଅଚଳ ଅକ୍ଷର* sub_Table_ଚାବି, ଅଚଳ ଅକ୍ଷର* item_ଚାବି,
                            UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ଅଚଳ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ଚାବି, sub_Table_ଚାବି, item_ଚାବି,
                                                   &len, &status);
  ଯଦି (U_SUCCESS(status)) {
    ଫେରନ୍ତୁ result.setTo(s, len);
  }

  result.setToBogus();
  ଫେରନ୍ତୁ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ଯଦି 0  // currently unused

ଶ୍ରେଣୀ DefaultLocaleDisplayNames : ସର୍ବସାଧାରଣ LocaleDisplayNames {
  UDialectHandling dialectHandling;

ସର୍ବସାଧାରଣ:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  ଆଭାସୀ ~DefaultLocaleDisplayNames();

  ଆଭାସୀ ଅଚଳ Locale& getLocale() ଅଚଳ U_LIFETIME_BOUND;
  ଆଭାସୀ UDialectHandling getDialectHandling() ଅଚଳ;

  ଆଭାସୀ UnicodeString& localeDisplayName(ଅଚଳ Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& localeDisplayName(ଅଚଳ ଅକ୍ଷର* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& languageDisplayName(ଅଚଳ ଅକ୍ଷର* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& regionDisplayName(ଅଚଳ ଅକ୍ଷର* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                        UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
  ଆଭାସୀ UnicodeString& ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                             ଅଚଳ ଅକ୍ଷର* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ଅଚଳ Locale&
DefaultLocaleDisplayNames::getLocale() ଅଚଳ U_LIFETIME_BOUND {
  ଫେରନ୍ତୁ Locale::get_ମୂଳ();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ଅଚଳ {
  ଫେରନ୍ତୁ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ଅଚଳ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ଅଚଳ ଅକ୍ଷର* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ଅଚଳ ଅକ୍ଷର* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଅଚଳ ଅକ୍ଷର* name = uscript_getName(scriptCode);
  ଯଦି (name) {
    ଫେରନ୍ତୁ result = UnicodeString(name, -1, US_INV);
  }
  ଫେରନ୍ତୁ result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ଅଚଳ ଅକ୍ଷର* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(ଚାବି, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* /*ଚାବି*/,
                                               ଅଚଳ ଅକ୍ଷର* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଫେରନ୍ତୁ result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

ଶ୍ରେଣୀ LocaleDisplayNamesImpl : ସର୍ବସାଧାରଣ LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ଚାବି_Type_Format;
    UDisplayContext capitalizationContext;
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ଅନ୍ୟଥା
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ଗଣନା CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_ଚାବି,
        k_Cap_Context_Usage_ଚାବି_ମୂଲ୍ୟ,
        k_Cap_Context_Usage_ଗଣନା
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     ତାର୍କିକ fCapitalization[k_Cap_Context_Usage_ଗଣନା];

ସର୍ବସାଧାରଣ:
    // constructor
    LocaleDisplayNamesImpl(ଅଚଳ Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ଅଚଳ Locale& locale, UDisplayContext *contexts, int32_t length);
    ଆଭାସୀ ~LocaleDisplayNamesImpl();

    ଆଭାସୀ ଅଚଳ Locale& getLocale() ଅଚଳ U_LIFETIME_BOUND override;
    ଆଭାସୀ UDialectHandling getDialectHandling() ଅଚଳ override;
    ଆଭାସୀ UDisplayContext getContext(UDisplayContextType type) ଅଚଳ override;

    ଆଭାସୀ UnicodeString& localeDisplayName(ଅଚଳ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString& localeDisplayName(ଅଚଳ ଅକ୍ଷର* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString&
    languageDisplayName(ଅଚଳ ଅକ୍ଷର* lang, UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString& scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString& regionDisplayName(ଅଚଳ ଅକ୍ଷର* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString&
    variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant, UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString& ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;
    ଆଭାସୀ UnicodeString&
    ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି, ଅଚଳ ଅକ୍ଷର* value,
                        UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ override;

ବ୍ୟକ୍ତିଗତ:
    UnicodeString& localeIdName(ଅଚଳ ଅକ୍ଷର* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                ତାର୍କିକ substitute) ଅଚଳ;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ଅଚଳ UnicodeString& src) ଅଚଳ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ;
    UnicodeString& scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script, UnicodeString& result U_LIFETIME_BOUND,
                                     ତାର୍କିକ skipAdjust) ଅଚଳ;
    UnicodeString& regionDisplayName(ଅଚଳ ଅକ୍ଷର* region, UnicodeString& result U_LIFETIME_BOUND,
                                     ତାର୍କିକ skipAdjust) ଅଚଳ;
    UnicodeString& variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      ତାର୍କିକ skipAdjust) ଅଚଳ;
    UnicodeString& ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି, UnicodeString& result U_LIFETIME_BOUND,
                                  ତାର୍କିକ skipAdjust) ଅଚଳ;
    UnicodeString& ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି, ଅଚଳ ଅକ୍ଷର* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       ତାର୍କିକ skipAdjust) ଅଚଳ;
    ଶୂନ୍ୟ initialize();

    ସଂରଚନା CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ଅଚଳ Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ଅଚଳ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ଯେତେବେଳେ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ବାଛ (selector) {
            ସ୍ଥିତି UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଭାଙ୍ଗ;
        }
    }
    initialize();
}

ସଂରଚନା LocaleDisplayNamesImpl::CapitalizationContextSink : ସର୍ବସାଧାରଣ ResourceSink {
    ତାର୍କିକ hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(ମିଥ୍ୟା), parent(_parent) {}
    ଆଭାସୀ ~CapitalizationContextSink();

    ଆଭାସୀ ଶୂନ୍ୟ put(ଅଚଳ ଅକ୍ଷର *ଚାବି, Resource_ମୂଲ୍ୟ &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; contexts.get_ଚାବି_And_ମୂଲ୍ୟ(i, ଚାବି, value); ++i) {

            CapContextUsage usageEnum;
            ଯଦି (uprv_strcmp(ଚାବି, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ଚାବି;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ଚାବି_ମୂଲ୍ୟ;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ଅନ୍ୟଥା {
                ଜାରି;
            }

            int32_t len = 0;
            ଅଚଳ int32_t* intVector = value.getIntVector(len, errorCode);
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
            ଯଦି (len < 2) { ଜାରି; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            ଯଦି (titlecaseInt == 0) { ଜାରି; }

            parent.fCapitalization[usageEnum] = ସତ୍ୟ;
            hasCapitalizationUsage = ସତ୍ୟ;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

ଶୂନ୍ୟ
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = ଏହି;
    nonConstThis->locale = langData.getLocale() == Locale::get_ମୂଳ()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    ଯଦି (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    ଯଦି (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    ଯଦି (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ଅନ୍ୟଥା {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    ଯଦି (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    ଚାବି_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, ଆକାର(fCapitalization));
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    ତାର୍କିକ needBrkIter = ମିଥ୍ୟା;
    ଯଦି (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        CapitalizationContextSink sink(*ଏହି);
        ures_get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        ଯଦି (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } ଅନ୍ୟଥା ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ଯଦି (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        ଯଦି (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ଅଚଳ Locale&
LocaleDisplayNamesImpl::getLocale() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ଅଚଳ {
    ଫେରନ୍ତୁ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ଅଚଳ {
    ବାଛ (type) {
        ସ୍ଥିତି UDISPCTX_TYPE_DIALECT_HANDLING:
            ଫେରନ୍ତୁ static_cast<UDisplayContext>(dialectHandling);
        ସ୍ଥିତି UDISPCTX_TYPE_CAPITALIZATION:
            ଫେରନ୍ତୁ capitalizationContext;
        ସ୍ଥିତି UDISPCTX_TYPE_DISPLAY_LENGTH:
            ଫେରନ୍ତୁ nameLength;
        ସ୍ଥିତି UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ଫେରନ୍ତୁ substitute;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ଯଦି ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ସ୍ଥିର UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ଫେରନ୍ତୁ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ଅଚଳ Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
  ଯଦି (loc.isBogus()) {
    result.setToBogus();
    ଫେରନ୍ତୁ result;
  }
  UnicodeString resultName;

  ଅଚଳ ଅକ୍ଷର* lang = loc.getLanguage();
  ଯଦି (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ଅଚଳ ଅକ୍ଷର* script = loc.getScript();
  ଅଚଳ ଅକ୍ଷର* country = loc.getCountry();
  ଅଚଳ ଅକ୍ଷର* variant = loc.getVariant();

  ତାର୍କିକ hasScript = uprv_strlen(script) > 0;
  ତାର୍କିକ hasCountry = uprv_strlen(country) > 0;
  ତାର୍କିକ hasVariant = uprv_strlen(variant) > 0;

  ଯଦି (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    କର { // loop construct is so we can break early out of search
      ଯଦି (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        ଯଦି (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ମିଥ୍ୟା);
          ଯଦି (!resultName.isBogus()) {
            hasScript = ମିଥ୍ୟା;
            hasCountry = ମିଥ୍ୟା;
            ଭାଙ୍ଗ;
          }
        }
      }
      ଯଦି (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        ଯଦି (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ମିଥ୍ୟା);
          ଯଦି (!resultName.isBogus()) {
            hasScript = ମିଥ୍ୟା;
            ଭାଙ୍ଗ;
          }
        }
      }
      ଯଦି (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        ଯଦି (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ମିଥ୍ୟା);
          ଯଦି (!resultName.isBogus()) {
            hasCountry = ମିଥ୍ୟା;
            ଭାଙ୍ଗ;
          }
        }
      }
    } ଯେତେବେଳେ (ମିଥ୍ୟା);
  }
  ଯଦି (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    ଯଦି (resultName.isBogus()) {
      result.setToBogus();
      ଫେରନ୍ତୁ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  ଯଦି (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, ସତ୍ୟ);
    ଯଦି (script_str.isBogus()) {
      result.setToBogus();
      ଫେରନ୍ତୁ result;
    }
    resultRemainder.append(script_str);
  }
  ଯଦି (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, ସତ୍ୟ);
    ଯଦି (region_str.isBogus()) {
      result.setToBogus();
      ଫେରନ୍ତୁ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  ଯଦି (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, ସତ୍ୟ);
    ଯଦି (variant_str.isBogus()) {
      result.setToBogus();
      ଫେରନ୍ତୁ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  ଯଦି (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ଅଚଳ ଅକ୍ଷର* ଚାବି;
    ଯେତେବେଳେ ((ଚାବି = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_ମୂଲ୍ୟ<CharString>(ଚାବି, status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ result;
      }
      ଚାବି_Display_Name(ଚାବି, temp, ସତ୍ୟ);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ଚାବି_ମୂଲ୍ୟ_Display_Name(ଚାବି, value.data(), temp2, ସତ୍ୟ);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ଯଦି (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } ଅନ୍ୟଥା ଯଦି (temp != UnicodeString(ଚାବି, -1, US_INV)) {
        UnicodeString temp3;
        ଚାବି_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } ଅନ୍ୟଥା {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ଯଦି (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    ଫେରନ୍ତୁ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  ଫେରନ୍ତୁ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ଅଚଳ UnicodeString& src) ଅଚଳ {
    ଯଦି (buffer.isEmpty()) {
        buffer.setTo(src);
    } ଅନ୍ୟଥା {
        ଅଚଳ UnicodeString *ମୂଲ୍ୟଗୁଡ଼ିକ[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(ମୂଲ୍ୟଗୁଡ଼ିକ, 2, buffer, nullptr, 0, status);
    }
    ଫେରନ୍ତୁ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ଅଚଳ ଅକ୍ଷର* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ଅଚଳ ଅକ୍ଷର* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, ତାର୍କିକ substitute) ଅଚଳ {
    ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        ଯଦି (!result.isBogus()) {
            ଫେରନ୍ତୁ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    ଯଦି (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ଅଚଳ ଅକ୍ଷର* canonLocId = canonLocale.getName();
        ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ଯଦି (!result.isBogus()) {
                ଫେରନ୍ତୁ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ଯଦି (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ଫେରନ୍ତୁ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ଅଚଳ ଅକ୍ଷର* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଯଦି (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ଫେରନ୍ତୁ result = UnicodeString(lang, -1, US_INV);
    }
    ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        ଯଦି (!result.isBogus()) {
            ଫେରନ୍ତୁ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    ଯଦି (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ଅଚଳ ଅକ୍ଷର* canonLocId = canonLocale.getName();
        ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ଯଦି (!result.isBogus()) {
                ଫେରନ୍ତୁ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ଯଦି (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ଫେରନ୍ତୁ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          ତାର୍କିକ skipAdjust) ଅଚଳ {
    ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        ଯଦି (!result.isBogus()) {
            ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    ଯଦି (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } ଅନ୍ୟଥା {
        langData.getNoFallback("Scripts", script, result);
    }
    ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ଅଚଳ ଅକ୍ଷର* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ scriptDisplayName(script, result, ମିଥ୍ୟା);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ scriptDisplayName(uscript_getName(scriptCode), result, ମିଥ୍ୟା);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ଅଚଳ ଅକ୍ଷର* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          ତାର୍କିକ skipAdjust) ଅଚଳ {
    ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        ଯଦି (!result.isBogus()) {
            ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    ଯଦି (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } ଅନ୍ୟଥା {
        regionData.getNoFallback("Countries", region, result);
    }
    ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ଅଚଳ ଅକ୍ଷର* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ regionDisplayName(region, result, ମିଥ୍ୟା);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           ତାର୍କିକ skipAdjust) ଅଚଳ {
    // don't have a resource for short variant names
    ଯଦି (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } ଅନ୍ୟଥା {
        langData.getNoFallback("Variants", variant, result);
    }
    ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ଅଚଳ ଅକ୍ଷର* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ variantDisplayName(variant, result, ମିଥ୍ୟା);
}

UnicodeString&
LocaleDisplayNamesImpl::ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       ତାର୍କିକ skipAdjust) ଅଚଳ {
    // don't have a resource for short key names
    ଯଦି (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", ଚାବି, result);
    } ଅନ୍ୟଥା {
        langData.getNoFallback("Keys", ଚାବି, result);
    }
    ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ଚାବି, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ଚାବି_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                       UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ ଚାବି_Display_Name(ଚାବି, result, ମିଥ୍ୟା);
}

UnicodeString&
LocaleDisplayNamesImpl::ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                            ଅଚଳ ଅକ୍ଷର* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            ତାର୍କିକ skipAdjust) ଅଚଳ {
    ଯଦି (uprv_strcmp(ଚାବି, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_ମୂଲ୍ୟ(value, -1, US_INV);
        int32_t len;
        ଅଚଳ char16_t *currencyName = ucurr_getName(ustr_ମୂଲ୍ୟ.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        ଯଦି (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_ମୂଲ୍ୟ;
            ଫେରନ୍ତୁ result;
        }
        result.setTo(currencyName, len);
        ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ଚାବି_ମୂଲ୍ୟ, result);
    }

    ଯଦି (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", ଚାବି, value, result);
        ଯଦି (!result.isBogus()) {
            ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ଚାବି_ମୂଲ୍ୟ, result);
        }
    }
    ଯଦି (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", ଚାବି, value, result);
    } ଅନ୍ୟଥା {
        langData.getNoFallback("Types", ଚାବି, value, result);
    }
    ଫେରନ୍ତୁ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ଚାବି_ମୂଲ୍ୟ, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ଅକ୍ଷର* ଚାବି,
                                            ଅଚଳ ଅକ୍ଷର* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ଅଚଳ {
    ଫେରନ୍ତୁ ଚାବି_ମୂଲ୍ୟ_Display_Name(ଚାବି, value, result, ମିଥ୍ୟା);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ଅଚଳ Locale& locale,
                                   UDialectHandling dialectHandling) {
    ଫେରନ୍ତୁ ନୂତନ LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ଅଚଳ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ଯଦି (contexts == nullptr) {
        length = 0;
    }
    ଫେରନ୍ତୁ ନୂତନ LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ଅଚଳ ଅକ୍ଷର * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ଫେରନ୍ତୁ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ଅଚଳ ଅକ୍ଷର * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ଫେରନ୍ତୁ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uldn_getLocale(ଅଚଳ ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  ଯଦି (ldn) {
    ଫେରନ୍ତୁ ((ଅଚଳ LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ଫେରନ୍ତୁ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ଅଚଳ ULocaleDisplayNames *ldn) {
  ଯଦି (ldn) {
    ଫେରନ୍ତୁ ((ଅଚଳ LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ଫେରନ୍ତୁ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ଅଚଳ ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ (UDisplayContext)0;
  }
  ଫେରନ୍ତୁ ((ଅଚଳ LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                       ଅଚଳ ଅକ୍ଷର *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  ଯଦି (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                         ଅଚଳ ଅକ୍ଷର *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                       ଅଚଳ ଅକ୍ଷର *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  ଫେରନ୍ତୁ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                       ଅଚଳ ଅକ୍ଷର *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ଅଚଳ ULocaleDisplayNames *ldn,
                        ଅଚଳ ଅକ୍ଷର *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ଚାବି_Display_Name(ଅଚଳ ULocaleDisplayNames *ldn,
                    ଅଚଳ ଅକ୍ଷର *ଚାବି,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || ଚାବି == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->ଚାବି_Display_Name(ଚାବି, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ଚାବି_ମୂଲ୍ୟ_Display_Name(ଅଚଳ ULocaleDisplayNames *ldn,
                         ଅଚଳ ଅକ୍ଷର *ଚାବି,
                         ଅଚଳ ଅକ୍ଷର *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ଯଦି (U_FAILURE(*pErrorCode)) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (ldn == nullptr || ଚାବି == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ଅଚଳ LocaleDisplayNames *)ldn)->ଚାବି_ମୂଲ୍ୟ_Display_Name(ଚାବି, value, temp);
  ଫେରନ୍ତୁ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
