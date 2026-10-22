// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"

#אם !UCONFIG_NO_FORMATTING

#ضمن "unicode/locdspnm.h"
#ضمن "unicode/simpleformatter.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/udisplaycontext.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/ucurr.h"
#ضمن "bytesinkutil.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "uassert.h"
#ضمن "ulocimp.h"
#ضمن "umutex.h"
#ضمن "ureslocs.h"
#ضمن "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
מחלקה ICUDataTable {
    ثابت حرف* ثابت path;
    Locale locale;

عام:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ثابت حرف* path, ثابت Locale& locale);
    ~ICUDataTable() = افتراضي;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت حرف* tableKey, ثابت حرف* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* tableKey, ثابت حرف* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUDataTable::get(ثابت حرف* tableKey, ثابت حرف* itemKey,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר get(tableKey, nullptr, itemKey, result);
}

مضمن UnicodeString& ICUDataTable::getNoFallback(ثابت حرف* tableKey, ثابت حرف* itemKey,
                                                  UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר getNoFallback(tableKey, nullptr, itemKey, result);
}

ICUDataTable::ICUDataTable(ثابت حرف* path, ثابت Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ثابت Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  החזר locale;
}

UnicodeString&
ICUDataTable::get(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                  UnicodeString &result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  אם (U_SUCCESS(status) && len > 0) {
    החזר result.setTo(s, len);
  }
  החזר result.setTo(UnicodeString(itemKey, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  אם (U_SUCCESS(status)) {
    החזר result.setTo(s, len);
  }

  result.setToBogus();
  החזר result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#אם 0  // currently unused

מחלקה DefaultLocaleDisplayNames : عام LocaleDisplayNames {
  UDialectHandling dialectHandling;

عام:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  virtual ~DefaultLocaleDisplayNames();

  virtual ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  virtual UDialectHandling getDialectHandling() ثابت;

  virtual UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& languageDisplayName(ثابت حرف* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& scriptDisplayName(ثابت حرف* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& regionDisplayName(ثابت حرف* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& variantDisplayName(ثابت حرف* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& keyDisplayName(ثابت حرف* key,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& keyValueDisplayName(ثابت حرف* key,
                                             ثابت حرف* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ثابت Locale&
DefaultLocaleDisplayNames::getLocale() ثابت U_LIFETIME_BOUND {
  החזר Locale::getRoot();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ثابت {
  החזר dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(scriptCode);
  אם (name) {
    החזר result = UnicodeString(name, -1, US_INV);
  }
  החזר result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyDisplayName(ثابت حرف* key,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(key, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyValueDisplayName(ثابت حرف* /* key */,
                                               ثابت حرف* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  החזר result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

מחלקה LocaleDisplayNamesImpl : عام LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter keyTypeFormat;
    UDisplayContext capitalizationContext;
#אם !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#אחרת
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    تعداد CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        kCapContextUsageKey,
        kCapContextUsageKeyValue,
        kCapContextUsageCount
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[kCapContextUsageCount];

عام:
    // constructor
    LocaleDisplayNamesImpl(ثابت Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    virtual ~LocaleDisplayNamesImpl();

    virtual ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    virtual UDialectHandling getDialectHandling() ثابت override;
    virtual UDisplayContext getContext(UDisplayContextType type) ثابت override;

    virtual UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString&
    languageDisplayName(ثابت حرف* lang, UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString&
    variantDisplayName(ثابت حرف* variant, UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString& keyDisplayName(ثابت حرف* key,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString&
    keyValueDisplayName(ثابت حرف* key, ثابت حرف* value,
                        UnicodeString& result U_LIFETIME_BOUND) ثابت override;

خاص:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                منطقي substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& result U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& result U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      منطقي skipAdjust) ثابت;
    UnicodeString& keyDisplayName(ثابت حرف* key, UnicodeString& result U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& keyValueDisplayName(ثابت حرف* key, ثابت حرف* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت;
    فراغ initialize();

    بنية CapitalizationContextSink;
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
    כלעוד (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        بدل (selector) {
            حالة UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                اكسر;
            حالة UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                اكسر;
            حالة UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                اكسر;
            حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                اكسر;
            افتراضي:
                اكسر;
        }
    }
    initialize();
}

بنية LocaleDisplayNamesImpl::CapitalizationContextSink : عام ResourceSink {
    منطقي hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(שקר), parent(_parent) {}
    virtual ~CapitalizationContextSink();

    virtual فراغ put(ثابت حرف *key, ResourceValue &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
        עבור (صحيح i = 0; contexts.getKeyAndValue(i, key, value); ++i) {

            CapContextUsage usageEnum;
            אם (uprv_strcmp(key, "key") == 0) {
                usageEnum = kCapContextUsageKey;
            } אחרת אם (uprv_strcmp(key, "keyValue") == 0) {
                usageEnum = kCapContextUsageKeyValue;
            } אחרת אם (uprv_strcmp(key, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } אחרת אם (uprv_strcmp(key, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } אחרת אם (uprv_strcmp(key, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } אחרת אם (uprv_strcmp(key, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } אחרת {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* intVector = value.getIntVector(len, errorCode);
            אם (U_FAILURE(errorCode)) { החזר; }
            אם (len < 2) { استمر; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            אם (titlecaseInt == 0) { استمر; }

            parent.fCapitalization[usageEnum] = אמת;
            hasCapitalizationUsage = אמת;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

فراغ
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = هذا;
    nonConstThis->locale = langData.getLocale() == Locale::getRoot()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    אם (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    אם (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    אם (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } אחרת {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    אם (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    keyTypeFormat.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#אם !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = שקר;
    אם (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        אם (U_FAILURE(status)) { החזר; }
        CapitalizationContextSink sink(*هذا);
        ures_getAllItemsWithFallback(resource.getAlias(), "contextTransforms", sink, status);
        אם (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } אחרת אם (U_FAILURE(status)) {
            החזר;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    אם (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        אם (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#אם !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ثابت Locale&
LocaleDisplayNamesImpl::getLocale() ثابت U_LIFETIME_BOUND {
    החזר locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ثابت {
    החזר dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            החזר static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            החזר capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_LENGTH:
            החזר nameLength;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            החזר substitute;
        افتراضي:
            اكسر;
    }
    החזר static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ثابت {
#אם !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    אם ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    החזר result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  אם (loc.isBogus()) {
    result.setToBogus();
    החזר result;
  }
  UnicodeString resultName;

  ثابت حرف* lang = loc.getLanguage();
  אם (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ثابت حرف* script = loc.getScript();
  ثابت حرف* country = loc.getCountry();
  ثابت حرف* variant = loc.getVariant();

  منطقي hasScript = uprv_strlen(script) > 0;
  منطقي hasCountry = uprv_strlen(country) > 0;
  منطقي hasVariant = uprv_strlen(variant) > 0;

  אם (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    עשה { // loop construct is so we can break early out of search
      אם (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        אם (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, שקר);
          אם (!resultName.isBogus()) {
            hasScript = שקר;
            hasCountry = שקר;
            اكسر;
          }
        }
      }
      אם (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        אם (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, שקר);
          אם (!resultName.isBogus()) {
            hasScript = שקר;
            اكسر;
          }
        }
      }
      אם (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        אם (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, שקר);
          אם (!resultName.isBogus()) {
            hasCountry = שקר;
            اكسر;
          }
        }
      }
    } כלעוד (שקר);
  }
  אם (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    אם (resultName.isBogus()) {
      result.setToBogus();
      החזר result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  אם (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, אמת);
    אם (script_str.isBogus()) {
      result.setToBogus();
      החזר result;
    }
    resultRemainder.append(script_str);
  }
  אם (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, אמת);
    אם (region_str.isBogus()) {
      result.setToBogus();
      החזר result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  אם (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, אמת);
    אם (variant_str.isBogus()) {
      result.setToBogus();
      החזר result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  אם (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ثابت حرف* key;
    כלעוד ((key = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.getKeywordValue<CharString>(key, status);
        אם (U_FAILURE(status)) {
            החזר result;
      }
      keyDisplayName(key, temp, אמת);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      keyValueDisplayName(key, value.data(), temp2, אמת);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      אם (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } אחרת אם (temp != UnicodeString(key, -1, US_INV)) {
        UnicodeString temp3;
        keyTypeFormat.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } אחרת {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  אם (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    החזר adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  החזר adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    אם (buffer.isEmpty()) {
        buffer.setTo(src);
    } אחרת {
        ثابت UnicodeString *values[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(values, 2, buffer, nullptr, 0, status);
    }
    החזר buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, منطقي substitute) ثابت {
    אם (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        אם (!result.isBogus()) {
            החזר result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    אם (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        אם (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            אם (!result.isBogus()) {
                החזר result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    אם (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    החזר result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    אם (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        החזר result = UnicodeString(lang, -1, US_INV);
    }
    אם (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        אם (!result.isBogus()) {
            החזר adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    אם (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        אם (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            אם (!result.isBogus()) {
                החזר adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    אם (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    החזר adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    אם (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        אם (!result.isBogus()) {
            החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    אם (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } אחרת {
        langData.getNoFallback("Scripts", script, result);
    }
    החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר scriptDisplayName(script, result, שקר);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר scriptDisplayName(uscript_getName(scriptCode), result, שקר);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    אם (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        אם (!result.isBogus()) {
            החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    אם (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } אחרת {
        regionData.getNoFallback("Countries", region, result);
    }
    החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר regionDisplayName(region, result, שקר);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    אם (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } אחרת {
        langData.getNoFallback("Variants", variant, result);
    }
    החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר variantDisplayName(variant, result, שקר);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(ثابت حرف* key,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    אם (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", key, result);
    } אחרת {
        langData.getNoFallback("Keys", key, result);
    }
    החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKey, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(ثابت حرف* key,
                                       UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר keyDisplayName(key, result, שקר);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(ثابت حرف* key,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    אם (uprv_strcmp(key, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustrValue(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustrValue.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        אם (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustrValue;
            החזר result;
        }
        result.setTo(currencyName, len);
        החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
    }

    אם (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", key, value, result);
        אם (!result.isBogus()) {
            החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
        }
    }
    אם (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", key, value, result);
    } אחרת {
        langData.getNoFallback("Types", key, value, result);
    }
    החזר skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(ثابت حرف* key,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    החזר keyValueDisplayName(key, value, result, שקר);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    החזר جديد LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    אם (contexts == nullptr) {
        length = 0;
    }
    החזר جديد LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר nullptr;
  }
  אם (locale == nullptr) {
    locale = uloc_getDefault();
  }
  החזר (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר nullptr;
  }
  אם (locale == nullptr) {
    locale = uloc_getDefault();
  }
  החזר (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  אם (ldn) {
    החזר ((ثابت LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  החזר nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocaleDisplayNames *ldn) {
  אם (ldn) {
    החזר ((ثابت LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  החזר ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר (UDisplayContext)0;
  }
  החזר ((ثابت LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  אם (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocaleDisplayNames *ldn,
                         ثابت حرف *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ثابت ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  החזר uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocaleDisplayNames *ldn,
                        ثابت حرف *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyDisplayName(ثابت ULocaleDisplayNames *ldn,
                    ثابت حرف *key,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || key == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->keyDisplayName(key, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyValueDisplayName(ثابت ULocaleDisplayNames *ldn,
                         ثابت حرف *key,
                         ثابت حرف *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  אם (U_FAILURE(*pErrorCode)) {
    החזר 0;
  }
  אם (ldn == nullptr || key == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    החזר 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->keyValueDisplayName(key, value, temp);
  החזר temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
