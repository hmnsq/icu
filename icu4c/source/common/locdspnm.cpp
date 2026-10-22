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
    ثابت حرف* ثابت مسیر;
    Locale locale;

عام:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_جدول(ثابت حرف* مسیر, ثابت Locale& locale);
    ~ICUData_جدول() = افتراضي;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت حرف* جدول_کلید, ثابت حرف* قلم_کلید,
                       UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* قلم_کلید,
                       UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* قلم_کلید,
                                 UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* قلم_کلید,
                                 UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* قلم_کلید,
                                        UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع get(جدول_کلید, اشاره_تهی, قلم_کلید, نتیجه);
}

مضمن UnicodeString& ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* قلم_کلید,
                                                  UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(جدول_کلید, اشاره_تهی, قلم_کلید, نتیجه);
}

ICUData_جدول::ICUData_جدول(ثابت حرف* مسیر, ثابت Locale& locale)
    : مسیر(مسیر), locale(locale)
{
    U_ASSERT(مسیر != اشاره_تهی);
}

ثابت Locale&
ICUData_جدول::getLocale() U_LIFETIME_BOUND {
  أرجع locale;
}

UnicodeString&
ICUData_جدول::get(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* قلم_کلید,
                  UnicodeString &نتیجه U_LIFETIME_BOUND) ثابت {
  UError_کد وضعیت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(مسیر, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, قلم_کلید,
                                                   &len, &وضعیت);
  إذا (U_SUCCESS(وضعیت) && len > 0) {
    أرجع نتیجه.مجموعه_مقصد(s, len);
  }
  أرجع نتیجه.مجموعه_مقصد(UnicodeString(قلم_کلید, -1, US_INV));
}

