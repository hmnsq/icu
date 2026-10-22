// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

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
    अचर वर्ण* अचर path;
    Locale locale;

सार्वजनिक:
    // Note: path should be a pointer to a statically allocated string.
    ICUDataTable(अचर वर्ण* path, अचर Locale& locale);
    ~ICUDataTable() = स्वभाव;

    अचर Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(अचर वर्ण* table_साँचो, अचर वर्ण* item_साँचो,
                       UnicodeString& result U_LIFETIME_BOUND) अचर;
    UnicodeString& get(अचर वर्ण* table_साँचो, अचर वर्ण* sub_Table_साँचो, अचर वर्ण* item_साँचो,
                       UnicodeString& result U_LIFETIME_BOUND) अचर;

    UnicodeString& getNoFallback(अचर वर्ण* table_साँचो, अचर वर्ण* item_साँचो,
                                 UnicodeString& result U_LIFETIME_BOUND) अचर;
    UnicodeString& getNoFallback(अचर वर्ण* table_साँचो, अचर वर्ण* sub_Table_साँचो, अचर वर्ण* item_साँचो,
                                 UnicodeString& result U_LIFETIME_BOUND) अचर;
};

अन्तःस्थ UnicodeString& ICUDataTable::get(अचर वर्ण* table_साँचो, अचर वर्ण* item_साँचो,
                                        UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ get(table_साँचो, nullptr, item_साँचो, result);
}

अन्तःस्थ UnicodeString& ICUDataTable::getNoFallback(अचर वर्ण* table_साँचो, अचर वर्ण* item_साँचो,
                                                  UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ getNoFallback(table_साँचो, nullptr, item_साँचो, result);
}

ICUDataTable::ICUDataTable(अचर वर्ण* path, अचर Locale& locale)
    : path(path), locale(locale)
{
    U_ASSERT(path != nullptr);
}

अचर Locale&
ICUDataTable::getLocale() U_LIFETIME_BOUND {
  फर्काऊ locale;
}

UnicodeString&
ICUDataTable::get(अचर वर्ण* table_साँचो, अचर वर्ण* sub_Table_साँचो, अचर वर्ण* item_साँचो,
                  UnicodeString &result U_LIFETIME_BOUND) अचर {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  अचर char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_साँचो, sub_Table_साँचो, item_साँचो,
                                                   &len, &status);
  यदि (U_SUCCESS(status) && len > 0) {
    फर्काऊ result.setTo(s, len);
  }
  फर्काऊ result.setTo(UnicodeString(item_साँचो, -1, US_INV));
}

