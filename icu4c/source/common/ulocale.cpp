// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/errorcode.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ulocale.h"
#అన్తర్భావ "unicode/locid.h"

#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_USE
#నిర్వచించు EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#నిర్వచించు CONST_అంతర్గత(e) (reinterpret_cast<స్థిరరాశి icu::Locale*>(e))
#నిర్వచించు అంతర్గత(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(స్థిరరాశి అక్షరం* localeID, int32_t length, UError_సంకేతం* err) {
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (length < 0) {
        తిరిగి EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    తిరిగి EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_ట్యాగ్(స్థిరరాశి అక్షరం* ట్యాగ్, int32_t length, UError_సంకేతం* err) {
  ఒకవేళ (U_FAILURE(*err)) { తిరిగి శూన్యసూచిక; }
  Locale l = icu::Locale::for_Language_ట్యాగ్(length < 0 ? StringPiece(ట్యాగ్) : StringPiece(ట్యాగ్, length), *err);
  ఒకవేళ (U_FAILURE(*err)) { తిరిగి శూన్యసూచిక; }
  తిరిగి EXTERNAL(l.clone());
}

శూన్య
ulocale_close(ULocale* locale) {
    తొలగించు అంతర్గత(locale);
}

#నిర్వచించు IMPL_ULOCALE_STRING_GETTER(N1, N2) \
స్థిరరాశి అక్షరం* ulocale_get ## N1(స్థిరరాశి ULocale* locale U_LIFETIME_BOUND) { \
    ఒకవేళ (locale == శూన్యసూచిక) తిరిగి శూన్యసూచిక; \
    తిరిగి CONST_అంతర్గత(locale)->get ## N2(); \
}

#నిర్వచించు IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#నిర్వచించు IMPL_ULOCALE_GET_KEYWORD_విలువ(N) \
int32_t ulocale_get ##N ( \
    స్థిరరాశి ULocale* locale, స్థిరరాశి అక్షరం* keyword, int32_t keyword_నిడివి, \
    అక్షరం* విలువ_బఫరు, int32_t బఫరు_సామర్థ్యం, UError_సంకేతం *err) { \
    ఒకవేళ (U_FAILURE(*err)) తిరిగి 0; \
    ఒకవేళ (locale == శూన్యసూచిక) { \
        *err = U_ILLEGAL_వాదం_ERROR; \
        తిరిగి 0; \
    } \
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars( \
        విలువ_బఫరు, బఫరు_సామర్థ్యం, \
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) { \
            CONST_అంతర్గత(locale)->get ## N( \
                keyword_నిడివి < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_నిడివి), \
                sink, స్థితి_2); \
        }, \
        *err); \
}

#నిర్వచించు IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(స్థిరరాశి ULocale* locale, UError_సంకేతం *err) { \
    ఒకవేళ (U_FAILURE(*err)) తిరిగి శూన్యసూచిక; \
    ఒకవేళ (locale == శూన్యసూచిక) { \
        *err = U_ILLEGAL_వాదం_ERROR; \
        తిరిగి శూన్యసూచిక; \
    } \
    తిరిగి uenum_openFromStringEnumeration( \
        CONST_అంతర్గత(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(పునాది_Name_2)
IMPL_ULOCALE_GET_KEYWORD_విలువ(Keyword_విలువ)
IMPL_ULOCALE_GET_KEYWORD_విలువ(Unicode_Keyword_విలువ)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

తార్కిక ulocale_isBogus(స్థిరరాశి ULocale* locale) {
    ఒకవేళ (locale == శూన్యసూచిక) తిరిగి అబద్ధం;
    తిరిగి CONST_అంతర్గత(locale)->isBogus();
}

/*eof*/
