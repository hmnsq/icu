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
    ثابت حرف* ثابت رستہ;
    Locale locale;

عام:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_جدول(ثابت حرف* رستہ, ثابت Locale& locale);
    ~ICUData_جدول() = افتراضي;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                       UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                       UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                                 UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                                 UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                                        UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع get(جدول_کلید, خالی_سوچک, شے_کلید, نتیجہ);
}

مضمن UnicodeString& ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                                                  UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(جدول_کلید, خالی_سوچک, شے_کلید, نتیجہ);
}

ICUData_جدول::ICUData_جدول(ثابت حرف* رستہ, ثابت Locale& locale)
    : رستہ(رستہ), locale(locale)
{
    U_ASSERT(رستہ != خالی_سوچک);
}

ثابت Locale&
ICUData_جدول::getLocale() U_LIFETIME_BOUND {
  أرجع locale;
}

UnicodeString&
ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                  UnicodeString &نتیجہ U_LIFETIME_BOUND) ثابت {
  UError_کوڈ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(رستہ, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, شے_کلید,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت) && len > 0) {
    أرجع نتیجہ.سیٹ_منزل(s, len);
  }
  أرجع نتیجہ.سیٹ_منزل(UnicodeString(شے_کلید, -1, US_INV));
}

UnicodeString&
ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  UError_کوڈ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(رستہ, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, شے_کلید,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت)) {
    أرجع نتیجہ.سیٹ_منزل(s, len);
  }

  نتیجہ.سیٹ_منزل_Bogus();
  أرجع نتیجہ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ناں::~Locale_Display_ناں() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#إذا 0  // currently unused

