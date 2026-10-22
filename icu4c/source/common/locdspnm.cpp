// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/locdspnm.h"
#شامل "unicode/simpleformatter.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/ures.h"
#شامل "unicode/udisplaycontext.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/ucurr.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "uassert.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "ureslocs.h"
#شامل "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
جماعت ICUData_جدول {
    ثابت حرف* ثابت راستہ;
    Locale locale;

عوامی:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_جدول(ثابت حرف* راستہ, ثابت Locale& locale);
    ~ICUData_جدول() = طےشدہ;

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

اندرونی UnicodeString& ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                                        UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس get(جدول_کلید, خالی_اشاریہ, شے_کلید, نتیجہ);
}

اندرونی UnicodeString& ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* شے_کلید,
                                                  UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس getNoFallback(جدول_کلید, خالی_اشاریہ, شے_کلید, نتیجہ);
}

ICUData_جدول::ICUData_جدول(ثابت حرف* راستہ, ثابت Locale& locale)
    : راستہ(راستہ), locale(locale)
{
    U_ASSERT(راستہ != خالی_اشاریہ);
}

ثابت Locale&
ICUData_جدول::getLocale() U_LIFETIME_BOUND {
  واپس locale;
}

UnicodeString&
ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                  UnicodeString &نتیجہ U_LIFETIME_BOUND) ثابت {
  UError_رمز حالت_2 = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(راستہ, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, شے_کلید,
                                                   &len, &حالت_2);
  اگر (U_SUCCESS(حالت_2) && len > 0) {
    واپس نتیجہ.سیٹ_مقصد(s, len);
  }
  واپس نتیجہ.سیٹ_مقصد(UnicodeString(شے_کلید, -1, US_INV));
}

UnicodeString&
ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* شے_کلید,
                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  UError_رمز حالت_2 = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(راستہ, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, شے_کلید,
                                                   &len, &حالت_2);
  اگر (U_SUCCESS(حالت_2)) {
    واپس نتیجہ.سیٹ_مقصد(s, len);
  }

  نتیجہ.سیٹ_مقصد_Bogus();
  واپس نتیجہ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نام::~Locale_Display_نام() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#اگر 0  // currently unused

