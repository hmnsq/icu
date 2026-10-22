// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_FORMATTING

#અન્તર્ભાવ "unicode/locdspnm.h"
#અન્તર્ભાવ "unicode/simpleformatter.h"
#અન્તર્ભાવ "unicode/ucasemap.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/udisplaycontext.h"
#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/ucurr.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "ureslocs.h"
#અન્તર્ભાવ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
વર્ગ ICUDataTable {
    અચલ અક્ષર* અચલ path;
    Locale locale;

જાહેર:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(અચલ અક્ષર* path, અચલ Locale& locale);
    ~ICUDataTable() = મૂળભૂત;

    અચલ Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* item_ચાવી,
                       UnicodeString& result U_LIFETIME_BOUND) અચલ;
    UnicodeString& get(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* sub_Table_ચાવી, અચલ અક્ષર* item_ચાવી,
                       UnicodeString& result U_LIFETIME_BOUND) અચલ;

    UnicodeString& getNoFallback(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* item_ચાવી,
                                 UnicodeString& result U_LIFETIME_BOUND) અચલ;
    UnicodeString& getNoFallback(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* sub_Table_ચાવી, અચલ અક્ષર* item_ચાવી,
                                 UnicodeString& result U_LIFETIME_BOUND) અચલ;
};

અંતઃરેખ UnicodeString& ICUDataTable::get(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* item_ચાવી,
                                        UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા get(table_ચાવી, nullptr, item_ચાવી, result);
}

અંતઃરેખ UnicodeString& ICUDataTable::getNoFallback(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* item_ચાવી,
                                                  UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા getNoFallback(table_ચાવી, nullptr, item_ચાવી, result);
}

ICUDataTable::ICUDataTable(અચલ અક્ષર* path, અચલ Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

અચલ Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  પાછા locale;
}

