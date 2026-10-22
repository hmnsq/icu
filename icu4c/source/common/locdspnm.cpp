// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_FORMATTING

#समावेश "unicode/locdspnm.h"
#समावेश "unicode/simpleformatter.h"
#समावेश "unicode/ucasemap.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/udisplaycontext.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/ucurr.h"
#समावेश "bytesinkutil.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "mutex.h"
#समावेश "uassert.h"
#समावेश "ulocimp.h"
#समावेश "umutex.h"
#समावेश "ureslocs.h"
#समावेश "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
श्रेणी ICUDataTable {
    स्थिर अक्षर* स्थिर path;
    Locale locale;

खुला:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(स्थिर अक्षर* path, स्थिर Locale& locale);
    ~ICUDataTable() = शेष;

    स्थिर Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& लो(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* item_कुंजी,
                       UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
    UnicodeString& लो(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* sub_Table_कुंजी, स्थिर अक्षर* item_कुंजी,
                       UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;

    UnicodeString& getNoFallback(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* item_कुंजी,
                                 UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
    UnicodeString& getNoFallback(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* sub_Table_कुंजी, स्थिर अक्षर* item_कुंजी,
                                 UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
};

अंतरभूत UnicodeString& ICUDataTable::लो(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* item_कुंजी,
                                        UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस लो(table_कुंजी, nullptr, item_कुंजी, परिणाम);
}

अंतरभूत UnicodeString& ICUDataTable::getNoFallback(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* item_कुंजी,
                                                  UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस getNoFallback(table_कुंजी, nullptr, item_कुंजी, परिणाम);
}

ICUDataTable::ICUDataTable(स्थिर अक्षर* path, स्थिर Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

स्थिर Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  वापस locale;
}

UnicodeString&
ICUDataTable::लो(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* sub_Table_कुंजी, स्थिर अक्षर* item_कुंजी,
                  UnicodeString &परिणाम U_LIFETIME_BOUND) स्थिर {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  स्थिर char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_कुंजी, sub_Table_कुंजी, item_कुंजी,
                                                   &len, &status);
  अगर (U_SUCCESS(status) && len > 0) {
    वापस परिणाम.setTo(s, len);
  }
  वापस परिणाम.setTo(UnicodeString(item_कुंजी, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(स्थिर अक्षर* table_कुंजी, स्थिर अक्षर* sub_Table_कुंजी, स्थिर अक्षर* item_कुंजी,
                            UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  स्थिर char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_कुंजी, sub_Table_कुंजी, item_कुंजी,
                                                   &len, &status);
  अगर (U_SUCCESS(status)) {
    वापस परिणाम.setTo(s, len);
  }

  परिणाम.setToBogus();
  वापस परिणाम;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#अगर 0  // currently unused

श्रेणी DefaultLocaleDisplayNames : खुला LocaleDisplayNames {
  UDialectHandling dialectHandling;

खुला:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  भव ~DefaultLocaleDisplayNames();

  भव स्थिर Locale& getLocale() स्थिर U_LIFETIME_BOUND;
  भव UDialectHandling getDialectHandling() स्थिर;

  भव UnicodeString& localeDisplayName(स्थिर Locale& locale,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& localeDisplayName(स्थिर अक्षर* localeId,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& languageDisplayName(स्थिर अक्षर* lang,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& scriptDisplayName(स्थिर अक्षर* script,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& regionDisplayName(स्थिर अक्षर* region,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& variantDisplayName(स्थिर अक्षर* variant,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                        UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
  भव UnicodeString& कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी,
                                             स्थिर अक्षर* value,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

स्थिर Locale&
DefaultLocaleDisplayNames::getLocale() स्थिर U_LIFETIME_BOUND {
  वापस Locale::get_मूल();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() स्थिर {
  वापस dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(स्थिर Locale& locale,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(स्थिर अक्षर* localeId,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(स्थिर अक्षर* lang,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(स्थिर अक्षर* script,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  स्थिर अक्षर* name = uscript_getName(scriptCode);
  अगर (name) {
    वापस परिणाम = UnicodeString(name, -1, US_INV);
  }
  वापस परिणाम.हटाओ();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(स्थिर अक्षर* region,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(स्थिर अक्षर* variant,
                                              UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(कुंजी, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::कुंजी_मूल्य_Display_Name(स्थिर अक्षर* /*कुंजी*/,
                                               स्थिर अक्षर* value,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  वापस परिणाम = UnicodeString(value, -1, US_INV);
}

#पूर्ण_अगर  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

श्रेणी LocaleDisplayNamesImpl : खुला LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable lang_तथ्य;
    ICUDataTable region_तथ्य;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter कुंजी_Type_Format;
    UDisplayContext capitalizationContext;
#अगर !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#अन्यथा
    UObject* capitalizationBrkIter;
#पूर्ण_अगर
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_लंबाई;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    क्रमागत CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_कुंजी,
        k_Cap_Context_Usage_कुंजी_मूल्य,
        k_Cap_Context_Usage_गिनती
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     bool fCapitalization[k_Cap_Context_Usage_गिनती];

खुला:
    // constructor
    LocaleDisplayNamesImpl(स्थिर Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(स्थिर Locale& locale, UDisplayContext *contexts, int32_t length);
    भव ~LocaleDisplayNamesImpl();

    भव स्थिर Locale& getLocale() स्थिर U_LIFETIME_BOUND override;
    भव UDialectHandling getDialectHandling() स्थिर override;
    भव UDisplayContext getContext(UDisplayContextType type) स्थिर override;

    भव UnicodeString& localeDisplayName(स्थिर Locale& locale,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString& localeDisplayName(स्थिर अक्षर* localeId,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString&
    languageDisplayName(स्थिर अक्षर* lang, UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString& scriptDisplayName(स्थिर अक्षर* script,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString& regionDisplayName(स्थिर अक्षर* region,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString&
    variantDisplayName(स्थिर अक्षर* variant, UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString& कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;
    भव UnicodeString&
    कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी, स्थिर अक्षर* value,
                        UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर override;

निजी:
    UnicodeString& localeIdName(स्थिर अक्षर* localeId, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                bool substitute) स्थिर;
    UnicodeString& appendWithSep(UnicodeString& बफर U_LIFETIME_BOUND,
                                 स्थिर UnicodeString& src) स्थिर;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर;
    UnicodeString& scriptDisplayName(स्थिर अक्षर* script, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     bool skipAdjust) स्थिर;
    UnicodeString& regionDisplayName(स्थिर अक्षर* region, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     bool skipAdjust) स्थिर;
    UnicodeString& variantDisplayName(स्थिर अक्षर* variant, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      bool skipAdjust) स्थिर;
    UnicodeString& कुंजी_Display_Name(स्थिर अक्षर* कुंजी, UnicodeString& परिणाम U_LIFETIME_BOUND,
                                  bool skipAdjust) स्थिर;
    UnicodeString& कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी, स्थिर अक्षर* value,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND,
                                       bool skipAdjust) स्थिर;
    व्योम initialize();

    काष्ठा CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(स्थिर Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_तथ्य(U_ICUDATA_LANG, locale)
    , region_तथ्य(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , name_लंबाई(UDISPCTX_लंबाई_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(स्थिर Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , lang_तथ्य(U_ICUDATA_LANG, locale)
    , region_तथ्य(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , name_लंबाई(UDISPCTX_लंबाई_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    जबतक (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        चयन (selector) {
            हाल UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                अवरोध;
            हाल UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                अवरोध;
            हाल UDISPCTX_TYPE_DISPLAY_लंबाई:
                name_लंबाई = value;
                अवरोध;
            हाल UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                अवरोध;
            शेष:
                अवरोध;
        }
    }
    initialize();
}

काष्ठा LocaleDisplayNamesImpl::CapitalizationContextSink : खुला ResourceSink {
    bool hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(false), parent(_parent) {}
    भव ~CapitalizationContextSink();

    भव व्योम दो(स्थिर अक्षर *कुंजी, Resource_मूल्य &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        क्रम (पूर्णांक i = 0; contexts.get_कुंजी_And_मूल्य(i, कुंजी, value); ++i) {

            CapContextUsage usageEnum;
            अगर (uprv_strcmp(कुंजी, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_कुंजी;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_कुंजी_मूल्य;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } अन्यथा अगर (uprv_strcmp(कुंजी, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } अन्यथा {
                जारी;
            }

            int32_t len = 0;
            स्थिर int32_t* intVector = value.getIntVector(len, errorCode);
            अगर (U_FAILURE(errorCode)) { वापस; }
            अगर (len < 2) { जारी; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            अगर (titlecaseInt == 0) { जारी; }

            parent.fCapitalization[usageEnum] = true;
            hasCapitalizationUsage = true;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

व्योम
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = यह;
    nonConstThis->locale = lang_तथ्य.getLocale() == Locale::get_मूल()
        ? region_तथ्य.getLocale()
        : lang_तथ्य.getLocale();

    UnicodeString sep;
    lang_तथ्य.getNoFallback("localeDisplayPattern", "separator", sep);
    अगर (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    lang_तथ्य.getNoFallback("localeDisplayPattern", "pattern", pattern);
    अगर (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    अगर (pattern.सूचकांक_Of(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } अन्यथा {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    lang_तथ्य.लो("localeDisplayPattern", "keyTypePattern", ktPattern);
    अगर (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    कुंजी_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, माप(fCapitalization));
#अगर !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    bool needBrkIter = false;
    अगर (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        अगर (U_FAILURE(status)) { वापस; }
        CapitalizationContextSink sink(*यह);
        ures_get_All_वस्तुएँ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        अगर (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } अन्यथा अगर (U_FAILURE(status)) {
            वापस;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    अगर (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        अगर (U_FAILURE(status)) {
            मिटाओ capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#पूर्ण_अगर
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#अगर !UCONFIG_NO_BREAK_ITERATION
    मिटाओ capitalizationBrkIter;
#पूर्ण_अगर
}

स्थिर Locale&
LocaleDisplayNamesImpl::getLocale() स्थिर U_LIFETIME_BOUND {
    वापस locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() स्थिर {
    वापस dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) स्थिर {
    चयन (type) {
        हाल UDISPCTX_TYPE_DIALECT_HANDLING:
            वापस static_cast<UDisplayContext>(dialectHandling);
        हाल UDISPCTX_TYPE_CAPITALIZATION:
            वापस capitalizationContext;
        हाल UDISPCTX_TYPE_DISPLAY_लंबाई:
            वापस name_लंबाई;
        हाल UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            वापस substitute;
        शेष:
            अवरोध;
    }
    वापस static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
#अगर !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    अगर ( परिणाम.length() > 0 && u_islower(परिणाम.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        अटल UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        परिणाम.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#पूर्ण_अगर
    वापस परिणाम;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(स्थिर Locale& loc,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
  अगर (loc.isBogus()) {
    परिणाम.setToBogus();
    वापस परिणाम;
  }
  UnicodeString परिणाम_Name;

  स्थिर अक्षर* lang = loc.getLanguage();
  अगर (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  स्थिर अक्षर* script = loc.getScript();
  स्थिर अक्षर* country = loc.getCountry();
  स्थिर अक्षर* variant = loc.getVariant();

  bool hasScript = uprv_strlen(script) > 0;
  bool hasCountry = uprv_strlen(country) > 0;
  bool hasVariant = uprv_strlen(variant) > 0;

  अगर (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString बफर;
    करो { // loop construct is so we can break early out of search
      अगर (hasScript && hasCountry) {
        बफर.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        अगर (U_SUCCESS(status)) {
          localeIdName(बफर.तथ्य(), परिणाम_Name, false);
          अगर (!परिणाम_Name.isBogus()) {
            hasScript = false;
            hasCountry = false;
            अवरोध;
          }
        }
      }
      अगर (hasScript) {
        बफर.append(lang, status)
              .append('_', status)
              .append(script, status);
        अगर (U_SUCCESS(status)) {
          localeIdName(बफर.तथ्य(), परिणाम_Name, false);
          अगर (!परिणाम_Name.isBogus()) {
            hasScript = false;
            अवरोध;
          }
        }
      }
      अगर (hasCountry) {
        बफर.append(lang, status)
              .append('_', status)
              .append(country, status);
        अगर (U_SUCCESS(status)) {
          localeIdName(बफर.तथ्य(), परिणाम_Name, false);
          अगर (!परिणाम_Name.isBogus()) {
            hasCountry = false;
            अवरोध;
          }
        }
      }
    } जबतक (false);
  }
  अगर (परिणाम_Name.isBogus() || परिणाम_Name.isEmpty()) {
    localeIdName(lang, परिणाम_Name, substitute == UDISPCTX_SUBSTITUTE);
    अगर (परिणाम_Name.isBogus()) {
      परिणाम.setToBogus();
      वापस परिणाम;
    }
  }

  UnicodeString परिणाम_Remainder;
  UnicodeString अस्थायी;
  UErrorCode status = U_ZERO_ERROR;

  अगर (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, अस्थायी, true);
    अगर (script_str.isBogus()) {
      परिणाम.setToBogus();
      वापस परिणाम;
    }
    परिणाम_Remainder.append(script_str);
  }
  अगर (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, अस्थायी, true);
    अगर (region_str.isBogus()) {
      परिणाम.setToBogus();
      वापस परिणाम;
    }
    appendWithSep(परिणाम_Remainder, region_str);
  }
  अगर (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, अस्थायी, true);
    अगर (variant_str.isBogus()) {
      परिणाम.setToBogus();
      वापस परिणाम;
    }
    appendWithSep(परिणाम_Remainder, variant_str);
  }
  परिणाम_Remainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  परिणाम_Remainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  अगर (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    स्थिर अक्षर* कुंजी;
    जबतक ((कुंजी = e->next((int32_t*)nullptr, status)) != nullptr) {
        स्वतः value = loc.get_Keyword_मूल्य<CharString>(कुंजी, status);
        अगर (U_FAILURE(status)) {
            वापस परिणाम;
      }
      कुंजी_Display_Name(कुंजी, अस्थायी, true);
      अस्थायी.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      अस्थायी.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      कुंजी_मूल्य_Display_Name(कुंजी, value.तथ्य(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      अगर (temp2 != UnicodeString(value.तथ्य(), -1, US_INV)) {
        appendWithSep(परिणाम_Remainder, temp2);
      } अन्यथा अगर (अस्थायी != UnicodeString(कुंजी, -1, US_INV)) {
        UnicodeString temp3;
        कुंजी_Type_Format.format(अस्थायी, temp2, temp3, status);
        appendWithSep(परिणाम_Remainder, temp3);
      } अन्यथा {
        appendWithSep(परिणाम_Remainder, अस्थायी)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  अगर (!परिणाम_Remainder.isEmpty()) {
    format.format(परिणाम_Name, परिणाम_Remainder, परिणाम.हटाओ(), status);
    वापस adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
  }

  परिणाम = परिणाम_Name;
  वापस adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& बफर U_LIFETIME_BOUND, स्थिर UnicodeString& src) स्थिर {
    अगर (बफर.isEmpty()) {
        बफर.setTo(src);
    } अन्यथा {
        स्थिर UnicodeString *मूल्यावली[2] = { &बफर, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मूल्यावली, 2, बफर, nullptr, 0, status);
    }
    वापस बफर;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(स्थिर अक्षर* localeId,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस localeDisplayName(Locale(localeId), परिणाम);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(स्थिर अक्षर* localeId,
                                     UnicodeString& परिणाम U_LIFETIME_BOUND, bool substitute) स्थिर {
    अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
        lang_तथ्य.getNoFallback("Languages%short", localeId, परिणाम);
        अगर (!परिणाम.isBogus()) {
            वापस परिणाम;
        }
    }
    lang_तथ्य.getNoFallback("Languages", localeId, परिणाम);
    अगर (परिणाम.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        स्थिर अक्षर* canonLocId = canonLocale.getName();
        अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
            lang_तथ्य.getNoFallback("Languages%short", canonLocId, परिणाम);
            अगर (!परिणाम.isBogus()) {
                वापस परिणाम;
            }
        }
        lang_तथ्य.getNoFallback("Languages", canonLocId, परिणाम);
    }
    अगर (परिणाम.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        परिणाम.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    वापस परिणाम;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(स्थिर अक्षर* lang,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    अगर (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        वापस परिणाम = UnicodeString(lang, -1, US_INV);
    }
    अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
        lang_तथ्य.getNoFallback("Languages%short", lang, परिणाम);
        अगर (!परिणाम.isBogus()) {
            वापस adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
        }
    }
    lang_तथ्य.getNoFallback("Languages", lang, परिणाम);
    अगर (परिणाम.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        स्थिर अक्षर* canonLocId = canonLocale.getName();
        अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
            lang_तथ्य.getNoFallback("Languages%short", canonLocId, परिणाम);
            अगर (!परिणाम.isBogus()) {
                वापस adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
            }
        }
        lang_तथ्य.getNoFallback("Languages", canonLocId, परिणाम);
    }
    अगर (परिणाम.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        परिणाम.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    वापस adjustForUsageAndContext(kCapContextUsageLanguage, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(स्थिर अक्षर* script,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND,
                                          bool skipAdjust) स्थिर {
    अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
        lang_तथ्य.getNoFallback("Scripts%short", script, परिणाम);
        अगर (!परिणाम.isBogus()) {
            वापस skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageScript, परिणाम);
        }
    }
    अगर (substitute == UDISPCTX_SUBSTITUTE) {
        lang_तथ्य.लो("Scripts", script, परिणाम);
    } अन्यथा {
        lang_तथ्य.getNoFallback("Scripts", script, परिणाम);
    }
    वापस skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageScript, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(स्थिर अक्षर* script,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस scriptDisplayName(script, परिणाम, false);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस scriptDisplayName(uscript_getName(scriptCode), परिणाम, false);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(स्थिर अक्षर* region,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND,
                                          bool skipAdjust) स्थिर {
    अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
         region_तथ्य.getNoFallback("Countries%short", region, परिणाम);
        अगर (!परिणाम.isBogus()) {
            वापस skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageTerritory, परिणाम);
        }
    }
    अगर (substitute == UDISPCTX_SUBSTITUTE) {
        region_तथ्य.लो("Countries", region, परिणाम);
    } अन्यथा {
        region_तथ्य.getNoFallback("Countries", region, परिणाम);
    }
    वापस skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageTerritory, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(स्थिर अक्षर* region,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस regionDisplayName(region, परिणाम, false);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(स्थिर अक्षर* variant,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND,
                                           bool skipAdjust) स्थिर {
    // don't have a resource for short variant names
    अगर (substitute == UDISPCTX_SUBSTITUTE) {
        lang_तथ्य.लो("Variants", variant, परिणाम);
    } अन्यथा {
        lang_तथ्य.getNoFallback("Variants", variant, परिणाम);
    }
    वापस skipAdjust? परिणाम: adjustForUsageAndContext(kCapContextUsageVariant, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(स्थिर अक्षर* variant,
                                           UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस variantDisplayName(variant, परिणाम, false);
}

UnicodeString&
LocaleDisplayNamesImpl::कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND,
                                       bool skipAdjust) स्थिर {
    // don't have a resource for short key names
    अगर (substitute == UDISPCTX_SUBSTITUTE) {
        lang_तथ्य.लो("Keys", कुंजी, परिणाम);
    } अन्यथा {
        lang_तथ्य.getNoFallback("Keys", कुंजी, परिणाम);
    }
    वापस skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुंजी, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                       UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस कुंजी_Display_Name(कुंजी, परिणाम, false);
}

UnicodeString&
LocaleDisplayNamesImpl::कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी,
                                            स्थिर अक्षर* value,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND,
                                            bool skipAdjust) स्थिर {
    अगर (uprv_strcmp(कुंजी, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_मूल्य(value, -1, US_INV);
        int32_t len;
        स्थिर char16_t *currencyName = ucurr_getName(ustr_मूल्य.get_Terminated_बफर(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        अगर (U_FAILURE(sts)) {
            // Return the value as is on failure
            परिणाम = ustr_मूल्य;
            वापस परिणाम;
        }
        परिणाम.setTo(currencyName, len);
        वापस skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुंजी_मूल्य, परिणाम);
    }

    अगर (name_लंबाई == UDISPCTX_लंबाई_SHORT) {
        lang_तथ्य.getNoFallback("Types%short", कुंजी, value, परिणाम);
        अगर (!परिणाम.isBogus()) {
            वापस skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुंजी_मूल्य, परिणाम);
        }
    }
    अगर (substitute == UDISPCTX_SUBSTITUTE) {
        lang_तथ्य.लो("Types", कुंजी, value, परिणाम);
    } अन्यथा {
        lang_तथ्य.getNoFallback("Types", कुंजी, value, परिणाम);
    }
    वापस skipAdjust? परिणाम: adjustForUsageAndContext(k_Cap_Context_Usage_कुंजी_मूल्य, परिणाम);
}

UnicodeString&
LocaleDisplayNamesImpl::कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी,
                                            स्थिर अक्षर* value,
                                            UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर {
    वापस कुंजी_मूल्य_Display_Name(कुंजी, value, परिणाम, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(स्थिर Locale& locale,
                                   UDialectHandling dialectHandling) {
    वापस नया LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(स्थिर Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    अगर (contexts == nullptr) {
        length = 0;
    }
    वापस नया LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(स्थिर अक्षर * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस nullptr;
  }
  अगर (locale == nullptr) {
    locale = uloc_getDefault();
  }
  वापस (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(स्थिर अक्षर * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस nullptr;
  }
  अगर (locale == nullptr) {
    locale = uloc_getDefault();
  }
  वापस (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI व्योम U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  मिटाओ (LocaleDisplayNames *)ldn;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uldn_getLocale(स्थिर ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  अगर (ldn) {
    वापस ((स्थिर LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  वापस nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(स्थिर ULocaleDisplayNames *ldn) {
  अगर (ldn) {
    वापस ((स्थिर LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  वापस ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(स्थिर ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस (UDisplayContext)0;
  }
  वापस ((स्थिर LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(स्थिर ULocaleDisplayNames *ldn,
                       स्थिर अक्षर *locale,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || locale == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->localeDisplayName(locale, अस्थायी);
  अगर (अस्थायी.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(स्थिर ULocaleDisplayNames *ldn,
                         स्थिर अक्षर *lang,
                         char16_t *परिणाम,
                         int32_t max_परिणाम_आकार,
                         UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || lang == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->languageDisplayName(lang, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(स्थिर ULocaleDisplayNames *ldn,
                       स्थिर अक्षर *script,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || script == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->scriptDisplayName(script, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(स्थिर ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *परिणाम,
                           int32_t max_परिणाम_आकार,
                           UErrorCode *pErrorCode) {
  वापस uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), परिणाम, max_परिणाम_आकार, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(स्थिर ULocaleDisplayNames *ldn,
                       स्थिर अक्षर *region,
                       char16_t *परिणाम,
                       int32_t max_परिणाम_आकार,
                       UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || region == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->regionDisplayName(region, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(स्थिर ULocaleDisplayNames *ldn,
                        स्थिर अक्षर *variant,
                        char16_t *परिणाम,
                        int32_t max_परिणाम_आकार,
                        UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || variant == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->variantDisplayName(variant, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_कुंजी_Display_Name(स्थिर ULocaleDisplayNames *ldn,
                    स्थिर अक्षर *कुंजी,
                    char16_t *परिणाम,
                    int32_t max_परिणाम_आकार,
                    UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || कुंजी == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0) || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->कुंजी_Display_Name(कुंजी, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_कुंजी_मूल्य_Display_Name(स्थिर ULocaleDisplayNames *ldn,
                         स्थिर अक्षर *कुंजी,
                         स्थिर अक्षर *value,
                         char16_t *परिणाम,
                         int32_t max_परिणाम_आकार,
                         UErrorCode *pErrorCode) {
  अगर (U_FAILURE(*pErrorCode)) {
    वापस 0;
  }
  अगर (ldn == nullptr || कुंजी == nullptr || value == nullptr || (परिणाम == nullptr && max_परिणाम_आकार > 0)
      || max_परिणाम_आकार < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  UnicodeString अस्थायी(परिणाम, 0, max_परिणाम_आकार);
  ((स्थिर LocaleDisplayNames *)ldn)->कुंजी_मूल्य_Display_Name(कुंजी, value, अस्थायी);
  वापस अस्थायी.extract(परिणाम, max_परिणाम_आकार, *pErrorCode);
}

#पूर्ण_अगर
