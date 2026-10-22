// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_FORMATTING

#include "unicode/locdspnm.h"
#include "unicode/simpleformatter.h"
#include "unicode/ucasemap.h"
#include "unicode/ures.h"
#include "unicode/udisplaycontext.h"
#include "unicode/brkiter.h"
#include "unicode/ucurr.h"
#include "bytesinkutil.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "mutex.h"
#include "uassert.h"
#include "ulocimp.h"
#include "umutex.h"
#include "ureslocs.h"
#include "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
class ICUDataTable {
    const var_na* const path;
    Locale locale;

public:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(const var_na* path, const Locale& locale);
    ~ICUDataTable() = default;

    const Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(const var_na* tableKey, const var_na* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& get(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) const;

    UnicodeString& getNoFallback(const var_na* tableKey, const var_na* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& getNoFallback(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
};

anta_hstha UnicodeString& ICUDataTable::get(const var_na* tableKey, const var_na* itemKey,
                                        UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ get(tableKey, nullptr, itemKey, result);
}

anta_hstha UnicodeString& ICUDataTable::getNoFallback(const var_na* tableKey, const var_na* itemKey,
                                                  UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ getNoFallback(tableKey, nullptr, itemKey, result);
}

ICUDataTable::ICUDataTable(const var_na* path, const Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

const Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ត្រឡប់ locale;
}

UnicodeString&
ICUDataTable::get(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                  UnicodeString &result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  ប្រសិនបើ (U_SUCCESS(status) && len > 0) {
    ត្រឡប់ result.setTo(s, len);
  }
  ត្រឡប់ result.setTo(UnicodeString(itemKey, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                            UnicodeString& result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  ប្រសិនបើ (U_SUCCESS(status)) {
    ត្រឡប់ result.setTo(s, len);
  }

  result.setToBogus();
  ត្រឡប់ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ប្រសិនបើ 0  // currently unused

class DefaultLocaleDisplayNames : public LocaleDisplayNames {
  UDialectHandling dialectHandling;

public:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  virtual ~DefaultLocaleDisplayNames();

  virtual const Locale& getLocale() const U_LIFETIME_BOUND;
  virtual UDialectHandling getDialectHandling() const;

  virtual UnicodeString& localeDisplayName(const Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& localeDisplayName(const var_na* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& languageDisplayName(const var_na* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& scriptDisplayName(const var_na* script,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& regionDisplayName(const var_na* region,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& variantDisplayName(const var_na* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& keyDisplayName(const var_na* key,
                                        UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& keyValueDisplayName(const var_na* key,
                                             const var_na* value,
                                             UnicodeString& result U_LIFETIME_BOUND) const;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

const Locale&
DefaultLocaleDisplayNames::getLocale() const U_LIFETIME_BOUND {
  ត្រឡប់ Locale::getRoot();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() const {
  ត្រឡប់ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const var_na* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(const var_na* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(const var_na* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  const var_na* name = uscript_getName(scriptCode);
  ប្រសិនបើ (name) {
    ត្រឡប់ result = UnicodeString(name, -1, US_INV);
  }
  ត្រឡប់ result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(const var_na* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(const var_na* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyDisplayName(const var_na* key,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(key, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyValueDisplayName(const var_na* /* key */,
                                               const var_na* value,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  ត្រឡប់ result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

class LocaleDisplayNamesImpl : public LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter keyTypeFormat;
    UDisplayContext capitalizationContext;
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#បើមិនដូច្នេះ
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ga_nanaa CapContextUsage {
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
     bool fCapitalization[kCapContextUsageCount];

public:
    // constructor
    LocaleDisplayNamesImpl(const Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(const Locale& locale, UDisplayContext *contexts, int32_t length);
    virtual ~LocaleDisplayNamesImpl();

    virtual const Locale& getLocale() const U_LIFETIME_BOUND override;
    virtual UDialectHandling getDialectHandling() const override;
    virtual UDisplayContext getContext(UDisplayContextType type) const override;

    virtual UnicodeString& localeDisplayName(const Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& localeDisplayName(const var_na* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    languageDisplayName(const var_na* lang, UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& scriptDisplayName(const var_na* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& regionDisplayName(const var_na* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    variantDisplayName(const var_na* variant, UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& keyDisplayName(const var_na* key,
                                          UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    keyValueDisplayName(const var_na* key, const var_na* value,
                        UnicodeString& result U_LIFETIME_BOUND) const override;

private:
    UnicodeString& localeIdName(const var_na* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                bool substitute) const;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 const UnicodeString& src) const;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& scriptDisplayName(const var_na* script, UnicodeString& result U_LIFETIME_BOUND,
                                     bool skipAdjust) const;
    UnicodeString& regionDisplayName(const var_na* region, UnicodeString& result U_LIFETIME_BOUND,
                                     bool skipAdjust) const;
    UnicodeString& variantDisplayName(const var_na* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      bool skipAdjust) const;
    UnicodeString& keyDisplayName(const var_na* key, UnicodeString& result U_LIFETIME_BOUND,
                                  bool skipAdjust) const;
    UnicodeString& keyValueDisplayName(const var_na* key, const var_na* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const;
    void initialize();

    sa_nracanaa CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(const Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(const Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ខណៈពេល (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        switch (selector) {
            pak_sa UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                bha_nga;
            pak_sa UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                bha_nga;
            pak_sa UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                bha_nga;
            pak_sa UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                bha_nga;
            default:
                bha_nga;
        }
    }
    initialize();
}

sa_nracanaa LocaleDisplayNamesImpl::CapitalizationContextSink : public ResourceSink {
    bool hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(false), parent(_parent) {}
    virtual ~CapitalizationContextSink();

    virtual void put(const var_na *key, ResourceValue &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        សម្រាប់ (ចំនួនគត់ i = 0; contexts.getKeyAndValue(i, key, value); ++i) {

            CapContextUsage usageEnum;
            ប្រសិនបើ (uprv_strcmp(key, "key") == 0) {
                usageEnum = kCapContextUsageKey;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "keyValue") == 0) {
                usageEnum = kCapContextUsageKeyValue;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } បើមិនដូច្នេះ {
                continue;
            }

            int32_t len = 0;
            const int32_t* intVector = value.getIntVector(len, errorCode);
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
            ប្រសិនបើ (len < 2) { continue; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            ប្រសិនបើ (titlecaseInt == 0) { continue; }

            parent.fCapitalization[usageEnum] = true;
            hasCapitalizationUsage = true;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

void
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = this;
    nonConstThis->locale = langData.getLocale() == Locale::getRoot()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    ប្រសិនបើ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    ប្រសិនបើ (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    ប្រសិនបើ (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } បើមិនដូច្នេះ {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    ប្រសិនបើ (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    keyTypeFormat.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, parimaa_na(fCapitalization));
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    bool needBrkIter = false;
    ប្រសិនបើ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        ប្រសិនបើ (U_FAILURE(status)) { ត្រឡប់; }
        CapitalizationContextSink sink(*this);
        ures_getAllItemsWithFallback(resource.getAlias(), "contextTransforms", sink, status);
        ប្រសិនបើ (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } បើមិនដូច្នេះ ប្រសិនបើ (U_FAILURE(status)) {
            ត្រឡប់;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ប្រសិនបើ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        ប្រសិនបើ (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

const Locale&
LocaleDisplayNamesImpl::getLocale() const U_LIFETIME_BOUND {
    ត្រឡប់ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() const {
    ត្រឡប់ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) const {
    switch (type) {
        pak_sa UDISPCTX_TYPE_DIALECT_HANDLING:
            ត្រឡប់ static_cast<UDisplayContext>(dialectHandling);
        pak_sa UDISPCTX_TYPE_CAPITALIZATION:
            ត្រឡប់ capitalizationContext;
        pak_sa UDISPCTX_TYPE_DISPLAY_LENGTH:
            ត្រឡប់ nameLength;
        pak_sa UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ត្រឡប់ substitute;
        default:
            bha_nga;
    }
    ត្រឡប់ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) const {
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ប្រសិនបើ ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        static UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ត្រឡប់ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  ប្រសិនបើ (loc.isBogus()) {
    result.setToBogus();
    ត្រឡប់ result;
  }
  UnicodeString resultName;

  const var_na* lang = loc.getLanguage();
  ប្រសិនបើ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  const var_na* script = loc.getScript();
  const var_na* country = loc.getCountry();
  const var_na* variant = loc.getVariant();

  bool hasScript = uprv_strlen(script) > 0;
  bool hasCountry = uprv_strlen(country) > 0;
  bool hasVariant = uprv_strlen(variant) > 0;

  ប្រសិនបើ (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    do { // loop construct is so we can break early out of search
      ប្រសិនបើ (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        ប្រសិនបើ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ប្រសិនបើ (!resultName.isBogus()) {
            hasScript = false;
            hasCountry = false;
            bha_nga;
          }
        }
      }
      ប្រសិនបើ (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        ប្រសិនបើ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ប្រសិនបើ (!resultName.isBogus()) {
            hasScript = false;
            bha_nga;
          }
        }
      }
      ប្រសិនបើ (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        ប្រសិនបើ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ប្រសិនបើ (!resultName.isBogus()) {
            hasCountry = false;
            bha_nga;
          }
        }
      }
    } ខណៈពេល (false);
  }
  ប្រសិនបើ (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    ប្រសិនបើ (resultName.isBogus()) {
      result.setToBogus();
      ត្រឡប់ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  ប្រសិនបើ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    ប្រសិនបើ (script_str.isBogus()) {
      result.setToBogus();
      ត្រឡប់ result;
    }
    resultRemainder.append(script_str);
  }
  ប្រសិនបើ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    ប្រសិនបើ (region_str.isBogus()) {
      result.setToBogus();
      ត្រឡប់ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  ប្រសិនបើ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    ប្រសិនបើ (variant_str.isBogus()) {
      result.setToBogus();
      ត្រឡប់ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  ប្រសិនបើ (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    const var_na* key;
    ខណៈពេល ((key = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.getKeywordValue<CharString>(key, status);
        ប្រសិនបើ (U_FAILURE(status)) {
            ត្រឡប់ result;
      }
      keyDisplayName(key, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      keyValueDisplayName(key, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ប្រសិនបើ (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } បើមិនដូច្នេះ ប្រសិនបើ (temp != UnicodeString(key, -1, US_INV)) {
        UnicodeString temp3;
        keyTypeFormat.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } បើមិនដូច្នេះ {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ប្រសិនបើ (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    ត្រឡប់ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  ត្រឡប់ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, const UnicodeString& src) const {
    ប្រសិនបើ (buffer.isEmpty()) {
        buffer.setTo(src);
    } បើមិនដូច្នេះ {
        const UnicodeString *values[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(values, 2, buffer, nullptr, 0, status);
    }
    ត្រឡប់ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const var_na* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(const var_na* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, bool substitute) const {
    ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        ប្រសិនបើ (!result.isBogus()) {
            ត្រឡប់ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    ប្រសិនបើ (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        const var_na* canonLocId = canonLocale.getName();
        ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ប្រសិនបើ (!result.isBogus()) {
                ត្រឡប់ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ប្រសិនបើ (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ត្រឡប់ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(const var_na* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    ប្រសិនបើ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ត្រឡប់ result = UnicodeString(lang, -1, US_INV);
    }
    ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        ប្រសិនបើ (!result.isBogus()) {
            ត្រឡប់ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    ប្រសិនបើ (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        const var_na* canonLocId = canonLocale.getName();
        ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ប្រសិនបើ (!result.isBogus()) {
                ត្រឡប់ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ប្រសិនបើ (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ត្រឡប់ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const var_na* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        ប្រសិនបើ (!result.isBogus()) {
            ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    ប្រសិនបើ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } បើមិនដូច្នេះ {
        langData.getNoFallback("Scripts", script, result);
    }
    ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const var_na* script,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ scriptDisplayName(script, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ scriptDisplayName(uscript_getName(scriptCode), result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const var_na* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        ប្រសិនបើ (!result.isBogus()) {
            ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    ប្រសិនបើ (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } បើមិនដូច្នេះ {
        regionData.getNoFallback("Countries", region, result);
    }
    ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const var_na* region,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ regionDisplayName(region, result, false);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const var_na* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           bool skipAdjust) const {
    // don't have a resource for short variant names
    ប្រសិនបើ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } បើមិនដូច្នេះ {
        langData.getNoFallback("Variants", variant, result);
    }
    ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const var_na* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ variantDisplayName(variant, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const var_na* key,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const {
    // don't have a resource for short key names
    ប្រសិនបើ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", key, result);
    } បើមិនដូច្នេះ {
        langData.getNoFallback("Keys", key, result);
    }
    ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKey, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const var_na* key,
                                       UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ keyDisplayName(key, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const var_na* key,
                                            const var_na* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            bool skipAdjust) const {
    ប្រសិនបើ (uprv_strcmp(key, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustrValue(value, -1, US_INV);
        int32_t len;
        const char16_t *currencyName = ucurr_getName(ustrValue.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        ប្រសិនបើ (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustrValue;
            ត្រឡប់ result;
        }
        result.setTo(currencyName, len);
        ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
    }

    ប្រសិនបើ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", key, value, result);
        ប្រសិនបើ (!result.isBogus()) {
            ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
        }
    }
    ប្រសិនបើ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", key, value, result);
    } បើមិនដូច្នេះ {
        langData.getNoFallback("Types", key, value, result);
    }
    ត្រឡប់ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const var_na* key,
                                            const var_na* value,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    ត្រឡប់ keyValueDisplayName(key, value, result, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDialectHandling dialectHandling) {
    ត្រឡប់ new LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ប្រសិនបើ (contexts == nullptr) {
        length = 0;
    }
    ត្រឡប់ new LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(const var_na * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ nullptr;
  }
  ប្រសិនបើ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ត្រឡប់ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(const var_na * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ nullptr;
  }
  ប្រសិនបើ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ត្រឡប់ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI void U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI const var_na* U_EXPORT2
uldn_getLocale(const ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  ប្រសិនបើ (ldn) {
    ត្រឡប់ ((const LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ត្រឡប់ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(const ULocaleDisplayNames *ldn) {
  ប្រសិនបើ (ldn) {
    ត្រឡប់ ((const LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ត្រឡប់ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(const ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ (UDisplayContext)0;
  }
  ត្រឡប់ ((const LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  ប្រសិនបើ (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(const ULocaleDisplayNames *ldn,
                         const var_na *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(const ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  ត្រឡប់ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(const ULocaleDisplayNames *ldn,
                        const var_na *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyDisplayName(const ULocaleDisplayNames *ldn,
                    const var_na *key,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || key == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyDisplayName(key, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyValueDisplayName(const ULocaleDisplayNames *ldn,
                         const var_na *key,
                         const var_na *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
    ត្រឡប់ 0;
  }
  ប្រសិនបើ (ldn == nullptr || key == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ត្រឡប់ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyValueDisplayName(key, value, temp);
  ត្រឡប់ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