UnicodeString&
ICUDataTable::get(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* sub_Table_ચાવી, અચલ અક્ષર* item_ચાવી,
                  UnicodeString &result U_LIFETIME_BOUND) અચલ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  અચલ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ચાવી, sub_Table_ચાવી, item_ચાવી,
                                                   &len, &status);
  જો (U_SUCCESS(status) && len > 0) {
    પાછા result.setTo(s, len);
  }
  પાછા result.setTo(UnicodeString(item_ચાવી, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(અચલ અક્ષર* table_ચાવી, અચલ અક્ષર* sub_Table_ચાવી, અચલ અક્ષર* item_ચાવી,
                            UnicodeString& result U_LIFETIME_BOUND) અચલ {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  અચલ char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_ચાવી, sub_Table_ચાવી, item_ચાવી,
                                                   &len, &status);
  જો (U_SUCCESS(status)) {
    પાછા result.setTo(s, len);
  }

  result.setToBogus();
  પાછા result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#જો 0  // currently unused

વર્ગ DefaultLocaleDisplayNames : જાહેર LocaleDisplayNames {
  UDialectHandling dialectHandling;

જાહેર:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  આભાસી ~DefaultLocaleDisplayNames();

  આભાસી અચલ Locale& getLocale() અચલ U_LIFETIME_BOUND;
  આભાસી UDialectHandling getDialectHandling() અચલ;

  આભાસી UnicodeString& localeDisplayName(અચલ Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& localeDisplayName(અચલ અક્ષર* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& languageDisplayName(અચલ અક્ષર* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& scriptDisplayName(અચલ અક્ષર* script,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& regionDisplayName(અચલ અક્ષર* region,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& variantDisplayName(અચલ અક્ષર* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& ચાવી_Display_Name(અચલ અક્ષર* ચાવી,
                                        UnicodeString& result U_LIFETIME_BOUND) અચલ;
  આભાસી UnicodeString& ચાવી_કિંમત_Display_Name(અચલ અક્ષર* ચાવી,
                                             અચલ અક્ષર* value,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

અચલ Locale&
DefaultLocaleDisplayNames::getLocale() અચલ U_LIFETIME_BOUND {
  પાછા Locale::get_મૂળ();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() અચલ {
  પાછા dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(અચલ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(અચલ અક્ષર* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(અચલ અક્ષર* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(અચલ અક્ષર* script,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ {
  અચલ અક્ષર* name = uscript_getName(scriptCode);
  જો (name) {
    પાછા result = UnicodeString(name, -1, US_INV);
  }
  પાછા result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(અચલ અક્ષર* region,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(અચલ અક્ષર* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ચાવી_Display_Name(અચલ અક્ષર* ચાવી,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(ચાવી, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::ચાવી_કિંમત_Display_Name(અચલ અક્ષર* /*ચાવી*/,
                                               અચલ અક્ષર* value,
                                               UnicodeString& result U_LIFETIME_BOUND) અચલ {
  પાછા result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

વર્ગ LocaleDisplayNamesImpl : જાહેર LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ચાવી_Type_Format;
    UDisplayContext capitalizationContext;
#જો !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#નહીંતર
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    ગણના CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_ચાવી,
        k_Cap_Context_Usage_ચાવી_કિંમત,
        k_Cap_Context_Usage_ગણતરી
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     તાર્કિક fCapitalization[k_Cap_Context_Usage_ગણતરી];

જાહેર:
    // constructor
    LocaleDisplayNamesImpl(અચલ Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(અચલ Locale& locale, UDisplayContext *contexts, int32_t length);
    આભાસી ~LocaleDisplayNamesImpl();

    આભાસી અચલ Locale& getLocale() અચલ U_LIFETIME_BOUND override;
    આભાસી UDialectHandling getDialectHandling() અચલ override;
    આભાસી UDisplayContext getContext(UDisplayContextType type) અચલ override;

    આભાસી UnicodeString& localeDisplayName(અચલ Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString& localeDisplayName(અચલ અક્ષર* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString&
    languageDisplayName(અચલ અક્ષર* lang, UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString& scriptDisplayName(અચલ અક્ષર* script,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString& regionDisplayName(અચલ અક્ષર* region,
                                             UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString&
    variantDisplayName(અચલ અક્ષર* variant, UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString& ચાવી_Display_Name(અચલ અક્ષર* ચાવી,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ override;
    આભાસી UnicodeString&
    ચાવી_કિંમત_Display_Name(અચલ અક્ષર* ચાવી, અચલ અક્ષર* value,
                        UnicodeString& result U_LIFETIME_BOUND) અચલ override;

ખાનગી:
    UnicodeString& localeIdName(અચલ અક્ષર* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                તાર્કિક substitute) અચલ;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 અચલ UnicodeString& src) અચલ;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) અચલ;
    UnicodeString& scriptDisplayName(અચલ અક્ષર* script, UnicodeString& result U_LIFETIME_BOUND,
                                     તાર્કિક skipAdjust) અચલ;
    UnicodeString& regionDisplayName(અચલ અક્ષર* region, UnicodeString& result U_LIFETIME_BOUND,
                                     તાર્કિક skipAdjust) અચલ;
    UnicodeString& variantDisplayName(અચલ અક્ષર* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      તાર્કિક skipAdjust) અચલ;
    UnicodeString& ચાવી_Display_Name(અચલ અક્ષર* ચાવી, UnicodeString& result U_LIFETIME_BOUND,
                                  તાર્કિક skipAdjust) અચલ;
    UnicodeString& ચાવી_કિંમત_Display_Name(અચલ અક્ષર* ચાવી, અચલ અક્ષર* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       તાર્કિક skipAdjust) અચલ;
    શૂન્ય initialize();

    સંરચના CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(અચલ Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(અચલ Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    જ્યારે (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        પસંદગી (selector) {
            સ્થિતિ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                તોડો;
            સ્થિતિ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                તોડો;
            સ્થિતિ UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                તોડો;
            સ્થિતિ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                તોડો;
            મૂળભૂત:
                તોડો;
        }
    }
    initialize();
}

સંરચના LocaleDisplayNamesImpl::CapitalizationContextSink : જાહેર ResourceSink {
    તાર્કિક hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(ખોટું), parent(_parent) {}
    આભાસી ~CapitalizationContextSink();

    આભાસી શૂન્ય put(અચલ અક્ષર *ચાવી, Resource_કિંમત &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        માટે (પૂર્ણાંક i = 0; contexts.get_ચાવી_And_કિંમત(i, ચાવી, value); ++i) {

            CapContextUsage usageEnum;
            જો (uprv_strcmp(ચાવી, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ચાવી;
            } નહીંતર જો (uprv_strcmp(ચાવી, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ચાવી_કિંમત;
            } નહીંતર જો (uprv_strcmp(ચાવી, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } નહીંતર જો (uprv_strcmp(ચાવી, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } નહીંતર જો (uprv_strcmp(ચાવી, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } નહીંતર જો (uprv_strcmp(ચાવી, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } નહીંતર {
                ચાલુ;
            }

            int32_t len = 0;
            અચલ int32_t* intVector = value.getIntVector(len, errorCode);
            જો (U_FAILURE(errorCode)) { પાછા; }
            જો (len < 2) { ચાલુ; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            જો (titlecaseInt == 0) { ચાલુ; }

            parent.fCapitalization[usageEnum] = સાચું;
            hasCapitalizationUsage = સાચું;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

શૂન્ય
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = આ;
    nonConstThis->locale = langData.getLocale() == Locale::get_મૂળ()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    જો (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    જો (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    જો (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } નહીંતર {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    જો (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    ચાવી_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, કદ(fCapitalization));
#જો !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    તાર્કિક needBrkIter = ખોટું;
    જો (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        જો (U_FAILURE(status)) { પાછા; }
        CapitalizationContextSink sink(*આ);
        ures_get_All_વસ્તુઓ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        જો (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } નહીંતર જો (U_FAILURE(status)) {
            પાછા;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    જો (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        જો (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#જો !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

અચલ Locale&
LocaleDisplayNamesImpl::getLocale() અચલ U_LIFETIME_BOUND {
    પાછા locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() અચલ {
    પાછા dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) અચલ {
    પસંદગી (type) {
        સ્થિતિ UDISPCTX_TYPE_DIALECT_HANDLING:
            પાછા static_cast<UDisplayContext>(dialectHandling);
        સ્થિતિ UDISPCTX_TYPE_CAPITALIZATION:
            પાછા capitalizationContext;
        સ્થિતિ UDISPCTX_TYPE_DISPLAY_LENGTH:
            પાછા nameLength;
        સ્થિતિ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            પાછા substitute;
        મૂળભૂત:
            તોડો;
    }
    પાછા static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) અચલ {
#જો !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    જો ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        સ્થિર UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    પાછા result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(અચલ Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
  જો (loc.isBogus()) {
    result.setToBogus();
    પાછા result;
  }
  UnicodeString resultName;

  અચલ અક્ષર* lang = loc.getLanguage();
  જો (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  અચલ અક્ષર* script = loc.getScript();
  અચલ અક્ષર* country = loc.getCountry();
  અચલ અક્ષર* variant = loc.getVariant();

  તાર્કિક hasScript = uprv_strlen(script) > 0;
  તાર્કિક hasCountry = uprv_strlen(country) > 0;
  તાર્કિક hasVariant = uprv_strlen(variant) > 0;

  જો (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    કરો { // loop construct is so we can break early out of search
      જો (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        જો (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ખોટું);
          જો (!resultName.isBogus()) {
            hasScript = ખોટું;
            hasCountry = ખોટું;
            તોડો;
          }
        }
      }
      જો (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        જો (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ખોટું);
          જો (!resultName.isBogus()) {
            hasScript = ખોટું;
            તોડો;
          }
        }
      }
      જો (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        જો (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, ખોટું);
          જો (!resultName.isBogus()) {
            hasCountry = ખોટું;
            તોડો;
          }
        }
      }
    } જ્યારે (ખોટું);
  }
  જો (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    જો (resultName.isBogus()) {
      result.setToBogus();
      પાછા result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  જો (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, સાચું);
    જો (script_str.isBogus()) {
      result.setToBogus();
      પાછા result;
    }
    resultRemainder.append(script_str);
  }
  જો (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, સાચું);
    જો (region_str.isBogus()) {
      result.setToBogus();
      પાછા result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  જો (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, સાચું);
    જો (variant_str.isBogus()) {
      result.setToBogus();
      પાછા result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  જો (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    અચલ અક્ષર* ચાવી;
    જ્યારે ((ચાવી = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_કિંમત<CharString>(ચાવી, status);
        જો (U_FAILURE(status)) {
            પાછા result;
      }
      ચાવી_Display_Name(ચાવી, temp, સાચું);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      ચાવી_કિંમત_Display_Name(ચાવી, value.data(), temp2, સાચું);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      જો (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } નહીંતર જો (temp != UnicodeString(ચાવી, -1, US_INV)) {
        UnicodeString temp3;
        ચાવી_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } નહીંતર {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  જો (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    પાછા adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  પાછા adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, અચલ UnicodeString& src) અચલ {
    જો (buffer.isEmpty()) {
        buffer.setTo(src);
    } નહીંતર {
        અચલ UnicodeString *કિંમતો[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(કિંમતો, 2, buffer, nullptr, 0, status);
    }
    પાછા buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(અચલ અક્ષર* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(અચલ અક્ષર* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, તાર્કિક substitute) અચલ {
    જો (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        જો (!result.isBogus()) {
            પાછા result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    જો (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        અચલ અક્ષર* canonLocId = canonLocale.getName();
        જો (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            જો (!result.isBogus()) {
                પાછા result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    જો (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    પાછા result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(અચલ અક્ષર* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) અચલ {
    જો (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        પાછા result = UnicodeString(lang, -1, US_INV);
    }
    જો (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        જો (!result.isBogus()) {
            પાછા adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    જો (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        અચલ અક્ષર* canonLocId = canonLocale.getName();
        જો (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            જો (!result.isBogus()) {
                પાછા adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    જો (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    પાછા adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(અચલ અક્ષર* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          તાર્કિક skipAdjust) અચલ {
    જો (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        જો (!result.isBogus()) {
            પાછા skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    જો (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } નહીંતર {
        langData.getNoFallback("Scripts", script, result);
    }
    પાછા skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(અચલ અક્ષર* script,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા scriptDisplayName(script, result, ખોટું);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા scriptDisplayName(uscript_getName(scriptCode), result, ખોટું);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(અચલ અક્ષર* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          તાર્કિક skipAdjust) અચલ {
    જો (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        જો (!result.isBogus()) {
            પાછા skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    જો (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } નહીંતર {
        regionData.getNoFallback("Countries", region, result);
    }
    પાછા skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(અચલ અક્ષર* region,
                                          UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા regionDisplayName(region, result, ખોટું);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(અચલ અક્ષર* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           તાર્કિક skipAdjust) અચલ {
    // don't have a resource for short variant names
    જો (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } નહીંતર {
        langData.getNoFallback("Variants", variant, result);
    }
    પાછા skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(અચલ અક્ષર* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા variantDisplayName(variant, result, ખોટું);
}

UnicodeString&
LocaleDisplayNamesImpl::ચાવી_Display_Name(અચલ અક્ષર* ચાવી,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       તાર્કિક skipAdjust) અચલ {
    // don't have a resource for short key names
    જો (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", ચાવી, result);
    } નહીંતર {
        langData.getNoFallback("Keys", ચાવી, result);
    }
    પાછા skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ચાવી, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ચાવી_Display_Name(અચલ અક્ષર* ચાવી,
                                       UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા ચાવી_Display_Name(ચાવી, result, ખોટું);
}

UnicodeString&
LocaleDisplayNamesImpl::ચાવી_કિંમત_Display_Name(અચલ અક્ષર* ચાવી,
                                            અચલ અક્ષર* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            તાર્કિક skipAdjust) અચલ {
    જો (uprv_strcmp(ચાવી, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_કિંમત(value, -1, US_INV);
        int32_t len;
        અચલ char16_t *currencyName = ucurr_getName(ustr_કિંમત.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        જો (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_કિંમત;
            પાછા result;
        }
        result.setTo(currencyName, len);
        પાછા skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ચાવી_કિંમત, result);
    }

    જો (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", ચાવી, value, result);
        જો (!result.isBogus()) {
            પાછા skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ચાવી_કિંમત, result);
        }
    }
    જો (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", ચાવી, value, result);
    } નહીંતર {
        langData.getNoFallback("Types", ચાવી, value, result);
    }
    પાછા skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_ચાવી_કિંમત, result);
}

UnicodeString&
LocaleDisplayNamesImpl::ચાવી_કિંમત_Display_Name(અચલ અક્ષર* ચાવી,
                                            અચલ અક્ષર* value,
                                            UnicodeString& result U_LIFETIME_BOUND) અચલ {
    પાછા ચાવી_કિંમત_Display_Name(ચાવી, value, result, ખોટું);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(અચલ Locale& locale,
                                   UDialectHandling dialectHandling) {
    પાછા નવું LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(અચલ Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    જો (contexts == nullptr) {
        length = 0;
    }
    પાછા નવું LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(અચલ અક્ષર * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા nullptr;
  }
  જો (locale == nullptr) {
    locale = uloc_getDefault();
  }
  પાછા (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(અચલ અક્ષર * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા nullptr;
  }
  જો (locale == nullptr) {
    locale = uloc_getDefault();
  }
  પાછા (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI શૂન્ય U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
uldn_getLocale(અચલ ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  જો (ldn) {
    પાછા ((અચલ LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  પાછા nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(અચલ ULocaleDisplayNames *ldn) {
  જો (ldn) {
    પાછા ((અચલ LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  પાછા ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(અચલ ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા (UDisplayContext)0;
  }
  પાછા ((અચલ LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(અચલ ULocaleDisplayNames *ldn,
                       અચલ અક્ષર *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  જો (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(અચલ ULocaleDisplayNames *ldn,
                         અચલ અક્ષર *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(અચલ ULocaleDisplayNames *ldn,
                       અચલ અક્ષર *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(અચલ ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  પાછા uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(અચલ ULocaleDisplayNames *ldn,
                       અચલ અક્ષર *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(અચલ ULocaleDisplayNames *ldn,
                        અચલ અક્ષર *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ચાવી_Display_Name(અચલ ULocaleDisplayNames *ldn,
                    અચલ અક્ષર *ચાવી,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || ચાવી == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->ચાવી_Display_Name(ચાવી, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_ચાવી_કિંમત_Display_Name(અચલ ULocaleDisplayNames *ldn,
                         અચલ અક્ષર *ચાવી,
                         અચલ અક્ષર *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  જો (U_FAILURE(*pErrorCode)) {
    પાછા 0;
  }
  જો (ldn == nullptr || ચાવી == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((અચલ LocaleDisplayNames *)ldn)->ચાવી_કિંમત_Display_Name(ચાવી, value, temp);
  પાછા temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
