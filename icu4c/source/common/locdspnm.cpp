// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_FORMATTING

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
    ध्रुव वर्ण* ध्रुव path;
    Locale locale;

सार्वजनिक:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(ध्रुव वर्ण* path, ध्रुव Locale& locale);
    ~ICUDataTable() = स्वभाव;

    ध्रुव Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                       UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& get(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* sub_Table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                       UnicodeString& result U_LIFETIME_BOUND) ध्रुव;

    UnicodeString& getNoFallback(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                 UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& getNoFallback(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* sub_Table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                 UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
};

अन्तःस्थ UnicodeString& ICUDataTable::get(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                        UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम get(table_कुञ्चिका, nullptr, item_कुञ्चिका, result);
}

अन्तःस्थ UnicodeString& ICUDataTable::getNoFallback(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                                                  UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम getNoFallback(table_कुञ्चिका, nullptr, item_कुञ्चिका, result);
}

ICUDataTable::ICUDataTable(ध्रुव वर्ण* path, ध्रुव Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

ध्रुव Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  प्रत्यागम locale;
}

UnicodeString&
ICUDataTable::get(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* sub_Table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                  UnicodeString &result U_LIFETIME_BOUND) ध्रुव {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ध्रुव char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_कुञ्चिका, sub_Table_कुञ्चिका, item_कुञ्चिका,
                                                   &len, &status);
  जइ (U_SUCCESS(status) && len > 0) {
    प्रत्यागम result.setTo(s, len);
  }
  प्रत्यागम result.setTo(UnicodeString(item_कुञ्चिका, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(ध्रुव वर्ण* table_कुञ्चिका, ध्रुव वर्ण* sub_Table_कुञ्चिका, ध्रुव वर्ण* item_कुञ्चिका,
                            UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  ध्रुव char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_कुञ्चिका, sub_Table_कुञ्चिका, item_कुञ्चिका,
                                                   &len, &status);
  जइ (U_SUCCESS(status)) {
    प्रत्यागम result.setTo(s, len);
  }

  result.setToBogus();
  प्रत्यागम result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#जइ 0  // currently unused

वर्ग DefaultLocaleDisplayNames : सार्वजनिक LocaleDisplayNames {
  UDialectHandling dialectHandling;

सार्वजनिक:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  आभास ~DefaultLocaleDisplayNames();

  आभास ध्रुव Locale& getLocale() ध्रुव U_LIFETIME_BOUND;
  आभास UDialectHandling getDialectHandling() ध्रुव;

  आभास UnicodeString& localeDisplayName(ध्रुव Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& localeDisplayName(ध्रुव वर्ण* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& languageDisplayName(ध्रुव वर्ण* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& scriptDisplayName(ध्रुव वर्ण* script,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& regionDisplayName(ध्रुव वर्ण* region,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& variantDisplayName(ध्रुव वर्ण* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                        UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
  आभास UnicodeString& कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                             ध्रुव वर्ण* value,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

ध्रुव Locale&
DefaultLocaleDisplayNames::getLocale() ध्रुव U_LIFETIME_BOUND {
  प्रत्यागम Locale::get_मूलम्();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() ध्रुव {
  प्रत्यागम dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ध्रुव Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(ध्रुव वर्ण* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(ध्रुव वर्ण* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(ध्रुव वर्ण* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  ध्रुव वर्ण* name = uscript_getName(scriptCode);
  जइ (name) {
    प्रत्यागम result = UnicodeString(name, -1, US_INV);
  }
  प्रत्यागम result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(ध्रुव वर्ण* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(ध्रुव वर्ण* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(कुञ्चिका, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* /*कुञ्चिका*/,
                                               ध्रुव वर्ण* value,
                                               UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  प्रत्यागम result = UnicodeString(value, -1, US_INV);
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
    SimpleFormatter कुञ्चिका_Type_Format;
    UDisplayContext capitalizationContext;
#जइ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#अण्णहा
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
        k_Cap_Context_Usage_कुञ्चिका,
        k_Cap_Context_Usage_कुञ्चिका_मानम्,
        k_Cap_Context_Usage_गणनम्
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     तार्किक fCapitalization[k_Cap_Context_Usage_गणनम्];

सार्वजनिक:
    // constructor
    LocaleDisplayNamesImpl(ध्रुव Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(ध्रुव Locale& locale, UDisplayContext *contexts, int32_t length);
    आभास ~LocaleDisplayNamesImpl();

    आभास ध्रुव Locale& getLocale() ध्रुव U_LIFETIME_BOUND override;
    आभास UDialectHandling getDialectHandling() ध्रुव override;
    आभास UDisplayContext getContext(UDisplayContextType type) ध्रुव override;

    आभास UnicodeString& localeDisplayName(ध्रुव Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& localeDisplayName(ध्रुव वर्ण* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    languageDisplayName(ध्रुव वर्ण* lang, UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& scriptDisplayName(ध्रुव वर्ण* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& regionDisplayName(ध्रुव वर्ण* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    variantDisplayName(ध्रुव वर्ण* variant, UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;
    आभास UnicodeString&
    कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value,
                        UnicodeString& result U_LIFETIME_BOUND) ध्रुव override;

गुप्त:
    UnicodeString& localeIdName(ध्रुव वर्ण* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                तार्किक substitute) ध्रुव;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 ध्रुव UnicodeString& src) ध्रुव;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) ध्रुव;
    UnicodeString& scriptDisplayName(ध्रुव वर्ण* script, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) ध्रुव;
    UnicodeString& regionDisplayName(ध्रुव वर्ण* region, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) ध्रुव;
    UnicodeString& variantDisplayName(ध्रुव वर्ण* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      तार्किक skipAdjust) ध्रुव;
    UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका, UnicodeString& result U_LIFETIME_BOUND,
                                  तार्किक skipAdjust) ध्रुव;
    UnicodeString& कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) ध्रुव;
    शून्य initialize();

    संरचना CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ध्रुव Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(ध्रुव Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    जाव (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        विकल्प (selector) {
            पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                भङ्ग;
            पक्ष UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                भङ्ग;
            पक्ष UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                भङ्ग;
            पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                भङ्ग;
            स्वभाव:
                भङ्ग;
        }
    }
    initialize();
}

संरचना LocaleDisplayNamesImpl::CapitalizationContextSink : सार्वजनिक ResourceSink {
    तार्किक hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(मिच्छ), parent(_parent) {}
    आभास ~CapitalizationContextSink();

    आभास शून्य put(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        प्रति (पूर्णाङ्क i = 0; contexts.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); ++i) {

            CapContextUsage usageEnum;
            जइ (uprv_strcmp(कुञ्चिका, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_कुञ्चिका;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_कुञ्चिका_मानम्;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } अण्णहा जइ (uprv_strcmp(कुञ्चिका, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } अण्णहा {
                अनुवर्त;
            }

            int32_t len = 0;
            ध्रुव int32_t* intVector = value.getIntVector(len, errorCode);
            जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
            जइ (len < 2) { अनुवर्त; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            जइ (titlecaseInt == 0) { अनुवर्त; }

            parent.fCapitalization[usageEnum] = सच्च;
            hasCapitalizationUsage = सच्च;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

शून्य
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = एतत्;
    nonConstThis->locale = langData.getLocale() == Locale::get_मूलम्()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    जइ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    जइ (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    जइ (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } अण्णहा {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    जइ (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    कुञ्चिका_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, परिमाण(fCapitalization));
#जइ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    तार्किक needBrkIter = मिच्छ;
    जइ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        जइ (U_FAILURE(status)) { प्रत्यागम; }
        CapitalizationContextSink sink(*एतत्);
        ures_get_All_वस्तूनि_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        जइ (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } अण्णहा जइ (U_FAILURE(status)) {
            प्रत्यागम;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    जइ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        जइ (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#जइ !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

ध्रुव Locale&
LocaleDisplayNamesImpl::getLocale() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() ध्रुव {
    प्रत्यागम dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) ध्रुव {
    विकल्प (type) {
        पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
            प्रत्यागम static_cast<UDisplayContext>(dialectHandling);
        पक्ष UDISPCTX_TYPE_CAPITALIZATION:
            प्रत्यागम capitalizationContext;
        पक्ष UDISPCTX_TYPE_DISPLAY_LENGTH:
            प्रत्यागम nameLength;
        पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            प्रत्यागम substitute;
        स्वभाव:
            भङ्ग;
    }
    प्रत्यागम static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
#जइ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    जइ ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        स्थिर UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    प्रत्यागम result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ध्रुव Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
  जइ (loc.isBogus()) {
    result.setToBogus();
    प्रत्यागम result;
  }
  UnicodeString resultName;

  ध्रुव वर्ण* lang = loc.getLanguage();
  जइ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ध्रुव वर्ण* script = loc.getScript();
  ध्रुव वर्ण* country = loc.getCountry();
  ध्रुव वर्ण* variant = loc.getVariant();

  तार्किक hasScript = uprv_strlen(script) > 0;
  तार्किक hasCountry = uprv_strlen(country) > 0;
  तार्किक hasVariant = uprv_strlen(variant) > 0;

  जइ (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    करेहि { // loop construct is so we can break early out of search
      जइ (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        जइ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, मिच्छ);
          जइ (!resultName.isBogus()) {
            hasScript = मिच्छ;
            hasCountry = मिच्छ;
            भङ्ग;
          }
        }
      }
      जइ (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        जइ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, मिच्छ);
          जइ (!resultName.isBogus()) {
            hasScript = मिच्छ;
            भङ्ग;
          }
        }
      }
      जइ (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        जइ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, मिच्छ);
          जइ (!resultName.isBogus()) {
            hasCountry = मिच्छ;
            भङ्ग;
          }
        }
      }
    } जाव (मिच्छ);
  }
  जइ (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    जइ (resultName.isBogus()) {
      result.setToBogus();
      प्रत्यागम result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  जइ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, सच्च);
    जइ (script_str.isBogus()) {
      result.setToBogus();
      प्रत्यागम result;
    }
    resultRemainder.append(script_str);
  }
  जइ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, सच्च);
    जइ (region_str.isBogus()) {
      result.setToBogus();
      प्रत्यागम result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  जइ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, सच्च);
    जइ (variant_str.isBogus()) {
      result.setToBogus();
      प्रत्यागम result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  जइ (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    ध्रुव वर्ण* कुञ्चिका;
    जाव ((कुञ्चिका = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_मानम्<CharString>(कुञ्चिका, status);
        जइ (U_FAILURE(status)) {
            प्रत्यागम result;
      }
      कुञ्चिका_Display_Name(कुञ्चिका, temp, सच्च);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value.data(), temp2, सच्च);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      जइ (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } अण्णहा जइ (temp != UnicodeString(कुञ्चिका, -1, US_INV)) {
        UnicodeString temp3;
        कुञ्चिका_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } अण्णहा {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  जइ (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, ध्रुव UnicodeString& src) ध्रुव {
    जइ (buffer.isEmpty()) {
        buffer.setTo(src);
    } अण्णहा {
        ध्रुव UnicodeString *मानानि[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मानानि, 2, buffer, nullptr, 0, status);
    }
    प्रत्यागम buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(ध्रुव वर्ण* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(ध्रुव वर्ण* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, तार्किक substitute) ध्रुव {
    जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        जइ (!result.isBogus()) {
            प्रत्यागम result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    जइ (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ध्रुव वर्ण* canonLocId = canonLocale.getName();
        जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            जइ (!result.isBogus()) {
                प्रत्यागम result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    जइ (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    प्रत्यागम result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(ध्रुव वर्ण* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    जइ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        प्रत्यागम result = UnicodeString(lang, -1, US_INV);
    }
    जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        जइ (!result.isBogus()) {
            प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    जइ (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ध्रुव वर्ण* canonLocId = canonLocale.getName();
        जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            जइ (!result.isBogus()) {
                प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    जइ (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    प्रत्यागम adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ध्रुव वर्ण* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) ध्रुव {
    जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        जइ (!result.isBogus()) {
            प्रत्यागम skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    जइ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } अण्णहा {
        langData.getNoFallback("Scripts", script, result);
    }
    प्रत्यागम skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(ध्रुव वर्ण* script,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम scriptDisplayName(script, result, मिच्छ);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम scriptDisplayName(uscript_getName(scriptCode), result, मिच्छ);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ध्रुव वर्ण* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) ध्रुव {
    जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        जइ (!result.isBogus()) {
            प्रत्यागम skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    जइ (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } अण्णहा {
        regionData.getNoFallback("Countries", region, result);
    }
    प्रत्यागम skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(ध्रुव वर्ण* region,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम regionDisplayName(region, result, मिच्छ);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ध्रुव वर्ण* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           तार्किक skipAdjust) ध्रुव {
    // don't have a resource for short variant names
    जइ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } अण्णहा {
        langData.getNoFallback("Variants", variant, result);
    }
    प्रत्यागम skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(ध्रुव वर्ण* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम variantDisplayName(variant, result, मिच्छ);
}

UnicodeString&
LocaleDisplayNamesImpl::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) ध्रुव {
    // don't have a resource for short key names
    जइ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", कुञ्चिका, result);
    } अण्णहा {
        langData.getNoFallback("Keys", कुञ्चिका, result);
    }
    प्रत्यागम skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका, result);
}

UnicodeString&
LocaleDisplayNamesImpl::कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                       UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम कुञ्चिका_Display_Name(कुञ्चिका, result, मिच्छ);
}

UnicodeString&
LocaleDisplayNamesImpl::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                            ध्रुव वर्ण* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            तार्किक skipAdjust) ध्रुव {
    जइ (uprv_strcmp(कुञ्चिका, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_मानम्(value, -1, US_INV);
        int32_t len;
        ध्रुव char16_t *currencyName = ucurr_getName(ustr_मानम्.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        जइ (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_मानम्;
            प्रत्यागम result;
        }
        result.setTo(currencyName, len);
        प्रत्यागम skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, result);
    }

    जइ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", कुञ्चिका, value, result);
        जइ (!result.isBogus()) {
            प्रत्यागम skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, result);
        }
    }
    जइ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", कुञ्चिका, value, result);
    } अण्णहा {
        langData.getNoFallback("Types", कुञ्चिका, value, result);
    }
    प्रत्यागम skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_कुञ्चिका_मानम्, result);
}

UnicodeString&
LocaleDisplayNamesImpl::कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                            ध्रुव वर्ण* value,
                                            UnicodeString& result U_LIFETIME_BOUND) ध्रुव {
    प्रत्यागम कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value, result, मिच्छ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ध्रुव Locale& locale,
                                   UDialectHandling dialectHandling) {
    प्रत्यागम नूतन LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(ध्रुव Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    जइ (contexts == nullptr) {
        length = 0;
    }
    प्रत्यागम नूतन LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(ध्रुव वर्ण * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम nullptr;
  }
  जइ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  प्रत्यागम (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(ध्रुव वर्ण * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम nullptr;
  }
  जइ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  प्रत्यागम (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI शून्य U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uldn_getLocale(ध्रुव ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  जइ (ldn) {
    प्रत्यागम ((ध्रुव LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  प्रत्यागम nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ध्रुव ULocaleDisplayNames *ldn) {
  जइ (ldn) {
    प्रत्यागम ((ध्रुव LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  प्रत्यागम ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ध्रुव ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम (UDisplayContext)0;
  }
  प्रत्यागम ((ध्रुव LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                       ध्रुव वर्ण *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  जइ (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                         ध्रुव वर्ण *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                       ध्रुव वर्ण *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  प्रत्यागम uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                       ध्रुव वर्ण *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ध्रुव ULocaleDisplayNames *ldn,
                        ध्रुव वर्ण *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_कुञ्चिका_Display_Name(ध्रुव ULocaleDisplayNames *ldn,
                    ध्रुव वर्ण *कुञ्चिका,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || कुञ्चिका == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->कुञ्चिका_Display_Name(कुञ्चिका, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_कुञ्चिका_मानम्_Display_Name(ध्रुव ULocaleDisplayNames *ldn,
                         ध्रुव वर्ण *कुञ्चिका,
                         ध्रुव वर्ण *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  जइ (U_FAILURE(*pErrorCode)) {
    प्रत्यागम 0;
  }
  जइ (ldn == nullptr || कुञ्चिका == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    प्रत्यागम 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((ध्रुव LocaleDisplayNames *)ldn)->कुञ्चिका_मानम्_Display_Name(कुञ्चिका, value, temp);
  प्रत्यागम temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
