// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_FORMATTING

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
صنف ICUDataTable {
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
    أرجع get(tableKey, nullptr, itemKey, result);
}

مضمن UnicodeString& ICUDataTable::getNoFallback(ثابت حرف* tableKey, ثابت حرف* itemKey,
                                                  UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(tableKey, nullptr, itemKey, result);
}

ICUDataTable::ICUDataTable(ثابت حرف* path, ثابت Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ثابت Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  أرجع locale;
}

UnicodeString&
ICUDataTable::get(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                  UnicodeString &result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  𐤀𐤌 (U_SUCCESS(status) && len > 0) {
    أرجع result.setTo(s, len);
  }
  أرجع result.setTo(UnicodeString(itemKey, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ثابت حرف* tableKey, ثابت حرف* subTableKey, ثابت حرف* itemKey,
                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  𐤀𐤌 (U_SUCCESS(status)) {
    أرجع result.setTo(s, len);
  }

  result.setToBogus();
  أرجع result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#𐤀𐤌 0  // currently unused

صنف DefaultLocaleDisplayNames : عام LocaleDisplayNames {
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
  أرجع Locale::getRoot();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ثابت {
  أرجع dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(scriptCode);
  𐤀𐤌 (name) {
    أرجع result = UnicodeString(name, -1, US_INV);
  }
  أرجع result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyDisplayName(ثابت حرف* key,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(key, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyValueDisplayName(ثابت حرف* /* key */,
                                               ثابت حرف* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

صنف LocaleDisplayNamesImpl : عام LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter keyTypeFormat;
    UDisplayContext capitalizationContext;
#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وإلا
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
    طالما (length-- > 0) {
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
      : hasCapitalizationUsage(خطأ), parent(_parent) {}
    virtual ~CapitalizationContextSink();

    virtual فراغ put(ثابت حرف *key, ResourceValue &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        لكل (صحيح i = 0; contexts.getKeyAndValue(i, key, value); ++i) {

            CapContextUsage usageEnum;
            𐤀𐤌 (uprv_strcmp(key, "key") == 0) {
                usageEnum = kCapContextUsageKey;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "keyValue") == 0) {
                usageEnum = kCapContextUsageKeyValue;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وإلا 𐤀𐤌 (uprv_strcmp(key, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وإلا {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* intVector = value.getIntVector(len, errorCode);
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
            𐤀𐤌 (len < 2) { استمر; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            𐤀𐤌 (titlecaseInt == 0) { استمر; }

            parent.fCapitalization[usageEnum] = true;
            hasCapitalizationUsage = true;
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
    𐤀𐤌 (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    𐤀𐤌 (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    𐤀𐤌 (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وإلا {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    𐤀𐤌 (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    keyTypeFormat.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    𐤀𐤌 (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        𐤀𐤌 (U_FAILURE(status)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_getAllItemsWithFallback(resource.getAlias(), "contextTransforms", sink, status);
        𐤀𐤌 (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } وإلا 𐤀𐤌 (U_FAILURE(status)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    𐤀𐤌 (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        𐤀𐤌 (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ثابت Locale&
LocaleDisplayNamesImpl::getLocale() ثابت U_LIFETIME_BOUND {
    أرجع locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ثابت {
    أرجع dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            أرجع static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            أرجع capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_LENGTH:
            أرجع nameLength;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            أرجع substitute;
        افتراضي:
            اكسر;
    }
    أرجع static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ثابت {
#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    𐤀𐤌 ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    أرجع result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  𐤀𐤌 (loc.isBogus()) {
    result.setToBogus();
    أرجع result;
  }
  UnicodeString resultName;

  ثابت حرف* lang = loc.getLanguage();
  𐤀𐤌 (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ثابت حرف* script = loc.getScript();
  ثابت حرف* country = loc.getCountry();
  ثابت حرف* variant = loc.getVariant();

  منطقي hasScript = uprv_strlen(script) > 0;
  منطقي hasCountry = uprv_strlen(country) > 0;
  منطقي hasVariant = uprv_strlen(variant) > 0;

  𐤀𐤌 (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    𐤐𐤏𐤋 { // loop construct is so we can break early out of search
      𐤀𐤌 (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        𐤀𐤌 (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          𐤀𐤌 (!resultName.isBogus()) {
            hasScript = خطأ;
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
      𐤀𐤌 (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        𐤀𐤌 (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          𐤀𐤌 (!resultName.isBogus()) {
            hasScript = خطأ;
            اكسر;
          }
        }
      }
      𐤀𐤌 (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        𐤀𐤌 (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          𐤀𐤌 (!resultName.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  𐤀𐤌 (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    𐤀𐤌 (resultName.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  𐤀𐤌 (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    𐤀𐤌 (script_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    resultRemainder.append(script_str);
  }
  𐤀𐤌 (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    𐤀𐤌 (region_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  𐤀𐤌 (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    𐤀𐤌 (variant_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  𐤀𐤌 (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ثابت حرف* key;
    طالما ((key = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.getKeywordValue<CharString>(key, status);
        𐤀𐤌 (U_FAILURE(status)) {
            أرجع result;
      }
      keyDisplayName(key, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      keyValueDisplayName(key, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      𐤀𐤌 (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } وإلا 𐤀𐤌 (temp != UnicodeString(key, -1, US_INV)) {
        UnicodeString temp3;
        keyTypeFormat.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } وإلا {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  𐤀𐤌 (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    𐤀𐤌 (buffer.isEmpty()) {
        buffer.setTo(src);
    } وإلا {
        ثابت UnicodeString *values[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(values, 2, buffer, nullptr, 0, status);
    }
    أرجع buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, منطقي substitute) ثابت {
    𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        𐤀𐤌 (!result.isBogus()) {
            أرجع result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    𐤀𐤌 (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            𐤀𐤌 (!result.isBogus()) {
                أرجع result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    𐤀𐤌 (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    𐤀𐤌 (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        أرجع result = UnicodeString(lang, -1, US_INV);
    }
    𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        𐤀𐤌 (!result.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    𐤀𐤌 (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            𐤀𐤌 (!result.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    𐤀𐤌 (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        𐤀𐤌 (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    𐤀𐤌 (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } وإلا {
        langData.getNoFallback("Scripts", script, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(script, result, خطأ);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(uscript_getName(scriptCode), result, خطأ);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        𐤀𐤌 (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    𐤀𐤌 (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } وإلا {
        regionData.getNoFallback("Countries", region, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع regionDisplayName(region, result, خطأ);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    𐤀𐤌 (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } وإلا {
        langData.getNoFallback("Variants", variant, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع variantDisplayName(variant, result, خطأ);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(ثابت حرف* key,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    𐤀𐤌 (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", key, result);
    } وإلا {
        langData.getNoFallback("Keys", key, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKey, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(ثابت حرف* key,
                                       UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع keyDisplayName(key, result, خطأ);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(ثابت حرف* key,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    𐤀𐤌 (uprv_strcmp(key, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustrValue(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustrValue.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        𐤀𐤌 (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustrValue;
            أرجع result;
        }
        result.setTo(currencyName, len);
        أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
    }

    𐤀𐤌 (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", key, value, result);
        𐤀𐤌 (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
        }
    }
    𐤀𐤌 (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", key, value, result);
    } وإلا {
        langData.getNoFallback("Types", key, value, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(ثابت حرف* key,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع keyValueDisplayName(key, value, result, خطأ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    أرجع جديد LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    𐤀𐤌 (contexts == nullptr) {
        length = 0;
    }
    أرجع جديد LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع nullptr;
  }
  𐤀𐤌 (locale == nullptr) {
    locale = uloc_getDefault();
  }
  أرجع (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع nullptr;
  }
  𐤀𐤌 (locale == nullptr) {
    locale = uloc_getDefault();
  }
  أرجع (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  𐤀𐤌 (ldn) {
    أرجع ((ثابت LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  أرجع nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocaleDisplayNames *ldn) {
  𐤀𐤌 (ldn) {
    أرجع ((ثابت LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع (UDisplayContext)0;
  }
  أرجع ((ثابت LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  𐤀𐤌 (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocaleDisplayNames *ldn,
                         ثابت حرف *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ثابت ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  أرجع uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocaleDisplayNames *ldn,
                       ثابت حرف *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocaleDisplayNames *ldn,
                        ثابت حرف *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyDisplayName(ثابت ULocaleDisplayNames *ldn,
                    ثابت حرف *key,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || key == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->keyDisplayName(key, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyValueDisplayName(ثابت ULocaleDisplayNames *ldn,
                         ثابت حرف *key,
                         ثابت حرف *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  𐤀𐤌 (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  𐤀𐤌 (ldn == nullptr || key == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->keyValueDisplayName(key, value, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
