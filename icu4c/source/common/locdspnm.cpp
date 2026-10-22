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
صنف ICUData_جدول {
    ثابت حرف* ثابت لار;
    Locale locale;

عام:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_جدول(ثابت حرف* لار, ثابت Locale& locale);
    ~ICUData_جدول() = افتراضي;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت حرف* جدول_کلی, ثابت حرف* توکی_کلی,
                       UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* جدول_کلی, ثابت حرف* sub_جدول_کلی, ثابت حرف* توکی_کلی,
                       UnicodeString& پایله U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* جدول_کلی, ثابت حرف* توکی_کلی,
                                 UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* جدول_کلی, ثابت حرف* sub_جدول_کلی, ثابت حرف* توکی_کلی,
                                 UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUData_جدول::get(ثابت حرف* جدول_کلی, ثابت حرف* توکی_کلی,
                                        UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع get(جدول_کلی, تش_اشاره, توکی_کلی, پایله);
}

مضمن UnicodeString& ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلی, ثابت حرف* توکی_کلی,
                                                  UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(جدول_کلی, تش_اشاره, توکی_کلی, پایله);
}

ICUData_جدول::ICUData_جدول(ثابت حرف* لار, ثابت Locale& locale)
    : لار(لار), locale(locale)
{
    U_ASSERT(لار != تش_اشاره);
}

ثابت Locale&
ICUData_جدول::getLocale() U_LIFETIME_BOUND {
  أرجع locale;
}

UnicodeString&
ICUData_جدول::get(ثابت حرف* جدول_کلی, ثابت حرف* sub_جدول_کلی, ثابت حرف* توکی_کلی,
                  UnicodeString &پایله U_LIFETIME_BOUND) ثابت {
  UError_کوډ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(لار, locale.getName(),
                                                   جدول_کلی, sub_جدول_کلی, توکی_کلی,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت) && len > 0) {
    أرجع پایله.سېټ_منزل(s, len);
  }
  أرجع پایله.سېټ_منزل(UnicodeString(توکی_کلی, -1, US_INV));
}

UnicodeString&
ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلی, ثابت حرف* sub_جدول_کلی, ثابت حرف* توکی_کلی,
                            UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  UError_کوډ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(لار, locale.getName(),
                                                   جدول_کلی, sub_جدول_کلی, توکی_کلی,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت)) {
    أرجع پایله.سېټ_منزل(s, len);
  }

  پایله.سېټ_منزل_Bogus();
  أرجع پایله;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نومونه::~Locale_Display_نومونه() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#إذا 0  // currently unused

