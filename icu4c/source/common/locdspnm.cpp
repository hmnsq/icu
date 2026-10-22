// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_FORMATTING

#অন্তর্ভাব "unicode/locdspnm.h"
#অন্তর্ভাব "unicode/simpleformatter.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/udisplaycontext.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/ucurr.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "ureslocs.h"
#অন্তর্ভাব "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
শ্রেণী ICUDataTable {
    ধ্রুবক অক্ষর* ধ্রুবক path;
    Locale locale;

সর্বজনীন:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ধ্রুবক অক্ষর* path, ধ্রুবক Locale& locale);
    ~ICUDataTable() = পূর্বনির্ধারিত;

    ধ্রুবক Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                       UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
    UnicodeString& get(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* sub_Table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                       UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;

    UnicodeString& getNoFallback(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                                 UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
    UnicodeString& getNoFallback(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* sub_Table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                                 UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
};

অন্তঃরেখ UnicodeString& ICUDataTable::get(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                                        UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত get(table_চাবি, nullptr, item_চাবি, ফলাফল);
}

অন্তঃরেখ UnicodeString& ICUDataTable::getNoFallback(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                                                  UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত getNoFallback(table_চাবি, nullptr, item_চাবি, ফলাফল);
}

ICUDataTable::ICUDataTable(ধ্রুবক অক্ষর* path, ধ্রুবক Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ধ্রুবক Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ফেরত locale;
}

UnicodeString&
ICUDataTable::get(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* sub_Table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                  UnicodeString &ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ধ্রুবক char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_চাবি, sub_Table_চাবি, item_চাবি,
                                                   &len, &status);
  যদি (U_SUCCESS(status) && len > 0) {
    ফেরত ফলাফল.setTo(s, len);
  }
  ফেরত ফলাফল.setTo(UnicodeString(item_চাবি, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ধ্রুবক অক্ষর* table_চাবি, ধ্রুবক অক্ষর* sub_Table_চাবি, ধ্রুবক অক্ষর* item_চাবি,
                            UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ধ্রুবক char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_চাবি, sub_Table_চাবি, item_চাবি,
                                                   &len, &status);
  যদি (U_SUCCESS(status)) {
    ফেরত ফলাফল.setTo(s, len);
  }

  ফলাফল.setToBogus();
  ফেরত ফলাফল;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#যদি 0  // currently unused

শ্রেণী DefaultLocaleDisplayNames : সর্বজনীন LocaleDisplayNames {
  UDialectHandling dialectHandling;

সর্বজনীন:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  আভাসী ~DefaultLocaleDisplayNames();

  আভাসী ধ্রুবক Locale& getLocale() ধ্রুবক U_LIFETIME_BOUND;
  আভাসী UDialectHandling getDialectHandling() ধ্রুবক;

  আভাসী UnicodeString& localeDisplayName(ধ্রুবক Locale& locale,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& localeDisplayName(ধ্রুবক অক্ষর* localeId,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& languageDisplayName(ধ্রুবক অক্ষর* lang,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& scriptDisplayName(ধ্রুবক অক্ষর* script,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& regionDisplayName(ধ্রুবক অক্ষর* region,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& variantDisplayName(ধ্রুবক অক্ষর* variant,
                                            UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                        UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
  আভাসী UnicodeString& চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                             ধ্রুবক অক্ষর* value,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ধ্রুবক Locale&
DefaultLocaleDisplayNames::getLocale() ধ্রুবক U_LIFETIME_BOUND {
  ফেরত Locale::get_মূল();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ধ্রুবক {
  ফেরত dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ধ্রুবক Locale& locale,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ধ্রুবক অক্ষর* localeId,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ধ্রুবক অক্ষর* lang,
                                               UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ধ্রুবক অক্ষর* script,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ধ্রুবক অক্ষর* name = uscript_getName(scriptCode);
  যদি (name) {
    ফেরত ফলাফল = UnicodeString(name, -1, US_INV);
  }
  ফেরত ফলাফল.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ধ্রুবক অক্ষর* region,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ধ্রুবক অক্ষর* variant,
                                              UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(চাবি, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::চাবি_মান_Display_Name(ধ্রুবক অক্ষর* /*চাবি*/,
                                               ধ্রুবক অক্ষর* value,
                                               UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  ফেরত ফলাফল = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

শ্রেণী LocaleDisplayNamesImpl : সর্বজনীন LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable lang_তথ্য;
    ICUDataTable region_তথ্য;
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
    UDisplayContext name_দৈর্ঘ্য;
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
     যৌক্তিক fCapitalization[k_Cap_Context_Usage_গুনতি];

সর্বজনীন:
    // constructor
    LocaleDisplayNamesImpl(ধ্রুবক Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ধ্রুবক Locale& locale, UDisplayContext *contexts, int32_t length);
    আভাসী ~LocaleDisplayNamesImpl();

    আভাসী ধ্রুবক Locale& getLocale() ধ্রুবক U_LIFETIME_BOUND override;
    আভাসী UDialectHandling getDialectHandling() ধ্রুবক override;
    আভাসী UDisplayContext getContext(UDisplayContextType type) ধ্রুবক override;

    আভাসী UnicodeString& localeDisplayName(ধ্রুবক Locale& locale,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString& localeDisplayName(ধ্রুবক অক্ষর* localeId,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString&
    languageDisplayName(ধ্রুবক অক্ষর* lang, UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString& scriptDisplayName(ধ্রুবক অক্ষর* script,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString& regionDisplayName(ধ্রুবক অক্ষর* region,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString&
    variantDisplayName(ধ্রুবক অক্ষর* variant, UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString& চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;
    আভাসী UnicodeString&
    চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি, ধ্রুবক অক্ষর* value,
                        UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক override;

ব্যক্তিগত:
    UnicodeString& localeIdName(ধ্রুবক অক্ষর* localeId, UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                যৌক্তিক substitute) ধ্রুবক;
    UnicodeString& appendWithSep(UnicodeString& বাফার U_LIFETIME_BOUND,
                                 ধ্রুবক UnicodeString& src) ধ্রুবক;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক;
    UnicodeString& scriptDisplayName(ধ্রুবক অক্ষর* script, UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                     যৌক্তিক skipAdjust) ধ্রুবক;
    UnicodeString& regionDisplayName(ধ্রুবক অক্ষর* region, UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                     যৌক্তিক skipAdjust) ধ্রুবক;
    UnicodeString& variantDisplayName(ধ্রুবক অক্ষর* variant, UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                      যৌক্তিক skipAdjust) ধ্রুবক;
    UnicodeString& চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি, UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                  যৌক্তিক skipAdjust) ধ্রুবক;
    UnicodeString& চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি, ধ্রুবক অক্ষর* value,
                                       UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                       যৌক্তিক skipAdjust) ধ্রুবক;
    শূন্য initialize();

    কাঠামো CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ধ্রুবক Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_তথ্য(U_ICUDATA_LANG, locale)
    , region_তথ্য(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , name_দৈর্ঘ্য(UDISPCTX_দৈর্ঘ্য_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ধ্রুবক Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , lang_তথ্য(U_ICUDATA_LANG, locale)
    , region_তথ্য(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , name_দৈর্ঘ্য(UDISPCTX_দৈর্ঘ্য_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    যখন (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        নির্বাচন (selector) {
            ক্ষেত্র UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                বিরতি;
            ক্ষেত্র UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                বিরতি;
            ক্ষেত্র UDISPCTX_TYPE_DISPLAY_দৈর্ঘ্য:
                name_দৈর্ঘ্য = value;
                বিরতি;
            ক্ষেত্র UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                বিরতি;
            পূর্বনির্ধারিত:
                বিরতি;
        }
    }
    initialize();
}

কাঠামো LocaleDisplayNamesImpl::CapitalizationContextSink : সর্বজনীন ResourceSink {
    যৌক্তিক hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(মিথ্যা), parent(_parent) {}
    আভাসী ~CapitalizationContextSink();

    আভাসী শূন্য put(ধ্রুবক অক্ষর *চাবি, Resource_মান &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        জন্য (পূর্ণসংখ্যা i = 0; contexts.get_চাবি_And_মান(i, চাবি, value); ++i) {

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
                চালু;
            }

            int32_t len = 0;
            ধ্রুবক int32_t* intVector = value.getIntVector(len, errorCode);
            যদি (U_FAILURE(errorCode)) { ফেরত; }
            যদি (len < 2) { চালু; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            যদি (titlecaseInt == 0) { চালু; }

            parent.fCapitalization[usageEnum] = সত্য;
            hasCapitalizationUsage = সত্য;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

শূন্য
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = এই;
    nonConstThis->locale = lang_তথ্য.getLocale() == Locale::get_মূল()
        ? region_তথ্য.getLocale()
        : lang_তথ্য.getLocale();

    UnicodeString sep;
    lang_তথ্য.getNoFallback("localeDisplayPattern", "separator", sep);
    যদি (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    lang_তথ্য.getNoFallback("localeDisplayPattern", "pattern", pattern);
    যদি (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    যদি (pattern.সূচক_Of(static_cast<char16_t>(0xFF08)) >= 0) {
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
    lang_তথ্য.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    যদি (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    চাবি_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, আকার(fCapitalization));
#যদি !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    যৌক্তিক needBrkIter = মিথ্যা;
    যদি (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        যদি (U_FAILURE(status)) { ফেরত; }
        CapitalizationContextSink sink(*এই);
        ures_get_All_বস্তুসমূহ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        যদি (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } অন্যথা যদি (U_FAILURE(status)) {
            ফেরত;
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

ধ্রুবক Locale&
LocaleDisplayNamesImpl::getLocale() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ধ্রুবক {
    ফেরত dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ধ্রুবক {
    নির্বাচন (type) {
        ক্ষেত্র UDISPCTX_TYPE_DIALECT_HANDLING:
            ফেরত static_cast<UDisplayContext>(dialectHandling);
        ক্ষেত্র UDISPCTX_TYPE_CAPITALIZATION:
            ফেরত capitalizationContext;
        ক্ষেত্র UDISPCTX_TYPE_DISPLAY_দৈর্ঘ্য:
            ফেরত name_দৈর্ঘ্য;
        ক্ষেত্র UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ফেরত substitute;
        পূর্বনির্ধারিত:
            বিরতি;
    }
    ফেরত static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
#যদি !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    যদি ( ফলাফল.length() > 0 && u_islower(ফলাফল.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        স্থির UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        ফলাফল.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ফেরত ফলাফল;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ধ্রুবক Locale& loc,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
  যদি (loc.isBogus()) {
    ফলাফল.setToBogus();
    ফেরত ফলাফল;
  }
  UnicodeString ফলাফল_Name;

  ধ্রুবক অক্ষর* lang = loc.getLanguage();
  যদি (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ধ্রুবক অক্ষর* script = loc.getScript();
  ধ্রুবক অক্ষর* country = loc.getCountry();
  ধ্রুবক অক্ষর* variant = loc.getVariant();

  যৌক্তিক hasScript = uprv_strlen(script) > 0;
  যৌক্তিক hasCountry = uprv_strlen(country) > 0;
  যৌক্তিক hasVariant = uprv_strlen(variant) > 0;

  যদি (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString বাফার;
    করো { // loop construct is so we can break early out of search
      যদি (hasScript && hasCountry) {
        বাফার.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(বাফার.তথ্য(), ফলাফল_Name, মিথ্যা);
          যদি (!ফলাফল_Name.isBogus()) {
            hasScript = মিথ্যা;
            hasCountry = মিথ্যা;
            বিরতি;
          }
        }
      }
      যদি (hasScript) {
        বাফার.append(lang, status)
              .append('_', status)
              .append(script, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(বাফার.তথ্য(), ফলাফল_Name, মিথ্যা);
          যদি (!ফলাফল_Name.isBogus()) {
            hasScript = মিথ্যা;
            বিরতি;
          }
        }
      }
      যদি (hasCountry) {
        বাফার.append(lang, status)
              .append('_', status)
              .append(country, status);
        যদি (U_SUCCESS(status)) {
          localeIdName(বাফার.তথ্য(), ফলাফল_Name, মিথ্যা);
          যদি (!ফলাফল_Name.isBogus()) {
            hasCountry = মিথ্যা;
            বিরতি;
          }
        }
      }
    } যখন (মিথ্যা);
  }
  যদি (ফলাফল_Name.isBogus() || ফলাফল_Name.isEmpty()) {
    localeIdName(lang, ফলাফল_Name, substitute == UDISPCTX_SUBSTITUTE);
    যদি (ফলাফল_Name.isBogus()) {
      ফলাফল.setToBogus();
      ফেরত ফলাফল;
    }
  }

  UnicodeString ফলাফল_Remainder;
  UnicodeString অস্থায়ী;
  UErrorCode status = U_ZERO_ERROR;

  যদি (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, অস্থায়ী, সত্য);
    যদি (script_str.isBogus()) {
      ফলাফল.setToBogus();
      ফেরত ফলাফল;
    }
    ফলাফল_Remainder.append(script_str);
  }
  যদি (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, অস্থায়ী, সত্য);
    যদি (region_str.isBogus()) {
      ফলাফল.setToBogus();
      ফেরত ফলাফল;
    }
    appendWithSep(ফলাফল_Remainder, region_str);
  }
  যদি (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, অস্থায়ী, সত্য);
    যদি (variant_str.isBogus()) {
      ফলাফল.setToBogus();
      ফেরত ফলাফল;
    }
    appendWithSep(ফলাফল_Remainder, variant_str);
  }
  ফলাফল_Remainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  ফলাফল_Remainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  যদি (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ধ্রুবক অক্ষর* চাবি;
    যখন ((চাবি = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_মান<CharString>(চাবি, status);
        যদি (U_FAILURE(status)) {
            ফেরত ফলাফল;
      }
      চাবি_Display_Name(চাবি, অস্থায়ী, সত্য);
      অস্থায়ী.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      অস্থায়ী.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      চাবি_মান_Display_Name(চাবি, value.তথ্য(), temp2, সত্য);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      যদি (temp2 != UnicodeString(value.তথ্য(), -1, US_INV)) {
        appendWithSep(ফলাফল_Remainder, temp2);
      } অন্যথা যদি (অস্থায়ী != UnicodeString(চাবি, -1, US_INV)) {
        UnicodeString temp3;
        চাবি_Type_Format.format(অস্থায়ী, temp2, temp3, status);
        appendWithSep(ফলাফল_Remainder, temp3);
      } অন্যথা {
        appendWithSep(ফলাফল_Remainder, অস্থায়ী)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  যদি (!ফলাফল_Remainder.isEmpty()) {
    format.format(ফলাফল_Name, ফলাফল_Remainder, ফলাফল.remove(), status);
    ফেরত adjustForUsageAndContext(kCapContextUsageLanguage, ফলাফল);
  }

  ফলাফল = ফলাফল_Name;
  ফেরত adjustForUsageAndContext(kCapContextUsageLanguage, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& বাফার U_LIFETIME_BOUND, ধ্রুবক UnicodeString& src) ধ্রুবক {
    যদি (বাফার.isEmpty()) {
        বাফার.setTo(src);
    } অন্যথা {
        ধ্রুবক UnicodeString *মানসমূহ[2] = { &বাফার, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(মানসমূহ, 2, বাফার, nullptr, 0, status);
    }
    ফেরত বাফার;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ধ্রুবক অক্ষর* localeId,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত localeDisplayName(Locale(localeId), ফলাফল);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ধ্রুবক অক্ষর* localeId,
                                     UnicodeString& ফলাফল U_LIFETIME_BOUND, যৌক্তিক substitute) ধ্রুবক {
    যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
        lang_তথ্য.getNoFallback("Languages%short", localeId, ফলাফল);
        যদি (!ফলাফল.isBogus()) {
            ফেরত ফলাফল;
        }
    }
    lang_তথ্য.getNoFallback("Languages", localeId, ফলাফল);
    যদি (ফলাফল.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ধ্রুবক অক্ষর* canonLocId = canonLocale.getName();
        যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
            lang_তথ্য.getNoFallback("Languages%short", canonLocId, ফলাফল);
            যদি (!ফলাফল.isBogus()) {
                ফেরত ফলাফল;
            }
        }
        lang_তথ্য.getNoFallback("Languages", canonLocId, ফলাফল);
    }
    যদি (ফলাফল.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        ফলাফল.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ফেরত ফলাফল;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ধ্রুবক অক্ষর* lang,
                                            UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    যদি (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ফেরত ফলাফল = UnicodeString(lang, -1, US_INV);
    }
    যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
        lang_তথ্য.getNoFallback("Languages%short", lang, ফলাফল);
        যদি (!ফলাফল.isBogus()) {
            ফেরত adjustForUsageAndContext(kCapContextUsageLanguage, ফলাফল);
        }
    }
    lang_তথ্য.getNoFallback("Languages", lang, ফলাফল);
    যদি (ফলাফল.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ধ্রুবক অক্ষর* canonLocId = canonLocale.getName();
        যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
            lang_তথ্য.getNoFallback("Languages%short", canonLocId, ফলাফল);
            যদি (!ফলাফল.isBogus()) {
                ফেরত adjustForUsageAndContext(kCapContextUsageLanguage, ফলাফল);
            }
        }
        lang_তথ্য.getNoFallback("Languages", canonLocId, ফলাফল);
    }
    যদি (ফলাফল.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        ফলাফল.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ফেরত adjustForUsageAndContext(kCapContextUsageLanguage, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ধ্রুবক অক্ষর* script,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                          যৌক্তিক skipAdjust) ধ্রুবক {
    যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
        lang_তথ্য.getNoFallback("Scripts%short", script, ফলাফল);
        যদি (!ফলাফল.isBogus()) {
            ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(kCapContextUsageScript, ফলাফল);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        lang_তথ্য.get("Scripts", script, ফলাফল);
    } অন্যথা {
        lang_তথ্য.getNoFallback("Scripts", script, ফলাফল);
    }
    ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(kCapContextUsageScript, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ধ্রুবক অক্ষর* script,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত scriptDisplayName(script, ফলাফল, মিথ্যা);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত scriptDisplayName(uscript_getName(scriptCode), ফলাফল, মিথ্যা);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ধ্রুবক অক্ষর* region,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                          যৌক্তিক skipAdjust) ধ্রুবক {
    যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
         region_তথ্য.getNoFallback("Countries%short", region, ফলাফল);
        যদি (!ফলাফল.isBogus()) {
            ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(kCapContextUsageTerritory, ফলাফল);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        region_তথ্য.get("Countries", region, ফলাফল);
    } অন্যথা {
        region_তথ্য.getNoFallback("Countries", region, ফলাফল);
    }
    ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(kCapContextUsageTerritory, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ধ্রুবক অক্ষর* region,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত regionDisplayName(region, ফলাফল, মিথ্যা);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ধ্রুবক অক্ষর* variant,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                           যৌক্তিক skipAdjust) ধ্রুবক {
    // don't have a resource for short variant names
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        lang_তথ্য.get("Variants", variant, ফলাফল);
    } অন্যথা {
        lang_তথ্য.getNoFallback("Variants", variant, ফলাফল);
    }
    ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(kCapContextUsageVariant, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ধ্রুবক অক্ষর* variant,
                                           UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত variantDisplayName(variant, ফলাফল, মিথ্যা);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                       UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                       যৌক্তিক skipAdjust) ধ্রুবক {
    // don't have a resource for short key names
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        lang_তথ্য.get("Keys", চাবি, ফলাফল);
    } অন্যথা {
        lang_তথ্য.getNoFallback("Keys", চাবি, ফলাফল);
    }
    ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                       UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত চাবি_Display_Name(চাবি, ফলাফল, মিথ্যা);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                            ধ্রুবক অক্ষর* value,
                                            UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                            যৌক্তিক skipAdjust) ধ্রুবক {
    যদি (uprv_strcmp(চাবি, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_মান(value, -1, US_INV);
        int32_t len;
        ধ্রুবক char16_t *currencyName = ucurr_getName(ustr_মান.get_Terminated_বাফার(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        যদি (U_FAILURE(sts)) {
            // Return the value as is on failure
            ফলাফল = ustr_মান;
            ফেরত ফলাফল;
        }
        ফলাফল.setTo(currencyName, len);
        ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, ফলাফল);
    }

    যদি (name_দৈর্ঘ্য == UDISPCTX_দৈর্ঘ্য_SHORT) {
        lang_তথ্য.getNoFallback("Types%short", চাবি, value, ফলাফল);
        যদি (!ফলাফল.isBogus()) {
            ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, ফলাফল);
        }
    }
    যদি (substitute == UDISPCTX_SUBSTITUTE) {
        lang_তথ্য.get("Types", চাবি, value, ফলাফল);
    } অন্যথা {
        lang_তথ্য.getNoFallback("Types", চাবি, value, ফলাফল);
    }
    ফেরত skipAdjust? ফলাফল: adjustForUsageAndContext(k_Cap_Context_Usage_চাবি_মান, ফলাফল);
}

UnicodeString&
LocaleDisplayNamesImpl::চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                            ধ্রুবক অক্ষর* value,
                                            UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক {
    ফেরত চাবি_মান_Display_Name(চাবি, value, ফলাফল, মিথ্যা);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ধ্রুবক Locale& locale,
                                   UDialectHandling dialectHandling) {
    ফেরত নতুন LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ধ্রুবক Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    যদি (contexts == nullptr) {
        length = 0;
    }
    ফেরত নতুন LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ধ্রুবক অক্ষর * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত nullptr;
  }
  যদি (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ফেরত (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ধ্রুবক অক্ষর * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত nullptr;
  }
  যদি (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ফেরত (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI শূন্য U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uldn_getLocale(ধ্রুবক ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  যদি (ldn) {
    ফেরত ((ধ্রুবক LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ফেরত nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ধ্রুবক ULocaleDisplayNames *ldn) {
  যদি (ldn) {
    ফেরত ((ধ্রুবক LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ফেরত ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ধ্রুবক ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত (UDisplayContext)0;
  }
  ফেরত ((ধ্রুবক LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                       ধ্রুবক অক্ষর *locale,
                       char16_t *ফলাফল,
                       int32_t max_ফলাফল_আকার,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || locale == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->localeDisplayName(locale, অস্থায়ী);
  যদি (অস্থায়ী.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                         ধ্রুবক অক্ষর *lang,
                         char16_t *ফলাফল,
                         int32_t max_ফলাফল_আকার,
                         UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || lang == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->languageDisplayName(lang, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                       ধ্রুবক অক্ষর *script,
                       char16_t *ফলাফল,
                       int32_t max_ফলাফল_আকার,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || script == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->scriptDisplayName(script, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *ফলাফল,
                           int32_t max_ফলাফল_আকার,
                           UErrorCode *pErrorCode) {
  ফেরত uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), ফলাফল, max_ফলাফল_আকার, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                       ধ্রুবক অক্ষর *region,
                       char16_t *ফলাফল,
                       int32_t max_ফলাফল_আকার,
                       UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || region == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->regionDisplayName(region, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ধ্রুবক ULocaleDisplayNames *ldn,
                        ধ্রুবক অক্ষর *variant,
                        char16_t *ফলাফল,
                        int32_t max_ফলাফল_আকার,
                        UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || variant == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->variantDisplayName(variant, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_চাবি_Display_Name(ধ্রুবক ULocaleDisplayNames *ldn,
                    ধ্রুবক অক্ষর *চাবি,
                    char16_t *ফলাফল,
                    int32_t max_ফলাফল_আকার,
                    UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || চাবি == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0) || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->চাবি_Display_Name(চাবি, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_চাবি_মান_Display_Name(ধ্রুবক ULocaleDisplayNames *ldn,
                         ধ্রুবক অক্ষর *চাবি,
                         ধ্রুবক অক্ষর *value,
                         char16_t *ফলাফল,
                         int32_t max_ফলাফল_আকার,
                         UErrorCode *pErrorCode) {
  যদি (U_FAILURE(*pErrorCode)) {
    ফেরত 0;
  }
  যদি (ldn == nullptr || চাবি == nullptr || value == nullptr || (ফলাফল == nullptr && max_ফলাফল_আকার > 0)
      || max_ফলাফল_আকার < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  UnicodeString অস্থায়ী(ফলাফল, 0, max_ফলাফল_আকার);
  ((ধ্রুবক LocaleDisplayNames *)ldn)->চাবি_মান_Display_Name(চাবি, value, অস্থায়ী);
  ফেরত অস্থায়ী.extract(ফলাফল, max_ফলাফল_আকার, *pErrorCode);
}

#endif
