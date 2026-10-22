// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/locdspnm.h"
#شامل "unicode/simpleformatter.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/ures.h"
#شامل "unicode/udisplaycontext.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/ucurr.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "uassert.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "ureslocs.h"
#شامل "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
رده ICUDataTable {
    ثابت نویسه* ثابت path;
    Locale locale;

عمومی:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ثابت نویسه* path, ثابت Locale& locale);
    ~ICUDataTable() = پیشفرض;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت نویسه* table_کلید, ثابت نویسه* item_کلید,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت نویسه* table_کلید, ثابت نویسه* sub_Table_کلید, ثابت نویسه* item_کلید,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت نویسه* table_کلید, ثابت نویسه* item_کلید,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت نویسه* table_کلید, ثابت نویسه* sub_Table_کلید, ثابت نویسه* item_کلید,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
};

درونخطی UnicodeString& ICUDataTable::get(ثابت نویسه* table_کلید, ثابت نویسه* item_کلید,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت get(table_کلید, nullptr, item_کلید, result);
}

درونخطی UnicodeString& ICUDataTable::getNoFallback(ثابت نویسه* table_کلید, ثابت نویسه* item_کلید,
                                                  UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت getNoFallback(table_کلید, nullptr, item_کلید, result);
}

ICUDataTable::ICUDataTable(ثابت نویسه* path, ثابت Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ثابت Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  بازگشت locale;
}

