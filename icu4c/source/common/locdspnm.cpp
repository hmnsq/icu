// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING

#అన్తర్భావ "unicode/locdspnm.h"
#అన్తర్భావ "unicode/simpleformatter.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/udisplaycontext.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/ucurr.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ureslocs.h"
#అన్తర్భావ "uresimp.h"

U_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////

// Access resource data for locale components.
// Wrap code in uloc.c for now.
తరగతి ICUData_పట్టిక {
    స్థిరరాశి అక్షరం* స్థిరరాశి మార్గం;
    Locale locale;

బహిరంగ:
    // Note: path should be a pointer to a statically allocated string.
    ICUData_పట్టిక(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి Locale& locale);
    ~ICUData_పట్టిక() = అప్రమేయ;

    స్థిరరాశి Locale& getLocale() U_LIFETIME_BOUND;

    UnicodeString& get(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                       UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
    UnicodeString& get(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* sub_పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                       UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;

    UnicodeString& getNoFallback(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                                 UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
    UnicodeString& getNoFallback(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* sub_పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                                 UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
};

అంతఃవరుస UnicodeString& ICUData_పట్టిక::get(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                                        UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి get(పట్టిక_కీలకం, శూన్యసూచిక, అంశం_కీలకం, ఫలితం);
}

అంతఃవరుస UnicodeString& ICUData_పట్టిక::getNoFallback(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                                                  UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి getNoFallback(పట్టిక_కీలకం, శూన్యసూచిక, అంశం_కీలకం, ఫలితం);
}

ICUData_పట్టిక::ICUData_పట్టిక(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి Locale& locale)
    : మార్గం(మార్గం), locale(locale)
{
    U_ASSERT(మార్గం != శూన్యసూచిక);
}

స్థిరరాశి Locale&
ICUData_పట్టిక::getLocale() U_LIFETIME_BOUND {
  తిరిగి locale;
}

UnicodeString&
ICUData_పట్టిక::get(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* sub_పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                  UnicodeString &ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  int32_t len = 0;

  స్థిరరాశి char16_t *s = uloc_get_పట్టిక_String_With_Fallback(మార్గం, locale.getName(),
                                                   పట్టిక_కీలకం, sub_పట్టిక_కీలకం, అంశం_కీలకం,
                                                   &len, &స్థితి_2);
  ఒకవేళ (U_SUCCESS(స్థితి_2) && len > 0) {
    తిరిగి ఫలితం.సమితి_గమ్యం(s, len);
  }
  తిరిగి ఫలితం.సమితి_గమ్యం(UnicodeString(అంశం_కీలకం, -1, US_INV));
}

UnicodeString&
ICUData_పట్టిక::getNoFallback(స్థిరరాశి అక్షరం* పట్టిక_కీలకం, స్థిరరాశి అక్షరం* sub_పట్టిక_కీలకం, స్థిరరాశి అక్షరం* అంశం_కీలకం,
                            UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  int32_t len = 0;

  స్థిరరాశి char16_t *s = uloc_get_పట్టిక_String_With_Fallback(మార్గం, locale.getName(),
                                                   పట్టిక_కీలకం, sub_పట్టిక_కీలకం, అంశం_కీలకం,
                                                   &len, &స్థితి_2);
  ఒకవేళ (U_SUCCESS(స్థితి_2)) {
    తిరిగి ఫలితం.సమితి_గమ్యం(s, len);
  }

  ఫలితం.సమితి_గమ్యం_Bogus();
  తిరిగి ఫలితం;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_పేర్లు::~Locale_Display_పేర్లు() {}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ఒకవేళ 0  // currently unused

తరగతి Default_Locale_Display_పేర్లు : బహిరంగ Locale_Display_పేర్లు {
  UDialectHandling dialectHandling;

బహిరంగ:
  // constructor
  Default_Locale_Display_పేర్లు(UDialectHandling dialectHandling);

  మాయా ~Default_Locale_Display_పేర్లు();

  మాయా స్థిరరాశి Locale& getLocale() స్థిరరాశి U_LIFETIME_BOUND;
  మాయా UDialectHandling getDialectHandling() స్థిరరాశి;

  మాయా UnicodeString& localeDisplayName(స్థిరరాశి Locale& locale,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& localeDisplayName(స్థిరరాశి అక్షరం* localeId,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& languageDisplayName(స్థిరరాశి అక్షరం* lang,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& scriptDisplayName(UScript_సంకేతం script_సంకేతం,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& regionDisplayName(స్థిరరాశి అక్షరం* region,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& variantDisplayName(స్థిరరాశి అక్షరం* variant,
                                            UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                        UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
  మాయా UnicodeString& కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                             స్థిరరాశి అక్షరం* value,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
};

Default_Locale_Display_పేర్లు::Default_Locale_Display_పేర్లు(UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling) {
}

Default_Locale_Display_పేర్లు::~Default_Locale_Display_పేర్లు() {
}

స్థిరరాశి Locale&
Default_Locale_Display_పేర్లు::getLocale() స్థిరరాశి U_LIFETIME_BOUND {
  తిరిగి Locale::get_మూలం();
}

UDialectHandling
Default_Locale_Display_పేర్లు::getDialectHandling() స్థిరరాశి {
  తిరిగి dialectHandling;
}

UnicodeString&
Default_Locale_Display_పేర్లు::localeDisplayName(స్థిరరాశి Locale& locale,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(locale.getName(), -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::localeDisplayName(స్థిరరాశి అక్షరం* localeId,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(localeId, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::languageDisplayName(స్థిరరాశి అక్షరం* lang,
                                               UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(lang, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(script, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::scriptDisplayName(UScript_సంకేతం script_సంకేతం,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  స్థిరరాశి అక్షరం* name = uscript_getName(script_సంకేతం);
  ఒకవేళ (name) {
    తిరిగి ఫలితం = UnicodeString(name, -1, US_INV);
  }
  తిరిగి ఫలితం.remove();
}

UnicodeString&
Default_Locale_Display_పేర్లు::regionDisplayName(స్థిరరాశి అక్షరం* region,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(region, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::variantDisplayName(స్థిరరాశి అక్షరం* variant,
                                              UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(variant, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(కీలకం, -1, US_INV);
}

UnicodeString&
Default_Locale_Display_పేర్లు::కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* /*కీలకం*/,
                                               స్థిరరాశి అక్షరం* value,
                                               UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  తిరిగి ఫలితం = UnicodeString(value, -1, US_INV);
}

#అయితే_ముగింపు  // currently unused class DefaultLocaleDisplayNames

////////////////////////////////////////////////////////////////////////////////////////////////////

తరగతి Locale_Display_పేర్లు_Impl : బహిరంగ Locale_Display_పేర్లు {
    Locale locale;
    UDialectHandling dialectHandling;
    ICUData_పట్టిక lang_దత్తాంశం;
    ICUData_పట్టిక region_దత్తాంశం;
    SimpleFormatter separatorFormat;
    SimpleFormatter format;
    SimpleFormatter కీలకం_Type_Format;
    UDisplayContext capitalizationContext;
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    BreakIterator* capitalizationBrkIter;
#లేకపోతే
    UObject* capitalizationBrkIter;
#అయితే_ముగింపు
    UnicodeString formatOpenParen;
    UnicodeString formatReplaceOpenParen;
    UnicodeString formatCloseParen;
    UnicodeString formatReplaceCloseParen;
    UDisplayContext name_నిడివి;
    UDisplayContext substitute;

    // Constants for capitalization context usage types.
    గణన CapContextUsage {
        kCapContextUsageLanguage,
        kCapContextUsageScript,
        kCapContextUsageTerritory,
        kCapContextUsageVariant,
        k_Cap_Context_Usage_కీలకం,
        k_Cap_Context_Usage_కీలకం_విలువ,
        k_Cap_Context_Usage_లెక్క
    };
    // Capitalization transforms. For each usage type, indicates whether to titlecase for
    // the context specified in capitalizationContext (which we know at construction time)
     తార్కిక fCapitalization[k_Cap_Context_Usage_లెక్క];

బహిరంగ:
    // constructor
    Locale_Display_పేర్లు_Impl(స్థిరరాశి Locale& locale, UDialectHandling dialectHandling);
    Locale_Display_పేర్లు_Impl(స్థిరరాశి Locale& locale, UDisplayContext *contexts, int32_t length);
    మాయా ~Locale_Display_పేర్లు_Impl();

    మాయా స్థిరరాశి Locale& getLocale() స్థిరరాశి U_LIFETIME_BOUND override;
    మాయా UDialectHandling getDialectHandling() స్థిరరాశి override;
    మాయా UDisplayContext getContext(UDisplayContextType type) స్థిరరాశి override;

    మాయా UnicodeString& localeDisplayName(స్థిరరాశి Locale& locale,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString& localeDisplayName(స్థిరరాశి అక్షరం* localeId,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString&
    languageDisplayName(స్థిరరాశి అక్షరం* lang, UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString& scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString& scriptDisplayName(UScript_సంకేతం script_సంకేతం,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString& regionDisplayName(స్థిరరాశి అక్షరం* region,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString&
    variantDisplayName(స్థిరరాశి అక్షరం* variant, UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString& కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;
    మాయా UnicodeString&
    కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం, స్థిరరాశి అక్షరం* value,
                        UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి override;

అంతరంగ:
    UnicodeString& localeIdName(స్థిరరాశి అక్షరం* localeId, UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                తార్కిక substitute) స్థిరరాశి;
    UnicodeString& appendWithSep(UnicodeString& బఫరు U_LIFETIME_BOUND,
                                 స్థిరరాశి UnicodeString& src) స్థిరరాశి;
    UnicodeString& adjustForUsageAndContext(CapContextUsage usage,
                                            UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి;
    UnicodeString& scriptDisplayName(స్థిరరాశి అక్షరం* script, UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                     తార్కిక skipAdjust) స్థిరరాశి;
    UnicodeString& regionDisplayName(స్థిరరాశి అక్షరం* region, UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                     తార్కిక skipAdjust) స్థిరరాశి;
    UnicodeString& variantDisplayName(స్థిరరాశి అక్షరం* variant, UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                      తార్కిక skipAdjust) స్థిరరాశి;
    UnicodeString& కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం, UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                  తార్కిక skipAdjust) స్థిరరాశి;
    UnicodeString& కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం, స్థిరరాశి అక్షరం* value,
                                       UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                       తార్కిక skipAdjust) స్థిరరాశి;
    శూన్య initialize();

    నిర్మాణం CapitalizationContextSink;
};

Locale_Display_పేర్లు_Impl::Locale_Display_పేర్లు_Impl(స్థిరరాశి Locale& locale,
                                               UDialectHandling dialectHandling)
    : dialectHandling(dialectHandling)
    , lang_దత్తాంశం(U_ICUDATA_LANG, locale)
    , region_దత్తాంశం(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(శూన్యసూచిక)
    , name_నిడివి(UDISPCTX_నిడివి_నిండిన)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    initialize();
}

Locale_Display_పేర్లు_Impl::Locale_Display_పేర్లు_Impl(స్థిరరాశి Locale& locale,
                                               UDisplayContext *contexts, int32_t length)
    : dialectHandling(ULDN_STANDARD_పేర్లు)
    , lang_దత్తాంశం(U_ICUDATA_LANG, locale)
    , region_దత్తాంశం(U_ICUDATA_REGION, locale)
    , capitalizationContext(UDISPCTX_CAPITALIZATION_NONE)
    , capitalizationBrkIter(శూన్యసూచిక)
    , name_నిడివి(UDISPCTX_నిడివి_నిండిన)
    , substitute(UDISPCTX_SUBSTITUTE)
{
    అయితే (length-- > 0) {
        UDisplayContext value = *contexts++;
        UDisplayContextType selector =
            static_cast<UDisplayContextType>(static_cast<uint32_t>(value) >> 8);
        ఎంపిక (selector) {
            స్థితి UDISPCTX_TYPE_DIALECT_HANDLING:
                dialectHandling = static_cast<UDialectHandling>(value);
                ఆపు;
            స్థితి UDISPCTX_TYPE_CAPITALIZATION:
                capitalizationContext = value;
                ఆపు;
            స్థితి UDISPCTX_TYPE_DISPLAY_నిడివి:
                name_నిడివి = value;
                ఆపు;
            స్థితి UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
                substitute = value;
                ఆపు;
            అప్రమేయ:
                ఆపు;
        }
    }
    initialize();
}

నిర్మాణం Locale_Display_పేర్లు_Impl::CapitalizationContextSink : బహిరంగ ResourceSink {
    తార్కిక hasCapitalizationUsage;
    Locale_Display_పేర్లు_Impl& మాతృ;

    CapitalizationContextSink(Locale_Display_పేర్లు_Impl& మాతృ_2)
      : hasCapitalizationUsage(అబద్ధం), మాతృ(మాతృ_2) {}
    మాయా ~CapitalizationContextSink();

    మాయా శూన్య put(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &value, UBool /*noFallback*/,
            UError_సంకేతం &error_సంకేతం) override {
        Resource_పట్టిక contexts = value.get_పట్టిక(error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        కొరకు (పూర్ణాంకం i = 0; contexts.get_కీలకం_And_విలువ(i, కీలకం, value); ++i) {

            CapContextUsage usageEnum;
            ఒకవేళ (uprv_strcmp(కీలకం, "key") == 0) {
                usageEnum = k_Cap_Context_Usage_కీలకం;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "keyValue") == 0) {
                usageEnum = k_Cap_Context_Usage_కీలకం_విలువ;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "languages") == 0) {
                usageEnum = kCapContextUsageLanguage;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "script") == 0) {
                usageEnum = kCapContextUsageScript;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "territory") == 0) {
                usageEnum = kCapContextUsageTerritory;
            } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, "variant") == 0) {
                usageEnum = kCapContextUsageVariant;
            } లేకపోతే {
                కొనసాగు;
            }

            int32_t len = 0;
            స్థిరరాశి int32_t* int_సదిశ = value.get_Int_సదిశ(len, error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
            ఒకవేళ (len < 2) { కొనసాగు; }

            int32_t titlecaseInt = (మాతృ.capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU) ? int_సదిశ[0] : int_సదిశ[1];
            ఒకవేళ (titlecaseInt == 0) { కొనసాగు; }

            మాతృ.fCapitalization[usageEnum] = నిజం;
            hasCapitalizationUsage = నిజం;
        }
    }
};

// Virtual destructors must be defined out of line.
Locale_Display_పేర్లు_Impl::CapitalizationContextSink::~CapitalizationContextSink() {}

శూన్య
Locale_Display_పేర్లు_Impl::initialize() {
    Locale_Display_పేర్లు_Impl* nonConstThis = ఇది;
    nonConstThis->locale = lang_దత్తాంశం.getLocale() == Locale::get_మూలం()
        ? region_దత్తాంశం.getLocale()
        : lang_దత్తాంశం.getLocale();

    UnicodeString sep;
    lang_దత్తాంశం.getNoFallback("localeDisplayPattern", "separator", sep);
    ఒకవేళ (sep.isBogus()) {
        sep = UnicodeString("{0}, {1}", -1, US_INV);
    }
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    separatorFormat.apply_నమూనా_Min_Max_Arguments(sep, 2, 2, స్థితి_2);

    UnicodeString నమూనా;
    lang_దత్తాంశం.getNoFallback("localeDisplayPattern", "pattern", నమూనా);
    ఒకవేళ (నమూనా.isBogus()) {
        నమూనా = UnicodeString("{0} ({1})", -1, US_INV);
    }
    format.apply_నమూనా_Min_Max_Arguments(నమూనా, 2, 2, స్థితి_2);
    ఒకవేళ (నమూనా.సూచీ_యొక్క(static_cast<char16_t>(0xFF08)) >= 0) {
        formatOpenParen.సమితి_గమ్యం(static_cast<char16_t>(0xFF08));         // fullwidth (
        formatReplaceOpenParen.సమితి_గమ్యం(static_cast<char16_t>(0xFF3B));  // fullwidth [
        formatCloseParen.సమితి_గమ్యం(static_cast<char16_t>(0xFF09));        // fullwidth )
        formatReplaceCloseParen.సమితి_గమ్యం(static_cast<char16_t>(0xFF3D)); // fullwidth ]
    } లేకపోతే {
        formatOpenParen.సమితి_గమ్యం(static_cast<char16_t>(0x0028));         // (
        formatReplaceOpenParen.సమితి_గమ్యం(static_cast<char16_t>(0x005B));  // [
        formatCloseParen.సమితి_గమ్యం(static_cast<char16_t>(0x0029));        // )
        formatReplaceCloseParen.సమితి_గమ్యం(static_cast<char16_t>(0x005D)); // ]
    }

    UnicodeString kt_నమూనా;
    lang_దత్తాంశం.get("localeDisplayPattern", "keyTypePattern", kt_నమూనా);
    ఒకవేళ (kt_నమూనా.isBogus()) {
        kt_నమూనా = UnicodeString("{0}={1}", -1, US_INV);
    }
    కీలకం_Type_Format.apply_నమూనా_Min_Max_Arguments(kt_నమూనా, 2, 2, స్థితి_2);

    uprv_memset(fCapitalization, 0, పరిమాణం(fCapitalization));
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    // Only get the context data if we need it! This is a const object so we know now...
    // Also check whether we will need a break iterator (depends on the data)
    తార్కిక needBrkIter = అబద్ధం;
    ఒకవేళ (capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_STANDALONE) {
        Local_UResource_Bundle_సూచిక resource(ures_open(శూన్యసూచిక, locale.getName(), &స్థితి_2));
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        CapitalizationContextSink sink(*ఇది);
        ures_get_All_వస్తువులు_With_Fallback(resource.getAlias(), "contextTransforms", sink, స్థితి_2);
        ఒకవేళ (స్థితి_2 == U_MISSING_RESOURCE_ERROR) {
            // Silently ignore.  Not every locale has contextTransforms.
            స్థితి_2 = U_ZERO_ERROR;
        } లేకపోతే ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి;
        }
        needBrkIter = sink.hasCapitalizationUsage;
    }
    // Get a sentence break iterator if we will need it
    ఒకవేళ (needBrkIter || capitalizationContext == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_యొక్క_SENTENCE) {
        స్థితి_2 = U_ZERO_ERROR;
        capitalizationBrkIter = BreakIterator::createSentenceInstance(locale, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తొలగించు capitalizationBrkIter;
            capitalizationBrkIter = శూన్యసూచిక;
        }
    }
#అయితే_ముగింపు
}

Locale_Display_పేర్లు_Impl::~Locale_Display_పేర్లు_Impl() {
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    తొలగించు capitalizationBrkIter;
#అయితే_ముగింపు
}

స్థిరరాశి Locale&
Locale_Display_పేర్లు_Impl::getLocale() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి locale;
}

UDialectHandling
Locale_Display_పేర్లు_Impl::getDialectHandling() స్థిరరాశి {
    తిరిగి dialectHandling;
}

UDisplayContext
Locale_Display_పేర్లు_Impl::getContext(UDisplayContextType type) స్థిరరాశి {
    ఎంపిక (type) {
        స్థితి UDISPCTX_TYPE_DIALECT_HANDLING:
            తిరిగి static_cast<UDisplayContext>(dialectHandling);
        స్థితి UDISPCTX_TYPE_CAPITALIZATION:
            తిరిగి capitalizationContext;
        స్థితి UDISPCTX_TYPE_DISPLAY_నిడివి:
            తిరిగి name_నిడివి;
        స్థితి UDISPCTX_TYPE_SUBSTITUTE_HANDLING:
            తిరిగి substitute;
        అప్రమేయ:
            ఆపు;
    }
    తిరిగి static_cast<UDisplayContext>(0);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::adjustForUsageAndContext(CapContextUsage usage,
                                                 UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    // check to see whether we need to titlecase result
    ఒకవేళ ( ఫలితం.length() > 0 && u_islower(ఫలితం.char32At(0)) && capitalizationBrkIter!= శూన్యసూచిక &&
          ( capitalizationContext==UDISPCTX_CAPITALIZATION_FOR_BEGINNING_యొక్క_SENTENCE || fCapitalization[usage] ) ) {
        // note fCapitalization[usage] won't be set unless capitalizationContext is UI_LIST_OR_MENU or STANDALONE
        స్థిర UMutex capitalization_Brk_Iter_తాళం;
        Mutex తాళం(&capitalization_Brk_Iter_తాళం);
        ఫలితం.గమ్యం_Title(capitalizationBrkIter, locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    }
#అయితే_ముగింపు
    తిరిగి ఫలితం;
}

UnicodeString&
Locale_Display_పేర్లు_Impl::localeDisplayName(స్థిరరాశి Locale& loc,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
  ఒకవేళ (loc.isBogus()) {
    ఫలితం.సమితి_గమ్యం_Bogus();
    తిరిగి ఫలితం;
  }
  UnicodeString ఫలితం_Name;

  స్థిరరాశి అక్షరం* lang = loc.getLanguage();
  ఒకవేళ (uprv_strlen(lang) == 0) {
    lang = "root";
  }
  స్థిరరాశి అక్షరం* script = loc.getScript();
  స్థిరరాశి అక్షరం* country = loc.getCountry();
  స్థిరరాశి అక్షరం* variant = loc.getVariant();

  తార్కిక hasScript = uprv_strlen(script) > 0;
  తార్కిక hasCountry = uprv_strlen(country) > 0;
  తార్కిక hasVariant = uprv_strlen(variant) > 0;

  ఒకవేళ (dialectHandling == ULDN_DIALECT_పేర్లు) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    CharString బఫరు;
    చేయి { // loop construct is so we can break early out of search
      ఒకవేళ (hasScript && hasCountry) {
        బఫరు.append(lang, స్థితి_2)
              .append('_', స్థితి_2)
              .append(script, స్థితి_2)
              .append('_', స్థితి_2)
              .append(country, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
          localeIdName(బఫరు.దత్తాంశం(), ఫలితం_Name, అబద్ధం);
          ఒకవేళ (!ఫలితం_Name.isBogus()) {
            hasScript = అబద్ధం;
            hasCountry = అబద్ధం;
            ఆపు;
          }
        }
      }
      ఒకవేళ (hasScript) {
        బఫరు.append(lang, స్థితి_2)
              .append('_', స్థితి_2)
              .append(script, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
          localeIdName(బఫరు.దత్తాంశం(), ఫలితం_Name, అబద్ధం);
          ఒకవేళ (!ఫలితం_Name.isBogus()) {
            hasScript = అబద్ధం;
            ఆపు;
          }
        }
      }
      ఒకవేళ (hasCountry) {
        బఫరు.append(lang, స్థితి_2)
              .append('_', స్థితి_2)
              .append(country, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
          localeIdName(బఫరు.దత్తాంశం(), ఫలితం_Name, అబద్ధం);
          ఒకవేళ (!ఫలితం_Name.isBogus()) {
            hasCountry = అబద్ధం;
            ఆపు;
          }
        }
      }
    } అయితే (అబద్ధం);
  }
  ఒకవేళ (ఫలితం_Name.isBogus() || ఫలితం_Name.is_ఖాళీలు()) {
    localeIdName(lang, ఫలితం_Name, substitute == UDISPCTX_SUBSTITUTE);
    ఒకవేళ (ఫలితం_Name.isBogus()) {
      ఫలితం.సమితి_గమ్యం_Bogus();
      తిరిగి ఫలితం;
    }
  }

  UnicodeString ఫలితం_Remainder;
  UnicodeString తాత్కాలిక;
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

  ఒకవేళ (hasScript) {
    UnicodeString script_str = scriptDisplayName(script, తాత్కాలిక, నిజం);
    ఒకవేళ (script_str.isBogus()) {
      ఫలితం.సమితి_గమ్యం_Bogus();
      తిరిగి ఫలితం;
    }
    ఫలితం_Remainder.append(script_str);
  }
  ఒకవేళ (hasCountry) {
    UnicodeString region_str = regionDisplayName(country, తాత్కాలిక, నిజం);
    ఒకవేళ (region_str.isBogus()) {
      ఫలితం.సమితి_గమ్యం_Bogus();
      తిరిగి ఫలితం;
    }
    appendWithSep(ఫలితం_Remainder, region_str);
  }
  ఒకవేళ (hasVariant) {
    UnicodeString variant_str = variantDisplayName(variant, తాత్కాలిక, నిజం);
    ఒకవేళ (variant_str.isBogus()) {
      ఫలితం.సమితి_గమ్యం_Bogus();
      తిరిగి ఫలితం;
    }
    appendWithSep(ఫలితం_Remainder, variant_str);
  }
  ఫలితం_Remainder.కనుగొను_And_Replace(formatOpenParen, formatReplaceOpenParen);
  ఫలితం_Remainder.కనుగొను_And_Replace(formatCloseParen, formatReplaceCloseParen);

  Local_సూచిక<StringEnumeration> e(loc.createKeywords(స్థితి_2));
  ఒకవేళ (e.isValid() && U_SUCCESS(స్థితి_2)) {
    UnicodeString temp2;
    స్థిరరాశి అక్షరం* కీలకం;
    అయితే ((కీలకం = e->next((int32_t*)శూన్యసూచిక, స్థితి_2)) != శూన్యసూచిక) {
        స్వయంచాలక value = loc.get_Keyword_విలువ<CharString>(కీలకం, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి ఫలితం;
      }
      కీలకం_Display_Name(కీలకం, తాత్కాలిక, నిజం);
      తాత్కాలిక.కనుగొను_And_Replace(formatOpenParen, formatReplaceOpenParen);
      తాత్కాలిక.కనుగొను_And_Replace(formatCloseParen, formatReplaceCloseParen);
      కీలకం_విలువ_Display_Name(కీలకం, value.దత్తాంశం(), temp2, నిజం);
      temp2.కనుగొను_And_Replace(formatOpenParen, formatReplaceOpenParen);
      temp2.కనుగొను_And_Replace(formatCloseParen, formatReplaceCloseParen);
      ఒకవేళ (temp2 != UnicodeString(value.దత్తాంశం(), -1, US_INV)) {
        appendWithSep(ఫలితం_Remainder, temp2);
      } లేకపోతే ఒకవేళ (తాత్కాలిక != UnicodeString(కీలకం, -1, US_INV)) {
        UnicodeString temp3;
        కీలకం_Type_Format.format(తాత్కాలిక, temp2, temp3, స్థితి_2);
        appendWithSep(ఫలితం_Remainder, temp3);
      } లేకపోతే {
        appendWithSep(ఫలితం_Remainder, తాత్కాలిక)
          .append(static_cast<char16_t>(0x3d) /* = */)
          .append(temp2);
      }
    }
  }

  ఒకవేళ (!ఫలితం_Remainder.is_ఖాళీలు()) {
    format.format(ఫలితం_Name, ఫలితం_Remainder, ఫలితం.remove(), స్థితి_2);
    తిరిగి adjustForUsageAndContext(kCapContextUsageLanguage, ఫలితం);
  }

  ఫలితం = ఫలితం_Name;
  తిరిగి adjustForUsageAndContext(kCapContextUsageLanguage, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::appendWithSep(UnicodeString& బఫరు U_LIFETIME_BOUND, స్థిరరాశి UnicodeString& src) స్థిరరాశి {
    ఒకవేళ (బఫరు.is_ఖాళీలు()) {
        బఫరు.సమితి_గమ్యం(src);
    } లేకపోతే {
        స్థిరరాశి UnicodeString *విలువలు[2] = { &బఫరు, &src };
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        separatorFormat.formatAndReplace(విలువలు, 2, బఫరు, శూన్యసూచిక, 0, స్థితి_2);
    }
    తిరిగి బఫరు;
}

UnicodeString&
Locale_Display_పేర్లు_Impl::localeDisplayName(స్థిరరాశి అక్షరం* localeId,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి localeDisplayName(Locale(localeId), ఫలితం);
}

//వ్యక్తిగత
UnicodeString&
Locale_Display_పేర్లు_Impl::localeIdName(స్థిరరాశి అక్షరం* localeId,
                                     UnicodeString& ఫలితం U_LIFETIME_BOUND, తార్కిక substitute) స్థిరరాశి {
    ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
        lang_దత్తాంశం.getNoFallback("Languages%short", localeId, ఫలితం);
        ఒకవేళ (!ఫలితం.isBogus()) {
            తిరిగి ఫలితం;
        }
    }
    lang_దత్తాంశం.getNoFallback("Languages", localeId, ఫలితం);
    ఒకవేళ (ఫలితం.isBogus() && uprv_strchr(localeId, '_') == శూన్యసూచిక) {
        // Canonicalize lang and try again, ICU-20870
        // (only for language codes without script or region)
        Locale canonLocale = Locale::createCanonical(localeId);
        స్థిరరాశి అక్షరం* canonLocId = canonLocale.getName();
        ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
            lang_దత్తాంశం.getNoFallback("Languages%short", canonLocId, ఫలితం);
            ఒకవేళ (!ఫలితం.isBogus()) {
                తిరిగి ఫలితం;
            }
        }
        lang_దత్తాంశం.getNoFallback("Languages", canonLocId, ఫలితం);
    }
    ఒకవేళ (ఫలితం.isBogus() && substitute) {
        // use key, this is what langData.get (with fallback) falls back to.
        ఫలితం.సమితి_గమ్యం(UnicodeString(localeId, -1, US_INV)); // use key (
    }
    తిరిగి ఫలితం;
}

UnicodeString&
Locale_Display_పేర్లు_Impl::languageDisplayName(స్థిరరాశి అక్షరం* lang,
                                            UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    ఒకవేళ (uprv_strcmp("root", lang) == 0 || uprv_strchr(lang, '_') != శూన్యసూచిక) {
        తిరిగి ఫలితం = UnicodeString(lang, -1, US_INV);
    }
    ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
        lang_దత్తాంశం.getNoFallback("Languages%short", lang, ఫలితం);
        ఒకవేళ (!ఫలితం.isBogus()) {
            తిరిగి adjustForUsageAndContext(kCapContextUsageLanguage, ఫలితం);
        }
    }
    lang_దత్తాంశం.getNoFallback("Languages", lang, ఫలితం);
    ఒకవేళ (ఫలితం.isBogus()) {
        // Canonicalize lang and try again, ICU-20870
        Locale canonLocale = Locale::createCanonical(lang);
        స్థిరరాశి అక్షరం* canonLocId = canonLocale.getName();
        ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
            lang_దత్తాంశం.getNoFallback("Languages%short", canonLocId, ఫలితం);
            ఒకవేళ (!ఫలితం.isBogus()) {
                తిరిగి adjustForUsageAndContext(kCapContextUsageLanguage, ఫలితం);
            }
        }
        lang_దత్తాంశం.getNoFallback("Languages", canonLocId, ఫలితం);
    }
    ఒకవేళ (ఫలితం.isBogus() && substitute == UDISPCTX_SUBSTITUTE) {
        // use key, this is what langData.get (with fallback) falls back to.
        ఫలితం.సమితి_గమ్యం(UnicodeString(lang, -1, US_INV)); // use key (
    }
    తిరిగి adjustForUsageAndContext(kCapContextUsageLanguage, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                          తార్కిక skipAdjust) స్థిరరాశి {
    ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
        lang_దత్తాంశం.getNoFallback("Scripts%short", script, ఫలితం);
        ఒకవేళ (!ఫలితం.isBogus()) {
            తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(kCapContextUsageScript, ఫలితం);
        }
    }
    ఒకవేళ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_దత్తాంశం.get("Scripts", script, ఫలితం);
    } లేకపోతే {
        lang_దత్తాంశం.getNoFallback("Scripts", script, ఫలితం);
    }
    తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(kCapContextUsageScript, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి scriptDisplayName(script, ఫలితం, అబద్ధం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::scriptDisplayName(UScript_సంకేతం script_సంకేతం,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి scriptDisplayName(uscript_getName(script_సంకేతం), ఫలితం, అబద్ధం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::regionDisplayName(స్థిరరాశి అక్షరం* region,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                          తార్కిక skipAdjust) స్థిరరాశి {
    ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
         region_దత్తాంశం.getNoFallback("Countries%short", region, ఫలితం);
        ఒకవేళ (!ఫలితం.isBogus()) {
            తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(kCapContextUsageTerritory, ఫలితం);
        }
    }
    ఒకవేళ (substitute == UDISPCTX_SUBSTITUTE) {
        region_దత్తాంశం.get("Countries", region, ఫలితం);
    } లేకపోతే {
        region_దత్తాంశం.getNoFallback("Countries", region, ఫలితం);
    }
    తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(kCapContextUsageTerritory, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::regionDisplayName(స్థిరరాశి అక్షరం* region,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి regionDisplayName(region, ఫలితం, అబద్ధం);
}


UnicodeString&
Locale_Display_పేర్లు_Impl::variantDisplayName(స్థిరరాశి అక్షరం* variant,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                           తార్కిక skipAdjust) స్థిరరాశి {
    // don't have a resource for short variant names
    ఒకవేళ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_దత్తాంశం.get("Variants", variant, ఫలితం);
    } లేకపోతే {
        lang_దత్తాంశం.getNoFallback("Variants", variant, ఫలితం);
    }
    తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(kCapContextUsageVariant, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::variantDisplayName(స్థిరరాశి అక్షరం* variant,
                                           UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి variantDisplayName(variant, ఫలితం, అబద్ధం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                       UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                       తార్కిక skipAdjust) స్థిరరాశి {
    // don't have a resource for short key names
    ఒకవేళ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_దత్తాంశం.get("Keys", కీలకం, ఫలితం);
    } లేకపోతే {
        lang_దత్తాంశం.getNoFallback("Keys", కీలకం, ఫలితం);
    }
    తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(k_Cap_Context_Usage_కీలకం, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                       UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి కీలకం_Display_Name(కీలకం, ఫలితం, అబద్ధం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                            స్థిరరాశి అక్షరం* value,
                                            UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                            తార్కిక skipAdjust) స్థిరరాశి {
    ఒకవేళ (uprv_strcmp(కీలకం, "currency") == 0) {
        // ICU4C does not have ICU4J CurrencyDisplayInfo equivalent for now.
        UError_సంకేతం sts = U_ZERO_ERROR;
        UnicodeString ustr_విలువ(value, -1, US_INV);
        int32_t len;
        స్థిరరాశి char16_t *currencyName = ucurr_getName(ustr_విలువ.get_Terminated_బఫరు(),
            locale.get_పునాది_Name(), UCURR_LONG_NAME, శూన్యసూచిక /* isChoiceFormat */, &len, &sts);
        ఒకవేళ (U_FAILURE(sts)) {
            // Return the value as is on failure
            ఫలితం = ustr_విలువ;
            తిరిగి ఫలితం;
        }
        ఫలితం.సమితి_గమ్యం(currencyName, len);
        తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(k_Cap_Context_Usage_కీలకం_విలువ, ఫలితం);
    }

    ఒకవేళ (name_నిడివి == UDISPCTX_నిడివి_SHORT) {
        lang_దత్తాంశం.getNoFallback("Types%short", కీలకం, value, ఫలితం);
        ఒకవేళ (!ఫలితం.isBogus()) {
            తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(k_Cap_Context_Usage_కీలకం_విలువ, ఫలితం);
        }
    }
    ఒకవేళ (substitute == UDISPCTX_SUBSTITUTE) {
        lang_దత్తాంశం.get("Types", కీలకం, value, ఫలితం);
    } లేకపోతే {
        lang_దత్తాంశం.getNoFallback("Types", కీలకం, value, ఫలితం);
    }
    తిరిగి skipAdjust? ఫలితం: adjustForUsageAndContext(k_Cap_Context_Usage_కీలకం_విలువ, ఫలితం);
}

UnicodeString&
Locale_Display_పేర్లు_Impl::కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                            స్థిరరాశి అక్షరం* value,
                                            UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి {
    తిరిగి కీలకం_విలువ_Display_Name(కీలకం, value, ఫలితం, అబద్ధం);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale_Display_పేర్లు*
Locale_Display_పేర్లు::createInstance(స్థిరరాశి Locale& locale,
                                   UDialectHandling dialectHandling) {
    తిరిగి కొత్త Locale_Display_పేర్లు_Impl(locale, dialectHandling);
}

Locale_Display_పేర్లు*
Locale_Display_పేర్లు::createInstance(స్థిరరాశి Locale& locale,
                                   UDisplayContext *contexts, int32_t length) {
    ఒకవేళ (contexts == శూన్యసూచిక) {
        length = 0;
    }
    తిరిగి కొత్త Locale_Display_పేర్లు_Impl(locale, contexts, length);
}

U_NAMESPACE_చివర

////////////////////////////////////////////////////////////////////////////////////////////////////

U_NAMESPACE_USE

U_CAPI ULocale_Display_పేర్లు * U_EXPORT2
uldn_open(స్థిరరాశి అక్షరం * locale,
          UDialectHandling dialectHandling,
          UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ (locale == శూన్యసూచిక) {
    locale = uloc_getDefault();
  }
  తిరిగి (ULocale_Display_పేర్లు *)Locale_Display_పేర్లు::createInstance(Locale(locale), dialectHandling);
}

U_CAPI ULocale_Display_పేర్లు * U_EXPORT2
uldn_openForContext(స్థిరరాశి అక్షరం * locale,
                    UDisplayContext *contexts, int32_t length,
                    UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ (locale == శూన్యసూచిక) {
    locale = uloc_getDefault();
  }
  తిరిగి (ULocale_Display_పేర్లు *)Locale_Display_పేర్లు::createInstance(Locale(locale), contexts, length);
}


U_CAPI శూన్య U_EXPORT2
uldn_close(ULocale_Display_పేర్లు *ldn) {
  తొలగించు (Locale_Display_పేర్లు *)ldn;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uldn_getLocale(స్థిరరాశి ULocale_Display_పేర్లు* ldn U_LIFETIME_BOUND) {
  ఒకవేళ (ldn) {
    తిరిగి ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->getLocale().getName();
  }
  తిరిగి శూన్యసూచిక;
}

U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(స్థిరరాశి ULocale_Display_పేర్లు *ldn) {
  ఒకవేళ (ldn) {
    తిరిగి ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->getDialectHandling();
  }
  తిరిగి ULDN_STANDARD_పేర్లు;
}

U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
              UDisplayContextType type,
              UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి (UDisplayContext)0;
  }
  తిరిగి ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->getContext(type);
}

U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *locale,
                       char16_t *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || locale == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->localeDisplayName(locale, తాత్కాలిక);
  ఒకవేళ (తాత్కాలిక.isBogus()) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                         స్థిరరాశి అక్షరం *lang,
                         char16_t *ఫలితం,
                         int32_t max_ఫలితం_కొలత,
                         UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || lang == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->languageDisplayName(lang, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *script,
                       char16_t *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || script == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->scriptDisplayName(script, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_script_సంకేతం_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                           UScript_సంకేతం script_సంకేతం,
                           char16_t *ఫలితం,
                           int32_t max_ఫలితం_కొలత,
                           UError_సంకేతం *p_Error_సంకేతం) {
  తిరిగి uldn_scriptDisplayName(ldn, uscript_getName(script_సంకేతం), ఫలితం, max_ఫలితం_కొలత, p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *region,
                       char16_t *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || region == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->regionDisplayName(region, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                        స్థిరరాశి అక్షరం *variant,
                        char16_t *ఫలితం,
                        int32_t max_ఫలితం_కొలత,
                        UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || variant == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->variantDisplayName(variant, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_కీలకం_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                    స్థిరరాశి అక్షరం *కీలకం,
                    char16_t *ఫలితం,
                    int32_t max_ఫలితం_కొలత,
                    UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || కీలకం == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0) || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->కీలకం_Display_Name(కీలకం, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uldn_కీలకం_విలువ_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                         స్థిరరాశి అక్షరం *కీలకం,
                         స్థిరరాశి అక్షరం *value,
                         char16_t *ఫలితం,
                         int32_t max_ఫలితం_కొలత,
                         UError_సంకేతం *p_Error_సంకేతం) {
  ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
    తిరిగి 0;
  }
  ఒకవేళ (ldn == శూన్యసూచిక || కీలకం == శూన్యసూచిక || value == శూన్యసూచిక || (ఫలితం == శూన్యసూచిక && max_ఫలితం_కొలత > 0)
      || max_ఫలితం_కొలత < 0) {
    *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  UnicodeString తాత్కాలిక(ఫలితం, 0, max_ఫలితం_కొలత);
  ((స్థిరరాశి Locale_Display_పేర్లు *)ldn)->కీలకం_విలువ_Display_Name(కీలకం, value, తాత్కాలిక);
  తిరిగి తాత్కాలిక.extract(ఫలితం, max_ఫలితం_కొలత, *p_Error_సంకేతం);
}

#అయితే_ముగింపు
