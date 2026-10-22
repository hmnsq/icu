// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"

#ᮚᮦᮔ᮪ !UCONFIG_NO_FORMATTING

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
    ᮝᮜᮤ get(tableKey, nullptr, itemKey, result);
}

anta_hstha UnicodeString& ICUDataTable::getNoFallback(const var_na* tableKey, const var_na* itemKey,
                                                  UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ getNoFallback(tableKey, nullptr, itemKey, result);
}

ICUDataTable::ICUDataTable(const var_na* path, const Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

const Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  ᮝᮜᮤ locale;
}

UnicodeString&
ICUDataTable::get(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                  UnicodeString &result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  ᮚᮦᮔ᮪ (U_SUCCESS(status) && len > 0) {
    ᮝᮜᮤ result.setTo(s, len);
  }
  ᮝᮜᮤ result.setTo(UnicodeString(itemKey, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(const var_na* tableKey, const var_na* subTableKey, const var_na* itemKey,
                            UnicodeString& result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
    ᮝᮜᮤ result.setTo(s, len);
  }

  result.setToBogus();
  ᮝᮜᮤ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ᮚᮦᮔ᮪ 0  // currently unused

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
  ᮝᮜᮤ Locale::getRoot();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() const {
  ᮝᮜᮤ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const var_na* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(const var_na* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(const var_na* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  const var_na* name = uscript_getName(scriptCode);
  ᮚᮦᮔ᮪ (name) {
    ᮝᮜᮤ result = UnicodeString(name, -1, US_INV);
  }
  ᮝᮜᮤ result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(const var_na* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(const var_na* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyDisplayName(const var_na* key,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(key, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyValueDisplayName(const var_na* /* key */,
                                               const var_na* value,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  ᮝᮜᮤ result = UnicodeString(value, -1, US_INV);
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
#ᮚᮦᮔ᮪ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ᮜᮤᮚ
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
    ᮞᮨᮓᮨᮜ (length-- > 0) {
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
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
        for (puur_naa_nka i = 0; contexts.getKeyAndValue(i, key, value); ++i) {

            CapContextUsage usageEnum;
            ᮚᮦᮔ᮪ (uprv_strcmp(key, "key") == 0) {
                usageEnum = kCapContextUsageKey;
            } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, "keyValue") == 0) {
                usageEnum = kCapContextUsageKeyValue;
            } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ᮜᮤᮚ {
                continue;
            }

            int32_t len = 0;
            const int32_t* intVector = value.getIntVector(len, errorCode);
            ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
            ᮚᮦᮔ᮪ (len < 2) { continue; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            ᮚᮦᮔ᮪ (titlecaseInt == 0) { continue; }

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
    ᮚᮦᮔ᮪ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    ᮚᮦᮔ᮪ (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    ᮚᮦᮔ᮪ (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ᮜᮤᮚ {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    ᮚᮦᮔ᮪ (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    keyTypeFormat.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, parimaa_na(fCapitalization));
#ᮚᮦᮔ᮪ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    bool needBrkIter = false;
    ᮚᮦᮔ᮪ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        ᮚᮦᮔ᮪ (U_FAILURE(status)) { ᮝᮜᮤ; }
        CapitalizationContextSink sink(*this);
        ures_getAllItemsWithFallback(resource.getAlias(), "contextTransforms", sink, status);
        ᮚᮦᮔ᮪ (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (U_FAILURE(status)) {
            ᮝᮜᮤ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ᮚᮦᮔ᮪ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        ᮚᮦᮔ᮪ (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#ᮚᮦᮔ᮪ !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

const Locale&
LocaleDisplayNamesImpl::getLocale() const U_LIFETIME_BOUND {
    ᮝᮜᮤ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() const {
    ᮝᮜᮤ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) const {
    switch (type) {
        pak_sa UDISPCTX_TYPE_DIALECT_HANDLING:
            ᮝᮜᮤ static_cast<UDisplayContext>(dialectHandling);
        pak_sa UDISPCTX_TYPE_CAPITALIZATION:
            ᮝᮜᮤ capitalizationContext;
        pak_sa UDISPCTX_TYPE_DISPLAY_LENGTH:
            ᮝᮜᮤ nameLength;
        pak_sa UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            ᮝᮜᮤ substitute;
        default:
            bha_nga;
    }
    ᮝᮜᮤ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) const {
#ᮚᮦᮔ᮪ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ᮚᮦᮔ᮪ ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        static UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    ᮝᮜᮤ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  ᮚᮦᮔ᮪ (loc.isBogus()) {
    result.setToBogus();
    ᮝᮜᮤ result;
  }
  UnicodeString resultName;

  const var_na* lang = loc.getLanguage();
  ᮚᮦᮔ᮪ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  const var_na* script = loc.getScript();
  const var_na* country = loc.getCountry();
  const var_na* variant = loc.getVariant();

  bool hasScript = uprv_strlen(script) > 0;
  bool hasCountry = uprv_strlen(country) > 0;
  bool hasVariant = uprv_strlen(variant) > 0;

  ᮚᮦᮔ᮪ (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    do { // loop construct is so we can break early out of search
      ᮚᮦᮔ᮪ (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ᮚᮦᮔ᮪ (!resultName.isBogus()) {
            hasScript = false;
            hasCountry = false;
            bha_nga;
          }
        }
      }
      ᮚᮦᮔ᮪ (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ᮚᮦᮔ᮪ (!resultName.isBogus()) {
            hasScript = false;
            bha_nga;
          }
        }
      }
      ᮚᮦᮔ᮪ (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          ᮚᮦᮔ᮪ (!resultName.isBogus()) {
            hasCountry = false;
            bha_nga;
          }
        }
      }
    } ᮞᮨᮓᮨᮜ (false);
  }
  ᮚᮦᮔ᮪ (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    ᮚᮦᮔ᮪ (resultName.isBogus()) {
      result.setToBogus();
      ᮝᮜᮤ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  ᮚᮦᮔ᮪ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    ᮚᮦᮔ᮪ (script_str.isBogus()) {
      result.setToBogus();
      ᮝᮜᮤ result;
    }
    resultRemainder.append(script_str);
  }
  ᮚᮦᮔ᮪ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    ᮚᮦᮔ᮪ (region_str.isBogus()) {
      result.setToBogus();
      ᮝᮜᮤ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  ᮚᮦᮔ᮪ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    ᮚᮦᮔ᮪ (variant_str.isBogus()) {
      result.setToBogus();
      ᮝᮜᮤ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  ᮚᮦᮔ᮪ (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    const var_na* key;
    ᮞᮨᮓᮨᮜ ((key = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.getKeywordValue<CharString>(key, status);
        ᮚᮦᮔ᮪ (U_FAILURE(status)) {
            ᮝᮜᮤ result;
      }
      keyDisplayName(key, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      keyValueDisplayName(key, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ᮚᮦᮔ᮪ (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } ᮜᮤᮚ ᮚᮦᮔ᮪ (temp != UnicodeString(key, -1, US_INV)) {
        UnicodeString temp3;
        keyTypeFormat.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } ᮜᮤᮚ {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ᮚᮦᮔ᮪ (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    ᮝᮜᮤ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  ᮝᮜᮤ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, const UnicodeString& src) const {
    ᮚᮦᮔ᮪ (buffer.isEmpty()) {
        buffer.setTo(src);
    } ᮜᮤᮚ {
        const UnicodeString *values[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(values, 2, buffer, nullptr, 0, status);
    }
    ᮝᮜᮤ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const var_na* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(const var_na* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, bool substitute) const {
    ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        ᮚᮦᮔ᮪ (!result.isBogus()) {
            ᮝᮜᮤ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    ᮚᮦᮔ᮪ (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        const var_na* canonLocId = canonLocale.getName();
        ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ᮚᮦᮔ᮪ (!result.isBogus()) {
                ᮝᮜᮤ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ᮚᮦᮔ᮪ (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    ᮝᮜᮤ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(const var_na* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    ᮚᮦᮔ᮪ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        ᮝᮜᮤ result = UnicodeString(lang, -1, US_INV);
    }
    ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        ᮚᮦᮔ᮪ (!result.isBogus()) {
            ᮝᮜᮤ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    ᮚᮦᮔ᮪ (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        const var_na* canonLocId = canonLocale.getName();
        ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            ᮚᮦᮔ᮪ (!result.isBogus()) {
                ᮝᮜᮤ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    ᮚᮦᮔ᮪ (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    ᮝᮜᮤ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const var_na* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        ᮚᮦᮔ᮪ (!result.isBogus()) {
            ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    ᮚᮦᮔ᮪ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } ᮜᮤᮚ {
        langData.getNoFallback("Scripts", script, result);
    }
    ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const var_na* script,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ scriptDisplayName(script, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ scriptDisplayName(uscript_getName(scriptCode), result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const var_na* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        ᮚᮦᮔ᮪ (!result.isBogus()) {
            ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    ᮚᮦᮔ᮪ (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } ᮜᮤᮚ {
        regionData.getNoFallback("Countries", region, result);
    }
    ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const var_na* region,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ regionDisplayName(region, result, false);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const var_na* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           bool skipAdjust) const {
    // don't have a resource for short variant names
    ᮚᮦᮔ᮪ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } ᮜᮤᮚ {
        langData.getNoFallback("Variants", variant, result);
    }
    ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const var_na* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ variantDisplayName(variant, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const var_na* key,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const {
    // don't have a resource for short key names
    ᮚᮦᮔ᮪ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", key, result);
    } ᮜᮤᮚ {
        langData.getNoFallback("Keys", key, result);
    }
    ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKey, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const var_na* key,
                                       UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ keyDisplayName(key, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const var_na* key,
                                            const var_na* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            bool skipAdjust) const {
    ᮚᮦᮔ᮪ (uprv_strcmp(key, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustrValue(value, -1, US_INV);
        int32_t len;
        const char16_t *currencyName = ucurr_getName(ustrValue.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        ᮚᮦᮔ᮪ (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustrValue;
            ᮝᮜᮤ result;
        }
        result.setTo(currencyName, len);
        ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
    }

    ᮚᮦᮔ᮪ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", key, value, result);
        ᮚᮦᮔ᮪ (!result.isBogus()) {
            ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
        }
    }
    ᮚᮦᮔ᮪ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", key, value, result);
    } ᮜᮤᮚ {
        langData.getNoFallback("Types", key, value, result);
    }
    ᮝᮜᮤ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const var_na* key,
                                            const var_na* value,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    ᮝᮜᮤ keyValueDisplayName(key, value, result, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDialectHandling dialectHandling) {
    ᮝᮜᮤ new LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ᮚᮦᮔ᮪ (contexts == nullptr) {
        length = 0;
    }
    ᮝᮜᮤ new LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(const var_na * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ nullptr;
  }
  ᮚᮦᮔ᮪ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ᮝᮜᮤ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(const var_na * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ nullptr;
  }
  ᮚᮦᮔ᮪ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  ᮝᮜᮤ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI void U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI const var_na* U_EXPORT2
uldn_getLocale(const ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  ᮚᮦᮔ᮪ (ldn) {
    ᮝᮜᮤ ((const LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  ᮝᮜᮤ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(const ULocaleDisplayNames *ldn) {
  ᮚᮦᮔ᮪ (ldn) {
    ᮝᮜᮤ ((const LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  ᮝᮜᮤ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(const ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ (UDisplayContext)0;
  }
  ᮝᮜᮤ ((const LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  ᮚᮦᮔ᮪ (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(const ULocaleDisplayNames *ldn,
                         const var_na *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(const ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  ᮝᮜᮤ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(const ULocaleDisplayNames *ldn,
                       const var_na *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(const ULocaleDisplayNames *ldn,
                        const var_na *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyDisplayName(const ULocaleDisplayNames *ldn,
                    const var_na *key,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || key == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyDisplayName(key, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyValueDisplayName(const ULocaleDisplayNames *ldn,
                         const var_na *key,
                         const var_na *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) {
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪ (ldn == nullptr || key == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    ᮝᮜᮤ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyValueDisplayName(key, value, temp);
  ᮝᮜᮤ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
