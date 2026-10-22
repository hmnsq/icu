// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"

#если !UCONFIG_NO_FORMATTING

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
    const char* const path;
    Locale locale;

public:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(const char* path, const Locale& locale);
    ~ICUDataTable() = default;

    const Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(const char* tableKey, const char* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& get(const char* tableKey, const char* subTableKey, const char* itemKey,
                       UnicodeString& result U_LIFETIME_BOUND) const;

    UnicodeString& getNoFallback(const char* tableKey, const char* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& getNoFallback(const char* tableKey, const char* subTableKey, const char* itemKey,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
};

inline UnicodeString& ICUDataTable::get(const char* tableKey, const char* itemKey,
                                        UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть get(tableKey, nullptr, itemKey, result);
}

inline UnicodeString& ICUDataTable::getNoFallback(const char* tableKey, const char* itemKey,
                                                  UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть getNoFallback(tableKey, nullptr, itemKey, result);
}

ICUDataTable::ICUDataTable(const char* path, const Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

const Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  вернуть locale;
}

UnicodeString&
ICUDataTable::get(const char* tableKey, const char* subTableKey, const char* itemKey,
                  UnicodeString &result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  если (U_SUCCESS(status) && len > 0) {
    вернуть result.setTo(s, len);
  }
  вернуть result.setTo(UnicodeString(itemKey, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(const char* tableKey, const char* subTableKey, const char* itemKey,
                            UnicodeString& result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   tableKey, subTableKey, itemKey,
                                                   &len, &status);
  если (U_SUCCESS(status)) {
    вернуть result.setTo(s, len);
  }

  result.setToBogus();
  вернуть result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#если 0  // currently unused

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
  virtual UnicodeString& localeDisplayName(const char* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& languageDisplayName(const char* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& scriptDisplayName(const char* script,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& regionDisplayName(const char* region,
                                           UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& variantDisplayName(const char* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& keyDisplayName(const char* key,
                                        UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& keyValueDisplayName(const char* key,
                                             const char* value,
                                             UnicodeString& result U_LIFETIME_BOUND) const;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

const Locale&
DefaultLocaleDisplayNames::getLocale() const U_LIFETIME_BOUND {
  вернуть Locale::getRoot();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() const {
  вернуть dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const char* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(const char* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(const char* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  const char* name = uscript_getName(scriptCode);
  если (name) {
    вернуть result = UnicodeString(name, -1, US_INV);
  }
  вернуть result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(const char* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(const char* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyDisplayName(const char* key,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(key, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::keyValueDisplayName(const char* /* key */,
                                               const char* value,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  вернуть result = UnicodeString(value, -1, US_INV);
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
#если !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#иначе
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    enum CapContextUsage {
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
    virtual UnicodeString& localeDisplayName(const char* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    languageDisplayName(const char* lang, UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& scriptDisplayName(const char* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& regionDisplayName(const char* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    variantDisplayName(const char* variant, UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString& keyDisplayName(const char* key,
                                          UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    keyValueDisplayName(const char* key, const char* value,
                        UnicodeString& result U_LIFETIME_BOUND) const override;

private:
    UnicodeString& localeIdName(const char* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                bool substitute) const;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 const UnicodeString& src) const;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& scriptDisplayName(const char* script, UnicodeString& result U_LIFETIME_BOUND,
                                     bool skipAdjust) const;
    UnicodeString& regionDisplayName(const char* region, UnicodeString& result U_LIFETIME_BOUND,
                                     bool skipAdjust) const;
    UnicodeString& variantDisplayName(const char* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      bool skipAdjust) const;
    UnicodeString& keyDisplayName(const char* key, UnicodeString& result U_LIFETIME_BOUND,
                                  bool skipAdjust) const;
    UnicodeString& keyValueDisplayName(const char* key, const char* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const;
    void initialize();

    struct CapitalizationContextSink;
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
    пока (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        switch (selector) {
            case UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                break;
            case UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                break;
            case UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                break;
            case UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                break;
            default:
                break;
        }
    }
    initialize();
}

struct LocaleDisplayNamesImpl::CapitalizationContextSink : public ResourceSink {
    bool hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(false), parent(_parent) {}
    virtual ~CapitalizationContextSink();

    virtual void put(const char *key, ResourceValue &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        если (U_FAILURE(errorCode)) { вернуть; }
        для (целое i = 0; contexts.getKeyAndValue(i, key, value); ++i) {

            CapContextUsage usageEnum;
            если (uprv_strcmp(key, "key") == 0) {
                usageEnum = kCapContextUsageKey;
            } иначе если (uprv_strcmp(key, "keyValue") == 0) {
                usageEnum = kCapContextUsageKeyValue;
            } иначе если (uprv_strcmp(key, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } иначе если (uprv_strcmp(key, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } иначе если (uprv_strcmp(key, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } иначе если (uprv_strcmp(key, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } иначе {
                continue;
            }

            int32_t len = 0;
            const int32_t* intVector = value.getIntVector(len, errorCode);
            если (U_FAILURE(errorCode)) { вернуть; }
            если (len < 2) { continue; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            если (titlecaseInt == 0) { continue; }

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
    если (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    если (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    если (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } иначе {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    если (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    keyTypeFormat.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, sizeof(fCapitalization));
#если !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    bool needBrkIter = false;
    если (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        если (U_FAILURE(status)) { вернуть; }
        CapitalizationContextSink sink(*this);
        ures_getAllItemsWithFallback(resource.getAlias(), "contextTransforms", sink, status);
        если (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } иначе если (U_FAILURE(status)) {
            вернуть;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    если (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        если (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#если !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

const Locale&
LocaleDisplayNamesImpl::getLocale() const U_LIFETIME_BOUND {
    вернуть locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() const {
    вернуть dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) const {
    switch (type) {
        case UDISPCTX_TYPE_DIALECT_HANDLING:
            вернуть static_cast<UDisplayContext>(dialectHandling);
        case UDISPCTX_TYPE_CAPITALIZATION:
            вернуть capitalizationContext;
        case UDISPCTX_TYPE_DISPLAY_LENGTH:
            вернуть nameLength;
        case UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            вернуть substitute;
        default:
            break;
    }
    вернуть static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) const {
#если !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    если ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        static UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    вернуть result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  если (loc.isBogus()) {
    result.setToBogus();
    вернуть result;
  }
  UnicodeString resultName;

  const char* lang = loc.getLanguage();
  если (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  const char* script = loc.getScript();
  const char* country = loc.getCountry();
  const char* variant = loc.getVariant();

  bool hasScript = uprv_strlen(script) > 0;
  bool hasCountry = uprv_strlen(country) > 0;
  bool hasVariant = uprv_strlen(variant) > 0;

  если (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    do { // loop construct is so we can break early out of search
      если (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        если (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          если (!resultName.isBogus()) {
            hasScript = false;
            hasCountry = false;
            break;
          }
        }
      }
      если (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        если (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          если (!resultName.isBogus()) {
            hasScript = false;
            break;
          }
        }
      }
      если (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        если (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          если (!resultName.isBogus()) {
            hasCountry = false;
            break;
          }
        }
      }
    } пока (false);
  }
  если (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    если (resultName.isBogus()) {
      result.setToBogus();
      вернуть result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  если (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    если (script_str.isBogus()) {
      result.setToBogus();
      вернуть result;
    }
    resultRemainder.append(script_str);
  }
  если (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    если (region_str.isBogus()) {
      result.setToBogus();
      вернуть result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  если (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    если (variant_str.isBogus()) {
      result.setToBogus();
      вернуть result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  если (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    const char* key;
    пока ((key = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.getKeywordValue<CharString>(key, status);
        если (U_FAILURE(status)) {
            вернуть result;
      }
      keyDisplayName(key, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      keyValueDisplayName(key, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      если (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } иначе если (temp != UnicodeString(key, -1, US_INV)) {
        UnicodeString temp3;
        keyTypeFormat.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } иначе {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  если (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    вернуть adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  вернуть adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, const UnicodeString& src) const {
    если (buffer.isEmpty()) {
        buffer.setTo(src);
    } иначе {
        const UnicodeString *values[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(values, 2, buffer, nullptr, 0, status);
    }
    вернуть buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const char* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(const char* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, bool substitute) const {
    если (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        если (!result.isBogus()) {
            вернуть result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    если (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        const char* canonLocId = canonLocale.getName();
        если (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            если (!result.isBogus()) {
                вернуть result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    если (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    вернуть result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(const char* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    если (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        вернуть result = UnicodeString(lang, -1, US_INV);
    }
    если (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        если (!result.isBogus()) {
            вернуть adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    если (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        const char* canonLocId = canonLocale.getName();
        если (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            если (!result.isBogus()) {
                вернуть adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    если (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    вернуть adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const char* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    если (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        если (!result.isBogus()) {
            вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    если (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } иначе {
        langData.getNoFallback("Scripts", script, result);
    }
    вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const char* script,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть scriptDisplayName(script, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть scriptDisplayName(uscript_getName(scriptCode), result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const char* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    если (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        если (!result.isBogus()) {
            вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    если (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } иначе {
        regionData.getNoFallback("Countries", region, result);
    }
    вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const char* region,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть regionDisplayName(region, result, false);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const char* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           bool skipAdjust) const {
    // don't have a resource for short variant names
    если (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } иначе {
        langData.getNoFallback("Variants", variant, result);
    }
    вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const char* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть variantDisplayName(variant, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const char* key,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const {
    // don't have a resource for short key names
    если (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", key, result);
    } иначе {
        langData.getNoFallback("Keys", key, result);
    }
    вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKey, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyDisplayName(const char* key,
                                       UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть keyDisplayName(key, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const char* key,
                                            const char* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            bool skipAdjust) const {
    если (uprv_strcmp(key, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustrValue(value, -1, US_INV);
        int32_t len;
        const char16_t *currencyName = ucurr_getName(ustrValue.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        если (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustrValue;
            вернуть result;
        }
        result.setTo(currencyName, len);
        вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
    }

    если (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", key, value, result);
        если (!result.isBogus()) {
            вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
        }
    }
    если (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", key, value, result);
    } иначе {
        langData.getNoFallback("Types", key, value, result);
    }
    вернуть skipAdjust? result: adjustForUsageAndContext(kCapContextUsageKeyValue, result);
}

UnicodeString&
LocaleDisplayNamesImpl::keyValueDisplayName(const char* key,
                                            const char* value,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    вернуть keyValueDisplayName(key, value, result, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDialectHandling dialectHandling) {
    вернуть new LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    если (contexts == nullptr) {
        length = 0;
    }
    вернуть new LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(const char * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть nullptr;
  }
  если (locale == nullptr) {
    locale = uloc_getDefault();
  }
  вернуть (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(const char * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть nullptr;
  }
  если (locale == nullptr) {
    locale = uloc_getDefault();
  }
  вернуть (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI void U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI const char* U_EXPORT2
uldn_getLocale(const ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  если (ldn) {
    вернуть ((const LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  вернуть nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(const ULocaleDisplayNames *ldn) {
  если (ldn) {
    вернуть ((const LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  вернуть ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(const ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть (UDisplayContext)0;
  }
  вернуть ((const LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(const ULocaleDisplayNames *ldn,
                       const char *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  если (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(const ULocaleDisplayNames *ldn,
                         const char *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(const ULocaleDisplayNames *ldn,
                       const char *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(const ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  вернуть uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(const ULocaleDisplayNames *ldn,
                       const char *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(const ULocaleDisplayNames *ldn,
                        const char *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyDisplayName(const ULocaleDisplayNames *ldn,
                    const char *key,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || key == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyDisplayName(key, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_keyValueDisplayName(const ULocaleDisplayNames *ldn,
                         const char *key,
                         const char *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  если (U_FAILURE(*pErrorCode)) {
    вернуть 0;
  }
  если (ldn == nullptr || key == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    вернуть 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->keyValueDisplayName(key, value, temp);
  вернуть temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