صنف Default_Locale_Display_نومونه : عام Locale_Display_نومونه {
  UDialectHandling dialectHandling;

عام:
  // constructor
  Default_Locale_Display_نومونه(UDialectHandling dialectHandling);

  مجازي ~Default_Locale_Display_نومونه();

  مجازي ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  مجازي UDialectHandling getDialectHandling() ثابت;

  مجازي UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& languageDisplayName(ثابت حرف* lang,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& scriptDisplayName(ثابت حرف* script,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& scriptDisplayName(UScript_کوډ script_کوډ,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& regionDisplayName(ثابت حرف* region,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& variantDisplayName(ثابت حرف* variant,
                                            UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& کلی_Display_Name(ثابت حرف* کلی,
                                        UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& کلی_ارزښت_Display_Name(ثابت حرف* کلی,
                                             ثابت حرف* value,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
};

Default_Locale_Display_نومونه::Default_Locale_Display_نومونه(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_نومونه::~Default_Locale_Display_نومونه() {
}

ثابت Locale&
Default_Locale_Display_نومونه::getLocale() ثابت U_LIFETIME_BOUND {
  أرجع Locale::get_ریښه();
}

UDialectHandling
Default_Locale_Display_نومونه::getDialectHandling() ثابت {
  أرجع dialectHandling;
}

UnicodeString&
Default_Locale_Display_نومونه::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::scriptDisplayName(UScript_کوډ script_کوډ,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(script_کوډ);
  إذا (name) {
    أرجع پایله = UnicodeString(name, -1, US_INV);
  }
  أرجع پایله.remove();
}

UnicodeString&
Default_Locale_Display_نومونه::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::کلی_Display_Name(ثابت حرف* کلی,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(کلی, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نومونه::کلی_ارزښت_Display_Name(ثابت حرف* /*کلی*/,
                                               ثابت حرف* value,
                                               UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  أرجع پایله = UnicodeString(value, -1, US_INV);
}

#د_که_پای  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

صنف Locale_Display_نومونه_Impl : عام Locale_Display_نومونه {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_جدول lang_معلومات;
    ICUData_جدول region_معلومات;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter کلی_Type_Format;
    UDisplayContext capitalizationContext;
#إذا !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وإلا
    UObject* capitalizationBrkIter;
#د_که_پای
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_اوږدتیا;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    تعداد CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_کلی,
        k_Cap_Context_Usage_کلی_ارزښت,
        k_Cap_Context_Usage_شمېر
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[k_Cap_Context_Usage_شمېر];

عام:
    // constructor
    Locale_Display_نومونه_Impl(ثابت Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_نومونه_Impl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازي ~Locale_Display_نومونه_Impl();

    مجازي ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    مجازي UDialectHandling getDialectHandling() ثابت override;
    مجازي UDisplayContext getContext(UDisplayContextType type) ثابت override;

    مجازي UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    languageDisplayName(ثابت حرف* lang, UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& scriptDisplayName(UScript_کوډ script_کوډ,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    variantDisplayName(ثابت حرف* variant, UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& کلی_Display_Name(ثابت حرف* کلی,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    کلی_ارزښت_Display_Name(ثابت حرف* کلی, ثابت حرف* value,
                        UnicodeString& پایله U_LIFETIME_BOUND) ثابت override;

خاص:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& پایله U_LIFETIME_BOUND,
                                منطقي substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& پایله U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& پایله U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& پایله U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& پایله U_LIFETIME_BOUND,
                                      منطقي skipAdjust) ثابت;
    UnicodeString& کلی_Display_Name(ثابت حرف* کلی, UnicodeString& پایله U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& کلی_ارزښت_Display_Name(ثابت حرف* کلی, ثابت حرف* value,
                                       UnicodeString& پایله U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت;
    فراغ initialize();

    بنية CapitalizationContextSink;
};

Locale_Display_نومونه_Impl::Locale_Display_نومونه_Impl(ثابت Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_معلومات(U_ICUDATA_LANG, locale)
    , region_معلومات(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(تش_اشاره)
    , name_اوږدتیا(UDISPCTX_اوږدتیا_ډک)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_نومونه_Impl::Locale_Display_نومونه_Impl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_نومونه)
    , lang_معلومات(U_ICUDATA_LANG, locale)
    , region_معلومات(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(تش_اشاره)
    , name_اوږدتیا(UDISPCTX_اوږدتیا_ډک)
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
            حالة UDISPCTX_TYPE_DISPLAY_اوږدتیا:
                name_اوږدتیا = value;
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

بنية Locale_Display_نومونه_Impl::CapitalizationContextSink : عام ResourceSink {
    منطقي hasCapitalizationUsage;
    Locale_Display_نومونه_Impl& پلار;

    CapitalizationContextSink(Locale_Display_نومونه_Impl& پلار_2)
      : hasCapitalizationUsage(خطأ), پلار(پلار_2) {}
    مجازي ~CapitalizationContextSink();

    مجازي فراغ put(ثابت حرف *کلی, Resource_ارزښت &value, UBool /*noFallback*/,
            UError_کوډ &error_کوډ) override {
        Resource_جدول contexts = value.get_جدول(error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        لكل (صحيح i = 0; contexts.get_کلی_And_ارزښت(i, کلی, value); ++i) {

            CapContextUsage usageEnum;
            إذا (uprv_strcmp(کلی, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_کلی;
            } وإلا إذا (uprv_strcmp(کلی, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_کلی_ارزښت;
            } وإلا إذا (uprv_strcmp(کلی, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وإلا إذا (uprv_strcmp(کلی, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وإلا إذا (uprv_strcmp(کلی, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وإلا إذا (uprv_strcmp(کلی, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وإلا {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* int_ویکټور = value.get_Int_ویکټور(len, error_کوډ);
            إذا (U_FAILURE(error_کوډ)) { أرجع; }
            إذا (len < 2) { استمر; }

            int32_t titlecaseInt = (پلار.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_ویکټور[0] : int_ویکټور[1];
            إذا (titlecaseInt == 0) { استمر; }

            پلار.fCapitalization[usageEnum] = ریښتیا;
            hasCapitalizationUsage = ریښتیا;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_نومونه_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

فراغ
Locale_Display_نومونه_Impl::initialize() {
    Locale_Display_نومونه_Impl* nonConstThis = هذا;
    nonConstThis->locale = lang_معلومات.getLocale() == Locale::get_ریښه()
        ? region_معلومات.getLocale()
        : lang_معلومات.getLocale();

    UnicodeString sep;
    lang_معلومات.getNoFallback("localeDisplayPattern", "separator", sep);
    إذا (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_کوډ حالت = U_ZERO_ERROR;
    separatorFormat.apply_نمونه_Min_Max_Arguments(sep, 2, 2, حالت);

    UnicodeString نمونه;
    lang_معلومات.getNoFallback("localeDisplayPattern", "pattern", نمونه);
    إذا (نمونه.isBogus()) {
        نمونه = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_نمونه_Min_Max_Arguments(نمونه, 2, 2, حالت);
    إذا (نمونه.شاخص_د(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.سېټ_منزل(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.سېټ_منزل(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.سېټ_منزل(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.سېټ_منزل(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وإلا {
        formatOpenParen.سېټ_منزل(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.سېټ_منزل(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.سېټ_منزل(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.سېټ_منزل(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_نمونه;
    lang_معلومات.get("localeDisplayPattern", "keyTypePattern", kt_نمونه);
    إذا (kt_نمونه.isBogus()) {
        kt_نمونه = UnicodeString("{0}={1}", -1, US_INV);
    }
    کلی_Type_Format.apply_نمونه_Min_Max_Arguments(kt_نمونه, 2, 2, حالت);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#إذا !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    إذا (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_اشاره_ګر resource(ures_open(تش_اشاره, locale.getName(), &حالت));
        إذا (U_FAILURE(حالت)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_get_All_توکي_With_Fallback(resource.getAlias(), "contextTransforms", sink, حالت);
        إذا (حالت == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            حالت = U_ZERO_ERROR;
        } وإلا إذا (U_FAILURE(حالت)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    إذا (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_د_SENTENCE) {
        حالت = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, حالت);
        إذا (U_FAILURE(حالت)) {
            ړنګول capitalizationBrkIter;
            capitalizationBrkIter = تش_اشاره;
        }
    }
#د_که_پای
}

Locale_Display_نومونه_Impl::~Locale_Display_نومونه_Impl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    ړنګول capitalizationBrkIter;
#د_که_پای
}

ثابت Locale&
Locale_Display_نومونه_Impl::getLocale() ثابت U_LIFETIME_BOUND {
    أرجع locale;
}

UDialectHandling
Locale_Display_نومونه_Impl::getDialectHandling() ثابت {
    أرجع dialectHandling;
}

UDisplayContext
Locale_Display_نومونه_Impl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            أرجع static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            أرجع capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_اوږدتیا:
            أرجع name_اوږدتیا;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            أرجع substitute;
        افتراضي:
            اكسر;
    }
    أرجع static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_نومونه_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
#إذا !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    إذا ( پایله.length() > 0 && u_islower(پایله.char32At(0)) && capitalizationBrkIter!= تش_اشاره &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_د_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalization_Brk_Iter_قلف;
        Mutex قلف(&capitalization_Brk_Iter_قلف);
        پایله.منزل_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#د_که_پای
    أرجع پایله;
}

UnicodeString&
Locale_Display_نومونه_Impl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
  إذا (loc.isBogus()) {
    پایله.سېټ_منزل_Bogus();
    أرجع پایله;
  }
  UnicodeString پایله_Name;

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

  إذا (dialectHandling == ULDN_DIALECT_نومونه) {
    UError_کوډ حالت = U_ZERO_ERROR;
    CharString بفر;
    افعل { // loop construct is so we can break early out of search
      إذا (hasScript && hasCountry) {
        بفر.append(lang, حالت)
              .append('_', حالت)
              .append(script, حالت)
              .append('_', حالت)
              .append(country, حالت);
        إذا (U_SUCCESS(حالت)) {
          localeIdName(بفر.معلومات(), پایله_Name, خطأ);
          إذا (!پایله_Name.isBogus()) {
            hasScript = خطأ;
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasScript) {
        بفر.append(lang, حالت)
              .append('_', حالت)
              .append(script, حالت);
        إذا (U_SUCCESS(حالت)) {
          localeIdName(بفر.معلومات(), پایله_Name, خطأ);
          إذا (!پایله_Name.isBogus()) {
            hasScript = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasCountry) {
        بفر.append(lang, حالت)
              .append('_', حالت)
              .append(country, حالت);
        إذا (U_SUCCESS(حالت)) {
          localeIdName(بفر.معلومات(), پایله_Name, خطأ);
          إذا (!پایله_Name.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  إذا (پایله_Name.isBogus() || پایله_Name.is_تشه()) {
    localeIdName(lang, پایله_Name, substitute == UDISPCTX_SUBSTITUTE);
    إذا (پایله_Name.isBogus()) {
      پایله.سېټ_منزل_Bogus();
      أرجع پایله;
    }
  }

  UnicodeString پایله_Remainder;
  UnicodeString لنډمهاله;
  UError_کوډ حالت = U_ZERO_ERROR;

  إذا (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, لنډمهاله, ریښتیا);
    إذا (script_str.isBogus()) {
      پایله.سېټ_منزل_Bogus();
      أرجع پایله;
    }
    پایله_Remainder.append(script_str);
  }
  إذا (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, لنډمهاله, ریښتیا);
    إذا (region_str.isBogus()) {
      پایله.سېټ_منزل_Bogus();
      أرجع پایله;
    }
    appendWithSep(پایله_Remainder, region_str);
  }
  إذا (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, لنډمهاله, ریښتیا);
    إذا (variant_str.isBogus()) {
      پایله.سېټ_منزل_Bogus();
      أرجع پایله;
    }
    appendWithSep(پایله_Remainder, variant_str);
  }
  پایله_Remainder.پیدا_کړه_And_Replace(formatOpenParen, formatReplaceOpenParen);
  پایله_Remainder.پیدا_کړه_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_اشاره_ګر<StringEnumeration> e(loc.createKeywords(حالت));
  إذا (e.isValid() && U_SUCCESS(حالت)) {
    UnicodeString temp2;
    ثابت حرف* کلی;
    طالما ((کلی = e->next((int32_t*)تش_اشاره, حالت)) != تش_اشاره) {
        خپلکاری value = loc.get_Keyword_ارزښت<CharString>(کلی, حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع پایله;
      }
      کلی_Display_Name(کلی, لنډمهاله, ریښتیا);
      لنډمهاله.پیدا_کړه_And_Replace(formatOpenParen, formatReplaceOpenParen);
      لنډمهاله.پیدا_کړه_And_Replace(formatCloseParen, formatReplaceCloseParen);
      کلی_ارزښت_Display_Name(کلی, value.معلومات(), temp2, ریښتیا);
      temp2.پیدا_کړه_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.پیدا_کړه_And_Replace(formatCloseParen, formatReplaceCloseParen);
      إذا (temp2 != UnicodeString(value.معلومات(), -1, US_INV)) {
        appendWithSep(پایله_Remainder, temp2);
      } وإلا إذا (لنډمهاله != UnicodeString(کلی, -1, US_INV)) {
        UnicodeString temp3;
        کلی_Type_Format.format(لنډمهاله, temp2, temp3, حالت);
        appendWithSep(پایله_Remainder, temp3);
      } وإلا {
        appendWithSep(پایله_Remainder, لنډمهاله)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  إذا (!پایله_Remainder.is_تشه()) {
    format.format(پایله_Name, پایله_Remainder, پایله.remove(), حالت);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, پایله);
  }

  پایله = پایله_Name;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    إذا (بفر.is_تشه()) {
        بفر.سېټ_منزل(src);
    } وإلا {
        ثابت UnicodeString *ارزښتونه[2] = { &بفر, &src };
        UError_کوډ حالت = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(ارزښتونه, 2, بفر, تش_اشاره, 0, حالت);
    }
    أرجع بفر;
}

UnicodeString&
Locale_Display_نومونه_Impl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع localeDisplayName(Locale(localeId), پایله);
}

//شخصي
UnicodeString&
Locale_Display_نومونه_Impl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& پایله U_LIFETIME_BOUND, منطقي substitute) ثابت {
    إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
        lang_معلومات.getNoFallback("Languages%short", localeId, پایله);
        إذا (!پایله.isBogus()) {
            أرجع پایله;
        }
    }
    lang_معلومات.getNoFallback("Languages", localeId, پایله);
    إذا (پایله.isBogus() && uprv_strchr(localeId, '_') == تش_اشاره) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
            lang_معلومات.getNoFallback("Languages%short", canonLocId, پایله);
            إذا (!پایله.isBogus()) {
                أرجع پایله;
            }
        }
        lang_معلومات.getNoFallback("Languages", canonLocId, پایله);
    }
    إذا (پایله.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        پایله.سېټ_منزل(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع پایله;
}

UnicodeString&
Locale_Display_نومونه_Impl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    إذا (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != تش_اشاره) {
        أرجع پایله = UnicodeString(lang, -1, US_INV);
    }
    إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
        lang_معلومات.getNoFallback("Languages%short", lang, پایله);
        إذا (!پایله.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, پایله);
        }
    }
    lang_معلومات.getNoFallback("Languages", lang, پایله);
    إذا (پایله.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
            lang_معلومات.getNoFallback("Languages%short", canonLocId, پایله);
            إذا (!پایله.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, پایله);
            }
        }
        lang_معلومات.getNoFallback("Languages", canonLocId, پایله);
    }
    إذا (پایله.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        پایله.سېټ_منزل(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& پایله U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
        lang_معلومات.getNoFallback("Scripts%short", script, پایله);
        إذا (!پایله.isBogus()) {
            أرجع skipAdjust? پایله: adjustForUsageAndContext(kCapContextUsageScript, پایله);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Scripts", script, پایله);
    } وإلا {
        lang_معلومات.getNoFallback("Scripts", script, پایله);
    }
    أرجع skipAdjust? پایله: adjustForUsageAndContext(kCapContextUsageScript, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(script, پایله, خطأ);
}

UnicodeString&
Locale_Display_نومونه_Impl::scriptDisplayName(UScript_کوډ script_کوډ,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(uscript_getName(script_کوډ), پایله, خطأ);
}

UnicodeString&
Locale_Display_نومونه_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& پایله U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
         region_معلومات.getNoFallback("Countries%short", region, پایله);
        إذا (!پایله.isBogus()) {
            أرجع skipAdjust? پایله: adjustForUsageAndContext(kCapContextUsageTerritory, پایله);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        region_معلومات.get("Countries", region, پایله);
    } وإلا {
        region_معلومات.getNoFallback("Countries", region, پایله);
    }
    أرجع skipAdjust? پایله: adjustForUsageAndContext(kCapContextUsageTerritory, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع regionDisplayName(region, پایله, خطأ);
}


UnicodeString&
Locale_Display_نومونه_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& پایله U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Variants", variant, پایله);
    } وإلا {
        lang_معلومات.getNoFallback("Variants", variant, پایله);
    }
    أرجع skipAdjust? پایله: adjustForUsageAndContext(kCapContextUsageVariant, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع variantDisplayName(variant, پایله, خطأ);
}

UnicodeString&
Locale_Display_نومونه_Impl::کلی_Display_Name(ثابت حرف* کلی,
                                       UnicodeString& پایله U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Keys", کلی, پایله);
    } وإلا {
        lang_معلومات.getNoFallback("Keys", کلی, پایله);
    }
    أرجع skipAdjust? پایله: adjustForUsageAndContext(k_Cap_Context_Usage_کلی, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::کلی_Display_Name(ثابت حرف* کلی,
                                       UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع کلی_Display_Name(کلی, پایله, خطأ);
}

UnicodeString&
Locale_Display_نومونه_Impl::کلی_ارزښت_Display_Name(ثابت حرف* کلی,
                                            ثابت حرف* value,
                                            UnicodeString& پایله U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    إذا (uprv_strcmp(کلی, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_کوډ sts = U_ZERO_ERROR;
        UnicodeString ustr_ارزښت(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_ارزښت.get_Terminated_بفر(),
            locale.get_بنسټ_Name(), UCURR_LONG_NAME, تش_اشاره /* isChoiceFormat */, &len, &sts);
        إذا (U_FAILURE(sts)) {
            // Return the value as is on failure
            پایله = ustr_ارزښت;
            أرجع پایله;
        }
        پایله.سېټ_منزل(currencyName, len);
        أرجع skipAdjust? پایله: adjustForUsageAndContext(k_Cap_Context_Usage_کلی_ارزښت, پایله);
    }

    إذا (name_اوږدتیا == UDISPCTX_اوږدتیا_SHORT) {
        lang_معلومات.getNoFallback("Types%short", کلی, value, پایله);
        إذا (!پایله.isBogus()) {
            أرجع skipAdjust? پایله: adjustForUsageAndContext(k_Cap_Context_Usage_کلی_ارزښت, پایله);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Types", کلی, value, پایله);
    } وإلا {
        lang_معلومات.getNoFallback("Types", کلی, value, پایله);
    }
    أرجع skipAdjust? پایله: adjustForUsageAndContext(k_Cap_Context_Usage_کلی_ارزښت, پایله);
}

UnicodeString&
Locale_Display_نومونه_Impl::کلی_ارزښت_Display_Name(ثابت حرف* کلی,
                                            ثابت حرف* value,
                                            UnicodeString& پایله U_LIFETIME_BOUND) ثابت {
    أرجع کلی_ارزښت_Display_Name(کلی, value, پایله, خطأ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نومونه*
Locale_Display_نومونه::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    أرجع جديد Locale_Display_نومونه_Impl(locale, dialectHandling);
}

Locale_Display_نومونه*
Locale_Display_نومونه::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    إذا (contexts == تش_اشاره) {
        length = 0;
    }
    أرجع جديد Locale_Display_نومونه_Impl(locale, contexts, length);
}

U_NAMESPACE_پای_ټکی

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_نومونه * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع تش_اشاره;
  }
  إذا (locale == تش_اشاره) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نومونه *)Locale_Display_نومونه::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_نومونه * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع تش_اشاره;
  }
  إذا (locale == تش_اشاره) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نومونه *)Locale_Display_نومونه::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocale_Display_نومونه *ldn) {
  ړنګول (Locale_Display_نومونه *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_نومونه* ldn U_LIFETIME_BOUND) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نومونه *)ldn)->getLocale().getName();
  }
  أرجع تش_اشاره;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_نومونه *ldn) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نومونه *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_نومونه;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocale_Display_نومونه *ldn,
              UDisplayContextType type,
              UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع (UDisplayContext)0;
  }
  أرجع ((ثابت Locale_Display_نومونه *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *locale,
                       char16_t *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || locale == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->localeDisplayName(locale, لنډمهاله);
  إذا (لنډمهاله.isBogus()) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocale_Display_نومونه *ldn,
                         ثابت حرف *lang,
                         char16_t *پایله,
                         int32_t max_پایله_اندازه,
                         UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || lang == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->languageDisplayName(lang, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *script,
                       char16_t *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || script == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->scriptDisplayName(script, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_script_کوډ_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                           UScript_کوډ script_کوډ,
                           char16_t *پایله,
                           int32_t max_پایله_اندازه,
                           UError_کوډ *p_Error_کوډ) {
  أرجع uldn_scriptDisplayName(ldn, uscript_getName(script_کوډ), پایله, max_پایله_اندازه, p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *region,
                       char16_t *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || region == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->regionDisplayName(region, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocale_Display_نومونه *ldn,
                        ثابت حرف *variant,
                        char16_t *پایله,
                        int32_t max_پایله_اندازه,
                        UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || variant == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->variantDisplayName(variant, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_کلی_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                    ثابت حرف *کلی,
                    char16_t *پایله,
                    int32_t max_پایله_اندازه,
                    UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || کلی == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0) || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->کلی_Display_Name(کلی, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uldn_کلی_ارزښت_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                         ثابت حرف *کلی,
                         ثابت حرف *value,
                         char16_t *پایله,
                         int32_t max_پایله_اندازه,
                         UError_کوډ *p_Error_کوډ) {
  إذا (U_FAILURE(*p_Error_کوډ)) {
    أرجع 0;
  }
  إذا (ldn == تش_اشاره || کلی == تش_اشاره || value == تش_اشاره || (پایله == تش_اشاره && max_پایله_اندازه > 0)
      || max_پایله_اندازه < 0) {
    *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString لنډمهاله(پایله, 0, max_پایله_اندازه);
  ((ثابت Locale_Display_نومونه *)ldn)->کلی_ارزښت_Display_Name(کلی, value, لنډمهاله);
  أرجع لنډمهاله.extract(پایله, max_پایله_اندازه, *p_Error_کوډ);
}

#د_که_پای