جماعت Default_Locale_Display_نام : عوامی Locale_Display_نام {
  UDialectHandling dialectHandling;

عوامی:
  // constructor
  Default_Locale_Display_نام(UDialectHandling dialectHandling);

  مجازی ~Default_Locale_Display_نام();

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
  مجازی UnicodeString& scriptDisplayName(UScript_رمز script_رمز,
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

Default_Locale_Display_نام::Default_Locale_Display_نام(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_نام::~Default_Locale_Display_نام() {
}

ثابت Locale&
Default_Locale_Display_نام::getLocale() ثابت U_LIFETIME_BOUND {
  واپس Locale::get_جڑ();
}

UDialectHandling
Default_Locale_Display_نام::getDialectHandling() ثابت {
  واپس dialectHandling;
}

UnicodeString&
Default_Locale_Display_نام::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::scriptDisplayName(UScript_رمز script_رمز,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(script_رمز);
  اگر (name) {
    واپس نتیجہ = UnicodeString(name, -1, US_INV);
  }
  واپس نتیجہ.remove();
}

UnicodeString&
Default_Locale_Display_نام::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(کلید, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نام::کلید_قدر_Display_Name(ثابت حرف* /*کلید*/,
                                               ثابت حرف* value,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  واپس نتیجہ = UnicodeString(value, -1, US_INV);
}

#اگر_ختم  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

جماعت Locale_Display_نام_Impl : عوامی Locale_Display_نام {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_جدول lang_ڈیٹا;
    ICUData_جدول region_ڈیٹا;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter کلید_Type_Format;
    UDisplayContext capitalizationContext;
#اگر !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#ورنہ
    UObject* capitalizationBrkIter;
#اگر_ختم
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_لمبائی;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    شمار CapContextUsage {
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
     منطقی fCapitalization[k_Cap_Context_Usage_گنتی];

عوامی:
    // constructor
    Locale_Display_نام_Impl(ثابت Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_نام_Impl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازی ~Locale_Display_نام_Impl();

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
    مجازی UnicodeString& scriptDisplayName(UScript_رمز script_رمز,
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

نجی:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                منطقی substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     منطقی skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     منطقی skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      منطقی skipAdjust) ثابت;
    UnicodeString& کلید_Display_Name(ثابت حرف* کلید, UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                  منطقی skipAdjust) ثابت;
    UnicodeString& کلید_قدر_Display_Name(ثابت حرف* کلید, ثابت حرف* value,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                       منطقی skipAdjust) ثابت;
    خالی initialize();

    ساخت CapitalizationContextSink;
};

Locale_Display_نام_Impl::Locale_Display_نام_Impl(ثابت Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_ڈیٹا(U_ICUDATA_LANG, locale)
    , region_ڈیٹا(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالی_اشاریہ)
    , name_لمبائی(UDISPCTX_لمبائی_بھرا)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_نام_Impl::Locale_Display_نام_Impl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_نام)
    , lang_ڈیٹا(U_ICUDATA_LANG, locale)
    , region_ڈیٹا(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالی_اشاریہ)
    , name_لمبائی(UDISPCTX_لمبائی_بھرا)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    جب (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        انتخاب (selector) {
            حالت UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                توڑو;
            حالت UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                توڑو;
            حالت UDISPCTX_TYPE_DISPLAY_لمبائی:
                name_لمبائی = value;
                توڑو;
            حالت UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                توڑو;
            طےشدہ:
                توڑو;
        }
    }
    initialize();
}

ساخت Locale_Display_نام_Impl::CapitalizationContextSink : عوامی ResourceSink {
    منطقی hasCapitalizationUsage;
    Locale_Display_نام_Impl& والد;

    CapitalizationContextSink(Locale_Display_نام_Impl& والد_2)
      : hasCapitalizationUsage(جھوٹ), والد(والد_2) {}
    مجازی ~CapitalizationContextSink();

    مجازی خالی put(ثابت حرف *کلید, Resource_قدر &value, UBool /*noFallback*/,
            UError_رمز &error_رمز) override {
        Resource_جدول contexts = value.get_جدول(error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        برائے (صحیح i = 0; contexts.get_کلید_And_قدر(i, کلید, value); ++i) {

            CapContextUsage usageEnum;
            اگر (uprv_strcmp(کلید, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید;
            } ورنہ اگر (uprv_strcmp(کلید, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید_قدر;
            } ورنہ اگر (uprv_strcmp(کلید, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } ورنہ اگر (uprv_strcmp(کلید, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } ورنہ اگر (uprv_strcmp(کلید, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } ورنہ اگر (uprv_strcmp(کلید, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } ورنہ {
                جاری;
            }

            int32_t len = 0;
            ثابت int32_t* int_سمتیہ = value.get_Int_سمتیہ(len, error_رمز);
            اگر (U_FAILURE(error_رمز)) { واپس; }
            اگر (len < 2) { جاری; }

            int32_t titlecaseInt = (والد.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_سمتیہ[0] : int_سمتیہ[1];
            اگر (titlecaseInt == 0) { جاری; }

            والد.fCapitalization[usageEnum] = سچ;
            hasCapitalizationUsage = سچ;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_نام_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

خالی
Locale_Display_نام_Impl::initialize() {
    Locale_Display_نام_Impl* nonConstThis = یہ;
    nonConstThis->locale = lang_ڈیٹا.getLocale() == Locale::get_جڑ()
        ? region_ڈیٹا.getLocale()
        : lang_ڈیٹا.getLocale();

    UnicodeString sep;
    lang_ڈیٹا.getNoFallback("localeDisplayPattern", "separator", sep);
    اگر (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_رمز حالت_2 = U_ZERO_ERROR;
    separatorFormat.apply_نمونہ_Min_Max_Arguments(sep, 2, 2, حالت_2);

    UnicodeString نمونہ;
    lang_ڈیٹا.getNoFallback("localeDisplayPattern", "pattern", نمونہ);
    اگر (نمونہ.isBogus()) {
        نمونہ = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_نمونہ_Min_Max_Arguments(نمونہ, 2, 2, حالت_2);
    اگر (نمونہ.اشاریہ_کا(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.سیٹ_مقصد(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.سیٹ_مقصد(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.سیٹ_مقصد(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.سیٹ_مقصد(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } ورنہ {
        formatOpenParen.سیٹ_مقصد(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.سیٹ_مقصد(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.سیٹ_مقصد(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.سیٹ_مقصد(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_نمونہ;
    lang_ڈیٹا.get("localeDisplayPattern", "keyTypePattern", kt_نمونہ);
    اگر (kt_نمونہ.isBogus()) {
        kt_نمونہ = UnicodeString("{0}={1}", -1, US_INV);
    }
    کلید_Type_Format.apply_نمونہ_Min_Max_Arguments(kt_نمونہ, 2, 2, حالت_2);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#اگر !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقی needBrkIter = جھوٹ;
    اگر (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_اشارہ_گر resource(ures_open(خالی_اشاریہ, locale.getName(), &حالت_2));
        اگر (U_FAILURE(حالت_2)) { واپس; }
        CapitalizationContextSink sink(*یہ);
        ures_get_All_اشیاء_With_Fallback(resource.getAlias(), "contextTransforms", sink, حالت_2);
        اگر (حالت_2 == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            حالت_2 = U_ZERO_ERROR;
        } ورنہ اگر (U_FAILURE(حالت_2)) {
            واپس;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    اگر (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_کا_SENTENCE) {
        حالت_2 = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            حذف capitalizationBrkIter;
            capitalizationBrkIter = خالی_اشاریہ;
        }
    }
#اگر_ختم
}

Locale_Display_نام_Impl::~Locale_Display_نام_Impl() {
#اگر !UCONFIG_NO_BREAK_ITERATION
    حذف capitalizationBrkIter;
#اگر_ختم
}

ثابت Locale&
Locale_Display_نام_Impl::getLocale() ثابت U_LIFETIME_BOUND {
    واپس locale;
}

UDialectHandling
Locale_Display_نام_Impl::getDialectHandling() ثابت {
    واپس dialectHandling;
}

UDisplayContext
Locale_Display_نام_Impl::getContext(UDisplayContextType type) ثابت {
    انتخاب (type) {
        حالت UDISPCTX_TYPE_DIALECT_HANDLING:
            واپس static_cast<UDisplayContext>(dialectHandling);
        حالت UDISPCTX_TYPE_CAPITALIZATION:
            واپس capitalizationContext;
        حالت UDISPCTX_TYPE_DISPLAY_لمبائی:
            واپس name_لمبائی;
        حالت UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            واپس substitute;
        طےشدہ:
            توڑو;
    }
    واپس static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_نام_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
#اگر !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    اگر ( نتیجہ.length() > 0 && u_islower(نتیجہ.char32At(0)) && capitalizationBrkIter!= خالی_اشاریہ &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_کا_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساکن UMutex capitalization_Brk_Iter_تالا;
        Mutex تالا(&capitalization_Brk_Iter_تالا);
        نتیجہ.مقصد_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#اگر_ختم
    واپس نتیجہ;
}

UnicodeString&
Locale_Display_نام_Impl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
  اگر (loc.isBogus()) {
    نتیجہ.سیٹ_مقصد_Bogus();
    واپس نتیجہ;
  }
  UnicodeString نتیجہ_Name;

  ثابت حرف* lang = loc.getLanguage();
  اگر (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  ثابت حرف* script = loc.getScript();
  ثابت حرف* country = loc.getCountry();
  ثابت حرف* variant = loc.getVariant();

  منطقی hasScript = uprv_strlen(script) > 0;
  منطقی hasCountry = uprv_strlen(country) > 0;
  منطقی hasVariant = uprv_strlen(variant) > 0;

  اگر (dialectHandling == ULDN_DIALECT_نام) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    CharString بفر;
    کرو { // loop construct is so we can break early out of search
      اگر (hasScript && hasCountry) {
        بفر.append(lang, حالت_2)
              .append('_', حالت_2)
              .append(script, حالت_2)
              .append('_', حالت_2)
              .append(country, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
          localeIdName(بفر.ڈیٹا(), نتیجہ_Name, جھوٹ);
          اگر (!نتیجہ_Name.isBogus()) {
            hasScript = جھوٹ;
            hasCountry = جھوٹ;
            توڑو;
          }
        }
      }
      اگر (hasScript) {
        بفر.append(lang, حالت_2)
              .append('_', حالت_2)
              .append(script, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
          localeIdName(بفر.ڈیٹا(), نتیجہ_Name, جھوٹ);
          اگر (!نتیجہ_Name.isBogus()) {
            hasScript = جھوٹ;
            توڑو;
          }
        }
      }
      اگر (hasCountry) {
        بفر.append(lang, حالت_2)
              .append('_', حالت_2)
              .append(country, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
          localeIdName(بفر.ڈیٹا(), نتیجہ_Name, جھوٹ);
          اگر (!نتیجہ_Name.isBogus()) {
            hasCountry = جھوٹ;
            توڑو;
          }
        }
      }
    } جب (جھوٹ);
  }
  اگر (نتیجہ_Name.isBogus() || نتیجہ_Name.is_خالی_خانے()) {
    localeIdName(lang, نتیجہ_Name, substitute == UDISPCTX_SUBSTITUTE);
    اگر (نتیجہ_Name.isBogus()) {
      نتیجہ.سیٹ_مقصد_Bogus();
      واپس نتیجہ;
    }
  }

  UnicodeString نتیجہ_Remainder;
  UnicodeString عارضی;
  UError_رمز حالت_2 = U_ZERO_ERROR;

  اگر (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, عارضی, سچ);
    اگر (script_str.isBogus()) {
      نتیجہ.سیٹ_مقصد_Bogus();
      واپس نتیجہ;
    }
    نتیجہ_Remainder.append(script_str);
  }
  اگر (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, عارضی, سچ);
    اگر (region_str.isBogus()) {
      نتیجہ.سیٹ_مقصد_Bogus();
      واپس نتیجہ;
    }
    appendWithSep(نتیجہ_Remainder, region_str);
  }
  اگر (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, عارضی, سچ);
    اگر (variant_str.isBogus()) {
      نتیجہ.سیٹ_مقصد_Bogus();
      واپس نتیجہ;
    }
    appendWithSep(نتیجہ_Remainder, variant_str);
  }
  نتیجہ_Remainder.ڈھونڈو_And_Replace(formatOpenParen, formatReplaceOpenParen);
  نتیجہ_Remainder.ڈھونڈو_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_اشارہ_گر<StringEnumeration> e(loc.createKeywords(حالت_2));
  اگر (e.isValid() && U_SUCCESS(حالت_2)) {
    UnicodeString temp2;
    ثابت حرف* کلید;
    جب ((کلید = e->next((int32_t*)خالی_اشاریہ, حالت_2)) != خالی_اشاریہ) {
        خودکار value = loc.get_Keyword_قدر<CharString>(کلید, حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس نتیجہ;
      }
      کلید_Display_Name(کلید, عارضی, سچ);
      عارضی.ڈھونڈو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      عارضی.ڈھونڈو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      کلید_قدر_Display_Name(کلید, value.ڈیٹا(), temp2, سچ);
      temp2.ڈھونڈو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.ڈھونڈو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      اگر (temp2 != UnicodeString(value.ڈیٹا(), -1, US_INV)) {
        appendWithSep(نتیجہ_Remainder, temp2);
      } ورنہ اگر (عارضی != UnicodeString(کلید, -1, US_INV)) {
        UnicodeString temp3;
        کلید_Type_Format.format(عارضی, temp2, temp3, حالت_2);
        appendWithSep(نتیجہ_Remainder, temp3);
      } ورنہ {
        appendWithSep(نتیجہ_Remainder, عارضی)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  اگر (!نتیجہ_Remainder.is_خالی_خانے()) {
    format.format(نتیجہ_Name, نتیجہ_Remainder, نتیجہ.remove(), حالت_2);
    واپس adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
  }

  نتیجہ = نتیجہ_Name;
  واپس adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    اگر (بفر.is_خالی_خانے()) {
        بفر.سیٹ_مقصد(src);
    } ورنہ {
        ثابت UnicodeString *اقدار[2] = { &بفر, &src };
        UError_رمز حالت_2 = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(اقدار, 2, بفر, خالی_اشاریہ, 0, حالت_2);
    }
    واپس بفر;
}

UnicodeString&
Locale_Display_نام_Impl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس localeDisplayName(Locale(localeId), نتیجہ);
}

//نجی
UnicodeString&
Locale_Display_نام_Impl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& نتیجہ U_LIFETIME_BOUND, منطقی substitute) ثابت {
    اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
        lang_ڈیٹا.getNoFallback("Languages%short", localeId, نتیجہ);
        اگر (!نتیجہ.isBogus()) {
            واپس نتیجہ;
        }
    }
    lang_ڈیٹا.getNoFallback("Languages", localeId, نتیجہ);
    اگر (نتیجہ.isBogus() && uprv_strchr(localeId, '_') == خالی_اشاریہ) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
            lang_ڈیٹا.getNoFallback("Languages%short", canonLocId, نتیجہ);
            اگر (!نتیجہ.isBogus()) {
                واپس نتیجہ;
            }
        }
        lang_ڈیٹا.getNoFallback("Languages", canonLocId, نتیجہ);
    }
    اگر (نتیجہ.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجہ.سیٹ_مقصد(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    واپس نتیجہ;
}

UnicodeString&
Locale_Display_نام_Impl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    اگر (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != خالی_اشاریہ) {
        واپس نتیجہ = UnicodeString(lang, -1, US_INV);
    }
    اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
        lang_ڈیٹا.getNoFallback("Languages%short", lang, نتیجہ);
        اگر (!نتیجہ.isBogus()) {
            واپس adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
        }
    }
    lang_ڈیٹا.getNoFallback("Languages", lang, نتیجہ);
    اگر (نتیجہ.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
            lang_ڈیٹا.getNoFallback("Languages%short", canonLocId, نتیجہ);
            اگر (!نتیجہ.isBogus()) {
                واپس adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
            }
        }
        lang_ڈیٹا.getNoFallback("Languages", canonLocId, نتیجہ);
    }
    اگر (نتیجہ.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجہ.سیٹ_مقصد(UnicodeString(lang, -1, US_INV)); // use key (
    }
    واپس adjustForUsageAndContext(kCapContextUsageLanguage, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                          منطقی skipAdjust) ثابت {
    اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
        lang_ڈیٹا.getNoFallback("Scripts%short", script, نتیجہ);
        اگر (!نتیجہ.isBogus()) {
            واپس skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageScript, نتیجہ);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ڈیٹا.get("Scripts", script, نتیجہ);
    } ورنہ {
        lang_ڈیٹا.getNoFallback("Scripts", script, نتیجہ);
    }
    واپس skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageScript, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس scriptDisplayName(script, نتیجہ, جھوٹ);
}

UnicodeString&
Locale_Display_نام_Impl::scriptDisplayName(UScript_رمز script_رمز,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس scriptDisplayName(uscript_getName(script_رمز), نتیجہ, جھوٹ);
}

UnicodeString&
Locale_Display_نام_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                          منطقی skipAdjust) ثابت {
    اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
         region_ڈیٹا.getNoFallback("Countries%short", region, نتیجہ);
        اگر (!نتیجہ.isBogus()) {
            واپس skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجہ);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        region_ڈیٹا.get("Countries", region, نتیجہ);
    } ورنہ {
        region_ڈیٹا.getNoFallback("Countries", region, نتیجہ);
    }
    واپس skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس regionDisplayName(region, نتیجہ, جھوٹ);
}


UnicodeString&
Locale_Display_نام_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                           منطقی skipAdjust) ثابت {
    // don't have a resource for short variant names
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ڈیٹا.get("Variants", variant, نتیجہ);
    } ورنہ {
        lang_ڈیٹا.getNoFallback("Variants", variant, نتیجہ);
    }
    واپس skipAdjust? نتیجہ: adjustForUsageAndContext(kCapContextUsageVariant, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس variantDisplayName(variant, نتیجہ, جھوٹ);
}

UnicodeString&
Locale_Display_نام_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                       منطقی skipAdjust) ثابت {
    // don't have a resource for short key names
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ڈیٹا.get("Keys", کلید, نتیجہ);
    } ورنہ {
        lang_ڈیٹا.getNoFallback("Keys", کلید, نتیجہ);
    }
    واپس skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس کلید_Display_Name(کلید, نتیجہ, جھوٹ);
}

UnicodeString&
Locale_Display_نام_Impl::کلید_قدر_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                            منطقی skipAdjust) ثابت {
    اگر (uprv_strcmp(کلید, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_رمز sts = U_ZERO_ERROR;
        UnicodeString ustr_قدر(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_قدر.get_Terminated_بفر(),
            locale.get_بنیاد_Name(), UCURR_LONG_NAME, خالی_اشاریہ /* isChoiceFormat */, &len, &sts);
        اگر (U_FAILURE(sts)) {
            // Return the value as is on failure
            نتیجہ = ustr_قدر;
            واپس نتیجہ;
        }
        نتیجہ.سیٹ_مقصد(currencyName, len);
        واپس skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
    }

    اگر (name_لمبائی == UDISPCTX_لمبائی_SHORT) {
        lang_ڈیٹا.getNoFallback("Types%short", کلید, value, نتیجہ);
        اگر (!نتیجہ.isBogus()) {
            واپس skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
        }
    }
    اگر (substitute == UDISPCTX_SUBSTITUTE) {
        lang_ڈیٹا.get("Types", کلید, value, نتیجہ);
    } ورنہ {
        lang_ڈیٹا.getNoFallback("Types", کلید, value, نتیجہ);
    }
    واپس skipAdjust? نتیجہ: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_قدر, نتیجہ);
}

UnicodeString&
Locale_Display_نام_Impl::کلید_قدر_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت {
    واپس کلید_قدر_Display_Name(کلید, value, نتیجہ, جھوٹ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نام*
Locale_Display_نام::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    واپس نیا Locale_Display_نام_Impl(locale, dialectHandling);
}

Locale_Display_نام*
Locale_Display_نام::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    اگر (contexts == خالی_اشاریہ) {
        length = 0;
    }
    واپس نیا Locale_Display_نام_Impl(locale, contexts, length);
}

U_NAMESPACE_انتہا

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_نام * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس خالی_اشاریہ;
  }
  اگر (locale == خالی_اشاریہ) {
    locale = uloc_getDefault();
  }
  واپس (ULocale_Display_نام *)Locale_Display_نام::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_نام * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس خالی_اشاریہ;
  }
  اگر (locale == خالی_اشاریہ) {
    locale = uloc_getDefault();
  }
  واپس (ULocale_Display_نام *)Locale_Display_نام::createInstance(Locale(locale), contexts, length);
}


U_CAPI خالی U_EXPORT2
uldn_close(ULocale_Display_نام *ldn) {
  حذف (Locale_Display_نام *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_نام* ldn U_LIFETIME_BOUND) {
  اگر (ldn) {
    واپس ((ثابت Locale_Display_نام *)ldn)->getLocale().getName();
  }
  واپس خالی_اشاریہ;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_نام *ldn) {
  اگر (ldn) {
    واپس ((ثابت Locale_Display_نام *)ldn)->getDialectHandling();
  }
  واپس ULDN_STANDARD_نام;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocale_Display_نام *ldn,
              UDisplayContextType type,
              UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس (UDisplayContext)0;
  }
  واپس ((ثابت Locale_Display_نام *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocale_Display_نام *ldn,
                       ثابت حرف *locale,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_سائز,
                       UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || locale == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->localeDisplayName(locale, عارضی);
  اگر (عارضی.isBogus()) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocale_Display_نام *ldn,
                         ثابت حرف *lang,
                         char16_t *نتیجہ,
                         int32_t max_نتیجہ_سائز,
                         UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || lang == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->languageDisplayName(lang, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocale_Display_نام *ldn,
                       ثابت حرف *script,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_سائز,
                       UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || script == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->scriptDisplayName(script, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_script_رمز_Display_Name(ثابت ULocale_Display_نام *ldn,
                           UScript_رمز script_رمز,
                           char16_t *نتیجہ,
                           int32_t max_نتیجہ_سائز,
                           UError_رمز *p_Error_رمز) {
  واپس uldn_scriptDisplayName(ldn, uscript_getName(script_رمز), نتیجہ, max_نتیجہ_سائز, p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocale_Display_نام *ldn,
                       ثابت حرف *region,
                       char16_t *نتیجہ,
                       int32_t max_نتیجہ_سائز,
                       UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || region == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->regionDisplayName(region, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocale_Display_نام *ldn,
                        ثابت حرف *variant,
                        char16_t *نتیجہ,
                        int32_t max_نتیجہ_سائز,
                        UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || variant == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->variantDisplayName(variant, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_Display_Name(ثابت ULocale_Display_نام *ldn,
                    ثابت حرف *کلید,
                    char16_t *نتیجہ,
                    int32_t max_نتیجہ_سائز,
                    UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || کلید == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0) || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->کلید_Display_Name(کلید, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_قدر_Display_Name(ثابت ULocale_Display_نام *ldn,
                         ثابت حرف *کلید,
                         ثابت حرف *value,
                         char16_t *نتیجہ,
                         int32_t max_نتیجہ_سائز,
                         UError_رمز *p_Error_رمز) {
  اگر (U_FAILURE(*p_Error_رمز)) {
    واپس 0;
  }
  اگر (ldn == خالی_اشاریہ || کلید == خالی_اشاریہ || value == خالی_اشاریہ || (نتیجہ == خالی_اشاریہ && max_نتیجہ_سائز > 0)
      || max_نتیجہ_سائز < 0) {
    *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  UnicodeString عارضی(نتیجہ, 0, max_نتیجہ_سائز);
  ((ثابت Locale_Display_نام *)ldn)->کلید_قدر_Display_Name(کلید, value, عارضی);
  واپس عارضی.extract(نتیجہ, max_نتیجہ_سائز, *p_Error_رمز);
}

#اگر_ختم
