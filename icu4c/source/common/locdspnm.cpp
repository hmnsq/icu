// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_FORMATTING

#അന്തര്ഭാവ "unicode/locdspnm.h"
#അന്തര്ഭാവ "unicode/simpleformatter.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/udisplaycontext.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/ucurr.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "ureslocs.h"
#അന്തര്ഭാവ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
വർഗ്ഗം ICUDataTable {
    സ്ഥിരം അക്ഷരം* സ്ഥിരം path;
    Locale locale;

പൊതു:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം Locale& locale);
    ~ICUDataTable() = സ്ഥിരസ്ഥിതി;

    സ്ഥിരം Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                       UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
    UnicodeString& get(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* sub_Table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                       UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;

    UnicodeString& getNoFallback(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                                 UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
    UnicodeString& getNoFallback(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* sub_Table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                                 UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
};

വരിയിൽ UnicodeString& ICUDataTable::get(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                                        UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ get(table_താക്കോൽ, nullptr, item_താക്കോൽ, result);
}

വരിയിൽ UnicodeString& ICUDataTable::getNoFallback(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                                                  UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ getNoFallback(table_താക്കോൽ, nullptr, item_താക്കോൽ, result);
}

ICUDataTable::ICUDataTable(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

സ്ഥിരം Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  തിരികെ locale;
}

UnicodeString&
ICUDataTable::get(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* sub_Table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                  UnicodeString &result U_LIFETIME_BOUND) സ്ഥിരം {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  സ്ഥിരം char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_താക്കോൽ, sub_Table_താക്കോൽ, item_താക്കോൽ,
                                                   &len, &status);
  എങ്കിൽ (U_SUCCESS(status) && len > 0) {
    തിരികെ result.setTo(s, len);
  }
  തിരികെ result.setTo(UnicodeString(item_താക്കോൽ, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(സ്ഥിരം അക്ഷരം* table_താക്കോൽ, സ്ഥിരം അക്ഷരം* sub_Table_താക്കോൽ, സ്ഥിരം അക്ഷരം* item_താക്കോൽ,
                            UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  സ്ഥിരം char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_താക്കോൽ, sub_Table_താക്കോൽ, item_താക്കോൽ,
                                                   &len, &status);
  എങ്കിൽ (U_SUCCESS(status)) {
    തിരികെ result.setTo(s, len);
  }

  result.setToBogus();
  തിരികെ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#എങ്കിൽ 0  // currently unused

വർഗ്ഗം DefaultLocaleDisplayNames : പൊതു LocaleDisplayNames {
  UDialectHandling dialectHandling;

പൊതു:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  മായാ ~DefaultLocaleDisplayNames();

  മായാ സ്ഥിരം Locale& getLocale() സ്ഥിരം U_LIFETIME_BOUND;
  മായാ UDialectHandling getDialectHandling() സ്ഥിരം;

  മായാ UnicodeString& localeDisplayName(സ്ഥിരം Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& localeDisplayName(സ്ഥിരം അക്ഷരം* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& languageDisplayName(സ്ഥിരം അക്ഷരം* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& variantDisplayName(സ്ഥിരം അക്ഷരം* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                        UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
  മായാ UnicodeString& താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                             സ്ഥിരം അക്ഷരം* value,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

സ്ഥിരം Locale&
DefaultLocaleDisplayNames::getLocale() സ്ഥിരം U_LIFETIME_BOUND {
  തിരികെ Locale::get_വേര്();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() സ്ഥിരം {
  തിരികെ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(സ്ഥിരം Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(സ്ഥിരം അക്ഷരം* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(സ്ഥിരം അക്ഷരം* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  സ്ഥിരം അക്ഷരം* name = uscript_getName(scriptCode);
  എങ്കിൽ (name) {
    തിരികെ result = UnicodeString(name, -1, US_INV);
  }
  തിരികെ result.remove();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(സ്ഥിരം അക്ഷരം* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(താക്കോൽ, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* /*താക്കോൽ*/,
                                               സ്ഥിരം അക്ഷരം* value,
                                               UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  തിരികെ result = UnicodeString(value, -1, US_INV);
}

#endif  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

വർഗ്ഗം LocaleDisplayNamesImpl : പൊതു LocaleDisplayNames {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUDataTable langData;
    ICUDataTable regionData;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter താക്കോൽ_Type_Format;
    UDisplayContext capitalizationContext;
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#അല്ലെങ്കിൽ
    UObject* capitalizationBrkIter;
#endif
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext nameLength;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    എണ്ണൽ CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_താക്കോൽ,
        k_Cap_Context_Usage_താക്കോൽ_മൂല്യം,
        k_Cap_Context_Usage_എണ്ണം
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     തർക്കം fCapitalization[k_Cap_Context_Usage_എണ്ണം];

പൊതു:
    // constructor
    LocaleDisplayNamesImpl(സ്ഥിരം Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(സ്ഥിരം Locale& locale, UDisplayContext *contexts, int32_t length);
    മായാ ~LocaleDisplayNamesImpl();

    മായാ സ്ഥിരം Locale& getLocale() സ്ഥിരം U_LIFETIME_BOUND override;
    മായാ UDialectHandling getDialectHandling() സ്ഥിരം override;
    മായാ UDisplayContext getContext(UDisplayContextType type) സ്ഥിരം override;

    മായാ UnicodeString& localeDisplayName(സ്ഥിരം Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString& localeDisplayName(സ്ഥിരം അക്ഷരം* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString&
    languageDisplayName(സ്ഥിരം അക്ഷരം* lang, UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString& scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString& regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString&
    variantDisplayName(സ്ഥിരം അക്ഷരം* variant, UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString& താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;
    മായാ UnicodeString&
    താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ, സ്ഥിരം അക്ഷരം* value,
                        UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം override;

സ്വകാര്യ:
    UnicodeString& localeIdName(സ്ഥിരം അക്ഷരം* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                തർക്കം substitute) സ്ഥിരം;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 സ്ഥിരം UnicodeString& src) സ്ഥിരം;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം;
    UnicodeString& scriptDisplayName(സ്ഥിരം അക്ഷരം* script, UnicodeString& result U_LIFETIME_BOUND,
                                     തർക്കം skipAdjust) സ്ഥിരം;
    UnicodeString& regionDisplayName(സ്ഥിരം അക്ഷരം* region, UnicodeString& result U_LIFETIME_BOUND,
                                     തർക്കം skipAdjust) സ്ഥിരം;
    UnicodeString& variantDisplayName(സ്ഥിരം അക്ഷരം* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      തർക്കം skipAdjust) സ്ഥിരം;
    UnicodeString& താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ, UnicodeString& result U_LIFETIME_BOUND,
                                  തർക്കം skipAdjust) സ്ഥിരം;
    UnicodeString& താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ, സ്ഥിരം അക്ഷരം* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       തർക്കം skipAdjust) സ്ഥിരം;
    ശൂന്യം initialize();

    ഘടന CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(സ്ഥിരം Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(സ്ഥിരം Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    ആകുമ്പോൾ (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        തിരഞ്ഞെടുപ്പ് (selector) {
            അവസ്ഥ UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                നിർത്തുക;
            അവസ്ഥ UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                നിർത്തുക;
            അവസ്ഥ UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                നിർത്തുക;
            അവസ്ഥ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                നിർത്തുക;
        }
    }
    initialize();
}

ഘടന LocaleDisplayNamesImpl::CapitalizationContextSink : പൊതു ResourceSink {
    തർക്കം hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(അസത്യം), parent(_parent) {}
    മായാ ~CapitalizationContextSink();

    മായാ ശൂന്യം put(സ്ഥിരം അക്ഷരം *താക്കോൽ, Resource_മൂല്യം &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; contexts.get_താക്കോൽ_And_മൂല്യം(i, താക്കോൽ, value); ++i) {

            CapContextUsage usageEnum;
            എങ്കിൽ (uprv_strcmp(താക്കോൽ, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_താക്കോൽ;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_താക്കോൽ_മൂല്യം;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } അല്ലെങ്കിൽ {
                തുടരുക;
            }

            int32_t len = 0;
            സ്ഥിരം int32_t* intVector = value.getIntVector(len, errorCode);
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
            എങ്കിൽ (len < 2) { തുടരുക; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            എങ്കിൽ (titlecaseInt == 0) { തുടരുക; }

            parent.fCapitalization[usageEnum] = സത്യം;
            hasCapitalizationUsage = സത്യം;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

ശൂന്യം
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = ഇത്;
    nonConstThis->locale = langData.getLocale() == Locale::get_വേര്()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    എങ്കിൽ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    എങ്കിൽ (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    എങ്കിൽ (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.setTo(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.setTo(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } അല്ലെങ്കിൽ {
        formatOpenParen.setTo(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.setTo(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.setTo(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.setTo(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString ktPattern;
    langData.get("localeDisplayPattern", "keyTypePattern", ktPattern);
    എങ്കിൽ (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    താക്കോൽ_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, വലുപ്പം(fCapitalization));
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    തർക്കം needBrkIter = അസത്യം;
    എങ്കിൽ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        CapitalizationContextSink sink(*ഇത്);
        ures_get_All_ഇനങ്ങൾ_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        എങ്കിൽ (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } അല്ലെങ്കിൽ എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    എങ്കിൽ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        എങ്കിൽ (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

സ്ഥിരം Locale&
LocaleDisplayNamesImpl::getLocale() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() സ്ഥിരം {
    തിരികെ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) സ്ഥിരം {
    തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ UDISPCTX_TYPE_DIALECT_HANDLING:
            തിരികെ static_cast<UDisplayContext>(dialectHandling);
        അവസ്ഥ UDISPCTX_TYPE_CAPITALIZATION:
            തിരികെ capitalizationContext;
        അവസ്ഥ UDISPCTX_TYPE_DISPLAY_LENGTH:
            തിരികെ nameLength;
        അവസ്ഥ UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            തിരികെ substitute;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക;
    }
    തിരികെ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    എങ്കിൽ ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        സ്ഥിതം UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    തിരികെ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(സ്ഥിരം Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
  എങ്കിൽ (loc.isBogus()) {
    result.setToBogus();
    തിരികെ result;
  }
  UnicodeString resultName;

  സ്ഥിരം അക്ഷരം* lang = loc.getLanguage();
  എങ്കിൽ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  സ്ഥിരം അക്ഷരം* script = loc.getScript();
  സ്ഥിരം അക്ഷരം* country = loc.getCountry();
  സ്ഥിരം അക്ഷരം* variant = loc.getVariant();

  തർക്കം hasScript = uprv_strlen(script) > 0;
  തർക്കം hasCountry = uprv_strlen(country) > 0;
  തർക്കം hasVariant = uprv_strlen(variant) > 0;

  എങ്കിൽ (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    ചെയ്യുക { // loop construct is so we can break early out of search
      എങ്കിൽ (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        എങ്കിൽ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, അസത്യം);
          എങ്കിൽ (!resultName.isBogus()) {
            hasScript = അസത്യം;
            hasCountry = അസത്യം;
            നിർത്തുക;
          }
        }
      }
      എങ്കിൽ (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        എങ്കിൽ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, അസത്യം);
          എങ്കിൽ (!resultName.isBogus()) {
            hasScript = അസത്യം;
            നിർത്തുക;
          }
        }
      }
      എങ്കിൽ (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        എങ്കിൽ (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, അസത്യം);
          എങ്കിൽ (!resultName.isBogus()) {
            hasCountry = അസത്യം;
            നിർത്തുക;
          }
        }
      }
    } ആകുമ്പോൾ (അസത്യം);
  }
  എങ്കിൽ (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    എങ്കിൽ (resultName.isBogus()) {
      result.setToBogus();
      തിരികെ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  എങ്കിൽ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, സത്യം);
    എങ്കിൽ (script_str.isBogus()) {
      result.setToBogus();
      തിരികെ result;
    }
    resultRemainder.append(script_str);
  }
  എങ്കിൽ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, സത്യം);
    എങ്കിൽ (region_str.isBogus()) {
      result.setToBogus();
      തിരികെ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  എങ്കിൽ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, സത്യം);
    എങ്കിൽ (variant_str.isBogus()) {
      result.setToBogus();
      തിരികെ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  എങ്കിൽ (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    സ്ഥിരം അക്ഷരം* താക്കോൽ;
    ആകുമ്പോൾ ((താക്കോൽ = e->next((int32_t*)nullptr, status)) != nullptr) {
        auto value = loc.get_Keyword_മൂല്യം<CharString>(താക്കോൽ, status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ result;
      }
      താക്കോൽ_Display_Name(താക്കോൽ, temp, സത്യം);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      താക്കോൽ_മൂല്യം_Display_Name(താക്കോൽ, value.data(), temp2, സത്യം);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      എങ്കിൽ (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } അല്ലെങ്കിൽ എങ്കിൽ (temp != UnicodeString(താക്കോൽ, -1, US_INV)) {
        UnicodeString temp3;
        താക്കോൽ_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } അല്ലെങ്കിൽ {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  എങ്കിൽ (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.remove(), status);
    തിരികെ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  തിരികെ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, സ്ഥിരം UnicodeString& src) സ്ഥിരം {
    എങ്കിൽ (buffer.isEmpty()) {
        buffer.setTo(src);
    } അല്ലെങ്കിൽ {
        സ്ഥിരം UnicodeString *മൂല്യങ്ങൾ[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(മൂല്യങ്ങൾ, 2, buffer, nullptr, 0, status);
    }
    തിരികെ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(സ്ഥിരം അക്ഷരം* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(സ്ഥിരം അക്ഷരം* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, തർക്കം substitute) സ്ഥിരം {
    എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        എങ്കിൽ (!result.isBogus()) {
            തിരികെ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    എങ്കിൽ (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        സ്ഥിരം അക്ഷരം* canonLocId = canonLocale.getName();
        എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            എങ്കിൽ (!result.isBogus()) {
                തിരികെ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    എങ്കിൽ (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    തിരികെ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(സ്ഥിരം അക്ഷരം* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    എങ്കിൽ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        തിരികെ result = UnicodeString(lang, -1, US_INV);
    }
    എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        എങ്കിൽ (!result.isBogus()) {
            തിരികെ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    എങ്കിൽ (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        സ്ഥിരം അക്ഷരം* canonLocId = canonLocale.getName();
        എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            എങ്കിൽ (!result.isBogus()) {
                തിരികെ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    എങ്കിൽ (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    തിരികെ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          തർക്കം skipAdjust) സ്ഥിരം {
    എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        എങ്കിൽ (!result.isBogus()) {
            തിരികെ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    എങ്കിൽ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } അല്ലെങ്കിൽ {
        langData.getNoFallback("Scripts", script, result);
    }
    തിരികെ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ scriptDisplayName(script, result, അസത്യം);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ scriptDisplayName(uscript_getName(scriptCode), result, അസത്യം);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          തർക്കം skipAdjust) സ്ഥിരം {
    എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        എങ്കിൽ (!result.isBogus()) {
            തിരികെ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    എങ്കിൽ (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } അല്ലെങ്കിൽ {
        regionData.getNoFallback("Countries", region, result);
    }
    തിരികെ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ regionDisplayName(region, result, അസത്യം);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(സ്ഥിരം അക്ഷരം* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           തർക്കം skipAdjust) സ്ഥിരം {
    // don't have a resource for short variant names
    എങ്കിൽ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } അല്ലെങ്കിൽ {
        langData.getNoFallback("Variants", variant, result);
    }
    തിരികെ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(സ്ഥിരം അക്ഷരം* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ variantDisplayName(variant, result, അസത്യം);
}

UnicodeString&
LocaleDisplayNamesImpl::താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       തർക്കം skipAdjust) സ്ഥിരം {
    // don't have a resource for short key names
    എങ്കിൽ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", താക്കോൽ, result);
    } അല്ലെങ്കിൽ {
        langData.getNoFallback("Keys", താക്കോൽ, result);
    }
    തിരികെ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_താക്കോൽ, result);
}

UnicodeString&
LocaleDisplayNamesImpl::താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                       UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ താക്കോൽ_Display_Name(താക്കോൽ, result, അസത്യം);
}

UnicodeString&
LocaleDisplayNamesImpl::താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                            സ്ഥിരം അക്ഷരം* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            തർക്കം skipAdjust) സ്ഥിരം {
    എങ്കിൽ (uprv_strcmp(താക്കോൽ, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_മൂല്യം(value, -1, US_INV);
        int32_t len;
        സ്ഥിരം char16_t *currencyName = ucurr_getName(ustr_മൂല്യം.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        എങ്കിൽ (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_മൂല്യം;
            തിരികെ result;
        }
        result.setTo(currencyName, len);
        തിരികെ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_താക്കോൽ_മൂല്യം, result);
    }

    എങ്കിൽ (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", താക്കോൽ, value, result);
        എങ്കിൽ (!result.isBogus()) {
            തിരികെ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_താക്കോൽ_മൂല്യം, result);
        }
    }
    എങ്കിൽ (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", താക്കോൽ, value, result);
    } അല്ലെങ്കിൽ {
        langData.getNoFallback("Types", താക്കോൽ, value, result);
    }
    തിരികെ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_താക്കോൽ_മൂല്യം, result);
}

UnicodeString&
LocaleDisplayNamesImpl::താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                            സ്ഥിരം അക്ഷരം* value,
                                            UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം {
    തിരികെ താക്കോൽ_മൂല്യം_Display_Name(താക്കോൽ, value, result, അസത്യം);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(സ്ഥിരം Locale& locale,
                                   UDialectHandling dialectHandling) {
    തിരികെ പുതിയ LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(സ്ഥിരം Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    എങ്കിൽ (contexts == nullptr) {
        length = 0;
    }
    തിരികെ പുതിയ LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(സ്ഥിരം അക്ഷരം * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  തിരികെ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(സ്ഥിരം അക്ഷരം * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ (locale == nullptr) {
    locale = uloc_getDefault();
  }
  തിരികെ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI ശൂന്യം U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uldn_getLocale(സ്ഥിരം ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  എങ്കിൽ (ldn) {
    തിരികെ ((സ്ഥിരം LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  തിരികെ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(സ്ഥിരം ULocaleDisplayNames *ldn) {
  എങ്കിൽ (ldn) {
    തിരികെ ((സ്ഥിരം LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  തിരികെ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(സ്ഥിരം ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ (UDisplayContext)0;
  }
  തിരികെ ((സ്ഥിരം LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                       സ്ഥിരം അക്ഷരം *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  എങ്കിൽ (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                         സ്ഥിരം അക്ഷരം *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                       സ്ഥിരം അക്ഷരം *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  തിരികെ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                       സ്ഥിരം അക്ഷരം *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(സ്ഥിരം ULocaleDisplayNames *ldn,
                        സ്ഥിരം അക്ഷരം *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_താക്കോൽ_Display_Name(സ്ഥിരം ULocaleDisplayNames *ldn,
                    സ്ഥിരം അക്ഷരം *താക്കോൽ,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || താക്കോൽ == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->താക്കോൽ_Display_Name(താക്കോൽ, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം ULocaleDisplayNames *ldn,
                         സ്ഥിരം അക്ഷരം *താക്കോൽ,
                         സ്ഥിരം അക്ഷരം *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  എങ്കിൽ (U_FAILURE(*pErrorCode)) {
    തിരികെ 0;
  }
  എങ്കിൽ (ldn == nullptr || താക്കോൽ == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((സ്ഥിരം LocaleDisplayNames *)ldn)->താക്കോൽ_മൂല്യം_Display_Name(താക്കോൽ, value, temp);
  തിരികെ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
