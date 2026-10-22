// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_FORMATTING

#அந்தர்பாவ "unicode/locdspnm.h"
#அந்தர்பாவ "unicode/simpleformatter.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/udisplaycontext.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/ucurr.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "ureslocs.h"
#அந்தர்பாவ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
வகுப்பு ICUDataTable {
    மாறிலி எழுத்து* மாறிலி path;
    Locale locale;

பொது:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(மாறிலி எழுத்து* path, மாறிலி Locale& locale);
    ~ICUDataTable() = இயல்பு;

    மாறிலி Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* item_சாவி,
                       UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
    UnicodeString& get(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* sub_Table_சாவி, மாறிலி எழுத்து* item_சாவி,
                       UnicodeString& result U_LIFETIME_BOUND) மாறிலி;

    UnicodeString& getNoFallback(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* item_சாவி,
                                 UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
    UnicodeString& getNoFallback(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* sub_Table_சாவி, மாறிலி எழுத்து* item_சாவி,
                                 UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
};

உள்வரி UnicodeString& ICUDataTable::get(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* item_சாவி,
                                        UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு get(table_சாவி, nullptr, item_சாவி, result);
}

உள்வரி UnicodeString& ICUDataTable::getNoFallback(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* item_சாவி,
                                                  UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு getNoFallback(table_சாவி, nullptr, item_சாவி, result);
}

ICUDataTable::ICUDataTable(மாறிலி எழுத்து* path, மாறிலி Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

மாறிலி Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  திரும்பு locale;
}

UnicodeString&
ICUDataTable::get(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* sub_Table_சாவி, மாறிலி எழுத்து* item_சாவி,
                  UnicodeString &result U_LIFETIME_BOUND) மாறிலி {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  மாறிலி char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_சாவி, sub_Table_சாவி, item_சாவி,
                                                   &len, &status);
  எனில் (U_SUCCESS(status) && len > 0) {
    திரும்பு result.setTo(s, len);
  }
  திரும்பு result.setTo(UnicodeString(item_சாவி, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(மாறிலி எழுத்து* table_சாவி, மாறிலி எழுத்து* sub_Table_சாவி, மாறிலி எழுத்து* item_சாவி,
                            UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  மாறிலி char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_சாவி, sub_Table_சாவி, item_சாவி,
                                                   &len, &status);
  எனில் (U_SUCCESS(status)) {
    திரும்பு result.setTo(s, len);
  }

  result.setToBogus();
  திரும்பு result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#எனில் 0  // currently unused

வகுப்பு DefaultLocaleDisplayNames : பொது LocaleDisplayNames {
  UDialectHandling dialectHandling;

பொது:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  மெய்நிகர் ~DefaultLocaleDisplayNames();

  மெய்நிகர் மாறிலி Locale& getLocale() மாறிலி U_LIFETIME_BOUND;
  மெய்நிகர் UDialectHandling getDialectHandling() மாறிலி;

  மெய்நிகர் UnicodeString& localeDisplayName(மாறிலி Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& localeDisplayName(மாறிலி எழுத்து* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& languageDisplayName(மாறிலி எழுத்து* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& scriptDisplayName(மாறிலி எழுத்து* script,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& regionDisplayName(மாறிலி எழுத்து* region,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& variantDisplayName(மாறிலி எழுத்து* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& சாவி_Display_Name(மாறிலி எழுத்து* சாவி,
                                        UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
  மெய்நிகர் UnicodeString& சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* சாவி,
                                             மாறிலி எழுத்து* value,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

மாறிலி Locale&
DefaultLocaleDisplayNames::getLocale() மாறிலி U_LIFETIME_BOUND {
  திரும்பு Locale::get_வேர்();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() மாறிலி {
  திரும்பு dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(மாறிலி Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(மாறிலி எழுத்து* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(மாறிலி எழுத்து* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(மாறிலி எழுத்து* script,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  மாறிலி எழுத்து* name = uscript_getName(scriptCode);
  எனில் (name) {
    திரும்பு result = UnicodeString(name, -1, US_INV);
  }
  திரும்பு result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(மாறிலி எழுத்து* region,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(மாறிலி எழுத்து* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::சாவி_Display_Name(மாறிலி எழுத்து* சாவி,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(சாவி, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* /*சாவி*/,
                                               மாறிலி எழுத்து* value,
                                               UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  திரும்பு result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

வகுப்பு LocaleDisplayNamesImpl : பொது LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter சாவி_Type_Format;
    UDisplayContext capitalizationContext;
#எனில் !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#இல்லையெனில்
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    எண்ணிக்கை CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_சாவி,
        k_Cap_Context_Usage_சாவி_மதிப்பு,
        k_Cap_Context_Usage_மொத்தம்
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     தருக்க fCapitalization[k_Cap_Context_Usage_மொத்தம்];

பொது:
    // constructor
    LocaleDisplayNamesImpl(மாறிலி Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(மாறிலி Locale& locale, UDisplayContext *contexts, int32_t length);
    மெய்நிகர் ~LocaleDisplayNamesImpl();

    மெய்நிகர் மாறிலி Locale& getLocale() மாறிலி U_LIFETIME_BOUND override;
    மெய்நிகர் UDialectHandling getDialectHandling() மாறிலி override;
    மெய்நிகர் UDisplayContext getContext(UDisplayContextType type) மாறிலி override;

    மெய்நிகர் UnicodeString& localeDisplayName(மாறிலி Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString& localeDisplayName(மாறிலி எழுத்து* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString&
    languageDisplayName(மாறிலி எழுத்து* lang, UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString& scriptDisplayName(மாறிலி எழுத்து* script,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString& regionDisplayName(மாறிலி எழுத்து* region,
                                             UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString&
    variantDisplayName(மாறிலி எழுத்து* variant, UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString& சாவி_Display_Name(மாறிலி எழுத்து* சாவி,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;
    மெய்நிகர் UnicodeString&
    சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* சாவி, மாறிலி எழுத்து* value,
                        UnicodeString& result U_LIFETIME_BOUND) மாறிலி override;

தனிப்பட்ட:
    UnicodeString& localeIdName(மாறிலி எழுத்து* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                தருக்க substitute) மாறிலி;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 மாறிலி UnicodeString& src) மாறிலி;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) மாறிலி;
    UnicodeString& scriptDisplayName(மாறிலி எழுத்து* script, UnicodeString& result U_LIFETIME_BOUND,
                                     தருக்க skipAdjust) மாறிலி;
    UnicodeString& regionDisplayName(மாறிலி எழுத்து* region, UnicodeString& result U_LIFETIME_BOUND,
                                     தருக்க skipAdjust) மாறிலி;
    UnicodeString& variantDisplayName(மாறிலி எழுத்து* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      தருக்க skipAdjust) மாறிலி;
    UnicodeString& சாவி_Display_Name(மாறிலி எழுத்து* சாவி, UnicodeString& result U_LIFETIME_BOUND,
                                  தருக்க skipAdjust) மாறிலி;
    UnicodeString& சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* சாவி, மாறிலி எழுத்து* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       தருக்க skipAdjust) மாறிலி;
    வெறுமை initialize();

    கட்டமைப்பு CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(மாறிலி Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(மாறிலி Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    போது (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        தேர்வு (selector) {
            நிலை UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                நிறுத்து;
            நிலை UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                நிறுத்து;
            நிலை UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                நிறுத்து;
            நிலை UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                நிறுத்து;
            இயல்பு:
                நிறுத்து;
        }
    }
    initialize();
}

கட்டமைப்பு LocaleDisplayNamesImpl::CapitalizationContextSink : பொது ResourceSink {
    தருக்க hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(பொய்), parent(_parent) {}
    மெய்நிகர் ~CapitalizationContextSink();

    மெய்நிகர் வெறுமை put(மாறிலி எழுத்து *சாவி, Resource_மதிப்பு &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        க்கு (முழு i = 0; contexts.get_சாவி_And_மதிப்பு(i, சாவி, value); ++i) {

            CapContextUsage usageEnum;
            எனில் (uprv_strcmp(சாவி, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_சாவி;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_சாவி_மதிப்பு;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } இல்லையெனில் எனில் (uprv_strcmp(சாவி, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } இல்லையெனில் {
                தொடர்;
            }

            int32_t len = 0;
            மாறிலி int32_t* intVector = value.getIntVector(len, errorCode);
            எனில் (U_FAILURE(errorCode)) { திரும்பு; }
            எனில் (len < 2) { தொடர்; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            எனில் (titlecaseInt == 0) { தொடர்; }

            parent.fCapitalization[usageEnum] = உண்மை;
            hasCapitalizationUsage = உண்மை;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

வெறுமை
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = இது;
    nonConstThis->locale = langData.getLocale() == Locale::get_வேர்()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    எனில் (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    எனில் (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    எனில் (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } இல்லையெனில் {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    எனில் (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    சாவி_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, அளவு(fCapitalization));
#எனில் !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    தருக்க needBrkIter = பொய்;
    எனில் (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        எனில் (U_FAILURE(status)) { திரும்பு; }
        CapitalizationContextSink sink(*இது);
        ures_get_All_பொருட்கள்_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        எனில் (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } இல்லையெனில் எனில் (U_FAILURE(status)) {
            திரும்பு;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    எனில் (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        எனில் (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#எனில் !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

மாறிலி Locale&
LocaleDisplayNamesImpl::getLocale() மாறிலி U_LIFETIME_BOUND {
    திரும்பு locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() மாறிலி {
    திரும்பு dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) மாறிலி {
    தேர்வு (type) {
        நிலை UDISPCTX_TYPE_DIALECT_HANDLING:
            திரும்பு static_cast<UDisplayContext>(dialectHandling);
        நிலை UDISPCTX_TYPE_CAPITALIZATION:
            திரும்பு capitalizationContext;
        நிலை UDISPCTX_TYPE_DISPLAY_LENGTH:
            திரும்பு nameLength;
        நிலை UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            திரும்பு substitute;
        இயல்பு:
            நிறுத்து;
    }
    திரும்பு static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
#எனில் !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    எனில் ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        நிலையான UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    திரும்பு result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(மாறிலி Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
  எனில் (loc.isBogus()) {
    result.setToBogus();
    திரும்பு result;
  }
  UnicodeString resultName;

  மாறிலி எழுத்து* lang = loc.getLanguage();
  எனில் (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  மாறிலி எழுத்து* script = loc.getScript();
  மாறிலி எழுத்து* country = loc.getCountry();
  மாறிலி எழுத்து* variant = loc.getVariant();

  தருக்க hasScript = uprv_strlen(script) > 0;
  தருக்க hasCountry = uprv_strlen(country) > 0;
  தருக்க hasVariant = uprv_strlen(variant) > 0;

  எனில் (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    செய் { // loop construct is so we can break early out of search
      எனில் (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        எனில் (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, பொய்);
          எனில் (!resultName.isBogus()) {
            hasScript = பொய்;
            hasCountry = பொய்;
            நிறுத்து;
          }
        }
      }
      எனில் (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        எனில் (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, பொய்);
          எனில் (!resultName.isBogus()) {
            hasScript = பொய்;
            நிறுத்து;
          }
        }
      }
      எனில் (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        எனில் (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, பொய்);
          எனில் (!resultName.isBogus()) {
            hasCountry = பொய்;
            நிறுத்து;
          }
        }
      }
    } போது (பொய்);
  }
  எனில் (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    எனில் (resultName.isBogus()) {
      result.setToBogus();
      திரும்பு result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  எனில் (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, உண்மை);
    எனில் (script_str.isBogus()) {
      result.setToBogus();
      திரும்பு result;
    }
    resultRemainder.append(script_str);
  }
  எனில் (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, உண்மை);
    எனில் (region_str.isBogus()) {
      result.setToBogus();
      திரும்பு result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  எனில் (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, உண்மை);
    எனில் (variant_str.isBogus()) {
      result.setToBogus();
      திரும்பு result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  எனில் (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    மாறிலி எழுத்து* சாவி;
    போது ((சாவி = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_மதிப்பு<CharString>(சாவி, status);
        எனில் (U_FAILURE(status)) {
            திரும்பு result;
      }
      சாவி_Display_Name(சாவி, temp, உண்மை);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      சாவி_மதிப்பு_Display_Name(சாவி, value.data(), temp2, உண்மை);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      எனில் (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } இல்லையெனில் எனில் (temp != UnicodeString(சாவி, -1, US_INV)) {
        UnicodeString temp3;
        சாவி_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } இல்லையெனில் {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  எனில் (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    திரும்பு adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  திரும்பு adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, மாறிலி UnicodeString& src) மாறிலி {
    எனில் (buffer.isEmpty()) {
        buffer.setTo(src);
    } இல்லையெனில் {
        மாறிலி UnicodeString *மதிப்புகள்[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(மதிப்புகள், 2, buffer, nullptr, 0, status);
    }
    திரும்பு buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(மாறிலி எழுத்து* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(மாறிலி எழுத்து* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, தருக்க substitute) மாறிலி {
    எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        எனில் (!result.isBogus()) {
            திரும்பு result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    எனில் (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        மாறிலி எழுத்து* canonLocId = canonLocale.getName();
        எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            எனில் (!result.isBogus()) {
                திரும்பு result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    எனில் (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    திரும்பு result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(மாறிலி எழுத்து* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    எனில் (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        திரும்பு result = UnicodeString(lang, -1, US_INV);
    }
    எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        எனில் (!result.isBogus()) {
            திரும்பு adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    எனில் (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        மாறிலி எழுத்து* canonLocId = canonLocale.getName();
        எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            எனில் (!result.isBogus()) {
                திரும்பு adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    எனில் (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    திரும்பு adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(மாறிலி எழுத்து* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          தருக்க skipAdjust) மாறிலி {
    எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        எனில் (!result.isBogus()) {
            திரும்பு skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    எனில் (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } இல்லையெனில் {
        langData.getNoFallback("Scripts", script, result);
    }
    திரும்பு skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(மாறிலி எழுத்து* script,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு scriptDisplayName(script, result, பொய்);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு scriptDisplayName(uscript_getName(scriptCode), result, பொய்);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(மாறிலி எழுத்து* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          தருக்க skipAdjust) மாறிலி {
    எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        எனில் (!result.isBogus()) {
            திரும்பு skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    எனில் (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } இல்லையெனில் {
        regionData.getNoFallback("Countries", region, result);
    }
    திரும்பு skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(மாறிலி எழுத்து* region,
                                          UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு regionDisplayName(region, result, பொய்);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(மாறிலி எழுத்து* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           தருக்க skipAdjust) மாறிலி {
    // don't have a resource for short variant names
    எனில் (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } இல்லையெனில் {
        langData.getNoFallback("Variants", variant, result);
    }
    திரும்பு skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(மாறிலி எழுத்து* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு variantDisplayName(variant, result, பொய்);
}

UnicodeString&
LocaleDisplayNamesImpl::சாவி_Display_Name(மாறிலி எழுத்து* சாவி,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       தருக்க skipAdjust) மாறிலி {
    // don't have a resource for short key names
    எனில் (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", சாவி, result);
    } இல்லையெனில் {
        langData.getNoFallback("Keys", சாவி, result);
    }
    திரும்பு skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_சாவி, result);
}

UnicodeString&
LocaleDisplayNamesImpl::சாவி_Display_Name(மாறிலி எழுத்து* சாவி,
                                       UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு சாவி_Display_Name(சாவி, result, பொய்);
}

UnicodeString&
LocaleDisplayNamesImpl::சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* சாவி,
                                            மாறிலி எழுத்து* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            தருக்க skipAdjust) மாறிலி {
    எனில் (uprv_strcmp(சாவி, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_மதிப்பு(value, -1, US_INV);
        int32_t len;
        மாறிலி char16_t *currencyName = ucurr_getName(ustr_மதிப்பு.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        எனில் (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_மதிப்பு;
            திரும்பு result;
        }
        result.setTo(currencyName, len);
        திரும்பு skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_சாவி_மதிப்பு, result);
    }

    எனில் (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", சாவி, value, result);
        எனில் (!result.isBogus()) {
            திரும்பு skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_சாவி_மதிப்பு, result);
        }
    }
    எனில் (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", சாவி, value, result);
    } இல்லையெனில் {
        langData.getNoFallback("Types", சாவி, value, result);
    }
    திரும்பு skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_சாவி_மதிப்பு, result);
}

UnicodeString&
LocaleDisplayNamesImpl::சாவி_மதிப்பு_Display_Name(மாறிலி எழுத்து* சாவி,
                                            மாறிலி எழுத்து* value,
                                            UnicodeString& result U_LIFETIME_BOUND) மாறிலி {
    திரும்பு சாவி_மதிப்பு_Display_Name(சாவி, value, result, பொய்);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(மாறிலி Locale& locale,
                                   UDialectHandling dialectHandling) {
    திரும்பு புதிய LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(மாறிலி Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    எனில் (contexts == nullptr) {
        length = 0;
    }
    திரும்பு புதிய LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(மாறிலி எழுத்து * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு nullptr;
  }
  எனில் (locale == nullptr) {
    locale = uloc_getDefault();
  }
  திரும்பு (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(மாறிலி எழுத்து * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு nullptr;
  }
  எனில் (locale == nullptr) {
    locale = uloc_getDefault();
  }
  திரும்பு (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI வெறுமை U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uldn_getLocale(மாறிலி ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  எனில் (ldn) {
    திரும்பு ((மாறிலி LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  திரும்பு nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(மாறிலி ULocaleDisplayNames *ldn) {
  எனில் (ldn) {
    திரும்பு ((மாறிலி LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  திரும்பு ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(மாறிலி ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு (UDisplayContext)0;
  }
  திரும்பு ((மாறிலி LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                       மாறிலி எழுத்து *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  எனில் (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                         மாறிலி எழுத்து *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                       மாறிலி எழுத்து *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  திரும்பு uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                       மாறிலி எழுத்து *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(மாறிலி ULocaleDisplayNames *ldn,
                        மாறிலி எழுத்து *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_சாவி_Display_Name(மாறிலி ULocaleDisplayNames *ldn,
                    மாறிலி எழுத்து *சாவி,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || சாவி == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->சாவி_Display_Name(சாவி, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_சாவி_மதிப்பு_Display_Name(மாறிலி ULocaleDisplayNames *ldn,
                         மாறிலி எழுத்து *சாவி,
                         மாறிலி எழுத்து *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  எனில் (U_FAILURE(*pErrorCode)) {
    திரும்பு 0;
  }
  எனில் (ldn == nullptr || சாவி == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    திரும்பு 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((மாறிலி LocaleDisplayNames *)ldn)->சாவி_மதிப்பு_Display_Name(சாவி, value, temp);
  திரும்பு temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