صنف Default_Locale_Display_ناں : عام Locale_Display_ناں {
  UDialectHandling dialectHandling;

عام:
  // constructor
  Default_Locale_Display_ناں(UDialectHandling dialectHandling);

  مجازی ~Default_Locale_Display_ناں();

  مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  مجازی UDialectHandling getDialectHandling() ثابت;

  مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& languageDisplayName(ثابت حرف* lang,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(ثابت حرف* script,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(UScript_کوڈ script_کوڈ,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& regionDisplayName(ثابت حرف* region,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& variantDisplayName(ثابت حرف* variant,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_Display_Name(ثابت حرف* کلید,
                                        UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_قدر_Display_Name(ثابت حرف* کلید,
                                             ثابت حرف* value,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
};

Default_Locale_Display_ناں::Default_Locale_Display_ناں(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_ناں::~Default_Locale_Display_ناں() {
}

ثابت Locale&
Default_Locale_Display_ناں::getLocale() ثابت U_LIFETIME_BOUND {
  أرجع Locale::get_جڑ();
}

UDialectHandling
Default_Locale_Display_ناں::getDialectHandling() ثابت {
  أرجع dialectHandling;
}

UnicodeString&
Default_Locale_Display_ناں::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::scriptDisplayName(UScript_کوڈ script_کوڈ,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(script_کوڈ);
  إذا (name) {
    أرجع نتیجہ = UnicodeString(name, -1, US_INV);
  }
  أرجع نتیجہ.remove();
}

UnicodeString&
Default_Locale_Display_ناں::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(کلید, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_ناں::کلید_قدر_Display_Name(ثابت حرف* /*کلید*/,
                                               ثابت حرف* value,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  أرجع نتیجہ = UnicodeString(value, -1, US_INV);
}

#جے_انت  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

صنف Locale_Display_ناں_Impl : عام Locale_Display_ناں {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_جدول lang_انکڑے;
    ICUData_جدول region_انکڑے;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter کلید_Type_Format;
    UDisplayContext capitalizationContext;
#إذا !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وإلا
    UObject* capitalizationBrkIter;
#جے_انت
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_لمائی;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    تعداد CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_کلید,
        k_Cap_Context_Usage_کلید_قدر,
        k_Cap_Context_Usage_گنتی
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[k_Cap_Context_Usage_گنتی];

عام:
    // constructor
    Locale_Display_ناں_Impl(ثابت Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_ناں_Impl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازی ~Locale_Display_ناں_Impl();

    مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    مجازی UDialectHandling getDialectHandling() ثابت override;
    مجازی UDisplayContext getContext(UDisplayContextType type) ثابت override;

    مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    languageDisplayName(ثابت حرف* lang, UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(UScript_کوڈ script_کوڈ,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    variantDisplayName(ثابت حرف* variant, UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    کلید_قدر_Display_Name(ثابت حرف* کلید, ثابت حرف* value,
                        UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت override;

خاص:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                منطقي substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      منطقي skipAdjust) ثابت;
    UnicodeString& کلید_Display_Name(ثابت حرف* کلید, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& کلید_قدر_Display_Name(ثابت حرف* کلید, ثابت حرف* value,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت;
    فراغ initialize();

    بنية CapitalizationContextSink;
};

Locale_Display_ناں_Impl::Locale_Display_ناں_Impl(ثابت Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_انکڑے(U_ICUDATA_LANG, locale)
    , region_انکڑے(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالی_سوچک)
    , name_لمائی(UDISPCTX_لمائی_بھریا)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_ناں_Impl::Locale_Display_ناں_Impl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_ناں)
    , lang_انکڑے(U_ICUDATA_LANG, locale)
    , region_انکڑے(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالی_سوچک)
    , name_لمائی(UDISPCTX_لمائی_بھریا)
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
            حالة UDISPCTX_TYPE_DISPLAY_لمائی:
                name_لمائی = value;
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

بنية Locale_Display_ناں_Impl::CapitalizationContextSink : عام ResourceSink {
    منطقي hasCapitalizationUsage;
    Locale_Display_ناں_Impl& ماپا;

    CapitalizationContextSink(Locale_Display_ناں_Impl& ماپا_2)
      : hasCapitalizationUsage(خطأ), ماپا(ماپا_2) {}
    مجازی ~CapitalizationContextSink();

    مجازی فراغ put(ثابت حرف *کلید, Resource_قدر &value, UBool /*noFallback*/,
            UError_کوڈ &error_کوڈ) override {
        Resource_جدول contexts = value.get_جدول(error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        لكل (صحيح i = 0; contexts.get_کلید_And_قدر(i, کلید, value); ++i) {

            CapContextUsage usageEnum;
            إذا (uprv_strcmp(کلید, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید;
            } وإلا إذا (uprv_strcmp(کلید, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید_قدر;
            } وإلا إذا (uprv_strcmp(کلید, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وإلا إذا (uprv_strcmp(کلید, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وإلا إذا (uprv_strcmp(کلید, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وإلا إذا (uprv_strcmp(کلید, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وإلا {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* int_سمتیہ = value.get_Int_سمتیہ(len, error_کوڈ);
            إذا (U_FAILURE(error_کوڈ)) { أرجع; }
            إذا (len < 2) { استمر; }

            int32_t titlecaseInt = (ماپا.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_سمتیہ[0] : int_سمتیہ[1];
            إذا (titlecaseInt == 0) { استمر; }

            ماپا.fCapitalization[usageEnum] = سچ;
            hasCapitalizationUsage = سچ;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_ناں_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

فراغ
Locale_Display_ناں_Impl::initialize() {
    Locale_Display_ناں_Impl* nonConstThis = هذا;
    nonConstThis->locale = lang_انکڑے.getLocale() == Locale::get_جڑ()
        ? region_انکڑے.getLocale()
        : lang_انکڑے.getLocale();

    UnicodeString sep;
    lang_انکڑے.getNoFallback("localeDisplayPattern", "separator", sep);
    إذا (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_کوڈ حالت = U_ZERO_ERROR;
    separatorFormat.apply_نمونہ_Min_Max_Arguments(sep, 2, 2, حالت);

    UnicodeString نمونہ;
    lang_انکڑے.getNoFallback("localeDisplayPattern", "pattern", نمونہ);
    إذا (نمونہ.isBogus()) {
        نمونہ = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_نمونہ_Min_Max_Arguments(نمونہ, 2, 2, حالت);
    إذا (نمونہ.اشاریہ_دا(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.سیٹ_منزل(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.سیٹ_منزل(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.سیٹ_منزل(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.سیٹ_منزل(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وإلا {
        formatOpenParen.سیٹ_منزل(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.سیٹ_منزل(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.سیٹ_منزل(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.سیٹ_منزل(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_نمونہ;
    lang_انکڑے.get("localeDisplayPattern", "keyTypePattern", kt_نمونہ);
    إذا (kt_نمونہ.isBogus()) {
        kt_نمونہ = UnicodeString("{0}={1}", -1, US_INV);
    }
    کلید_Type_Format.apply_نمونہ_Min_Max_Arguments(kt_نمونہ, 2, 2, حالت);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#إذا !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    إذا (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_اشارہ_گر resource(ures_open(خالی_سوچک, locale.getName(), &حالت));
        إذا (U_FAILURE(حالت)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_get_All_اشیاء_With_Fallback(resource.getAlias(), "contextTransforms", sink, حالت);
        إذا (حالت == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            حالت = U_ZERO_ERROR;
        } وإلا إذا (U_FAILURE(حالت)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    إذا (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_دا_SENTENCE) {
        حالت = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, حالت);
        إذا (U_FAILURE(حالت)) {
            مٹاؤ capitalizationBrkIter;
            capitalizationBrkIter = خالی_سوچک;
        }
    }
#جے_انت
}

Locale_Display_ناں_Impl::~Locale_Display_ناں_Impl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    مٹاؤ capitalizationBrkIter;
#جے_انت
}

ثابت Locale&
Locale_Display_ناں_Impl::getLocale() ثابت U_LIFETIME_BOUND {
    أرجع locale;
}

UDialectHandling
Locale_Display_ناں_Impl::getDialectHandling() ثابت {
    أرجع dialectHandling;
}

UDisplayContext
Locale_Display_ناں_Impl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            أرجع static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            أرجع capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_لمائی:
            أرجع name_لمائی;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            أرجع substitute;
        افتراضي:
            اكسر;
    }
    أرجع static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_ناں_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
#إذا !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    إذا ( نتیجہ.length() > 0 && u_islower(نتیجہ.char32At(0)) && capitalizationBrkIter!= خالی_سوچک &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_دا_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalization_Brk_Iter_جندرا;
        Mutex جندرا(&capitalization_Brk_Iter_جندرا);
        نتیجہ.منزل_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#جے_انت
    أرجع نتیجہ;
}

UnicodeString&
Locale_Display_ناں_Impl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  إذا (loc.isBogus()) {
    نتیجہ.سیٹ_منزل_Bogus();
    أرجع نتیجہ;
  }
  UnicodeString نتیجہ_Name;

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

  إذا (dialectHandling == ULDN_DIALECT_ناں) {
    UError_کوڈ حالت = U_ZERO_ERROR;
    CharString بفر;
    افعل { // loop construct is so we can break early out of search
      إذا (hasScript && hasCountry) {
        بفر.append(lang, حالت)
              .append('_', حالت)
              .append(script, حالت)
              .append('_', حالت)
              .append(country, حالت);
        إذا (U_SUCCESS(حالت)) {
          localeIdName(بفر.انکڑے(), نتیجہ_Name, خطأ);
          إذا (!نتیجہ_Name.isBogus()) {
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
          localeIdName(بفر.انکڑے(), نتیجہ_Name, خطأ);
          إذا (!نتیجہ_Name.isBogus()) {
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
          localeIdName(بفر.انکڑے(), نتیجہ_Name, خطأ);
          إذا (!نتیجہ_Name.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  إذا (نتیجہ_Name.isBogus() || نتیجہ_Name.is_خالی_تھاواں()) {
    localeIdName(lang, نتیجہ_Name, substitute == UDISPCTX_SUBSTITUTE);
    إذا (نتیجہ_Name.isBogus()) {
      نتیجہ.سیٹ_منزل_Bogus();
      أرجع نتیجہ;
    }
  }

  UnicodeString نتیجہ_Remainder;
  UnicodeString عارضی;
  UError_کوڈ حالت = U_ZERO_ERROR;

  إذا (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, عارضی, سچ);
    إذا (script_str.isBogus()) {
      نتیجہ.سیٹ_منزل_Bogus();
      أرجع نتیجہ;
    }
    نتیجہ_Remainder.append(script_str);
  }
  إذا (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, عارضی, سچ);
    إذا (region_str.isBogus()) {
      نتیجہ.سیٹ_منزل_Bogus();
      أرجع نتیجہ;
    }
    appendWithSep(نتیجہ_Remainder, region_str);
  }
  إذا (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, عارضی, سچ);
    إذا (variant_str.isBogus()) {
      نتیجہ.سیٹ_منزل_Bogus();
      أرجع نتیجہ;
    }
    appendWithSep(نتیجہ_Remainder, variant_str);
  }
  نتیجہ_Remainder.لبھو_And_Replace(formatOpenParen, formatReplaceOpenParen);
  نتیجہ_Remainder.لبھو_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_اشارہ_گر<StringEnumeration> e(loc.createKeywords(حالت));
  إذا (e.isValid() && U_SUCCESS(حالت)) {
    UnicodeString temp2;
    ثابت حرف* کلید;
    طالما ((کلید = e->next((int32_t*)خالی_سوچک, حالت)) != خالی_سوچک) {
        خودکار value = loc.get_Keyword_قدر<CharString>(کلید, حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع نتیجہ;
      }
      کلید_Display_Name(کلید, عارضی, سچ);
      عارضی.لبھو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      عارضی.لبھو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      کلید_قدر_Display_Name(کلید, value.انکڑے(), temp2, سچ);
      temp2.لبھو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.لبھو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      إذا (temp2 != UnicodeString(value.انکڑے(), -1, US_INV)) {
        appendWithSep(نتیجہ_Remainder, temp2);
      } وإلا إذا (عارضی != UnicodeString(کلید, -1, US_INV)) {
        UnicodeString temp3;
        کلید_Type_Format.format(عارضی, temp2, temp3, حالت);
        appendWithSep(نتیجہ_Remainder, temp3);
      } وإلا {
        appendWithSep(نتیجہ_Remainder, عارضی)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  إذا (!نتیجہ_Remainder.is_خالی_تھاواں()) {
    format.format(نتیجہ_Name, نتیجہ_Remainder, نتیجہ.remove(), حالت);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
  }

  نتیجہ = نتیجہ_Name;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    إذا (بفر.is_خالی_تھاواں()) {
        بفر.سیٹ_منزل(src);
    } وإلا {
        ثابت UnicodeString *اقدار[2] = { &بفر, &src };
        UError_کوڈ حالت = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(اقدار, 2, بفر, خالی_سوچک, 0, حالت);
    }
    أرجع بفر;
}

UnicodeString&
Locale_Display_ناں_Impl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع localeDisplayName(Locale(localeId), نتیجہ);
}

//نجی
UnicodeString&
Locale_Display_ناں_Impl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& نتیجہ U_LIFETIME_BOUND, منطقي substitute) ثابت {
    إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
        lang_انکڑے.getNoFallback("Languages%short", localeId, نتیجہ);
        إذا (!نتیجہ.isBogus()) {
            أرجع نتیجہ;
        }
    }
    lang_انکڑے.getNoFallback("Languages", localeId, نتیجہ);
    إذا (نتیجہ.isBogus() && uprv_strchr(localeId, '_') == خالی_سوچک) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
            lang_انکڑے.getNoFallback("Languages%short", canonLocId, نتیجہ);
            إذا (!نتیجہ.isBogus()) {
                أرجع نتیجہ;
            }
        }
        lang_انکڑے.getNoFallback("Languages", canonLocId, نتیجہ);
    }
    إذا (نتیجہ.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجہ.سیٹ_منزل(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع نتیجہ;
}

UnicodeString&
Locale_Display_ناں_Impl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    إذا (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != خالی_سوچک) {
        أرجع نتیجہ = UnicodeString(lang, -1, US_INV);
    }
    إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
        lang_انکڑے.getNoFallback("Languages%short", lang, نتیجہ);
        إذا (!نتیجہ.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
        }
    }
    lang_انکڑے.getNoFallback("Languages", lang, نتیجہ);
    إذا (نتیجہ.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
            lang_انکڑے.getNoFallback("Languages%short", canonLocId, نتیجہ);
            إذا (!نتیجہ.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
            }
        }
        lang_انکڑے.getNoFallback("Languages", canonLocId, نتیجہ);
    }
    إذا (نتیجہ.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجہ.سیٹ_منزل(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
        lang_انکڑے.getNoFallback("Scripts%short", script, نتیجہ);
        إذا (!نتیجہ.isBogus()) {
            أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageScript, نتیجہ);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_انکڑے.get("Scripts", script, نتیجہ);
    } وإلا {
        lang_انکڑے.getNoFallback("Scripts", script, نتیجہ);
    }
    أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageScript, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(script, نتیجہ, خطأ);
}

UnicodeString&
Locale_Display_ناں_Impl::scriptDisplayName(UScript_کوڈ script_کوڈ,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(uscript_getName(script_کوڈ), نتیجہ, خطأ);
}

UnicodeString&
Locale_Display_ناں_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
         region_انکڑے.getNoFallback("Countries%short", region, نتیجہ);
        إذا (!نتیجہ.isBogus()) {
            أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجہ);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        region_انکڑے.get("Countries", region, نتیجہ);
    } وإلا {
        region_انکڑے.getNoFallback("Countries", region, نتیجہ);
    }
    أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع regionDisplayName(region, نتیجہ, خطأ);
}


UnicodeString&
Locale_Display_ناں_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_انکڑے.get("Variants", variant, نتیجہ);
    } وإلا {
        lang_انکڑے.getNoFallback("Variants", variant, نتیجہ);
    }
    أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageVariant, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع variantDisplayName(variant, نتیجہ, خطأ);
}

UnicodeString&
Locale_Display_ناں_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_انکڑے.get("Keys", کلید, نتیجہ);
    } وإلا {
        lang_انکڑے.getNoFallback("Keys", کلید, نتیجہ);
    }
    أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع کلید_Display_Name(کلید, نتیجہ, خطأ);
}

UnicodeString&
Locale_Display_ناں_Impl::کلید_قدر_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    إذا (uprv_strcmp(کلید, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_کوڈ sts = U_ZERO_ERROR;
        UnicodeString ustr_قدر(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_قدر.get_Terminated_بفر(),
            locale.get_ادھار_Name(), UCURR_LONG_NAME, خالی_سوچک /* isChoiceFormat */, &len, &sts);
        إذا (U_FAILURE(sts)) {
            // Return the value as is on failure
            نتیجہ = ustr_قدر;
            أرجع نتیجہ;
        }
        نتیجہ.سیٹ_منزل(currencyName, len);
        أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
    }

    إذا (name_لمائی == UDISPCTX_لمائی_SHORT) {
        lang_انکڑے.getNoFallback("Types%short", کلید, value, نتیجہ);
        إذا (!نتیجہ.isBogus()) {
            أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_انکڑے.get("Types", کلید, value, نتیجہ);
    } وإلا {
        lang_انکڑے.getNoFallback("Types", کلید, value, نتیجہ);
    }
    أرجع skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
}

UnicodeString&
Locale_Display_ناں_Impl::کلید_قدر_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    أرجع کلید_قدر_Display_Name(کلید, value, نتیجہ, خطأ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_ناں*
Locale_Display_ناں::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    أرجع جديد Locale_Display_ناں_Impl(locale, dialectHandling);
}

Locale_Display_ناں*
Locale_Display_ناں::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    إذا (contexts == خالی_سوچک) {
        length = 0;
    }
    أرجع جديد Locale_Display_ناں_Impl(locale, contexts, length);
}

U_NAMESPACE_سرا

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_ناں * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع خالی_سوچک;
  }
  إذا (locale == خالی_سوچک) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_ناں *)Locale_Display_ناں::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_ناں * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع خالی_سوچک;
  }
  إذا (locale == خالی_سوچک) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_ناں *)Locale_Display_ناں::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocale_Display_ناں *ldn) {
  مٹاؤ (Locale_Display_ناں *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_ناں* ldn U_LIFETIME_BOUND) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_ناں *)ldn)->getLocale().getName();
  }
  أرجع خالی_سوچک;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_ناں *ldn) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_ناں *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_ناں;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocale_Display_ناں *ldn,
              UDisplayContextType type,
              UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع (UDisplayContext)0;
  }
  أرجع ((ثابت Locale_Display_ناں *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocale_Display_ناں *ldn,
                       ثابت حرف *locale,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_ناپ,
                       UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || locale == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->localeDisplayName(locale, عارضی);
  إذا (عارضی.isBogus()) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocale_Display_ناں *ldn,
                         ثابت حرف *lang,
                         char16_t *نتیجہ,
                         int32_t max_نتیجہ_ناپ,
                         UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || lang == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->languageDisplayName(lang, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocale_Display_ناں *ldn,
                       ثابت حرف *script,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_ناپ,
                       UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || script == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->scriptDisplayName(script, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_script_کوڈ_Display_Name(ثابت ULocale_Display_ناں *ldn,
                           UScript_کوڈ script_کوڈ,
                           char16_t *نتیجہ,
                           int32_t max_نتیجہ_ناپ,
                           UError_کوڈ *p_Error_کوڈ) {
  أرجع uldn_scriptDisplayName(ldn, uscript_getName(script_کوڈ), نتیجہ, max_نتیجہ_ناپ, p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocale_Display_ناں *ldn,
                       ثابت حرف *region,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_ناپ,
                       UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || region == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->regionDisplayName(region, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocale_Display_ناں *ldn,
                        ثابت حرف *variant,
                        char16_t *نتیجہ,
                        int32_t max_نتیجہ_ناپ,
                        UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || variant == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->variantDisplayName(variant, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_Display_Name(ثابت ULocale_Display_ناں *ldn,
                    ثابت حرف *کلید,
                    char16_t *نتیجہ,
                    int32_t max_نتیجہ_ناپ,
                    UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || کلید == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0) || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->کلید_Display_Name(کلید, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_قدر_Display_Name(ثابت ULocale_Display_ناں *ldn,
                         ثابت حرف *کلید,
                         ثابت حرف *value,
                         char16_t *نتیجہ,
                         int32_t max_نتیجہ_ناپ,
                         UError_کوڈ *p_Error_کوڈ) {
  إذا (U_FAILURE(*p_Error_کوڈ)) {
    أرجع 0;
  }
  إذا (ldn == خالی_سوچک || کلید == خالی_سوچک || value == خالی_سوچک || (نتیجہ == خالی_سوچک && max_نتیجہ_ناپ > 0)
      || max_نتیجہ_ناپ < 0) {
    *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_ناپ);
  ((ثابت Locale_Display_ناں *)ldn)->کلید_قدر_Display_Name(کلید, value, عارضی);
  أرجع عارضی.extract(نتیجہ, max_نتیجہ_ناپ, *p_Error_کوڈ);
}

#جے_انت