UnicodeString&
ICUData_جدول::getNoFallback(ثابت حرف* جدول_کلید, ثابت حرف* sub_جدول_کلید, ثابت حرف* قلم_کلید,
                            UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  UError_کد وضعیت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(مسیر, locale.getName(),
                                                   جدول_کلید, sub_جدول_کلید, قلم_کلید,
                                                   &len, &وضعیت);
  إذا (U_SUCCESS(وضعیت)) {
    أرجع نتیجه.مجموعه_مقصد(s, len);
  }

  نتیجه.مجموعه_مقصد_Bogus();
  أرجع نتیجه;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نامها::~Locale_Display_نامها() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#إذا 0  // currently unused

صنف Default_Locale_Display_نامها : عام Locale_Display_نامها {
  UDialectHandling dialectHandling;

عام:
  // constructor
  Default_Locale_Display_نامها(UDialectHandling dialectHandling);

  مجازی ~Default_Locale_Display_نامها();

  مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  مجازی UDialectHandling getDialectHandling() ثابت;

  مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& languageDisplayName(ثابت حرف* lang,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(ثابت حرف* script,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& scriptDisplayName(UScript_کد script_کد,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& regionDisplayName(ثابت حرف* region,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& variantDisplayName(ثابت حرف* variant,
                                            UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_Display_Name(ثابت حرف* کلید,
                                        UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
  مجازی UnicodeString& کلید_مقدار_Display_Name(ثابت حرف* کلید,
                                             ثابت حرف* value,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
};

Default_Locale_Display_نامها::Default_Locale_Display_نامها(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_نامها::~Default_Locale_Display_نامها() {
}

ثابت Locale&
Default_Locale_Display_نامها::getLocale() ثابت U_LIFETIME_BOUND {
  أرجع Locale::get_ریشه();
}

UDialectHandling
Default_Locale_Display_نامها::getDialectHandling() ثابت {
  أرجع dialectHandling;
}

UnicodeString&
Default_Locale_Display_نامها::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::scriptDisplayName(UScript_کد script_کد,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(script_کد);
  إذا (name) {
    أرجع نتیجه = UnicodeString(name, -1, US_INV);
  }
  أرجع نتیجه.remove();
}

UnicodeString&
Default_Locale_Display_نامها::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(کلید, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نامها::کلید_مقدار_Display_Name(ثابت حرف* /*کلید*/,
                                               ثابت حرف* value,
                                               UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  أرجع نتیجه = UnicodeString(value, -1, US_INV);
}

#پایان_اگر  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

صنف Locale_Display_نامها_Impl : عام Locale_Display_نامها {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_جدول lang_داده;
    ICUData_جدول region_داده;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter کلید_Type_Format;
    UDisplayContext capitalizationContext;
#إذا !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وإلا
    UObject* capitalizationBrkIter;
#پایان_اگر
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_درازی;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    تعداد CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_کلید,
        k_Cap_Context_Usage_کلید_مقدار,
        k_Cap_Context_Usage_شمار
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[k_Cap_Context_Usage_شمار];

عام:
    // constructor
    Locale_Display_نامها_Impl(ثابت Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_نامها_Impl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازی ~Locale_Display_نامها_Impl();

    مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    مجازی UDialectHandling getDialectHandling() ثابت override;
    مجازی UDisplayContext getContext(UDisplayContextType type) ثابت override;

    مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    languageDisplayName(ثابت حرف* lang, UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& scriptDisplayName(UScript_کد script_کد,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    variantDisplayName(ثابت حرف* variant, UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString& کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;
    مجازی UnicodeString&
    کلید_مقدار_Display_Name(ثابت حرف* کلید, ثابت حرف* value,
                        UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت override;

خاص:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& نتیجه U_LIFETIME_BOUND,
                                منطقي substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& میانگیر U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& نتیجه U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& نتیجه U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& نتیجه U_LIFETIME_BOUND,
                                      منطقي skipAdjust) ثابت;
    UnicodeString& کلید_Display_Name(ثابت حرف* کلید, UnicodeString& نتیجه U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& کلید_مقدار_Display_Name(ثابت حرف* کلید, ثابت حرف* value,
                                       UnicodeString& نتیجه U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت;
    فراغ initialize();

    بنية CapitalizationContextSink;
};

Locale_Display_نامها_Impl::Locale_Display_نامها_Impl(ثابت Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_داده(U_ICUDATA_LANG, locale)
    , region_داده(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(اشاره_تهی)
    , name_درازی(UDISPCTX_درازی_پر)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_نامها_Impl::Locale_Display_نامها_Impl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_نامها)
    , lang_داده(U_ICUDATA_LANG, locale)
    , region_داده(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(اشاره_تهی)
    , name_درازی(UDISPCTX_درازی_پر)
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
            حالة UDISPCTX_TYPE_DISPLAY_درازی:
                name_درازی = value;
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

بنية Locale_Display_نامها_Impl::CapitalizationContextSink : عام ResourceSink {
    منطقي hasCapitalizationUsage;
    Locale_Display_نامها_Impl& والد;

    CapitalizationContextSink(Locale_Display_نامها_Impl& والد_2)
      : hasCapitalizationUsage(خطأ), والد(والد_2) {}
    مجازی ~CapitalizationContextSink();

    مجازی فراغ put(ثابت حرف *کلید, Resource_مقدار &value, UBool /*noFallback*/,
            UError_کد &error_کد) override {
        Resource_جدول contexts = value.get_جدول(error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        لكل (صحيح i = 0; contexts.get_کلید_And_مقدار(i, کلید, value); ++i) {

            CapContextUsage usageEnum;
            إذا (uprv_strcmp(کلید, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید;
            } وإلا إذا (uprv_strcmp(کلید, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_کلید_مقدار;
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
            ثابت int32_t* int_وکتور = value.get_Int_وکتور(len, error_کد);
            إذا (U_FAILURE(error_کد)) { أرجع; }
            إذا (len < 2) { استمر; }

            int32_t titlecaseInt = (والد.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_وکتور[0] : int_وکتور[1];
            إذا (titlecaseInt == 0) { استمر; }

            والد.fCapitalization[usageEnum] = درست;
            hasCapitalizationUsage = درست;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_نامها_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

فراغ
Locale_Display_نامها_Impl::initialize() {
    Locale_Display_نامها_Impl* nonConstThis = هذا;
    nonConstThis->locale = lang_داده.getLocale() == Locale::get_ریشه()
        ? region_داده.getLocale()
        : lang_داده.getLocale();

    UnicodeString sep;
    lang_داده.getNoFallback("localeDisplayPattern", "separator", sep);
    إذا (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_کد وضعیت = U_ZERO_ERROR;
    separatorFormat.apply_الگو_Min_Max_Arguments(sep, 2, 2, وضعیت);

    UnicodeString الگو;
    lang_داده.getNoFallback("localeDisplayPattern", "pattern", الگو);
    إذا (الگو.isBogus()) {
        الگو = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_الگو_Min_Max_Arguments(الگو, 2, 2, وضعیت);
    إذا (الگو.شاخص_از(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.مجموعه_مقصد(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.مجموعه_مقصد(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.مجموعه_مقصد(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.مجموعه_مقصد(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وإلا {
        formatOpenParen.مجموعه_مقصد(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.مجموعه_مقصد(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.مجموعه_مقصد(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.مجموعه_مقصد(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_الگو;
    lang_داده.get("localeDisplayPattern", "keyTypePattern", kt_الگو);
    إذا (kt_الگو.isBogus()) {
        kt_الگو = UnicodeString("{0}={1}", -1, US_INV);
    }
    کلید_Type_Format.apply_الگو_Min_Max_Arguments(kt_الگو, 2, 2, وضعیت);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#إذا !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    إذا (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_اشاره_گر resource(ures_open(اشاره_تهی, locale.getName(), &وضعیت));
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_get_All_اقلام_With_Fallback(resource.getAlias(), "contextTransforms", sink, وضعیت);
        إذا (وضعیت == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            وضعیت = U_ZERO_ERROR;
        } وإلا إذا (U_FAILURE(وضعیت)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    إذا (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_از_SENTENCE) {
        وضعیت = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            حذف capitalizationBrkIter;
            capitalizationBrkIter = اشاره_تهی;
        }
    }
#پایان_اگر
}

Locale_Display_نامها_Impl::~Locale_Display_نامها_Impl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    حذف capitalizationBrkIter;
#پایان_اگر
}

ثابت Locale&
Locale_Display_نامها_Impl::getLocale() ثابت U_LIFETIME_BOUND {
    أرجع locale;
}

UDialectHandling
Locale_Display_نامها_Impl::getDialectHandling() ثابت {
    أرجع dialectHandling;
}

UDisplayContext
Locale_Display_نامها_Impl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            أرجع static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            أرجع capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_درازی:
            أرجع name_درازی;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            أرجع substitute;
        افتراضي:
            اكسر;
    }
    أرجع static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_نامها_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
#إذا !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    إذا ( نتیجه.length() > 0 && u_islower(نتیجه.char32At(0)) && capitalizationBrkIter!= اشاره_تهی &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_از_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalization_Brk_Iter_قفل;
        Mutex قفل(&capitalization_Brk_Iter_قفل);
        نتیجه.مقصد_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#پایان_اگر
    أرجع نتیجه;
}

UnicodeString&
Locale_Display_نامها_Impl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
  إذا (loc.isBogus()) {
    نتیجه.مجموعه_مقصد_Bogus();
    أرجع نتیجه;
  }
  UnicodeString نتیجه_Name;

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

  إذا (dialectHandling == ULDN_DIALECT_نامها) {
    UError_کد وضعیت = U_ZERO_ERROR;
    CharString میانگیر;
    افعل { // loop construct is so we can break early out of search
      إذا (hasScript && hasCountry) {
        میانگیر.append(lang, وضعیت)
              .append('_', وضعیت)
              .append(script, وضعیت)
              .append('_', وضعیت)
              .append(country, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
          localeIdName(میانگیر.داده(), نتیجه_Name, خطأ);
          إذا (!نتیجه_Name.isBogus()) {
            hasScript = خطأ;
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasScript) {
        میانگیر.append(lang, وضعیت)
              .append('_', وضعیت)
              .append(script, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
          localeIdName(میانگیر.داده(), نتیجه_Name, خطأ);
          إذا (!نتیجه_Name.isBogus()) {
            hasScript = خطأ;
            اكسر;
          }
        }
      }
      إذا (hasCountry) {
        میانگیر.append(lang, وضعیت)
              .append('_', وضعیت)
              .append(country, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
          localeIdName(میانگیر.داده(), نتیجه_Name, خطأ);
          إذا (!نتیجه_Name.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  إذا (نتیجه_Name.isBogus() || نتیجه_Name.is_خالی()) {
    localeIdName(lang, نتیجه_Name, substitute == UDISPCTX_SUBSTITUTE);
    إذا (نتیجه_Name.isBogus()) {
      نتیجه.مجموعه_مقصد_Bogus();
      أرجع نتیجه;
    }
  }

  UnicodeString نتیجه_Remainder;
  UnicodeString موقت;
  UError_کد وضعیت = U_ZERO_ERROR;

  إذا (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, موقت, درست);
    إذا (script_str.isBogus()) {
      نتیجه.مجموعه_مقصد_Bogus();
      أرجع نتیجه;
    }
    نتیجه_Remainder.append(script_str);
  }
  إذا (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, موقت, درست);
    إذا (region_str.isBogus()) {
      نتیجه.مجموعه_مقصد_Bogus();
      أرجع نتیجه;
    }
    appendWithSep(نتیجه_Remainder, region_str);
  }
  إذا (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, موقت, درست);
    إذا (variant_str.isBogus()) {
      نتیجه.مجموعه_مقصد_Bogus();
      أرجع نتیجه;
    }
    appendWithSep(نتیجه_Remainder, variant_str);
  }
  نتیجه_Remainder.پیدا_کن_And_Replace(formatOpenParen, formatReplaceOpenParen);
  نتیجه_Remainder.پیدا_کن_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_اشاره_گر<StringEnumeration> e(loc.createKeywords(وضعیت));
  إذا (e.isValid() && U_SUCCESS(وضعیت)) {
    UnicodeString temp2;
    ثابت حرف* کلید;
    طالما ((کلید = e->next((int32_t*)اشاره_تهی, وضعیت)) != اشاره_تهی) {
        خودکار value = loc.get_Keyword_مقدار<CharString>(کلید, وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع نتیجه;
      }
      کلید_Display_Name(کلید, موقت, درست);
      موقت.پیدا_کن_And_Replace(formatOpenParen, formatReplaceOpenParen);
      موقت.پیدا_کن_And_Replace(formatCloseParen, formatReplaceCloseParen);
      کلید_مقدار_Display_Name(کلید, value.داده(), temp2, درست);
      temp2.پیدا_کن_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.پیدا_کن_And_Replace(formatCloseParen, formatReplaceCloseParen);
      إذا (temp2 != UnicodeString(value.داده(), -1, US_INV)) {
        appendWithSep(نتیجه_Remainder, temp2);
      } وإلا إذا (موقت != UnicodeString(کلید, -1, US_INV)) {
        UnicodeString temp3;
        کلید_Type_Format.format(موقت, temp2, temp3, وضعیت);
        appendWithSep(نتیجه_Remainder, temp3);
      } وإلا {
        appendWithSep(نتیجه_Remainder, موقت)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  إذا (!نتیجه_Remainder.is_خالی()) {
    format.format(نتیجه_Name, نتیجه_Remainder, نتیجه.remove(), وضعیت);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجه);
  }

  نتیجه = نتیجه_Name;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::appendWithSep(UnicodeString& میانگیر U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    إذا (میانگیر.is_خالی()) {
        میانگیر.مجموعه_مقصد(src);
    } وإلا {
        ثابت UnicodeString *مقادیر[2] = { &میانگیر, &src };
        UError_کد وضعیت = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(مقادیر, 2, میانگیر, اشاره_تهی, 0, وضعیت);
    }
    أرجع میانگیر;
}

UnicodeString&
Locale_Display_نامها_Impl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع localeDisplayName(Locale(localeId), نتیجه);
}

//خصوصی
UnicodeString&
Locale_Display_نامها_Impl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& نتیجه U_LIFETIME_BOUND, منطقي substitute) ثابت {
    إذا (name_درازی == UDISPCTX_درازی_SHORT) {
        lang_داده.getNoFallback("Languages%short", localeId, نتیجه);
        إذا (!نتیجه.isBogus()) {
            أرجع نتیجه;
        }
    }
    lang_داده.getNoFallback("Languages", localeId, نتیجه);
    إذا (نتیجه.isBogus() && uprv_strchr(localeId, '_') == اشاره_تهی) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_درازی == UDISPCTX_درازی_SHORT) {
            lang_داده.getNoFallback("Languages%short", canonLocId, نتیجه);
            إذا (!نتیجه.isBogus()) {
                أرجع نتیجه;
            }
        }
        lang_داده.getNoFallback("Languages", canonLocId, نتیجه);
    }
    إذا (نتیجه.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجه.مجموعه_مقصد(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع نتیجه;
}

UnicodeString&
Locale_Display_نامها_Impl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    إذا (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != اشاره_تهی) {
        أرجع نتیجه = UnicodeString(lang, -1, US_INV);
    }
    إذا (name_درازی == UDISPCTX_درازی_SHORT) {
        lang_داده.getNoFallback("Languages%short", lang, نتیجه);
        إذا (!نتیجه.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجه);
        }
    }
    lang_داده.getNoFallback("Languages", lang, نتیجه);
    إذا (نتیجه.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_درازی == UDISPCTX_درازی_SHORT) {
            lang_داده.getNoFallback("Languages%short", canonLocId, نتیجه);
            إذا (!نتیجه.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجه);
            }
        }
        lang_داده.getNoFallback("Languages", canonLocId, نتیجه);
    }
    إذا (نتیجه.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتیجه.مجموعه_مقصد(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_درازی == UDISPCTX_درازی_SHORT) {
        lang_داده.getNoFallback("Scripts%short", script, نتیجه);
        إذا (!نتیجه.isBogus()) {
            أرجع skipAdjust? نتیجه: adjustForUsageAndContext(kCapContextUsageScript, نتیجه);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_داده.get("Scripts", script, نتیجه);
    } وإلا {
        lang_داده.getNoFallback("Scripts", script, نتیجه);
    }
    أرجع skipAdjust? نتیجه: adjustForUsageAndContext(kCapContextUsageScript, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(script, نتیجه, خطأ);
}

UnicodeString&
Locale_Display_نامها_Impl::scriptDisplayName(UScript_کد script_کد,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(uscript_getName(script_کد), نتیجه, خطأ);
}

UnicodeString&
Locale_Display_نامها_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_درازی == UDISPCTX_درازی_SHORT) {
         region_داده.getNoFallback("Countries%short", region, نتیجه);
        إذا (!نتیجه.isBogus()) {
            أرجع skipAdjust? نتیجه: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجه);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        region_داده.get("Countries", region, نتیجه);
    } وإلا {
        region_داده.getNoFallback("Countries", region, نتیجه);
    }
    أرجع skipAdjust? نتیجه: adjustForUsageAndContext(kCapContextUsageTerritory, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع regionDisplayName(region, نتیجه, خطأ);
}


UnicodeString&
Locale_Display_نامها_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_داده.get("Variants", variant, نتیجه);
    } وإلا {
        lang_داده.getNoFallback("Variants", variant, نتیجه);
    }
    أرجع skipAdjust? نتیجه: adjustForUsageAndContext(kCapContextUsageVariant, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع variantDisplayName(variant, نتیجه, خطأ);
}

UnicodeString&
Locale_Display_نامها_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجه U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_داده.get("Keys", کلید, نتیجه);
    } وإلا {
        lang_داده.getNoFallback("Keys", کلید, نتیجه);
    }
    أرجع skipAdjust? نتیجه: adjustForUsageAndContext(k_Cap_Context_Usage_کلید, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::کلید_Display_Name(ثابت حرف* کلید,
                                       UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع کلید_Display_Name(کلید, نتیجه, خطأ);
}

UnicodeString&
Locale_Display_نامها_Impl::کلید_مقدار_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجه U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    إذا (uprv_strcmp(کلید, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_کد sts = U_ZERO_ERROR;
        UnicodeString ustr_مقدار(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_مقدار.get_Terminated_میانگیر(),
            locale.get_پایه_Name(), UCURR_LONG_NAME, اشاره_تهی /* isChoiceFormat */, &len, &sts);
        إذا (U_FAILURE(sts)) {
            // Return the value as is on failure
            نتیجه = ustr_مقدار;
            أرجع نتیجه;
        }
        نتیجه.مجموعه_مقصد(currencyName, len);
        أرجع skipAdjust? نتیجه: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, نتیجه);
    }

    إذا (name_درازی == UDISPCTX_درازی_SHORT) {
        lang_داده.getNoFallback("Types%short", کلید, value, نتیجه);
        إذا (!نتیجه.isBogus()) {
            أرجع skipAdjust? نتیجه: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, نتیجه);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_داده.get("Types", کلید, value, نتیجه);
    } وإلا {
        lang_داده.getNoFallback("Types", کلید, value, نتیجه);
    }
    أرجع skipAdjust? نتیجه: adjustForUsageAndContext(k_Cap_Context_Usage_کلید_مقدار, نتیجه);
}

UnicodeString&
Locale_Display_نامها_Impl::کلید_مقدار_Display_Name(ثابت حرف* کلید,
                                            ثابت حرف* value,
                                            UnicodeString& نتیجه U_LIFETIME_BOUND) ثابت {
    أرجع کلید_مقدار_Display_Name(کلید, value, نتیجه, خطأ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نامها*
Locale_Display_نامها::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    أرجع جديد Locale_Display_نامها_Impl(locale, dialectHandling);
}

Locale_Display_نامها*
Locale_Display_نامها::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    إذا (contexts == اشاره_تهی) {
        length = 0;
    }
    أرجع جديد Locale_Display_نامها_Impl(locale, contexts, length);
}

U_NAMESPACE_انتها

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_نامها * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع اشاره_تهی;
  }
  إذا (locale == اشاره_تهی) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نامها *)Locale_Display_نامها::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_نامها * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع اشاره_تهی;
  }
  إذا (locale == اشاره_تهی) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نامها *)Locale_Display_نامها::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocale_Display_نامها *ldn) {
  حذف (Locale_Display_نامها *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_نامها* ldn U_LIFETIME_BOUND) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نامها *)ldn)->getLocale().getName();
  }
  أرجع اشاره_تهی;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_نامها *ldn) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نامها *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_نامها;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocale_Display_نامها *ldn,
              UDisplayContextType type,
              UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع (UDisplayContext)0;
  }
  أرجع ((ثابت Locale_Display_نامها *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocale_Display_نامها *ldn,
                       ثابت حرف *locale,
                       char16_t *نتیجه,
                       int32_t max_نتیجه_اندازه,
                       UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || locale == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->localeDisplayName(locale, موقت);
  إذا (موقت.isBogus()) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocale_Display_نامها *ldn,
                         ثابت حرف *lang,
                         char16_t *نتیجه,
                         int32_t max_نتیجه_اندازه,
                         UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || lang == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->languageDisplayName(lang, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocale_Display_نامها *ldn,
                       ثابت حرف *script,
                       char16_t *نتیجه,
                       int32_t max_نتیجه_اندازه,
                       UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || script == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->scriptDisplayName(script, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_script_کد_Display_Name(ثابت ULocale_Display_نامها *ldn,
                           UScript_کد script_کد,
                           char16_t *نتیجه,
                           int32_t max_نتیجه_اندازه,
                           UError_کد *p_Error_کد) {
  أرجع uldn_scriptDisplayName(ldn, uscript_getName(script_کد), نتیجه, max_نتیجه_اندازه, p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocale_Display_نامها *ldn,
                       ثابت حرف *region,
                       char16_t *نتیجه,
                       int32_t max_نتیجه_اندازه,
                       UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || region == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->regionDisplayName(region, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocale_Display_نامها *ldn,
                        ثابت حرف *variant,
                        char16_t *نتیجه,
                        int32_t max_نتیجه_اندازه,
                        UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || variant == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->variantDisplayName(variant, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_Display_Name(ثابت ULocale_Display_نامها *ldn,
                    ثابت حرف *کلید,
                    char16_t *نتیجه,
                    int32_t max_نتیجه_اندازه,
                    UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || کلید == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0) || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->کلید_Display_Name(کلید, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uldn_کلید_مقدار_Display_Name(ثابت ULocale_Display_نامها *ldn,
                         ثابت حرف *کلید,
                         ثابت حرف *value,
                         char16_t *نتیجه,
                         int32_t max_نتیجه_اندازه,
                         UError_کد *p_Error_کد) {
  إذا (U_FAILURE(*p_Error_کد)) {
    أرجع 0;
  }
  إذا (ldn == اشاره_تهی || کلید == اشاره_تهی || value == اشاره_تهی || (نتیجه == اشاره_تهی && max_نتیجه_اندازه > 0)
      || max_نتیجه_اندازه < 0) {
    *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  UnicodeString موقت(نتیجه, 0, max_نتیجه_اندازه);
  ((ثابت Locale_Display_نامها *)ldn)->کلید_مقدار_Display_Name(کلید, value, موقت);
  أرجع موقت.extract(نتیجه, max_نتیجه_اندازه, *p_Error_کد);
}

#پایان_اگر