UnicodeString&
ICUDataTable::getNoFallback(अचर वर्ण* table_साँचो, अचर वर्ण* sub_Table_साँचो, अचर वर्ण* item_साँचो,
                            UnicodeString& result U_LIFETIME_BOUND) अचर {
  UErrorCode status = U_ZERO_ERROR;
  int32_t len = 0;

  अचर char16_t *s = uloc_getTableStringWithFallback(path, locale.getName(),
                                                   table_साँचो, sub_Table_साँचो, item_साँचो,
                                                   &len, &status);
  यदि (U_SUCCESS(status)) {
    फर्काऊ result.setTo(s, len);
  }

  result.setToBogus();
  फर्काऊ result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames::~LocaleDisplayNames() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#यदि 0  // currently unused

वर्ग DefaultLocaleDisplayNames : सार्वजनिक LocaleDisplayNames {
  UDialectHandling dialectHandling;

सार्वजनिक:
  // constructor
  DefaultLocaleDisplayNames(UDialectHandling dialectHandling);

  आभास ~DefaultLocaleDisplayNames();

  आभास अचर Locale& getLocale() अचर U_LIFETIME_BOUND;
  आभास UDialectHandling getDialectHandling() अचर;

  आभास UnicodeString& localeDisplayName(अचर Locale& locale,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& localeDisplayName(अचर वर्ण* localeId,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& languageDisplayName(अचर वर्ण* lang,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& scriptDisplayName(अचर वर्ण* script,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& regionDisplayName(अचर वर्ण* region,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& variantDisplayName(अचर वर्ण* variant,
                                            UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& साँचो_Display_Name(अचर वर्ण* साँचो,
                                        UnicodeString& result U_LIFETIME_BOUND) अचर;
  आभास UnicodeString& साँचो_मान_Display_Name(अचर वर्ण* साँचो,
                                             अचर वर्ण* value,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर;
};

DefaultLocaleDisplayNames::DefaultLocaleDisplayNames(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

DefaultLocaleDisplayNames::~DefaultLocaleDisplayNames() {
}

अचर Locale&
DefaultLocaleDisplayNames::getLocale() अचर U_LIFETIME_BOUND {
  फर्काऊ Locale::get_जरा();
}

UDialectHandling
DefaultLocaleDisplayNames::getDialectHandling() अचर {
  फर्काऊ dialectHandling;
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(अचर Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::localeDisplayName(अचर वर्ण* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::languageDisplayName(अचर वर्ण* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(अचर वर्ण* script,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(script, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर {
  अचर वर्ण* name = uscript_getName(scriptCode);
  यदि (name) {
    फर्काऊ result = UnicodeString(name, -1, US_INV);
  }
  फर्काऊ result.हटाओ();
}

UnicodeString&
DefaultLocaleDisplayNames::regionDisplayName(अचर वर्ण* region,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(region, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::variantDisplayName(अचर वर्ण* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::साँचो_Display_Name(अचर वर्ण* साँचो,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(साँचो, -1, US_INV);
}

UnicodeString&
DefaultLocaleDisplayNames::साँचो_मान_Display_Name(अचर वर्ण* /*साँचो*/,
                                               अचर वर्ण* value,
                                               UnicodeString& result U_LIFETIME_BOUND) अचर {
  फर्काऊ result = UnicodeString(value, -1, US_INV);
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
    SimpleFormatter साँचो_Type_Format;
    UDisplayContext capitalizationContext;
#यदि !UCONFIG_NO_BREAK_ITERATION
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
        k_Cap_Context_Usage_साँचो,
        k_Cap_Context_Usage_साँचो_मान,
        k_Cap_Context_Usage_गन्ती
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     तार्किक fCapitalization[k_Cap_Context_Usage_गन्ती];

सार्वजनिक:
    // constructor
    LocaleDisplayNamesImpl(अचर Locale& locale, UDialectHandling dialectHandling);
    LocaleDisplayNamesImpl(अचर Locale& locale, UDisplayContext *contexts, int32_t length);
    आभास ~LocaleDisplayNamesImpl();

    आभास अचर Locale& getLocale() अचर U_LIFETIME_BOUND override;
    आभास UDialectHandling getDialectHandling() अचर override;
    आभास UDisplayContext getContext(UDisplayContextType type) अचर override;

    आभास UnicodeString& localeDisplayName(अचर Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString& localeDisplayName(अचर वर्ण* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString&
    languageDisplayName(अचर वर्ण* lang, UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString& scriptDisplayName(अचर वर्ण* script,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString& regionDisplayName(अचर वर्ण* region,
                                             UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString&
    variantDisplayName(अचर वर्ण* variant, UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString& साँचो_Display_Name(अचर वर्ण* साँचो,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर override;
    आभास UnicodeString&
    साँचो_मान_Display_Name(अचर वर्ण* साँचो, अचर वर्ण* value,
                        UnicodeString& result U_LIFETIME_BOUND) अचर override;

गुप्त:
    UnicodeString& localeIdName(अचर वर्ण* localeId, UnicodeString& result U_LIFETIME_BOUND,
                                तार्किक substitute) अचर;
    UnicodeString& appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND,
                                 अचर UnicodeString& src) अचर;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& result U_LIFETIME_BOUND) अचर;
    UnicodeString& scriptDisplayName(अचर वर्ण* script, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) अचर;
    UnicodeString& regionDisplayName(अचर वर्ण* region, UnicodeString& result U_LIFETIME_BOUND,
                                     तार्किक skipAdjust) अचर;
    UnicodeString& variantDisplayName(अचर वर्ण* variant, UnicodeString& result U_LIFETIME_BOUND,
                                      तार्किक skipAdjust) अचर;
    UnicodeString& साँचो_Display_Name(अचर वर्ण* साँचो, UnicodeString& result U_LIFETIME_BOUND,
                                  तार्किक skipAdjust) अचर;
    UnicodeString& साँचो_मान_Display_Name(अचर वर्ण* साँचो, अचर वर्ण* value,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) अचर;
    शून्य initialize();

    संरचना CapitalizationContextSink;
};

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(अचर Locale& locale,
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

LocaleDisplayNamesImpl::LocaleDisplayNamesImpl(अचर Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_NAMES)
    , langData(U_ICUDATA_LANG, locale)
    , regionData(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(nullptr)
    , nameLength(UDISPCTX_LENGTH_FULL)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    जबसम्म (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        विकल्प (selector) {
            पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                रोक;
            पक्ष UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                रोक;
            पक्ष UDISPCTX_TYPE_DISPLAY_LENGTH:
                nameLength = value;
                रोक;
            पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                रोक;
            स्वभाव:
                रोक;
        }
    }
    initialize();
}

संरचना LocaleDisplayNamesImpl::CapitalizationContextSink : सार्वजनिक ResourceSink {
    तार्किक hasCapitalizationUsage;
    LocaleDisplayNamesImpl& parent;

    CapitalizationContextSink(LocaleDisplayNamesImpl& _parent)
      : hasCapitalizationUsage(असत्य), parent(_parent) {}
    आभास ~CapitalizationContextSink();

    आभास शून्य put(अचर वर्ण *साँचो, Resource_मान &value, UBool /*noFallback*/,
            UErrorCode &errorCode) override {
        ResourceTable contexts = value.getTable(errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        लागि (पूर्णाङ्क i = 0; contexts.get_साँचो_And_मान(i, साँचो, value); ++i) {

            CapContextUsage usageEnum;
            यदि (uprv_strcmp(साँचो, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_साँचो;
            } अन्यथा यदि (uprv_strcmp(साँचो, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_साँचो_मान;
            } अन्यथा यदि (uprv_strcmp(साँचो, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } अन्यथा यदि (uprv_strcmp(साँचो, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } अन्यथा यदि (uprv_strcmp(साँचो, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } अन्यथा यदि (uprv_strcmp(साँचो, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } अन्यथा {
                जारी;
            }

            int32_t len = 0;
            अचर int32_t* intVector = value.getIntVector(len, errorCode);
            यदि (U_FAILURE(errorCode)) { फर्काऊ; }
            यदि (len < 2) { जारी; }

            int32_t titlecaseInt = (parent.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? intVector[0] : intVector[1];
            यदि (titlecaseInt == 0) { जारी; }

            parent.fCapitalization[usageEnum] = सत्य;
            hasCapitalizationUsage = सत्य;
        }
    }
};

// Virtual destructors must be defined out of line.
LocaleDisplayNamesImpl::CapitalizationContextSink::~CapitalizationContextSink() {}

शून्य
LocaleDisplayNamesImpl::initialize() {
    LocaleDisplayNamesImpl* nonConstThis = एतत्;
    nonConstThis->locale = langData.getLocale() == Locale::get_जरा()
        ? regionData.getLocale()
        : langData.getLocale();

    UnicodeString sep;
    langData.getNoFallback("localeDisplayPattern", "separator", sep);
    यदि (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UErrorCode status = U_ZERO_ERROR;
    separatorFormat.applyPatternMinMaxArguments(sep, 2, 2, status);

    UnicodeString pattern;
    langData.getNoFallback("localeDisplayPattern", "pattern", pattern);
    यदि (pattern.isBogus()) {
        pattern = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.applyPatternMinMaxArguments(pattern, 2, 2, status);
    यदि (pattern.indexOf(static_cast<char16_t>(0xFF08)) >= 0) {
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
    यदि (ktPattern.isBogus()) {
        ktPattern = UnicodeString("{0}={1}", -1, US_INV);
    }
    साँचो_Type_Format.applyPatternMinMaxArguments(ktPattern, 2, 2, status);

    uprv_memset(fCapitalization, 0, परिमाण(fCapitalization));
#यदि !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    तार्किक needBrkIter = असत्य;
    यदि (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        LocalUResourceBundlePointer resource(ures_open(nullptr, locale.getName(), &status));
        यदि (U_FAILURE(status)) { फर्काऊ; }
        CapitalizationContextSink sink(*एतत्);
        ures_get_All_वस्तुहरू_With_Fallback(resource.getAlias(), "contextTransforms", sink, status);
        यदि (status == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            status = U_ZERO_ERROR;
        } अन्यथा यदि (U_FAILURE(status)) {
            फर्काऊ;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    यदि (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE) {
        status = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, status);
        यदि (U_FAILURE(status)) {
            delete capitalizationBrkIter;
            capitalizationBrkIter = nullptr;
        }
    }
#endif
}

LocaleDisplayNamesImpl::~LocaleDisplayNamesImpl() {
#यदि !UCONFIG_NO_BREAK_ITERATION
    delete capitalizationBrkIter;
#endif
}

अचर Locale&
LocaleDisplayNamesImpl::getLocale() अचर U_LIFETIME_BOUND {
    फर्काऊ locale;
}

UDialectHandling
LocaleDisplayNamesImpl::getDialectHandling() अचर {
    फर्काऊ dialectHandling;
}

UDisplayContext
LocaleDisplayNamesImpl::getContext(UDisplayContextType type) अचर {
    विकल्प (type) {
        पक्ष UDISPCTX_TYPE_DIALECT_HANDLING:
            फर्काऊ static_cast<UDisplayContext>(dialectHandling);
        पक्ष UDISPCTX_TYPE_CAPITALIZATION:
            फर्काऊ capitalizationContext;
        पक्ष UDISPCTX_TYPE_DISPLAY_LENGTH:
            फर्काऊ nameLength;
        पक्ष UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            फर्काऊ substitute;
        स्वभाव:
            रोक;
    }
    फर्काऊ static_cast<UDisplayContext>(0);
}

UnicodeString&
LocaleDisplayNamesImpl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& result U_LIFETIME_BOUND) अचर {
#यदि !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    यदि ( result.length() > 0 && u_islower(result.char32At(0)) && capitalizationBrkIter!= nullptr &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        स्थिर UMutex capitalizationBrkIterLock;
        Mutex lock(&capitalizationBrkIterLock);
        result.toTitle(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#endif
    फर्काऊ result;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(अचर Locale& loc,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
  यदि (loc.isBogus()) {
    result.setToBogus();
    फर्काऊ result;
  }
  UnicodeString resultName;

  अचर वर्ण* lang = loc.getLanguage();
  यदि (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  अचर वर्ण* script = loc.getScript();
  अचर वर्ण* country = loc.getCountry();
  अचर वर्ण* variant = loc.getVariant();

  तार्किक hasScript = uprv_strlen(script) > 0;
  तार्किक hasCountry = uprv_strlen(country) > 0;
  तार्किक hasVariant = uprv_strlen(variant) > 0;

  यदि (dialectHandling == ULDN_DIALECT_NAMES) {
    UErrorCode status = U_ZERO_ERROR;
    CharString buffer;
    गर { // loop construct is so we can break early out of search
      यदि (hasScript && hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status)
              .append('_', status)
              .append(country, status);
        यदि (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          यदि (!resultName.isBogus()) {
            hasScript = असत्य;
            hasCountry = असत्य;
            रोक;
          }
        }
      }
      यदि (hasScript) {
        buffer.append(lang, status)
              .append('_', status)
              .append(script, status);
        यदि (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          यदि (!resultName.isBogus()) {
            hasScript = असत्य;
            रोक;
          }
        }
      }
      यदि (hasCountry) {
        buffer.append(lang, status)
              .append('_', status)
              .append(country, status);
        यदि (U_SUCCESS(status)) {
          localeIdName(buffer.data(), resultName, असत्य);
          यदि (!resultName.isBogus()) {
            hasCountry = असत्य;
            रोक;
          }
        }
      }
    } जबसम्म (असत्य);
  }
  यदि (resultName.isBogus() || resultName.isEmpty()) {
    localeIdName(lang, resultName, substitute == UDISPCTX_SUBSTITUTE);
    यदि (resultName.isBogus()) {
      result.setToBogus();
      फर्काऊ result;
    }
  }

  UnicodeString resultRemainder;
  UnicodeString temp;
  UErrorCode status = U_ZERO_ERROR;

  यदि (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, temp, सत्य);
    यदि (script_str.isBogus()) {
      result.setToBogus();
      फर्काऊ result;
    }
    resultRemainder.append(script_str);
  }
  यदि (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, temp, सत्य);
    यदि (region_str.isBogus()) {
      result.setToBogus();
      फर्काऊ result;
    }
    appendWithSep(resultRemainder, region_str);
  }
  यदि (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, temp, सत्य);
    यदि (variant_str.isBogus()) {
      result.setToBogus();
      फर्काऊ result;
    }
    appendWithSep(resultRemainder, variant_str);
  }
  resultRemainder.findAndReplace(formatOpenParen, formatReplaceOpenParen);
  resultRemainder.findAndReplace(formatCloseParen, formatReplaceCloseParen);

  LocalPointer<StringEnumeration> e(loc.createKeywords(status));
  यदि (e.isValid() && U_SUCCESS(status)) {
    UnicodeString temp2;
    अचर वर्ण* साँचो;
    जबसम्म ((साँचो = e->next((int32_t*)nullptr, status)) != nullptr) {
        स्वतः value = loc.get_Keyword_मान<CharString>(साँचो, status);
        यदि (U_FAILURE(status)) {
            फर्काऊ result;
      }
      साँचो_Display_Name(साँचो, temp, सत्य);
      temp.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      साँचो_मान_Display_Name(साँचो, value.data(), temp2, सत्य);
      temp2.findAndReplace(formatOpenParen, formatReplaceOpenParen);
      temp2.findAndReplace(formatCloseParen, formatReplaceCloseParen);
      यदि (temp2 != UnicodeString(value.data(), -1, US_INV)) {
        appendWithSep(resultRemainder, temp2);
      } अन्यथा यदि (temp != UnicodeString(साँचो, -1, US_INV)) {
        UnicodeString temp3;
        साँचो_Type_Format.format(temp, temp2, temp3, status);
        appendWithSep(resultRemainder, temp3);
      } अन्यथा {
        appendWithSep(resultRemainder, temp)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  यदि (!resultRemainder.isEmpty()) {
    format.format(resultName, resultRemainder, result.हटाओ(), status);
    फर्काऊ adjustForUsageAndContext(kCapContextUsageLanguage, result);
  }

  result = resultName;
  फर्काऊ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::appendWithSep(UnicodeString& buffer U_LIFETIME_BOUND, अचर UnicodeString& src) अचर {
    यदि (buffer.isEmpty()) {
        buffer.setTo(src);
    } अन्यथा {
        अचर UnicodeString *मानहरू[2] = { &buffer, &src };
        UErrorCode status = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(मानहरू, 2, buffer, nullptr, 0, status);
    }
    फर्काऊ buffer;
}

UnicodeString&
LocaleDisplayNamesImpl::localeDisplayName(अचर वर्ण* localeId,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ localeDisplayName(Locale(localeId), result);
}

// private
UnicodeString&
LocaleDisplayNamesImpl::localeIdName(अचर वर्ण* localeId,
                                     UnicodeString& result U_LIFETIME_BOUND, तार्किक substitute) अचर {
    यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", localeId, result);
        यदि (!result.isBogus()) {
            फर्काऊ result;
        }
    }
    langData.getNoFallback("Languages", localeId, result);
    यदि (result.isBogus() && uprv_strchr(localeId, '_') == nullptr) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        अचर वर्ण* canonLocId = canonLocale.getName();
        यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            यदि (!result.isBogus()) {
                फर्काऊ result;
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    यदि (result.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    फर्काऊ result;
}

UnicodeString&
LocaleDisplayNamesImpl::languageDisplayName(अचर वर्ण* lang,
                                            UnicodeString& result U_LIFETIME_BOUND) अचर {
    यदि (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != nullptr) {
        फर्काऊ result = UnicodeString(lang, -1, US_INV);
    }
    यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Languages%short", lang, result);
        यदि (!result.isBogus()) {
            फर्काऊ adjustForUsageAndContext(kCapContextUsageLanguage, result);
        }
    }
    langData.getNoFallback("Languages", lang, result);
    यदि (result.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        अचर वर्ण* canonLocId = canonLocale.getName();
        यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
            langData.getNoFallback("Languages%short", canonLocId, result);
            यदि (!result.isBogus()) {
                फर्काऊ adjustForUsageAndContext(kCapContextUsageLanguage, result);
            }
        }
        langData.getNoFallback("Languages", canonLocId, result);
    }
    यदि (result.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        result.setTo(UnicodeString(lang, -1, US_INV)); // use key (
    }
    फर्काऊ adjustForUsageAndContext(kCapContextUsageLanguage, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(अचर वर्ण* script,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) अचर {
    यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Scripts%short", script, result);
        यदि (!result.isBogus()) {
            फर्काऊ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Scripts", script, result);
    } अन्यथा {
        langData.getNoFallback("Scripts", script, result);
    }
    फर्काऊ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageScript, result);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(अचर वर्ण* script,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ scriptDisplayName(script, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::scriptDisplayName(UScriptCode scriptCode,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ scriptDisplayName(uscript_getName(scriptCode), result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(अचर वर्ण* region,
                                          UnicodeString& result U_LIFETIME_BOUND,
                                          तार्किक skipAdjust) अचर {
    यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
         regionData.getNoFallback("Countries%short", region, result);
        यदि (!result.isBogus()) {
            फर्काऊ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        regionData.get("Countries", region, result);
    } अन्यथा {
        regionData.getNoFallback("Countries", region, result);
    }
    फर्काऊ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageTerritory, result);
}

UnicodeString&
LocaleDisplayNamesImpl::regionDisplayName(अचर वर्ण* region,
                                          UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ regionDisplayName(region, result, असत्य);
}


UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(अचर वर्ण* variant,
                                           UnicodeString& result U_LIFETIME_BOUND,
                                           तार्किक skipAdjust) अचर {
    // don't have a resource for short variant names
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Variants", variant, result);
    } अन्यथा {
        langData.getNoFallback("Variants", variant, result);
    }
    फर्काऊ skipAdjust? result: adjustForUsageAndContext(kCapContextUsageVariant, result);
}

UnicodeString&
LocaleDisplayNamesImpl::variantDisplayName(अचर वर्ण* variant,
                                           UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ variantDisplayName(variant, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::साँचो_Display_Name(अचर वर्ण* साँचो,
                                       UnicodeString& result U_LIFETIME_BOUND,
                                       तार्किक skipAdjust) अचर {
    // don't have a resource for short key names
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Keys", साँचो, result);
    } अन्यथा {
        langData.getNoFallback("Keys", साँचो, result);
    }
    फर्काऊ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_साँचो, result);
}

UnicodeString&
LocaleDisplayNamesImpl::साँचो_Display_Name(अचर वर्ण* साँचो,
                                       UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ साँचो_Display_Name(साँचो, result, असत्य);
}

UnicodeString&
LocaleDisplayNamesImpl::साँचो_मान_Display_Name(अचर वर्ण* साँचो,
                                            अचर वर्ण* value,
                                            UnicodeString& result U_LIFETIME_BOUND,
                                            तार्किक skipAdjust) अचर {
    यदि (uprv_strcmp(साँचो, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UErrorCode sts = U_ZERO_ERROR;
        UnicodeString ustr_मान(value, -1, US_INV);
        int32_t len;
        अचर char16_t *currencyName = ucurr_getName(ustr_मान.getTerminatedBuffer(),
            locale.getBaseName(), UCURR_LONG_NAME, nullptr /* isChoiceFormat */, &len, &sts);
        यदि (U_FAILURE(sts)) {
            // Return the value as is on failure
            result = ustr_मान;
            फर्काऊ result;
        }
        result.setTo(currencyName, len);
        फर्काऊ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_साँचो_मान, result);
    }

    यदि (nameLength == UDISPCTX_LENGTH_SHORT) {
        langData.getNoFallback("Types%short", साँचो, value, result);
        यदि (!result.isBogus()) {
            फर्काऊ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_साँचो_मान, result);
        }
    }
    यदि (substitute == UDISPCTX_SUBSTITUTE) {
        langData.get("Types", साँचो, value, result);
    } अन्यथा {
        langData.getNoFallback("Types", साँचो, value, result);
    }
    फर्काऊ skipAdjust? result: adjustForUsageAndContext(k_Cap_Context_Usage_साँचो_मान, result);
}

UnicodeString&
LocaleDisplayNamesImpl::साँचो_मान_Display_Name(अचर वर्ण* साँचो,
                                            अचर वर्ण* value,
                                            UnicodeString& result U_LIFETIME_BOUND) अचर {
    फर्काऊ साँचो_मान_Display_Name(साँचो, value, result, असत्य);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

LocaleDisplayNames*
LocaleDisplayNames::createInstance(अचर Locale& locale,
                                   UDialectHandling dialectHandling) {
    फर्काऊ नूतन LocaleDisplayNamesImpl(locale, dialectHandling);
}

LocaleDisplayNames*
LocaleDisplayNames::createInstance(अचर Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    यदि (contexts == nullptr) {
        length = 0;
    }
    फर्काऊ नूतन LocaleDisplayNamesImpl(locale, contexts, length);
}

U_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_open(अचर वर्ण * locale,
          UDialectHandling dialectHandling,
          UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ nullptr;
  }
  यदि (locale == nullptr) {
    locale = uloc_getDefault();
  }
  फर्काऊ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocaleDisplayNames * U_EXPORT2
uldn_openForContext(अचर वर्ण * locale,
                    UDisplayContext *contexts, int32_t length,
                    UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ nullptr;
  }
  यदि (locale == nullptr) {
    locale = uloc_getDefault();
  }
  फर्काऊ (ULocaleDisplayNames *)LocaleDisplayNames::createInstance(Locale(locale), contexts, length);
}


U_CAPI शून्य U_EXPORT2
uldn_close(ULocaleDisplayNames *ldn) {
  delete (LocaleDisplayNames *)ldn;
}

U_CAPI अचर वर्ण* U_EXPORT2
uldn_getLocale(अचर ULocaleDisplayNames* ldn U_LIFETIME_BOUND) {
  यदि (ldn) {
    फर्काऊ ((अचर LocaleDisplayNames *)ldn)->getLocale().getName();
  }
  फर्काऊ nullptr;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(अचर ULocaleDisplayNames *ldn) {
  यदि (ldn) {
    फर्काऊ ((अचर LocaleDisplayNames *)ldn)->getDialectHandling();
  }
  फर्काऊ ULDN_STANDARD_NAMES;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(अचर ULocaleDisplayNames *ldn,
              UDisplayContextType type,
              UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ (UDisplayContext)0;
  }
  फर्काऊ ((अचर LocaleDisplayNames *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(अचर ULocaleDisplayNames *ldn,
                       अचर वर्ण *locale,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || locale == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->localeDisplayName(locale, temp);
  यदि (temp.isBogus()) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(अचर ULocaleDisplayNames *ldn,
                         अचर वर्ण *lang,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || lang == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->languageDisplayName(lang, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(अचर ULocaleDisplayNames *ldn,
                       अचर वर्ण *script,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || script == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->scriptDisplayName(script, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptCodeDisplayName(अचर ULocaleDisplayNames *ldn,
                           UScriptCode scriptCode,
                           char16_t *result,
                           int32_t maxResultSize,
                           UErrorCode *pErrorCode) {
  फर्काऊ uldn_scriptDisplayName(ldn, uscript_getName(scriptCode), result, maxResultSize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(अचर ULocaleDisplayNames *ldn,
                       अचर वर्ण *region,
                       char16_t *result,
                       int32_t maxResultSize,
                       UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || region == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->regionDisplayName(region, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(अचर ULocaleDisplayNames *ldn,
                        अचर वर्ण *variant,
                        char16_t *result,
                        int32_t maxResultSize,
                        UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || variant == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->variantDisplayName(variant, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_साँचो_Display_Name(अचर ULocaleDisplayNames *ldn,
                    अचर वर्ण *साँचो,
                    char16_t *result,
                    int32_t maxResultSize,
                    UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || साँचो == nullptr || (result == nullptr && maxResultSize > 0) || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->साँचो_Display_Name(साँचो, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uldn_साँचो_मान_Display_Name(अचर ULocaleDisplayNames *ldn,
                         अचर वर्ण *साँचो,
                         अचर वर्ण *value,
                         char16_t *result,
                         int32_t maxResultSize,
                         UErrorCode *pErrorCode) {
  यदि (U_FAILURE(*pErrorCode)) {
    फर्काऊ 0;
  }
  यदि (ldn == nullptr || साँचो == nullptr || value == nullptr || (result == nullptr && maxResultSize > 0)
      || maxResultSize < 0) {
    *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
    फर्काऊ 0;
  }
  UnicodeString temp(result, 0, maxResultSize);
  ((अचर LocaleDisplayNames *)ldn)->साँचो_मान_Display_Name(साँचो, value, temp);
  फर्काऊ temp.extract(result, maxResultSize, *pErrorCode);
}

#endif
