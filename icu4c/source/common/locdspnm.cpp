// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

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

    UnicodeString& get(const char* table_सुसि, const char* item_सुसि,
                       UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& get(const char* table_सुसि, const char* sub_Table_सुसि, const char* item_सुसि,
                       UnicodeString& result U_LIFETIME_BOUND) const;

    UnicodeString& getNoFallback(const char* table_सुसि, const char* item_सुसि,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
    UnicodeString& getNoFallback(const char* table_सुसि, const char* sub_Table_सुसि, const char* item_सुसि,
                                 UnicodeString& result U_LIFETIME_BOUND) const;
};

inline UnicodeString& ICUDataTable::get(const char* table_सुसि, const char* item_सुसि,
                                        UnicodeString& result U_LIFETIME_BOUND) const {
    return get(table_सुसि, nullptr, item_सुसि, result);
}

inline UnicodeString& ICUDataTable::getNoFallback(const char* table_सुसि, const char* item_सुसि,
                                                  UnicodeString& result U_LIFETIME_BOUND) const {
    return getNoFallback(table_सुसि, nullptr, item_सुसि, result);
}

ICUDataTable::ICUDataTable(const char* path, const Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

const Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  return locale;
}

UnicodeString&
ICUDataTable::get(const char* table_सुसि, const char* sub_Table_सुसि, const char* item_सुसि,
                  UnicodeString &result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_सुसि, sub_Table_सुसि, item_सुसि,
                                                   &len, &status);
  if (U_SUCCESS(status) && len > 0) {
    return result.setTo(s, len);
  }
  return result.setTo(UnicodeString(item_सुसि, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(const char* table_सुसि, const char* sub_Table_सुसि, const char* item_सुसि,
                            UnicodeString& result U_LIFETIME_BOUND) const {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  const char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_सुसि, sub_Table_सुसि, item_सुसि,
                                                   &len, &status);
  if (U_SUCCESS(status)) {
    return result.setTo(s, len);
  }

  result.setToBogus();
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0  // currently unused

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
  virtual UnicodeString& सुसि_Display_Name(const char* सुसि,
                                        UnicodeString& result U_LIFETIME_BOUND) const;
  virtual UnicodeString& सुसि_हालागा_Display_Name(const char* सुसि,
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
  return Locale::get_उगात();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() const {
  return dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(const char* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(const char* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(const char* script,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  const char* name = uscript_getName(scriptCode);
  if (name) {
    return result = UnicodeString(name, -1, US_INV);
  }
  return result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(const char* region,
                                             UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(const char* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::सुसि_Display_Name(const char* सुसि,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(सुसि, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::सुसि_हालागा_Display_Name(const char* /*सुसि*/,
                                               const char* value,
                                               UnicodeString& result U_LIFETIME_BOUND) const {
  return result = UnicodeString(value, -1, US_INV);
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
    SimpleFormatter सुसि_Type_Format;
    UDisplayContext capitalizationContext;
#if !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#else
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
        k_Cap_Context_Usage_सुसि,
        k_Cap_Context_Usage_सुसि_हालागा,
        k_Cap_Context_Usage_बिलांग
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     bool fCapitalization[k_Cap_Context_Usage_बिलांग];

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
    virtual UnicodeString& सुसि_Display_Name(const char* सुसि,
                                          UnicodeString& result U_LIFETIME_BOUND) const override;
    virtual UnicodeString&
    सुसि_हालागा_Display_Name(const char* सुसि, const char* value,
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
    UnicodeString& सुसि_Display_Name(const char* सुसि, UnicodeString& result U_LIFETIME_BOUND,
                                  bool skipAdjust) const;
    UnicodeString& सुसि_हालागा_Display_Name(const char* सुसि, const char* value,
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
    while (length-- > 0) {
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

    virtual void put(const char *सुसि, Resource_हालागा &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        if (U_FAILURE(errorCode)) { return; }
        for (int i = 0; contexts.get_सुसि_And_हालागा(i, सुसि, value); ++i) {

            CapContextUsage usageEnum;
            if (uprv_strcmp(सुसि, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_सुसि;
            } else if (uprv_strcmp(सुसि, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_सुसि_हालागा;
            } else if (uprv_strcmp(सुसि, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } else if (uprv_strcmp(सुसि, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } else if (uprv_strcmp(सुसि, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } else if (uprv_strcmp(सुसि, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } else {
                continue;
            }

            int32_t len = 0;
            const int32_t* intVector = value.getIntVector(len, errorCode);
            if (U_FAILURE(errorCode)) { return; }
            if (len < 2) { continue; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            if (titlecaseInt == 0) { continue; }

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
    nonConstThis->locale = langData.getLocale() == Locale::get_उगात()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    if (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    if (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    if (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } else {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    if (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    सुसि_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, sizeof(fCapitalization));
#if !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    bool needBrkIter = false;
    if (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        if (U_FAILURE(status)) { return; }
        CapitalizationContextSink sink(*this);
        ures_get_All_मगा_बागाय_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        if (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } else if (U_FAILURE(status)) {
            return;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    if (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        if (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#if !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

const Locale&
LocaleDisplayNamesImpl::getLocale() const U_LIFETIME_BOUND {
    return locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() const {
    return dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) const {
    switch (type) {
        case UDISPCTX_TYPE_DIALECT_HANDLING:
            return static_cast<UDisplayContext>(dialectHandling);
        case UDISPCTX_TYPE_CAPITALIZATION:
            return capitalizationContext;
        case UDISPCTX_TYPE_DISPLAY_LENGTH:
            return nameLength;
        case UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            return substitute;
        default:
            break;
    }
    return static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) const {
#if !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    if ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        static UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    return result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
  if (loc.isBogus()) {
    result.setToBogus();
    return result;
  }
  UnicodeString resultName;

  const char* lang = loc.getLanguage();
  if (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  const char* script = loc.getScript();
  const char* country = loc.getCountry();
  const char* variant = loc.getVariant();

  bool hasScript = uprv_strlen(script) > 0;
  bool hasCountry = uprv_strlen(country) > 0;
  bool hasVariant = uprv_strlen(variant) > 0;

  if (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    do { // loop construct is so we can break early out of search
      if (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        if (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          if (!resultName.isBogus()) {
            hasScript = false;
            hasCountry = false;
            break;
          }
        }
      }
      if (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        if (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          if (!resultName.isBogus()) {
            hasScript = false;
            break;
          }
        }
      }
      if (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        if (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, false);
          if (!resultName.isBogus()) {
            hasCountry = false;
            break;
          }
        }
      }
    } while (false);
  }
  if (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    if (resultName.isBogus()) {
      result.setToBogus();
      return result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  if (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    if (script_str.isBogus()) {
      result.setToBogus();
      return result;
    }
    resultRemainder.append(script_str);
  }
  if (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    if (region_str.isBogus()) {
      result.setToBogus();
      return result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  if (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    if (variant_str.isBogus()) {
      result.setToBogus();
      return result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  if (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    const char* सुसि;
    while ((सुसि = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_हालागा<CharString>(सुसि, status);
        if (U_FAILURE(status)) {
            return result;
      }
      सुसि_Display_Name(सुसि, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      सुसि_हालागा_Display_Name(सुसि, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      if (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } else if (temp != UnicodeString(सुसि, -1, US_INV)) {
        UnicodeString temp3;
        सुसि_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } else {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  if (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    return adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  return adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, const UnicodeString& src) const {
    if (buffer.isEmpty()) {
        buffer.setTo(src);
    } else {
        const UnicodeString *मगा_हालागा[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मगा_हालागा, 2, buffer, nullptr, 0, status);
    }
    return buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(const char* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    return localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(const char* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, bool substitute) const {
    if (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        if (!result.isBogus()) {
            return result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    if (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        const char* canonLocId = canonLocale.getName();
        if (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            if (!result.isBogus()) {
                return result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    if (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    return result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(const char* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    if (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        return result = UnicodeString(lang, -1, US_INV);
    }
    if (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        if (!result.isBogus()) {
            return adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    if (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        const char* canonLocId = canonLocale.getName();
        if (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            if (!result.isBogus()) {
                return adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    if (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    return adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const char* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    if (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        if (!result.isBogus()) {
            return skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    if (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } else {
        langData.getNoFallback("Scripts", script, result);
    }
    return skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(const char* script,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    return scriptDisplayName(script, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    return scriptDisplayName(uscript_getName(scriptCode), result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const char* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          bool skipAdjust) const {
    if (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        if (!result.isBogus()) {
            return skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    if (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } else {
        regionData.getNoFallback("Countries", region, result);
    }
    return skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(const char* region,
                                          UnicodeString& result U_LIFETIME_BOUND) const {
    return regionDisplayName(region, result, false);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const char* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           bool skipAdjust) const {
    // don't have a resource for short variant names
    if (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } else {
        langData.getNoFallback("Variants", variant, result);
    }
    return skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(const char* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) const {
    return variantDisplayName(variant, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::सुसि_Display_Name(const char* सुसि,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       bool skipAdjust) const {
    // don't have a resource for short key names
    if (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", सुसि, result);
    } else {
        langData.getNoFallback("Keys", सुसि, result);
    }
    return skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_सुसि, result);
}

UnicodeString&
LocaleDisplayNamesImpl::सुसि_Display_Name(const char* सुसि,
                                       UnicodeString& result U_LIFETIME_BOUND) const {
    return सुसि_Display_Name(सुसि, result, false);
}

UnicodeString&
LocaleDisplayNamesImpl::सुसि_हालागा_Display_Name(const char* सुसि,
                                            const char* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            bool skipAdjust) const {
    if (uprv_strcmp(सुसि, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_हालागा(value, -1, US_INV);
        int32_t len;
        const char16_t *currencyName = ucurr_getName(ustr_हालागा.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        if (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_हालागा;
            return result;
        }
        result.setTo(currencyName, len);
        return skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_सुसि_हालागा, result);
    }

    if (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", सुसि, value, result);
        if (!result.isBogus()) {
            return skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_सुसि_हालागा, result);
        }
    }
    if (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", सुसि, value, result);
    } else {
        langData.getNoFallback("Types", सुसि, value, result);
    }
    return skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_सुसि_हालागा, result);
}

UnicodeString&
LocaleDisplayNamesImpl::सुसि_हालागा_Display_Name(const char* सुसि,
                                            const char* value,
                                            UnicodeString& result U_LIFETIME_BOUND) const {
    return सुसि_हालागा_Display_Name(सुसि, value, result, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDialectHandling dialectHandling) {
    return new LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(const Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    if (contexts == nullptr) {
        length = 0;
    }
    return new LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(const char * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return nullptr;
  }
  if (locale == nullptr) {
    locale = uloc_getDefault();
  }
  return (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(const char * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return nullptr;
  }
  if (locale == nullptr) {
    locale = uloc_getDefault();
  }
  return (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI void U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI const char* U_EXPORT2
uldn_getLocale(const ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  if (ldn) {
    return ((const LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  return nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(const ULocaleDisplayNames *ldn) {
  if (ldn) {
    return ((const LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  return ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(const ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return (UDisplayContext)0;
  }
  return ((const LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(const ULocaleDisplayNames *ldn,
                       const char *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  if (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(const ULocaleDisplayNames *ldn,
                         const char *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(const ULocaleDisplayNames *ldn,
                       const char *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(const ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  return uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(const ULocaleDisplayNames *ldn,
                       const char *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(const ULocaleDisplayNames *ldn,
                        const char *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_सुसि_Display_Name(const ULocaleDisplayNames *ldn,
                    const char *सुसि,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || सुसि == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->सुसि_Display_Name(सुसि, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_सुसि_हालागा_Display_Name(const ULocaleDisplayNames *ldn,
                         const char *सुसि,
                         const char *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  if (U_FAILURE(*pErrorCode)) {
    return 0;
  }
  if (ldn == nullptr || सुसि == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    return 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((const LocaleDisplayNames *)ldn)->सुसि_हालागा_Display_Name(सुसि, value, temp);
  return temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
