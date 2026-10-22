// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_FORMATTING

#অন্তর্ভাৱ "unicode/locdspnm.h"
#অন্তর্ভাৱ "unicode/simpleformatter.h"
#অন্তর্ভাৱ "unicode/ucasemap.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "unicode/udisplaycontext.h"
#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/ucurr.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ulocimp.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "ureslocs.h"
#অন্তর্ভাৱ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
শ্ৰেণী ICUDataTable {
    ধ্রুৱ ৱর্ণ* ধ্রুৱ path;
    Locale locale;

সার্ৱজনিক:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ধ্রুৱ ৱর্ণ* path, ধ্রুৱ Locale& locale);
    ~ICUDataTable() = স্ৱভাৱ;

    ধ্রুৱ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                       UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
    UnicodeString& get(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* sub_Table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                       UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;

    UnicodeString& getNoFallback(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                                 UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
    UnicodeString& getNoFallback(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* sub_Table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                                 UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
};

অন্তঃস্থ UnicodeString& ICUDataTable::get(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                                        UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই get(table_চাবি, nullptr, item_চাবি, result);
}

অন্তঃস্থ UnicodeString& ICUDataTable::getNoFallback(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                                                  UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই getNoFallback(table_চাবি, nullptr, item_চাবি, result);
}

ICUDataTable::ICUDataTable(ধ্রুৱ ৱর্ণ* path, ধ্রুৱ Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ধ্রুৱ Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ঘূৰাই locale;
}

UnicodeString&
ICUDataTable::get(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* sub_Table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                  UnicodeString &result U_LIFETIME_BOUND) ধ্রুৱ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ধ্রুৱ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_চাবি, sub_Table_চাবি, item_চাবি,
                                                   &len, &status);
  যদি (U_SUCCESS(status) && len > 0) {
    ঘূৰাই result.setTo(s, len);
  }
  ঘূৰাই result.setTo(UnicodeString(item_চাবি, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ধ্রুৱ ৱর্ণ* table_চাবি, ধ্রুৱ ৱর্ণ* sub_Table_চাবি, ধ্রুৱ ৱর্ণ* item_চাবি,
                            UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ধ্রুৱ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_চাবি, sub_Table_চাবি, item_চাবি,
                                                   &len, &status);
  যদি (U_SUCCESS(status)) {
    ঘূৰাই result.setTo(s, len);
  }

  result.setToBogus();
  ঘূৰাই result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#যদি 0  // currently unused

শ্ৰেণী DefaultLocaleDisplayNames : সার্ৱজনিক LocaleDisplayNames {
  UDialectHandling dialectHandling;

সার্ৱজনিক:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  আভাস ~DefaultLocaleDisplayNames();

  আভাস ধ্রুৱ Locale& getLocale() ধ্রুৱ U_LIFETIME_BOUND;
  আভাস UDialectHandling getDialectHandling() ধ্রুৱ;

  আভাস UnicodeString& localeDisplayName(ধ্রুৱ Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& localeDisplayName(ধ্রুৱ ৱর্ণ* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& languageDisplayName(ধ্রুৱ ৱর্ণ* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& scriptDisplayName(ধ্রুৱ ৱর্ণ* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& regionDisplayName(ধ্রুৱ ৱর্ণ* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& variantDisplayName(ধ্রুৱ ৱর্ণ* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                        UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
  আভাস UnicodeString& চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                             ধ্রুৱ ৱর্ণ* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ধ্রুৱ Locale&
DefaultLocaleDisplayNames::getLocale() ধ্রুৱ U_LIFETIME_BOUND {
  ঘূৰাই Locale::get_মূল();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ধ্রুৱ {
  ঘূৰাই dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ধ্রুৱ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ধ্রুৱ ৱর্ণ* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ধ্রুৱ ৱর্ণ* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ধ্রুৱ ৱর্ণ* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ধ্রুৱ ৱর্ণ* name = uscript_getName(scriptCode);
  যদি (name) {
    ঘূৰাই result = UnicodeString(name, -1, US_INV);
  }
  ঘূৰাই result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ধ্রুৱ ৱর্ণ* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ধ্রুৱ ৱর্ণ* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(চাবি, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* /*চাবি*/,
                                               ধ্রুৱ ৱর্ণ* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  ঘূৰাই result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

শ্ৰেণী LocaleDisplayNamesImpl : সার্ৱজনিক LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter চাবি_Type_Format;
    UDisplayContext capitalizationContext;
#যদি !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#অন্যথা
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    গণনা CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_চাবি,
        k_Cap_Context_Usage_চাবি_মান,
        k_Cap_Context_Usage_গুনতি
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     তার্কিক fCapitalization[k_Cap_Context_Usage_গুনতি];

সার্ৱজনিক:
    // constructor
    LocaleDisplayNamesImpl(ধ্রুৱ Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ধ্রুৱ Locale& locale, UDisplayContext *contexts, int32_t length);
    আভাস ~LocaleDisplayNamesImpl();

    আভাস ধ্রুৱ Locale& getLocale() ধ্রুৱ U_LIFETIME_BOUND override;
    আভাস UDialectHandling getDialectHandling() ধ্রুৱ override;
    আভাস UDisplayContext getContext(UDisplayContextType type) ধ্রুৱ override;

    আভাস UnicodeString& localeDisplayName(ধ্রুৱ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString& localeDisplayName(ধ্রুৱ ৱর্ণ* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString&
    languageDisplayName(ধ্রুৱ ৱর্ণ* lang, UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString& scriptDisplayName(ধ্রুৱ ৱর্ণ* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString& regionDisplayName(ধ্রুৱ ৱর্ণ* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString&
    variantDisplayName(ধ্রুৱ ৱর্ণ* variant, UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString& চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;
    আভাস UnicodeString&
    চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* চাবি, ধ্রুৱ ৱর্ণ* value,
                        UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ override;

গুপ্ত:
    UnicodeString& localeIdName(ধ্রুৱ ৱর্ণ* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                তার্কিক substitute) ধ্রুৱ;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ধ্রুৱ UnicodeString& src) ধ্রুৱ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ;
    UnicodeString& scriptDisplayName(ধ্রুৱ ৱর্ণ* script, UnicodeString& result U_LIFETIME_BOUND,
                                     তার্কিক skipAdjust) ধ্রুৱ;
    UnicodeString& regionDisplayName(ধ্রুৱ ৱর্ণ* region, UnicodeString& result U_LIFETIME_BOUND,
                                     তার্কিক skipAdjust) ধ্রুৱ;
    UnicodeString& variantDisplayName(ধ্রুৱ ৱর্ণ* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      তার্কিক skipAdjust) ধ্রুৱ;
    UnicodeString& চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি, UnicodeString& result U_LIFETIME_BOUND,
                                  তার্কিক skipAdjust) ধ্রুৱ;
    UnicodeString& চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* চাবি, ধ্রুৱ ৱর্ণ* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       তার্কিক skipAdjust) ধ্রুৱ;
    শূন্য initialize();

    সংরচনা CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ধ্রুৱ Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ধ্রুৱ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    যেতিয়ালৈ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ৱিকল্প (selector) {
            পক্ষ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ভঙ্গ;
            পক্ষ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ভঙ্গ;
            পক্ষ UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                ভঙ্গ;
            পক্ষ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ভঙ্গ;
            স্ৱভাৱ:
                ভঙ্গ;
        }
    }
    initialize();
}

সংরচনা LocaleDisplayNamesImpl::CapitalizationContextSink : সার্ৱজনিক ResourceSink {
    তার্কিক hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(মিছা), parent(_parent) {}
    আভাস ~CapitalizationContextSink();

    আভাস শূন্য put(ধ্রুৱ ৱর্ণ *চাবি, Resource_মান &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        বাবে (পূর্ণাঙ্ক i = 0; contexts.get_চাবি_And_মান(i, চাবি, value); ++i) {

            CapContextUsage usageEnum;
            যদি (uprv_strcmp(চাবি, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_চাবি;
            } অন্যথা যদি (uprv_strcmp(চাবি, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_চাবি_মান;
            } অন্যথা যদি (uprv_strcmp(চাবি, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } অন্যথা যদি (uprv_strcmp(চাবি, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } অন্যথা যদি (uprv_strcmp(চাবি, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } অন্যথা যদি (uprv_strcmp(চাবি, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } অন্যথা {
                অনুৱর্ত;
            }

            int32_t len = 0;
            ধ্রুৱ int32_t* intVector = value.getIntVector(len, errorCode);
            যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
            যদি (len < 2) { অনুৱর্ত; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            যদি (titlecaseInt == 0) { অনুৱর্ত; }

            parent.fCapitalization[usageEnum] = সঁচা;
            hasCapitalizationUsage = সঁচা;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

শূন্য
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = এতত্;
    nonConstThis->locale = langData.getLocale() == Locale::get_মূল()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    যদি (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    যদি (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    যদি (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } অন্যথা {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    যদি (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    চাবি_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, পরিমাণ(fCapitalization));
#যদি !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    তার্কিক needBrkIter = মিছা;
    যদি (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        যদি (U_FAILURE(status)) { ঘূৰাই; }
        CapitalizationContextSink sink(*এতত্);
        ures_get_All_বস্তুসমূহ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        যদি (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } অন্যথা যদি (U_FAILURE(status)) {
            ঘূৰাই;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    যদি (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        যদি (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#যদি !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ধ্রুৱ Locale&
LocaleDisplayNamesImpl::getLocale() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ধ্রুৱ {
    ঘূৰাই dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ধ্রুৱ {
    ৱিকল্প (type) {
        পক্ষ UDISPCTX_TYPE_DIALECT_HANDLING:
            ঘূৰাই static_cast<UDisplayContext>(dialectHandling);
        পক্ষ UDISPCTX_TYPE_CAPITALIZATION:
            ঘূৰাই capitalizationContext;
        পক্ষ UDISPCTX_TYPE_DISPLAY_LENGTH:
            ঘূৰাই nameLength;
        পক্ষ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ঘূৰাই substitute;
        স্ৱভাৱ:
            ভঙ্গ;
    }
    ঘূৰাই static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
#যদি !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    যদি ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        স্থির UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ঘূৰাই result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ধ্রুৱ Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
  যদি (loc.isBogus()) {
    result.setToBogus();
    ঘূৰাই result;
  }
  UnicodeString resultName;

  ধ্রুৱ ৱর্ণ* lang = loc.getLanguage();
  যদি (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ধ্রুৱ ৱর্ণ* script = loc.getScript();
  ধ্রুৱ ৱর্ণ* country = loc.getCountry();
  ধ্রুৱ ৱর্ণ* variant = loc.getVariant();

  তার্কিক hasScript = uprv_strlen(script) > 0;
  তার্কিক hasCountry = uprv_strlen(country) > 0;
  তার্কিক hasVariant = uprv_strlen(variant) > 0;

  যদি (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    কৰা { // loop construct is so we can break early out of search
      যদি (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, মিছা);
          যদি (!resultName.isBogus()) {
            hasScript = মিছা;
            hasCountry = মিছা;
            ভঙ্গ;
          }
        }
      }
      যদি (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, মিছা);
          যদি (!resultName.isBogus()) {
            hasScript = মিছা;
            ভঙ্গ;
          }
        }
      }
      যদি (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, মিছা);
          যদি (!resultName.isBogus()) {
            hasCountry = মিছা;
            ভঙ্গ;
          }
        }
      }
    } যেতিয়ালৈ (মিছা);
  }
  যদি (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    যদি (resultName.isBogus()) {
      result.setToBogus();
      ঘূৰাই result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  যদি (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, সঁচা);
    যদি (script_str.isBogus()) {
      result.setToBogus();
      ঘূৰাই result;
    }
    resultRemainder.append(script_str);
  }
  যদি (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, সঁচা);
    যদি (region_str.isBogus()) {
      result.setToBogus();
      ঘূৰাই result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  যদি (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, সঁচা);
    যদি (variant_str.isBogus()) {
      result.setToBogus();
      ঘূৰাই result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  যদি (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ধ্রুৱ ৱর্ণ* চাবি;
    যেতিয়ালৈ ((চাবি = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_মান<CharString>(চাবি, status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই result;
      }
      চাবি_Display_Name(চাবি, temp, সঁচা);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      চাবি_মান_Display_Name(চাবি, value.data(), temp2, সঁচা);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      যদি (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } অন্যথা যদি (temp != UnicodeString(চাবি, -1, US_INV)) {
        UnicodeString temp3;
        চাবি_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } অন্যথা {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  যদি (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    ঘূৰাই adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  ঘূৰাই adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ধ্রুৱ UnicodeString& src) ধ্রুৱ {
    যদি (buffer.isEmpty()) {
        buffer.setTo(src);
    } অন্যথা {
        ধ্রুৱ UnicodeString *মানসমূহ[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(মানসমূহ, 2, buffer, nullptr, 0, status);
    }
    ঘূৰাই buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ধ্রুৱ ৱর্ণ* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ধ্রুৱ ৱর্ণ* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, তার্কিক substitute) ধ্রুৱ {
    যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        যদি (!result.isBogus()) {
            ঘূৰাই result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    যদি (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ধ্রুৱ ৱর্ণ* canonLocId = canonLocale.getName();
        যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            যদি (!result.isBogus()) {
                ঘূৰাই result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    যদি (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ঘূৰাই result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ধ্রুৱ ৱর্ণ* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    যদি (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ঘূৰাই result = UnicodeString(lang, -1, US_INV);
    }
    যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        যদি (!result.isBogus()) {
            ঘূৰাই adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    যদি (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ধ্রুৱ ৱর্ণ* canonLocId = canonLocale.getName();
        যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            যদি (!result.isBogus()) {
                ঘূৰাই adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    যদি (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ঘূৰাই adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ধ্রুৱ ৱর্ণ* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          তার্কিক skipAdjust) ধ্রুৱ {
    যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        যদি (!result.isBogus()) {
            ঘূৰাই skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } অন্যথা {
        langData.getNoFallback("Scripts", script, result);
    }
    ঘূৰাই skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ধ্রুৱ ৱর্ণ* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই scriptDisplayName(script, result, মিছা);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই scriptDisplayName(uscript_getName(scriptCode), result, মিছা);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ধ্রুৱ ৱর্ণ* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          তার্কিক skipAdjust) ধ্রুৱ {
    যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        যদি (!result.isBogus()) {
            ঘূৰাই skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } অন্যথা {
        regionData.getNoFallback("Countries", region, result);
    }
    ঘূৰাই skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ধ্রুৱ ৱর্ণ* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই regionDisplayName(region, result, মিছা);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ধ্রুৱ ৱর্ণ* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           তার্কিক skipAdjust) ধ্রুৱ {
    // don't have a resource for short variant names
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } অন্যথা {
        langData.getNoFallback("Variants", variant, result);
    }
    ঘূৰাই skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ধ্রুৱ ৱর্ণ* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই variantDisplayName(variant, result, মিছা);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       তার্কিক skipAdjust) ধ্রুৱ {
    // don't have a resource for short key names
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", চাবি, result);
    } অন্যথা {
        langData.getNoFallback("Keys", চাবি, result);
    }
    ঘূৰাই skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি, result);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                       UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই চাবি_Display_Name(চাবি, result, মিছা);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                            ধ্রুৱ ৱর্ণ* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            তার্কিক skipAdjust) ধ্রুৱ {
    যদি (uprv_strcmp(চাবি, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_মান(value, -1, US_INV);
        int32_t len;
        ধ্রুৱ char16_t *currencyName = ucurr_getName(ustr_মান.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        যদি (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_মান;
            ঘূৰাই result;
        }
        result.setTo(currencyName, len);
        ঘূৰাই skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, result);
    }

    যদি (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", চাবি, value, result);
        যদি (!result.isBogus()) {
            ঘূৰাই skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, result);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", চাবি, value, result);
    } অন্যথা {
        langData.getNoFallback("Types", চাবি, value, result);
    }
    ঘূৰাই skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, result);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_মান_Display_Name(ধ্রুৱ ৱর্ণ* চাবি,
                                            ধ্রুৱ ৱর্ণ* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ধ্রুৱ {
    ঘূৰাই চাবি_মান_Display_Name(চাবি, value, result, মিছা);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ধ্রুৱ Locale& locale,
                                   UDialectHandling dialectHandling) {
    ঘূৰাই নূতন LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ধ্রুৱ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    যদি (contexts == nullptr) {
        length = 0;
    }
    ঘূৰাই নূতন LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ধ্রুৱ ৱর্ণ * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই nullptr;
  }
  যদি (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ঘূৰাই (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ধ্রুৱ ৱর্ণ * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই nullptr;
  }
  যদি (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ঘূৰাই (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI শূন্য U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uldn_getLocale(ধ্রুৱ ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  যদি (ldn) {
    ঘূৰাই ((ধ্রুৱ LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ঘূৰাই nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ধ্রুৱ ULocaleDisplayNames *ldn) {
  যদি (ldn) {
    ঘূৰাই ((ধ্রুৱ LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ঘূৰাই ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ধ্রুৱ ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই (UDisplayContext)0;
  }
  ঘূৰাই ((ধ্রুৱ LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                       ধ্রুৱ ৱর্ণ *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  যদি (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                         ধ্রুৱ ৱর্ণ *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                       ধ্রুৱ ৱর্ণ *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  ঘূৰাই uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                       ধ্রুৱ ৱর্ণ *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ধ্রুৱ ULocaleDisplayNames *ldn,
                        ধ্রুৱ ৱর্ণ *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_চাবি_Display_Name(ধ্রুৱ ULocaleDisplayNames *ldn,
                    ধ্রুৱ ৱর্ণ *চাবি,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || চাবি == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->চাবি_Display_Name(চাবি, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_চাবি_মান_Display_Name(ধ্রুৱ ULocaleDisplayNames *ldn,
                         ধ্রুৱ ৱর্ণ *চাবি,
                         ধ্রুৱ ৱর্ণ *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ঘূৰাই 0;
  }
  যদি (ldn == nullptr || চাবি == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ঘূৰাই 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ধ্রুৱ LocaleDisplayNames *)ldn)->চাবি_মান_Display_Name(চাবি, value, temp);
  ঘূৰাই temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
