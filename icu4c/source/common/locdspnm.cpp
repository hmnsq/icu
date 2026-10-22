// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

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

    UnicodeString& get(ثابت حرف* table_مفتاح, ثابت حرف* item_مفتاح,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* table_مفتاح, ثابت حرف* sub_Table_مفتاح, ثابت حرف* item_مفتاح,
                       UnicodeString& result U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* table_مفتاح, ثابت حرف* item_مفتاح,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* table_مفتاح, ثابت حرف* sub_Table_مفتاح, ثابت حرف* item_مفتاح,
                                 UnicodeString& result U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUDataTable::get(ثابت حرف* table_مفتاح, ثابت حرف* item_مفتاح,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع get(table_مفتاح, nullptr, item_مفتاح, result);
}

مضمن UnicodeString& ICUDataTable::getNoFallback(ثابت حرف* table_مفتاح, ثابت حرف* item_مفتاح,
                                                  UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(table_مفتاح, nullptr, item_مفتاح, result);
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
ICUDataTable::get(ثابت حرف* table_مفتاح, ثابت حرف* sub_Table_مفتاح, ثابت حرف* item_مفتاح,
                  UnicodeString &result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_مفتاح, sub_Table_مفتاح, item_مفتاح,
                                                   &len, &status);
  إذا (U_SUCCESS(status) && len > 0) {
    أرجع result.setTo(s, len);
  }
  أرجع result.setTo(UnicodeString(item_مفتاح, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ثابت حرف* table_مفتاح, ثابت حرف* sub_Table_مفتاح, ثابت حرف* item_مفتاح,
                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_مفتاح, sub_Table_مفتاح, item_مفتاح,
                                                   &len, &status);
  إذا (U_SUCCESS(status)) {
    أرجع result.setTo(s, len);
  }

  result.setToBogus();
  أرجع result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#إذا 0  // currently unused

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
  virtual UnicodeString& مفتاح_Display_Name(ثابت حرف* مفتاح,
                                        UnicodeString& result U_LIFETIME_BOUND) ثابت;
  virtual UnicodeString& مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح,
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
  أرجع Locale::get_جذر();
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
  إذا (name) {
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
DefaultLocaleDisplayNames::مفتاح_Display_Name(ثابت حرف* مفتاح,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت {
  أرجع result = UnicodeString(مفتاح, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::مفتاح_قيمة_Display_Name(ثابت حرف* /*مفتاح*/,
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
    SimpleFormatter مفتاح_Type_Format;
    UDisplayContext capitalizationContext;
#إذا !UCONFIG_NO_BREAK_ITERATION
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
        k_Cap_Context_Usage_مفتاح,
        k_Cap_Context_Usage_مفتاح_قيمة,
        k_Cap_Context_Usage_عدد
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[k_Cap_Context_Usage_عدد];

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
    virtual UnicodeString& مفتاح_Display_Name(ثابت حرف* مفتاح,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت override;
    virtual UnicodeString&
    مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح, ثابت حرف* value,
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
    UnicodeString& مفتاح_Display_Name(ثابت حرف* مفتاح, UnicodeString& result U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح, ثابت حرف* value,
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

    virtual فراغ put(ثابت حرف *مفتاح, Resource_قيمة &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        إذا (U_FAILURE(errorCode)) { أرجع; }
        لكل (صحيح i = 0; contexts.get_مفتاح_And_قيمة(i, مفتاح, value); ++i) {

            CapContextUsage usageEnum;
            إذا (uprv_strcmp(مفتاح, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_مفتاح;
            } وإلا إذا (uprv_strcmp(مفتاح, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_مفتاح_قيمة;
            } وإلا إذا (uprv_strcmp(مفتاح, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وإلا إذا (uprv_strcmp(مفتاح, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وإلا إذا (uprv_strcmp(مفتاح, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وإلا إذا (uprv_strcmp(مفتاح, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وإلا {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* intVector = value.getIntVector(len, errorCode);
            إذا (U_FAILURE(errorCode)) { أرجع; }
            إذا (len < 2) { استمر; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            إذا (titlecaseInt == 0) { استمر; }

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
    nonConstThis->locale = langData.getLocale() == Locale::get_جذر()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    إذا (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    إذا (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    إذا (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
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
    إذا (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    مفتاح_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#إذا !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    إذا (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        إذا (U_FAILURE(status)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_get_All_عناصر_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        إذا (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } وإلا إذا (U_FAILURE(status)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    إذا (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        إذا (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
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
#إذا !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    إذا ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
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
  إذا (loc.isBogus()) {
    result.setToBogus();
    أرجع result;
  }
  UnicodeString resultName;

  ثابت حرف* lang = loc.getLanguage();
  إذا (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ثابت حرف* script = loc.getScript();
  ثابت حرف* country = loc.getCountry();
  ثابت حرف* variant = loc.getVariant();

  منطقي hasScript = uprv_strlen(script) > 0;
  منطقي hasCountry = uprv_strlen(country) > 0;
  منطقي hasVariant = uprv_strlen(variant) > 0;

  إذا (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    افعل { // loop construct is so we can break early out of search
      إذا (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        إذا (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          إذا (!resultName.isBogus()) {
            hasScript = خطأ;
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        إذا (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          إذا (!resultName.isBogus()) {
            hasScript = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        إذا (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, خطأ);
          إذا (!resultName.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  إذا (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    إذا (resultName.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  إذا (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, true);
    إذا (script_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    resultRemainder.append(script_str);
  }
  إذا (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, true);
    إذا (region_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  إذا (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, true);
    إذا (variant_str.isBogus()) {
      result.setToBogus();
      أرجع result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  إذا (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ثابت حرف* مفتاح;
    طالما ((مفتاح = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_قيمة<CharString>(مفتاح, status);
        إذا (U_FAILURE(status)) {
            أرجع result;
      }
      مفتاح_Display_Name(مفتاح, temp, true);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      مفتاح_قيمة_Display_Name(مفتاح, value.data(), temp2, true);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      إذا (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } وإلا إذا (temp != UnicodeString(مفتاح, -1, US_INV)) {
        UnicodeString temp3;
        مفتاح_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } وإلا {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  إذا (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    إذا (buffer.isEmpty()) {
        buffer.setTo(src);
    } وإلا {
        ثابت UnicodeString *قيم[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(قيم, 2, buffer, nullptr, 0, status);
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
    إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        إذا (!result.isBogus()) {
            أرجع result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    إذا (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            إذا (!result.isBogus()) {
                أرجع result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    إذا (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    إذا (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        أرجع result = UnicodeString(lang, -1, US_INV);
    }
    إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        إذا (!result.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    إذا (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            إذا (!result.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    إذا (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        إذا (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
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
    إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        إذا (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
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
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
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
LocaleDisplayNamesImpl::مفتاح_Display_Name(ثابت حرف* مفتاح,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", مفتاح, result);
    } وإلا {
        langData.getNoFallback("Keys", مفتاح, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_مفتاح, result);
}

UnicodeString&
LocaleDisplayNamesImpl::مفتاح_Display_Name(ثابت حرف* مفتاح,
                                       UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع مفتاح_Display_Name(مفتاح, result, خطأ);
}

UnicodeString&
LocaleDisplayNamesImpl::مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    إذا (uprv_strcmp(مفتاح, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_قيمة(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_قيمة.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        إذا (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_قيمة;
            أرجع result;
        }
        result.setTo(currencyName, len);
        أرجع skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_مفتاح_قيمة, result);
    }

    إذا (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", مفتاح, value, result);
        إذا (!result.isBogus()) {
            أرجع skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_مفتاح_قيمة, result);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", مفتاح, value, result);
    } وإلا {
        langData.getNoFallback("Types", مفتاح, value, result);
    }
    أرجع skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_مفتاح_قيمة, result);
}

UnicodeString&
LocaleDisplayNamesImpl::مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح,
                                            ثابت حرف* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ثابت {
    أرجع مفتاح_قيمة_Display_Name(مفتاح, value, result, خطأ);
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
    إذا (contexts == nullptr) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع nullptr;
  }
  إذا (locale == nullptr) {
    locale = uloc_getDefault();
  }
  أرجع (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع nullptr;
  }
  إذا (locale == nullptr) {
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
  إذا (ldn) {
    أرجع ((ثابت LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  أرجع nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocaleDisplayNames *ldn) {
  إذا (ldn) {
    أرجع ((ثابت LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  إذا (U_FAILURE(*pErrorCode)) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  إذا (temp.isBogus()) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
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
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_مفتاح_Display_Name(ثابت ULocaleDisplayNames *ldn,
                    ثابت حرف *مفتاح,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || مفتاح == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->مفتاح_Display_Name(مفتاح, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_مفتاح_قيمة_Display_Name(ثابت ULocaleDisplayNames *ldn,
                         ثابت حرف *مفتاح,
                         ثابت حرف *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  إذا (U_FAILURE(*pErrorCode)) {
    أرجع 0;
  }
  إذا (ldn == nullptr || مفتاح == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    أرجع 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ثابت LocaleDisplayNames *)ldn)->مفتاح_قيمة_Display_Name(مفتاح, value, temp);
  أرجع temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