UnicodeString&
ICUDataTable::get(ثابت نویسه* table_کلید, ثابت نویسه* sub_Table_کلید, ثابت نویسه* item_کلید,
                  UnicodeString &result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_کلید, sub_Table_کلید, item_کلید,
                                                   &len, &status);
  اگر (U_SUCCESS(status) && len > 0) {
    بازگشت result.setTo(s, len);
  }
  بازگشت result.setTo(UnicodeString(item_کلید, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ثابت نویسه* table_کلید, ثابت نویسه* sub_Table_کلید, ثابت نویسه* item_کلید,
                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_کلید, sub_Table_کلید, item_کلید,
                                                   &len, &status);
  اگر (U_SUCCESS(status)) {
    بازگشت result.setTo(s, len);
  }

  result.setToBogus();
  بازگشت result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#اگر 0  // currently unused

رده DefaultLocaleDisplayNames : عمومی LocaleDisplayNames {
  UDialectHandling dialectHandling;

عمومی:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  مجازی ~DefaultLocaleDisplayNames();

  مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  مجازی UDialectHandling getDialectHandling() ثابت;

  مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& localeDisplayName(ثابت نویسه* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& languageDisplayName(ثابت نویسه* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(ثابت نویسه* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& regionDisplayName(ثابت نویسه* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& variantDisplayName(ثابت نویسه* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_Display_Name(ثابت نویسه* کلید,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_مقدار_Display_Name(ثابت نویسه* کلید,
                                             ثابت نویسه* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ثابت Locale&
DefaultLocaleDisplayNames::getLocale() ثابت U_LIFETIME_BOUND {
  بازگشت Locale::get_ریشه();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ثابت {
  بازگشت dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت نویسه* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ثابت نویسه* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ثابت نویسه* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  ثابت نویسه* name = uscript_getName(scriptCode);
  اگر (name) {
    بازگشت result = UnicodeString(name, -1, US_INV);
  }
  بازگشت result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ثابت نویسه* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ثابت نویسه* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::کلید_Display_Name(ثابت نویسه* کلید,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(کلید, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::کلید_مقدار_Display_Name(ثابت نویسه* /*کلید*/,
                                               ثابت نویسه* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  بازگشت result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

رده LocaleDisplayNamesImpl : عمومی LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter کلید_Type_Format;
    UDisplayContext capitalizationContext;
#اگر !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وگرنه
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    شمارش CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_کلید,
        k_Cap_Context_Usage_کلید_مقدار,
        k_Cap_Context_Usage_شمار
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقی fCapitalization[k_Cap_Context_Usage_شمار];

عمومی:
    // constructor
    LocaleDisplayNamesImpl(ثابت Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازی ~LocaleDisplayNamesImpl();

    مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    مجازی UDialectHandling getDialectHandling() ثابت override;
    مجازی UDisplayContext getContext(UDisplayContextType type) ثابت override;

    مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& localeDisplayName(ثابت نویسه* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    languageDisplayName(ثابت نویسه* lang, UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(ثابت نویسه* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& regionDisplayName(ثابت نویسه* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    variantDisplayName(ثابت نویسه* variant, UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& کلید_Display_Name(ثابت نویسه* کلید,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    کلید_مقدار_Display_Name(ثابت نویسه* کلید, ثابت نویسه* value,
                        UnicodeString& result U_LIFETIME_BOUND) ثابت override;

خصوصی:
    UnicodeString& localeIdName(ثابت نویسه* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                منطقی substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت نویسه* script, UnicodeString& result U_LIFETIME_BOUND,
                                     منطقی skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت نویسه* region, UnicodeString& result U_LIFETIME_BOUND,
                                     منطقی skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت نویسه* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      منطقی skipAdjust) ثابت;
    UnicodeString& کلید_Display_Name(ثابت نویسه* کلید, UnicodeString& result U_LIFETIME_BOUND,
                                  منطقی skipAdjust) ثابت;
    UnicodeString& کلید_مقدار_Display_Name(ثابت نویسه* کلید, ثابت نویسه* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقی skipAdjust) ثابت;
    تهی initialize();

    ساختار CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ثابت Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    وقتی (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        انتخاب (selector) {
            حالت UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                بشکن;
            حالت UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                بشکن;
            حالت UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                بشکن;
            حالت UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                بشکن;
            پیشفرض:
                بشکن;
        }
    }
    initialize();
}

ساختار LocaleDisplayNamesImpl::CapitalizationContextSink : عمومی ResourceSink {
    منطقی hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(نادرست), parent(_parent) {}
    مجازی ~CapitalizationContextSink();

    مجازی تهی put(ثابت نویسه *کلید, Resource_مقدار &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        برای (صحیح i = 0; contexts.get_کلید_And_مقدار(i, کلید, value); ++i) {

            CapContextUsage usageEnum;
            اگر (uprv_strcmp(کلید, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید;
            } وگرنه اگر (uprv_strcmp(کلید, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید_مقدار;
            } وگرنه اگر (uprv_strcmp(کلید, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وگرنه اگر (uprv_strcmp(کلید, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وگرنه اگر (uprv_strcmp(کلید, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وگرنه اگر (uprv_strcmp(کلید, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وگرنه {
                ادامه;
            }

            int32_t len = 0;
            ثابت int32_t* intVector = value.getIntVector(len, errorCode);
            اگر (U_FAILURE(errorCode)) { بازگشت; }
            اگر (len < 2) { ادامه; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            اگر (titlecaseInt == 0) { ادامه; }

            parent.fCapitalization[usageEnum] = درست;
            hasCapitalizationUsage = درست;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

تهی
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = این;
    nonConstThis->locale = langData.getLocale() == Locale::get_ریشه()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    اگر (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    اگر (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    اگر (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وگرنه {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    اگر (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    کلید_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, اندازه(fCapitalization));
#اگر !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقی needBrkIter = نادرست;
    اگر (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        اگر (U_FAILURE(status)) { بازگشت; }
        CapitalizationContextSink sink(*این);
        ures_get_All_اقلام_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        اگر (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } وگرنه اگر (U_FAILURE(status)) {
            بازگشت;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    اگر (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        اگر (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#اگر !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ثابت Locale&
LocaleDisplayNamesImpl::getLocale() ثابت U_LIFETIME_BOUND {
    بازگشت locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ثابت {
    بازگشت dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ثابت {
    انتخاب (type) {
        حالت UDISPCTX_TYPE_DIALECT_HANDLING:
            بازگشت static_cast<UDisplayContext>(dialectHandling);
        حالت UDISPCTX_TYPE_CAPITALIZATION:
            بازگشت capitalizationContext;
        حالت UDISPCTX_TYPE_DISPLAY_LENGTH:
            بازگشت nameLength;
        حالت UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            بازگشت substitute;
        پیشفرض:
            بشکن;
    }
    بازگشت static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ثابت {
#اگر !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    اگر ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ایستا UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    بازگشت result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  اگر (loc.isBogus()) {
    result.setToBogus();
    بازگشت result;
  }
  UnicodeString resultName;

  ثابت نویسه* lang = loc.getLanguage();
  اگر (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ثابت نویسه* script = loc.getScript();
  ثابت نویسه* country = loc.getCountry();
  ثابت نویسه* variant = loc.getVariant();

  منطقی hasScript = uprv_strlen(script) > 0;
  منطقی hasCountry = uprv_strlen(country) > 0;
  منطقی hasVariant = uprv_strlen(variant) > 0;

  اگر (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    انجامبده { // loop construct is so we can break early out of search
      اگر (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        اگر (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, نادرست);
          اگر (!resultName.isBogus()) {
            hasScript = نادرست;
            hasCountry = نادرست;
            بشکن;
          }
        }
      }
      اگر (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        اگر (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, نادرست);
          اگر (!resultName.isBogus()) {
            hasScript = نادرست;
            بشکن;
          }
        }
      }
      اگر (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        اگر (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, نادرست);
          اگر (!resultName.isBogus()) {
            hasCountry = نادرست;
            بشکن;
          }
        }
      }
    } وقتی (نادرست);
  }
  اگر (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    اگر (resultName.isBogus()) {
      result.setToBogus();
      بازگشت result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  اگر (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, درست);
    اگر (script_str.isBogus()) {
      result.setToBogus();
      بازگشت result;
    }
    resultRemainder.append(script_str);
  }
  اگر (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, درست);
    اگر (region_str.isBogus()) {
      result.setToBogus();
      بازگشت result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  اگر (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, درست);
    اگر (variant_str.isBogus()) {
      result.setToBogus();
      بازگشت result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  اگر (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ثابت نویسه* کلید;
    وقتی ((کلید = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_مقدار<CharString>(کلید, status);
        اگر (U_FAILURE(status)) {
            بازگشت result;
      }
      کلید_Display_Name(کلید, temp, درست);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      کلید_مقدار_Display_Name(کلید, value.data(), temp2, درست);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      اگر (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } وگرنه اگر (temp != UnicodeString(کلید, -1, US_INV)) {
        UnicodeString temp3;
        کلید_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } وگرنه {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  اگر (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    بازگشت adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  بازگشت adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    اگر (buffer.isEmpty()) {
        buffer.setTo(src);
    } وگرنه {
        ثابت UnicodeString *مقادیر[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(مقادیر, 2, buffer, nullptr, 0, status);
    }
    بازگشت buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت نویسه* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ثابت نویسه* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, منطقی substitute) ثابت {
    اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        اگر (!result.isBogus()) {
            بازگشت result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    اگر (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت نویسه* canonLocId = canonLocale.getName();
        اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            اگر (!result.isBogus()) {
                بازگشت result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    اگر (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    بازگشت result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ثابت نویسه* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    اگر (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        بازگشت result = UnicodeString(lang, -1, US_INV);
    }
    اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        اگر (!result.isBogus()) {
            بازگشت adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    اگر (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت نویسه* canonLocId = canonLocale.getName();
        اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            اگر (!result.isBogus()) {
                بازگشت adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    اگر (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    بازگشت adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت نویسه* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقی skipAdjust) ثابت {
    اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        اگر (!result.isBogus()) {
            بازگشت skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } وگرنه {
        langData.getNoFallback("Scripts", script, result);
    }
    بازگشت skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت نویسه* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت scriptDisplayName(script, result, نادرست);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت scriptDisplayName(uscript_getName(scriptCode), result, نادرست);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت نویسه* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقی skipAdjust) ثابت {
    اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        اگر (!result.isBogus()) {
            بازگشت skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } وگرنه {
        regionData.getNoFallback("Countries", region, result);
    }
    بازگشت skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت نویسه* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت regionDisplayName(region, result, نادرست);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت نویسه* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           منطقی skipAdjust) ثابت {
    // don't have a resource for short variant names
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } وگرنه {
        langData.getNoFallback("Variants", variant, result);
    }
    بازگشت skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت نویسه* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت variantDisplayName(variant, result, نادرست);
}

UnicodeString&
LocaleDisplayNamesImpl::کلید_Display_Name(ثابت نویسه* کلید,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقی skipAdjust) ثابت {
    // don't have a resource for short key names
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", کلید, result);
    } وگرنه {
        langData.getNoFallback("Keys", کلید, result);
    }
    بازگشت skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_کلید, result);
}

UnicodeString&
LocaleDisplayNamesImpl::کلید_Display_Name(ثابت نویسه* کلید,
                                       UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت کلید_Display_Name(کلید, result, نادرست);
}

UnicodeString&
LocaleDisplayNamesImpl::کلید_مقدار_Display_Name(ثابت نویسه* کلید,
                                            ثابت نویسه* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            منطقی skipAdjust) ثابت {
    اگر (uprv_strcmp(کلید, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_مقدار(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_مقدار.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        اگر (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_مقدار;
            بازگشت result;
        }
        result.setTo(currencyName, len);
        بازگشت skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, result);
    }

    اگر (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", کلید, value, result);
        اگر (!result.isBogus()) {
            بازگشت skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, result);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", کلید, value, result);
    } وگرنه {
        langData.getNoFallback("Types", کلید, value, result);
    }
    بازگشت skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, result);
}

UnicodeString&
LocaleDisplayNamesImpl::کلید_مقدار_Display_Name(ثابت نویسه* کلید,
                                            ثابت نویسه* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    بازگشت کلید_مقدار_Display_Name(کلید, value, result, نادرست);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    بازگشت جدید LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    اگر (contexts == nullptr) {
        length = 0;
    }
    بازگشت جدید LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ثابت نویسه * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت nullptr;
  }
  اگر (locale == nullptr) {
    locale = uloc_getDefault();
  }
  بازگشت (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ثابت نویسه * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت nullptr;
  }
  اگر (locale == nullptr) {
    locale = uloc_getDefault();
  }
  بازگشت (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI تهی U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ثابت نویسه* U_EXPORT2
uldn_getLocale(ثابت ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  اگر (ldn) {
    بازگشت ((ثابت LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  بازگشت nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocaleDisplayNames *ldn) {
  اگر (ldn) {
    بازگشت ((ثابت LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  بازگشت ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت (UDisplayContext)0;
  }
  بازگشت ((ثابت LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت نویسه *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  اگر (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocaleDisplayNames *ldn,
                         ثابت نویسه *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت نویسه *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ثابت ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  بازگشت uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت نویسه *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocaleDisplayNames *ldn,
                        ثابت نویسه *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_Display_Name(ثابت ULocaleDisplayNames *ldn,
                    ثابت نویسه *کلید,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || کلید == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->کلید_Display_Name(کلید, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_مقدار_Display_Name(ثابت ULocaleDisplayNames *ldn,
                         ثابت نویسه *کلید,
                         ثابت نویسه *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  اگر (U_FAILURE(*pErrorCode)) {
    بازگشت 0;
  }
  اگر (ldn == nullptr || کلید == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->کلید_مقدار_Display_Name(کلید, value, temp);
  بازگشت temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
