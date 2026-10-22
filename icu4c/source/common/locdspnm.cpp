// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/locdspnm.h"
#अन्तर्भाव "unicode/simpleformatter.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/udisplaycontext.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/ucurr.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ureslocs.h"
#अन्तर्भाव "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
वर्ग ICUDataTable {
    स्थिरांक अक्षर* स्थिरांक path;
    Locale locale;

सार्वजनिक:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(स्थिरांक अक्षर* path, स्थिरांक Locale& locale);
    ~ICUDataTable() = पूर्वनिर्धारित;

    स्थिरांक Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                       UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
    UnicodeString& get(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* sub_Table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                       UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;

    UnicodeString& getNoFallback(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                                 UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
    UnicodeString& getNoFallback(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* sub_Table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                                 UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
};

ओळीत UnicodeString& ICUDataTable::get(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                                        UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत get(table_किल्ली, nullptr, item_किल्ली, result);
}

ओळीत UnicodeString& ICUDataTable::getNoFallback(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                                                  UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत getNoFallback(table_किल्ली, nullptr, item_किल्ली, result);
}

ICUDataTable::ICUDataTable(स्थिरांक अक्षर* path, स्थिरांक Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

स्थिरांक Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  परत locale;
}

UnicodeString&
ICUDataTable::get(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* sub_Table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                  UnicodeString &result U_LIFETIME_BOUND) स्थिरांक {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  स्थिरांक char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_किल्ली, sub_Table_किल्ली, item_किल्ली,
                                                   &len, &status);
  जर (U_SUCCESS(status) && len > 0) {
    परत result.setTo(s, len);
  }
  परत result.setTo(UnicodeString(item_किल्ली, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(स्थिरांक अक्षर* table_किल्ली, स्थिरांक अक्षर* sub_Table_किल्ली, स्थिरांक अक्षर* item_किल्ली,
                            UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  स्थिरांक char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_किल्ली, sub_Table_किल्ली, item_किल्ली,
                                                   &len, &status);
  जर (U_SUCCESS(status)) {
    परत result.setTo(s, len);
  }

  result.setToBogus();
  परत result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#जर 0  // currently unused

वर्ग DefaultLocaleDisplayNames : सार्वजनिक LocaleDisplayNames {
  UDialectHandling dialectHandling;

सार्वजनिक:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  आभासी ~DefaultLocaleDisplayNames();

  आभासी स्थिरांक Locale& getLocale() स्थिरांक U_LIFETIME_BOUND;
  आभासी UDialectHandling getDialectHandling() स्थिरांक;

  आभासी UnicodeString& localeDisplayName(स्थिरांक Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& localeDisplayName(स्थिरांक अक्षर* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& languageDisplayName(स्थिरांक अक्षर* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& scriptDisplayName(स्थिरांक अक्षर* script,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& regionDisplayName(स्थिरांक अक्षर* region,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& variantDisplayName(स्थिरांक अक्षर* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली,
                                        UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
  आभासी UnicodeString& किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* किल्ली,
                                             स्थिरांक अक्षर* value,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

स्थिरांक Locale&
DefaultLocaleDisplayNames::getLocale() स्थिरांक U_LIFETIME_BOUND {
  परत Locale::get_मूळ();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() स्थिरांक {
  परत dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(स्थिरांक Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(स्थिरांक अक्षर* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(स्थिरांक अक्षर* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(स्थिरांक अक्षर* script,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  स्थिरांक अक्षर* name = uscript_getName(scriptCode);
  जर (name) {
    परत result = UnicodeString(name, -1, US_INV);
  }
  परत result.हटाओ();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(स्थिरांक अक्षर* region,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(स्थिरांक अक्षर* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(किल्ली, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* /*किल्ली*/,
                                               स्थिरांक अक्षर* value,
                                               UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  परत result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

वर्ग LocaleDisplayNamesImpl : सार्वजनिक LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter किल्ली_Type_Format;
    UDisplayContext capitalizationContext;
#जर !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#अन्यथा
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    गणना CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_किल्ली,
        k_Cap_Context_Usage_किल्ली_मूल्य,
        k_Cap_Context_Usage_मोजणी
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     तार्किक fCapitalization[k_Cap_Context_Usage_मोजणी];

सार्वजनिक:
    // constructor
    LocaleDisplayNamesImpl(स्थिरांक Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(स्थिरांक Locale& locale, UDisplayContext *contexts, int32_t length);
    आभासी ~LocaleDisplayNamesImpl();

    आभासी स्थिरांक Locale& getLocale() स्थिरांक U_LIFETIME_BOUND override;
    आभासी UDialectHandling getDialectHandling() स्थिरांक override;
    आभासी UDisplayContext getContext(UDisplayContextType type) स्थिरांक override;

    आभासी UnicodeString& localeDisplayName(स्थिरांक Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString& localeDisplayName(स्थिरांक अक्षर* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString&
    languageDisplayName(स्थिरांक अक्षर* lang, UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString& scriptDisplayName(स्थिरांक अक्षर* script,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString& regionDisplayName(स्थिरांक अक्षर* region,
                                             UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString&
    variantDisplayName(स्थिरांक अक्षर* variant, UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString& किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;
    आभासी UnicodeString&
    किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* किल्ली, स्थिरांक अक्षर* value,
                        UnicodeString& result U_LIFETIME_BOUND) स्थिरांक override;

खाजगी:
    UnicodeString& localeIdName(स्थिरांक अक्षर* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                तार्किक substitute) स्थिरांक;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 स्थिरांक UnicodeString& src) स्थिरांक;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) स्थिरांक;
    UnicodeString& scriptDisplayName(स्थिरांक अक्षर* script, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) स्थिरांक;
    UnicodeString& regionDisplayName(स्थिरांक अक्षर* region, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) स्थिरांक;
    UnicodeString& variantDisplayName(स्थिरांक अक्षर* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      तार्किक skipAdjust) स्थिरांक;
    UnicodeString& किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली, UnicodeString& result U_LIFETIME_BOUND,
                                  तार्किक skipAdjust) स्थिरांक;
    UnicodeString& किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* किल्ली, स्थिरांक अक्षर* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) स्थिरांक;
    शून्य initialize();

    संरचना CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(स्थिरांक Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(स्थिरांक Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    पर्यंत (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        निवड (selector) {
            स्थिती UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                थांबा;
            स्थिती UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                थांबा;
            स्थिती UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                थांबा;
            स्थिती UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                थांबा;
            पूर्वनिर्धारित:
                थांबा;
        }
    }
    initialize();
}

संरचना LocaleDisplayNamesImpl::CapitalizationContextSink : सार्वजनिक ResourceSink {
    तार्किक hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(असत्य), parent(_parent) {}
    आभासी ~CapitalizationContextSink();

    आभासी शून्य put(स्थिरांक अक्षर *किल्ली, Resource_मूल्य &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        साठी (पूर्णांक i = 0; contexts.get_किल्ली_And_मूल्य(i, किल्ली, value); ++i) {

            CapContextUsage usageEnum;
            जर (uprv_strcmp(किल्ली, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_किल्ली;
            } अन्यथा जर (uprv_strcmp(किल्ली, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_किल्ली_मूल्य;
            } अन्यथा जर (uprv_strcmp(किल्ली, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } अन्यथा जर (uprv_strcmp(किल्ली, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } अन्यथा जर (uprv_strcmp(किल्ली, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } अन्यथा जर (uprv_strcmp(किल्ली, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } अन्यथा {
                सुरूठेवा;
            }

            int32_t len = 0;
            स्थिरांक int32_t* intVector = value.getIntVector(len, errorCode);
            जर (U_FAILURE(errorCode)) { परत; }
            जर (len < 2) { सुरूठेवा; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            जर (titlecaseInt == 0) { सुरूठेवा; }

            parent.fCapitalization[usageEnum] = सत्य;
            hasCapitalizationUsage = सत्य;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

शून्य
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = हे;
    nonConstThis->locale = langData.getLocale() == Locale::get_मूळ()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    जर (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    जर (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    जर (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } अन्यथा {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    जर (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    किल्ली_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, आकार(fCapitalization));
#जर !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    तार्किक needBrkIter = असत्य;
    जर (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        जर (U_FAILURE(status)) { परत; }
        CapitalizationContextSink sink(*हे);
        ures_get_All_वस्तुसंच_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        जर (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } अन्यथा जर (U_FAILURE(status)) {
            परत;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    जर (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        जर (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#जर !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

स्थिरांक Locale&
LocaleDisplayNamesImpl::getLocale() स्थिरांक U_LIFETIME_BOUND {
    परत locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() स्थिरांक {
    परत dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) स्थिरांक {
    निवड (type) {
        स्थिती UDISPCTX_TYPE_DIALECT_HANDLING:
            परत static_cast<UDisplayContext>(dialectHandling);
        स्थिती UDISPCTX_TYPE_CAPITALIZATION:
            परत capitalizationContext;
        स्थिती UDISPCTX_TYPE_DISPLAY_LENGTH:
            परत nameLength;
        स्थिती UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            परत substitute;
        पूर्वनिर्धारित:
            थांबा;
    }
    परत static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
#जर !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    जर ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        स्थिर UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    परत result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(स्थिरांक Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
  जर (loc.isBogus()) {
    result.setToBogus();
    परत result;
  }
  UnicodeString resultName;

  स्थिरांक अक्षर* lang = loc.getLanguage();
  जर (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  स्थिरांक अक्षर* script = loc.getScript();
  स्थिरांक अक्षर* country = loc.getCountry();
  स्थिरांक अक्षर* variant = loc.getVariant();

  तार्किक hasScript = uprv_strlen(script) > 0;
  तार्किक hasCountry = uprv_strlen(country) > 0;
  तार्किक hasVariant = uprv_strlen(variant) > 0;

  जर (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    करा { // loop construct is so we can break early out of search
      जर (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        जर (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          जर (!resultName.isBogus()) {
            hasScript = असत्य;
            hasCountry = असत्य;
            थांबा;
          }
        }
      }
      जर (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        जर (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          जर (!resultName.isBogus()) {
            hasScript = असत्य;
            थांबा;
          }
        }
      }
      जर (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        जर (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          जर (!resultName.isBogus()) {
            hasCountry = असत्य;
            थांबा;
          }
        }
      }
    } पर्यंत (असत्य);
  }
  जर (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    जर (resultName.isBogus()) {
      result.setToBogus();
      परत result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  जर (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, सत्य);
    जर (script_str.isBogus()) {
      result.setToBogus();
      परत result;
    }
    resultRemainder.append(script_str);
  }
  जर (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, सत्य);
    जर (region_str.isBogus()) {
      result.setToBogus();
      परत result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  जर (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, सत्य);
    जर (variant_str.isBogus()) {
      result.setToBogus();
      परत result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  जर (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    स्थिरांक अक्षर* किल्ली;
    पर्यंत ((किल्ली = e->next((int32_t*)nullptr, status)) != nullptr) {
        स्वतः value = loc.get_Keyword_मूल्य<CharString>(किल्ली, status);
        जर (U_FAILURE(status)) {
            परत result;
      }
      किल्ली_Display_Name(किल्ली, temp, सत्य);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      किल्ली_मूल्य_Display_Name(किल्ली, value.data(), temp2, सत्य);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      जर (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } अन्यथा जर (temp != UnicodeString(किल्ली, -1, US_INV)) {
        UnicodeString temp3;
        किल्ली_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } अन्यथा {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  जर (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.हटाओ(), status);
    परत adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  परत adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, स्थिरांक UnicodeString& src) स्थिरांक {
    जर (buffer.isEmpty()) {
        buffer.setTo(src);
    } अन्यथा {
        स्थिरांक UnicodeString *मूल्ये[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मूल्ये, 2, buffer, nullptr, 0, status);
    }
    परत buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(स्थिरांक अक्षर* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(स्थिरांक अक्षर* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, तार्किक substitute) स्थिरांक {
    जर (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        जर (!result.isBogus()) {
            परत result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    जर (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        स्थिरांक अक्षर* canonLocId = canonLocale.getName();
        जर (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            जर (!result.isBogus()) {
                परत result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    जर (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    परत result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(स्थिरांक अक्षर* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    जर (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        परत result = UnicodeString(lang, -1, US_INV);
    }
    जर (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        जर (!result.isBogus()) {
            परत adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    जर (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        स्थिरांक अक्षर* canonLocId = canonLocale.getName();
        जर (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            जर (!result.isBogus()) {
                परत adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    जर (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    परत adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(स्थिरांक अक्षर* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) स्थिरांक {
    जर (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        जर (!result.isBogus()) {
            परत skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    जर (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } अन्यथा {
        langData.getNoFallback("Scripts", script, result);
    }
    परत skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(स्थिरांक अक्षर* script,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत scriptDisplayName(script, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत scriptDisplayName(uscript_getName(scriptCode), result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(स्थिरांक अक्षर* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) स्थिरांक {
    जर (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        जर (!result.isBogus()) {
            परत skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    जर (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } अन्यथा {
        regionData.getNoFallback("Countries", region, result);
    }
    परत skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(स्थिरांक अक्षर* region,
                                          UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत regionDisplayName(region, result, असत्य);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(स्थिरांक अक्षर* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           तार्किक skipAdjust) स्थिरांक {
    // don't have a resource for short variant names
    जर (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } अन्यथा {
        langData.getNoFallback("Variants", variant, result);
    }
    परत skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(स्थिरांक अक्षर* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत variantDisplayName(variant, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) स्थिरांक {
    // don't have a resource for short key names
    जर (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", किल्ली, result);
    } अन्यथा {
        langData.getNoFallback("Keys", किल्ली, result);
    }
    परत skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_किल्ली, result);
}

UnicodeString&
LocaleDisplayNamesImpl::किल्ली_Display_Name(स्थिरांक अक्षर* किल्ली,
                                       UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत किल्ली_Display_Name(किल्ली, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* किल्ली,
                                            स्थिरांक अक्षर* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            तार्किक skipAdjust) स्थिरांक {
    जर (uprv_strcmp(किल्ली, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_मूल्य(value, -1, US_INV);
        int32_t len;
        स्थिरांक char16_t *currencyName = ucurr_getName(ustr_मूल्य.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        जर (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_मूल्य;
            परत result;
        }
        result.setTo(currencyName, len);
        परत skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_किल्ली_मूल्य, result);
    }

    जर (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", किल्ली, value, result);
        जर (!result.isBogus()) {
            परत skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_किल्ली_मूल्य, result);
        }
    }
    जर (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", किल्ली, value, result);
    } अन्यथा {
        langData.getNoFallback("Types", किल्ली, value, result);
    }
    परत skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_किल्ली_मूल्य, result);
}

UnicodeString&
LocaleDisplayNamesImpl::किल्ली_मूल्य_Display_Name(स्थिरांक अक्षर* किल्ली,
                                            स्थिरांक अक्षर* value,
                                            UnicodeString& result U_LIFETIME_BOUND) स्थिरांक {
    परत किल्ली_मूल्य_Display_Name(किल्ली, value, result, असत्य);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(स्थिरांक Locale& locale,
                                   UDialectHandling dialectHandling) {
    परत नवीन LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(स्थिरांक Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    जर (contexts == nullptr) {
        length = 0;
    }
    परत नवीन LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(स्थिरांक अक्षर * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत nullptr;
  }
  जर (locale == nullptr) {
    locale = uloc_getDefault();
  }
  परत (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(स्थिरांक अक्षर * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत nullptr;
  }
  जर (locale == nullptr) {
    locale = uloc_getDefault();
  }
  परत (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI शून्य U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uldn_getLocale(स्थिरांक ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  जर (ldn) {
    परत ((स्थिरांक LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  परत nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(स्थिरांक ULocaleDisplayNames *ldn) {
  जर (ldn) {
    परत ((स्थिरांक LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  परत ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(स्थिरांक ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत (UDisplayContext)0;
  }
  परत ((स्थिरांक LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                       स्थिरांक अक्षर *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  जर (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                         स्थिरांक अक्षर *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                       स्थिरांक अक्षर *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  परत uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                       स्थिरांक अक्षर *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(स्थिरांक ULocaleDisplayNames *ldn,
                        स्थिरांक अक्षर *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_किल्ली_Display_Name(स्थिरांक ULocaleDisplayNames *ldn,
                    स्थिरांक अक्षर *किल्ली,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || किल्ली == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->किल्ली_Display_Name(किल्ली, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_किल्ली_मूल्य_Display_Name(स्थिरांक ULocaleDisplayNames *ldn,
                         स्थिरांक अक्षर *किल्ली,
                         स्थिरांक अक्षर *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  जर (U_FAILURE(*pErrorCode)) {
    परत 0;
  }
  जर (ldn == nullptr || किल्ली == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((स्थिरांक LocaleDisplayNames *)ldn)->किल्ली_मूल्य_Display_Name(किल्ली, value, temp);
  परत temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
