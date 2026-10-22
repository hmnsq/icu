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
    ثابت حرف* ثابت رستو;
    Locale locale;

عام:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_جدول(ثابت حرف* رستو, ثابت Locale& locale);
    ~ICUData_جدول() = افتراضي;

    ثابت Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(ثابت حرف* جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                       UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
    UnicodeString& get(ثابت حرف* جدول_ڪنجي, ثابت حرف* sub_جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                       UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;

    UnicodeString& getNoFallback(ثابت حرف* جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                                 UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
    UnicodeString& getNoFallback(ثابت حرف* جدول_ڪنجي, ثابت حرف* sub_جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                                 UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
};

مضمن UnicodeString& ICUData_جدول::get(ثابت حرف* جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                                        UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع get(جدول_ڪنجي, خالي_اشارو, شي_ڪنجي, نتيجو);
}

مضمن UnicodeString& ICUData_جدول::getNoFallback(ثابت حرف* جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                                                  UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع getNoFallback(جدول_ڪنجي, خالي_اشارو, شي_ڪنجي, نتيجو);
}

ICUData_جدول::ICUData_جدول(ثابت حرف* رستو, ثابت Locale& locale)
    : رستو(رستو), locale(locale)
{
    U_ASSERT(رستو != خالي_اشارو);
}

ثابت Locale&
ICUData_جدول::getLocale() U_LIFETIME_BOUND {
  أرجع locale;
}

UnicodeString&
ICUData_جدول::get(ثابت حرف* جدول_ڪنجي, ثابت حرف* sub_جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                  UnicodeString &نتيجو U_LIFETIME_BOUND) ثابت {
  UError_ڪوڊ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(رستو, locale.getName(),
                                                   جدول_ڪنجي, sub_جدول_ڪنجي, شي_ڪنجي,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت) && len > 0) {
    أرجع نتيجو.سيٽ_منزل(s, len);
  }
  أرجع نتيجو.سيٽ_منزل(UnicodeString(شي_ڪنجي, -1, US_INV));
}

UnicodeString&
ICUData_جدول::getNoFallback(ثابت حرف* جدول_ڪنجي, ثابت حرف* sub_جدول_ڪنجي, ثابت حرف* شي_ڪنجي,
                            UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  UError_ڪوڊ حالت = U_ZERO_ERROR;
  int32_t len = 0;

  ثابت char16_t *s = uloc_get_جدول_String_With_Fallback(رستو, locale.getName(),
                                                   جدول_ڪنجي, sub_جدول_ڪنجي, شي_ڪنجي,
                                                   &len, &حالت);
  إذا (U_SUCCESS(حالت)) {
    أرجع نتيجو.سيٽ_منزل(s, len);
  }

  نتيجو.سيٽ_منزل_Bogus();
  أرجع نتيجو;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نالا::~Locale_Display_نالا() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#إذا 0  // currently unused

صنف Default_Locale_Display_نالا : عام Locale_Display_نالا {
  UDialectHandling dialectHandling;

عام:
  // constructor
  Default_Locale_Display_نالا(UDialectHandling dialectHandling);

  مجازي ~Default_Locale_Display_نالا();

  مجازي ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND;
  مجازي UDialectHandling getDialectHandling() ثابت;

  مجازي UnicodeString& localeDisplayName(ثابت Locale& locale,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& languageDisplayName(ثابت حرف* lang,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& scriptDisplayName(ثابت حرف* script,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& scriptDisplayName(UScript_ڪوڊ script_ڪوڊ,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& regionDisplayName(ثابت حرف* region,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& variantDisplayName(ثابت حرف* variant,
                                            UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& ڪنجي_Display_Name(ثابت حرف* ڪنجي,
                                        UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
  مجازي UnicodeString& ڪنجي_قدر_Display_Name(ثابت حرف* ڪنجي,
                                             ثابت حرف* value,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
};

Default_Locale_Display_نالا::Default_Locale_Display_نالا(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_نالا::~Default_Locale_Display_نالا() {
}

ثابت Locale&
Default_Locale_Display_نالا::getLocale() ثابت U_LIFETIME_BOUND {
  أرجع Locale::get_پاڙ();
}

UDialectHandling
Default_Locale_Display_نالا::getDialectHandling() ثابت {
  أرجع dialectHandling;
}

UnicodeString&
Default_Locale_Display_نالا::localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::scriptDisplayName(UScript_ڪوڊ script_ڪوڊ,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  ثابت حرف* name = uscript_getName(script_ڪوڊ);
  إذا (name) {
    أرجع نتيجو = UnicodeString(name, -1, US_INV);
  }
  أرجع نتيجو.remove();
}

UnicodeString&
Default_Locale_Display_نالا::regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::ڪنجي_Display_Name(ثابت حرف* ڪنجي,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(ڪنجي, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_نالا::ڪنجي_قدر_Display_Name(ثابت حرف* /*ڪنجي*/,
                                               ثابت حرف* value,
                                               UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  أرجع نتيجو = UnicodeString(value, -1, US_INV);
}

#جيڪڏهن_پورو  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

صنف Locale_Display_نالا_Impl : عام Locale_Display_نالا {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_جدول lang_معلومات;
    ICUData_جدول region_معلومات;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter ڪنجي_Type_Format;
    UDisplayContext capitalizationContext;
#إذا !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#وإلا
    UObject* capitalizationBrkIter;
#جيڪڏهن_پورو
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_ڊيگهائي;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    تعداد CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_ڪنجي,
        k_Cap_Context_Usage_ڪنجي_قدر,
        k_Cap_Context_Usage_ڳڻپ
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     منطقي fCapitalization[k_Cap_Context_Usage_ڳڻپ];

عام:
    // constructor
    Locale_Display_نالا_Impl(ثابت Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_نالا_Impl(ثابت Locale& locale, UDisplayContext *contexts, int32_t length);
    مجازي ~Locale_Display_نالا_Impl();

    مجازي ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND override;
    مجازي UDialectHandling getDialectHandling() ثابت override;
    مجازي UDisplayContext getContext(UDisplayContextType type) ثابت override;

    مجازي UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    languageDisplayName(ثابت حرف* lang, UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& scriptDisplayName(UScript_ڪوڊ script_ڪوڊ,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    variantDisplayName(ثابت حرف* variant, UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString& ڪنجي_Display_Name(ثابت حرف* ڪنجي,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;
    مجازي UnicodeString&
    ڪنجي_قدر_Display_Name(ثابت حرف* ڪنجي, ثابت حرف* value,
                        UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت override;

خاص:
    UnicodeString& localeIdName(ثابت حرف* localeId, UnicodeString& نتيجو U_LIFETIME_BOUND,
                                منطقي substitute) ثابت;
    UnicodeString& appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND,
                                 ثابت UnicodeString& src) ثابت;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت;
    UnicodeString& scriptDisplayName(ثابت حرف* script, UnicodeString& نتيجو U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& regionDisplayName(ثابت حرف* region, UnicodeString& نتيجو U_LIFETIME_BOUND,
                                     منطقي skipAdjust) ثابت;
    UnicodeString& variantDisplayName(ثابت حرف* variant, UnicodeString& نتيجو U_LIFETIME_BOUND,
                                      منطقي skipAdjust) ثابت;
    UnicodeString& ڪنجي_Display_Name(ثابت حرف* ڪنجي, UnicodeString& نتيجو U_LIFETIME_BOUND,
                                  منطقي skipAdjust) ثابت;
    UnicodeString& ڪنجي_قدر_Display_Name(ثابت حرف* ڪنجي, ثابت حرف* value,
                                       UnicodeString& نتيجو U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت;
    فراغ initialize();

    بنية CapitalizationContextSink;
};

Locale_Display_نالا_Impl::Locale_Display_نالا_Impl(ثابت Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_معلومات(U_ICUDATA_LANG, locale)
    , region_معلومات(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالي_اشارو)
    , name_ڊيگهائي(UDISPCTX_ڊيگهائي_ڀريل)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_نالا_Impl::Locale_Display_نالا_Impl(ثابت Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_نالا)
    , lang_معلومات(U_ICUDATA_LANG, locale)
    , region_معلومات(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(خالي_اشارو)
    , name_ڊيگهائي(UDISPCTX_ڊيگهائي_ڀريل)
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
            حالة UDISPCTX_TYPE_DISPLAY_ڊيگهائي:
                name_ڊيگهائي = value;
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

بنية Locale_Display_نالا_Impl::CapitalizationContextSink : عام ResourceSink {
    منطقي hasCapitalizationUsage;
    Locale_Display_نالا_Impl& والد;

    CapitalizationContextSink(Locale_Display_نالا_Impl& والد_2)
      : hasCapitalizationUsage(خطأ), والد(والد_2) {}
    مجازي ~CapitalizationContextSink();

    مجازي فراغ put(ثابت حرف *ڪنجي, Resource_قدر &value, UBool /*noFallback*/,
            UError_ڪوڊ &error_ڪوڊ) override {
        Resource_جدول contexts = value.get_جدول(error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        لكل (صحيح i = 0; contexts.get_ڪنجي_And_قدر(i, ڪنجي, value); ++i) {

            CapContextUsage usageEnum;
            إذا (uprv_strcmp(ڪنجي, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_ڪنجي;
            } وإلا إذا (uprv_strcmp(ڪنجي, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_ڪنجي_قدر;
            } وإلا إذا (uprv_strcmp(ڪنجي, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } وإلا إذا (uprv_strcmp(ڪنجي, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } وإلا إذا (uprv_strcmp(ڪنجي, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } وإلا إذا (uprv_strcmp(ڪنجي, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } وإلا {
                استمر;
            }

            int32_t len = 0;
            ثابت int32_t* int_ويڪٽر = value.get_Int_ويڪٽر(len, error_ڪوڊ);
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
            إذا (len < 2) { استمر; }

            int32_t titlecaseInt = (والد.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_ويڪٽر[0] : int_ويڪٽر[1];
            إذا (titlecaseInt == 0) { استمر; }

            والد.fCapitalization[usageEnum] = سچ;
            hasCapitalizationUsage = سچ;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_نالا_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

فراغ
Locale_Display_نالا_Impl::initialize() {
    Locale_Display_نالا_Impl* nonConstThis = هذا;
    nonConstThis->locale = lang_معلومات.getLocale() == Locale::get_پاڙ()
        ? region_معلومات.getLocale()
        : lang_معلومات.getLocale();

    UnicodeString sep;
    lang_معلومات.getNoFallback("localeDisplayPattern", "separator", sep);
    إذا (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    separatorFormat.apply_نمونو_Min_Max_Arguments(sep, 2, 2, حالت);

    UnicodeString نمونو;
    lang_معلومات.getNoFallback("localeDisplayPattern", "pattern", نمونو);
    إذا (نمونو.isBogus()) {
        نمونو = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_نمونو_Min_Max_Arguments(نمونو, 2, 2, حالت);
    إذا (نمونو.اشاريو_جو(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.سيٽ_منزل(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.سيٽ_منزل(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.سيٽ_منزل(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.سيٽ_منزل(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } وإلا {
        formatOpenParen.سيٽ_منزل(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.سيٽ_منزل(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.سيٽ_منزل(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.سيٽ_منزل(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_نمونو;
    lang_معلومات.get("localeDisplayPattern", "keyTypePattern", kt_نمونو);
    إذا (kt_نمونو.isBogus()) {
        kt_نمونو = UnicodeString("{0}={1}", -1, US_INV);
    }
    ڪنجي_Type_Format.apply_نمونو_Min_Max_Arguments(kt_نمونو, 2, 2, حالت);

    uprv_memset(fCapitalization, 0, حجم(fCapitalization));
#إذا !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    منطقي needBrkIter = خطأ;
    إذا (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_اشارو resource(ures_open(خالي_اشارو, locale.getName(), &حالت));
        إذا (U_FAILURE(حالت)) { أرجع; }
        CapitalizationContextSink sink(*هذا);
        ures_get_All_شيون_With_Fallback(resource.getAlias(), "contextTransforms", sink, حالت);
        إذا (حالت == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            حالت = U_ZERO_ERROR;
        } وإلا إذا (U_FAILURE(حالت)) {
            أرجع;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    إذا (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_جو_SENTENCE) {
        حالت = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, حالت);
        إذا (U_FAILURE(حالت)) {
            ميٽيو capitalizationBrkIter;
            capitalizationBrkIter = خالي_اشارو;
        }
    }
#جيڪڏهن_پورو
}

Locale_Display_نالا_Impl::~Locale_Display_نالا_Impl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    ميٽيو capitalizationBrkIter;
#جيڪڏهن_پورو
}

ثابت Locale&
Locale_Display_نالا_Impl::getLocale() ثابت U_LIFETIME_BOUND {
    أرجع locale;
}

UDialectHandling
Locale_Display_نالا_Impl::getDialectHandling() ثابت {
    أرجع dialectHandling;
}

UDisplayContext
Locale_Display_نالا_Impl::getContext(UDisplayContextType type) ثابت {
    بدل (type) {
        حالة UDISPCTX_TYPE_DIALECT_HANDLING:
            أرجع static_cast<UDisplayContext>(dialectHandling);
        حالة UDISPCTX_TYPE_CAPITALIZATION:
            أرجع capitalizationContext;
        حالة UDISPCTX_TYPE_DISPLAY_ڊيگهائي:
            أرجع name_ڊيگهائي;
        حالة UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            أرجع substitute;
        افتراضي:
            اكسر;
    }
    أرجع static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_نالا_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
#إذا !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    إذا ( نتيجو.length() > 0 && u_islower(نتيجو.char32At(0)) && capitalizationBrkIter!= خالي_اشارو &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_جو_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        ساكن UMutex capitalization_Brk_Iter_تالو;
        Mutex تالو(&capitalization_Brk_Iter_تالو);
        نتيجو.منزل_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#جيڪڏهن_پورو
    أرجع نتيجو;
}

UnicodeString&
Locale_Display_نالا_Impl::localeDisplayName(ثابت Locale& loc,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
  إذا (loc.isBogus()) {
    نتيجو.سيٽ_منزل_Bogus();
    أرجع نتيجو;
  }
  UnicodeString نتيجو_Name;

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

  إذا (dialectHandling == ULDN_DIALECT_نالا) {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    CharString بفر;
    افعل { // loop construct is so we can break early out of search
      إذا (hasScript && hasCountry) {
        بفر.append(lang, حالت)
              .append('_', حالت)
              .append(script, حالت)
              .append('_', حالت)
              .append(country, حالت);
        إذا (U_SUCCESS(حالت)) {
          localeIdName(بفر.معلومات(), نتيجو_Name, خطأ);
          إذا (!نتيجو_Name.isBogus()) {
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
          localeIdName(بفر.معلومات(), نتيجو_Name, خطأ);
          إذا (!نتيجو_Name.isBogus()) {
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
          localeIdName(بفر.معلومات(), نتيجو_Name, خطأ);
          إذا (!نتيجو_Name.isBogus()) {
            hasCountry = خطأ;
            اكسر;
          }
        }
      }
    } طالما (خطأ);
  }
  إذا (نتيجو_Name.isBogus() || نتيجو_Name.is_خالي_جايون()) {
    localeIdName(lang, نتيجو_Name, substitute == UDISPCTX_SUBSTITUTE);
    إذا (نتيجو_Name.isBogus()) {
      نتيجو.سيٽ_منزل_Bogus();
      أرجع نتيجو;
    }
  }

  UnicodeString نتيجو_Remainder;
  UnicodeString عارضي;
  UError_ڪوڊ حالت = U_ZERO_ERROR;

  إذا (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, عارضي, سچ);
    إذا (script_str.isBogus()) {
      نتيجو.سيٽ_منزل_Bogus();
      أرجع نتيجو;
    }
    نتيجو_Remainder.append(script_str);
  }
  إذا (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, عارضي, سچ);
    إذا (region_str.isBogus()) {
      نتيجو.سيٽ_منزل_Bogus();
      أرجع نتيجو;
    }
    appendWithSep(نتيجو_Remainder, region_str);
  }
  إذا (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, عارضي, سچ);
    إذا (variant_str.isBogus()) {
      نتيجو.سيٽ_منزل_Bogus();
      أرجع نتيجو;
    }
    appendWithSep(نتيجو_Remainder, variant_str);
  }
  نتيجو_Remainder.ڳوليو_And_Replace(formatOpenParen, formatReplaceOpenParen);
  نتيجو_Remainder.ڳوليو_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_اشارو<StringEnumeration> e(loc.createKeywords(حالت));
  إذا (e.isValid() && U_SUCCESS(حالت)) {
    UnicodeString temp2;
    ثابت حرف* ڪنجي;
    طالما ((ڪنجي = e->next((int32_t*)خالي_اشارو, حالت)) != خالي_اشارو) {
        خودڪار value = loc.get_Keyword_قدر<CharString>(ڪنجي, حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع نتيجو;
      }
      ڪنجي_Display_Name(ڪنجي, عارضي, سچ);
      عارضي.ڳوليو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      عارضي.ڳوليو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ڪنجي_قدر_Display_Name(ڪنجي, value.معلومات(), temp2, سچ);
      temp2.ڳوليو_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.ڳوليو_And_Replace(formatCloseParen, formatReplaceCloseParen);
      إذا (temp2 != UnicodeString(value.معلومات(), -1, US_INV)) {
        appendWithSep(نتيجو_Remainder, temp2);
      } وإلا إذا (عارضي != UnicodeString(ڪنجي, -1, US_INV)) {
        UnicodeString temp3;
        ڪنجي_Type_Format.format(عارضي, temp2, temp3, حالت);
        appendWithSep(نتيجو_Remainder, temp3);
      } وإلا {
        appendWithSep(نتيجو_Remainder, عارضي)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  إذا (!نتيجو_Remainder.is_خالي_جايون()) {
    format.format(نتيجو_Name, نتيجو_Remainder, نتيجو.remove(), حالت);
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتيجو);
  }

  نتيجو = نتيجو_Name;
  أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::appendWithSep(UnicodeString& بفر U_LIFETIME_BOUND, ثابت UnicodeString& src) ثابت {
    إذا (بفر.is_خالي_جايون()) {
        بفر.سيٽ_منزل(src);
    } وإلا {
        ثابت UnicodeString *قدرون[2] = { &بفر, &src };
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(قدرون, 2, بفر, خالي_اشارو, 0, حالت);
    }
    أرجع بفر;
}

UnicodeString&
Locale_Display_نالا_Impl::localeDisplayName(ثابت حرف* localeId,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع localeDisplayName(Locale(localeId), نتيجو);
}

//ذاتي
UnicodeString&
Locale_Display_نالا_Impl::localeIdName(ثابت حرف* localeId,
                                     UnicodeString& نتيجو U_LIFETIME_BOUND, منطقي substitute) ثابت {
    إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
        lang_معلومات.getNoFallback("Languages%short", localeId, نتيجو);
        إذا (!نتيجو.isBogus()) {
            أرجع نتيجو;
        }
    }
    lang_معلومات.getNoFallback("Languages", localeId, نتيجو);
    إذا (نتيجو.isBogus() && uprv_strchr(localeId, '_') == خالي_اشارو) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
            lang_معلومات.getNoFallback("Languages%short", canonLocId, نتيجو);
            إذا (!نتيجو.isBogus()) {
                أرجع نتيجو;
            }
        }
        lang_معلومات.getNoFallback("Languages", canonLocId, نتيجو);
    }
    إذا (نتيجو.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتيجو.سيٽ_منزل(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    أرجع نتيجو;
}

UnicodeString&
Locale_Display_نالا_Impl::languageDisplayName(ثابت حرف* lang,
                                            UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    إذا (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != خالي_اشارو) {
        أرجع نتيجو = UnicodeString(lang, -1, US_INV);
    }
    إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
        lang_معلومات.getNoFallback("Languages%short", lang, نتيجو);
        إذا (!نتيجو.isBogus()) {
            أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتيجو);
        }
    }
    lang_معلومات.getNoFallback("Languages", lang, نتيجو);
    إذا (نتيجو.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        ثابت حرف* canonLocId = canonLocale.getName();
        إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
            lang_معلومات.getNoFallback("Languages%short", canonLocId, نتيجو);
            إذا (!نتيجو.isBogus()) {
                أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتيجو);
            }
        }
        lang_معلومات.getNoFallback("Languages", canonLocId, نتيجو);
    }
    إذا (نتيجو.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        نتيجو.سيٽ_منزل(UnicodeString(lang, -1, US_INV)); // use key (
    }
    أرجع adjustForUsageAndContext(kCapContextUsageLanguage, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
        lang_معلومات.getNoFallback("Scripts%short", script, نتيجو);
        إذا (!نتيجو.isBogus()) {
            أرجع skipAdjust? نتيجو: adjustForUsageAndContext(kCapContextUsageScript, نتيجو);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Scripts", script, نتيجو);
    } وإلا {
        lang_معلومات.getNoFallback("Scripts", script, نتيجو);
    }
    أرجع skipAdjust? نتيجو: adjustForUsageAndContext(kCapContextUsageScript, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::scriptDisplayName(ثابت حرف* script,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(script, نتيجو, خطأ);
}

UnicodeString&
Locale_Display_نالا_Impl::scriptDisplayName(UScript_ڪوڊ script_ڪوڊ,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع scriptDisplayName(uscript_getName(script_ڪوڊ), نتيجو, خطأ);
}

UnicodeString&
Locale_Display_نالا_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND,
                                          منطقي skipAdjust) ثابت {
    إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
         region_معلومات.getNoFallback("Countries%short", region, نتيجو);
        إذا (!نتيجو.isBogus()) {
            أرجع skipAdjust? نتيجو: adjustForUsageAndContext(kCapContextUsageTerritory, نتيجو);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        region_معلومات.get("Countries", region, نتيجو);
    } وإلا {
        region_معلومات.getNoFallback("Countries", region, نتيجو);
    }
    أرجع skipAdjust? نتيجو: adjustForUsageAndContext(kCapContextUsageTerritory, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::regionDisplayName(ثابت حرف* region,
                                          UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع regionDisplayName(region, نتيجو, خطأ);
}


UnicodeString&
Locale_Display_نالا_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND,
                                           منطقي skipAdjust) ثابت {
    // don't have a resource for short variant names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Variants", variant, نتيجو);
    } وإلا {
        lang_معلومات.getNoFallback("Variants", variant, نتيجو);
    }
    أرجع skipAdjust? نتيجو: adjustForUsageAndContext(kCapContextUsageVariant, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::variantDisplayName(ثابت حرف* variant,
                                           UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع variantDisplayName(variant, نتيجو, خطأ);
}

UnicodeString&
Locale_Display_نالا_Impl::ڪنجي_Display_Name(ثابت حرف* ڪنجي,
                                       UnicodeString& نتيجو U_LIFETIME_BOUND,
                                       منطقي skipAdjust) ثابت {
    // don't have a resource for short key names
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Keys", ڪنجي, نتيجو);
    } وإلا {
        lang_معلومات.getNoFallback("Keys", ڪنجي, نتيجو);
    }
    أرجع skipAdjust? نتيجو: adjustForUsageAndContext(k_Cap_Context_Usage_ڪنجي, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::ڪنجي_Display_Name(ثابت حرف* ڪنجي,
                                       UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع ڪنجي_Display_Name(ڪنجي, نتيجو, خطأ);
}

UnicodeString&
Locale_Display_نالا_Impl::ڪنجي_قدر_Display_Name(ثابت حرف* ڪنجي,
                                            ثابت حرف* value,
                                            UnicodeString& نتيجو U_LIFETIME_BOUND,
                                            منطقي skipAdjust) ثابت {
    إذا (uprv_strcmp(ڪنجي, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_ڪوڊ sts = U_ZERO_ERROR;
        UnicodeString ustr_قدر(value, -1, US_INV);
        int32_t len;
        ثابت char16_t *currencyName = ucurr_getName(ustr_قدر.get_Terminated_بفر(),
            locale.get_بنياد_Name(), UCURR_LONG_NAME, خالي_اشارو /* isChoiceFormat */, &len, &sts);
        إذا (U_FAILURE(sts)) {
            // Return the value as is on failure
            نتيجو = ustr_قدر;
            أرجع نتيجو;
        }
        نتيجو.سيٽ_منزل(currencyName, len);
        أرجع skipAdjust? نتيجو: adjustForUsageAndContext(k_Cap_Context_Usage_ڪنجي_قدر, نتيجو);
    }

    إذا (name_ڊيگهائي == UDISPCTX_ڊيگهائي_SHORT) {
        lang_معلومات.getNoFallback("Types%short", ڪنجي, value, نتيجو);
        إذا (!نتيجو.isBogus()) {
            أرجع skipAdjust? نتيجو: adjustForUsageAndContext(k_Cap_Context_Usage_ڪنجي_قدر, نتيجو);
        }
    }
    إذا (substitute == UDISPCTX_SUBSTITUTE) {
        lang_معلومات.get("Types", ڪنجي, value, نتيجو);
    } وإلا {
        lang_معلومات.getNoFallback("Types", ڪنجي, value, نتيجو);
    }
    أرجع skipAdjust? نتيجو: adjustForUsageAndContext(k_Cap_Context_Usage_ڪنجي_قدر, نتيجو);
}

UnicodeString&
Locale_Display_نالا_Impl::ڪنجي_قدر_Display_Name(ثابت حرف* ڪنجي,
                                            ثابت حرف* value,
                                            UnicodeString& نتيجو U_LIFETIME_BOUND) ثابت {
    أرجع ڪنجي_قدر_Display_Name(ڪنجي, value, نتيجو, خطأ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_نالا*
Locale_Display_نالا::createInstance(ثابت Locale& locale,
                                   UDialectHandling dialectHandling) {
    أرجع جديد Locale_Display_نالا_Impl(locale, dialectHandling);
}

Locale_Display_نالا*
Locale_Display_نالا::createInstance(ثابت Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    إذا (contexts == خالي_اشارو) {
        length = 0;
    }
    أرجع جديد Locale_Display_نالا_Impl(locale, contexts, length);
}

U_NAMESPACE_پڇاڙي

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_نالا * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع خالي_اشارو;
  }
  إذا (locale == خالي_اشارو) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نالا *)Locale_Display_نالا::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_نالا * U_EXPORT2
uldn_openForContext(ثابت حرف * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع خالي_اشارو;
  }
  إذا (locale == خالي_اشارو) {
    locale = uloc_getDefault();
  }
  أرجع (ULocale_Display_نالا *)Locale_Display_نالا::createInstance(Locale(locale), contexts, length);
}


U_CAPI فراغ U_EXPORT2
uldn_close(ULocale_Display_نالا *ldn) {
  ميٽيو (Locale_Display_نالا *)ldn;
}

U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_نالا* ldn U_LIFETIME_BOUND) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نالا *)ldn)->getLocale().getName();
  }
  أرجع خالي_اشارو;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_نالا *ldn) {
  إذا (ldn) {
    أرجع ((ثابت Locale_Display_نالا *)ldn)->getDialectHandling();
  }
  أرجع ULDN_STANDARD_نالا;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ثابت ULocale_Display_نالا *ldn,
              UDisplayContextType type,
              UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع (UDisplayContext)0;
  }
  أرجع ((ثابت Locale_Display_نالا *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ثابت ULocale_Display_نالا *ldn,
                       ثابت حرف *locale,
                       char16_t *نتيجو,
                       int32_t max_نتيجو_سائيز,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || locale == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->localeDisplayName(locale, عارضي);
  إذا (عارضي.isBogus()) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ثابت ULocale_Display_نالا *ldn,
                         ثابت حرف *lang,
                         char16_t *نتيجو,
                         int32_t max_نتيجو_سائيز,
                         UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || lang == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->languageDisplayName(lang, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ثابت ULocale_Display_نالا *ldn,
                       ثابت حرف *script,
                       char16_t *نتيجو,
                       int32_t max_نتيجو_سائيز,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || script == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->scriptDisplayName(script, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_script_ڪوڊ_Display_Name(ثابت ULocale_Display_نالا *ldn,
                           UScript_ڪوڊ script_ڪوڊ,
                           char16_t *نتيجو,
                           int32_t max_نتيجو_سائيز,
                           UError_ڪوڊ *p_Error_ڪوڊ) {
  أرجع uldn_scriptDisplayName(ldn, uscript_getName(script_ڪوڊ), نتيجو, max_نتيجو_سائيز, p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ثابت ULocale_Display_نالا *ldn,
                       ثابت حرف *region,
                       char16_t *نتيجو,
                       int32_t max_نتيجو_سائيز,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || region == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->regionDisplayName(region, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ثابت ULocale_Display_نالا *ldn,
                        ثابت حرف *variant,
                        char16_t *نتيجو,
                        int32_t max_نتيجو_سائيز,
                        UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || variant == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->variantDisplayName(variant, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_ڪنجي_Display_Name(ثابت ULocale_Display_نالا *ldn,
                    ثابت حرف *ڪنجي,
                    char16_t *نتيجو,
                    int32_t max_نتيجو_سائيز,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || ڪنجي == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0) || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->ڪنجي_Display_Name(ڪنجي, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
uldn_ڪنجي_قدر_Display_Name(ثابت ULocale_Display_نالا *ldn,
                         ثابت حرف *ڪنجي,
                         ثابت حرف *value,
                         char16_t *نتيجو,
                         int32_t max_نتيجو_سائيز,
                         UError_ڪوڊ *p_Error_ڪوڊ) {
  إذا (U_FAILURE(*p_Error_ڪوڊ)) {
    أرجع 0;
  }
  إذا (ldn == خالي_اشارو || ڪنجي == خالي_اشارو || value == خالي_اشارو || (نتيجو == خالي_اشارو && max_نتيجو_سائيز > 0)
      || max_نتيجو_سائيز < 0) {
    *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  UnicodeString عارضي(نتيجو, 0, max_نتيجو_سائيز);
  ((ثابت Locale_Display_نالا *)ldn)->ڪنجي_قدر_Display_Name(ڪنجي, value, عارضي);
  أرجع عارضي.extract(نتيجو, max_نتيجو_سائيز, *p_Error_ڪوڊ);
}

#جيڪڏهن_پورو
