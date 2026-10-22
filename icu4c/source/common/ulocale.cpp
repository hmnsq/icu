// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/errorcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ulocale.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_USE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONST_ਅੰਦਰੂਨੀ(e) (reinterpret_cast<ਅਚਲ icu::Locale*>(e))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਦਰੂਨੀ(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ਅਚਲ ਅੱਖਰ* localeID, int32_t length, UError_ਕੋਡ* err) {
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (length < 0) {
        ਵਾਪਸ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ਵਾਪਸ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_ਟੈਗ(ਅਚਲ ਅੱਖਰ* ਟੈਗ, int32_t length, UError_ਕੋਡ* err) {
  ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
  Locale l = icu::Locale::for_Language_ਟੈਗ(length < 0 ? StringPiece(ਟੈਗ) : StringPiece(ਟੈਗ, length), *err);
  ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
  ਵਾਪਸ EXTERNAL(l.clone());
}

ਖਾਲੀ
ulocale_close(ULocale* locale) {
    ਮਿਟਾਓ ਅੰਦਰੂਨੀ(locale);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ਅਚਲ ਅੱਖਰ* ulocale_get ## N1(ਅਚਲ ULocale* locale U_LIFETIME_BOUND) { \
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; \
    ਵਾਪਸ CONST_ਅੰਦਰੂਨੀ(locale)->get ## N2(); \
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPL_ULOCALE_GET_KEYWORD_ਮੁੱਲ(N) \
int32_t ulocale_get ##N ( \
    ਅਚਲ ULocale* locale, ਅਚਲ ਅੱਖਰ* keyword, int32_t keyword_ਲੰਮਾਈ, \
    ਅੱਖਰ* ਮੁੱਲ_ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ, UError_ਕੋਡ *err) { \
    ਜੇ (U_FAILURE(*err)) ਵਾਪਸ 0; \
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) { \
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR; \
        ਵਾਪਸ 0; \
    } \
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars( \
        ਮੁੱਲ_ਬਫਰ, ਬਫਰ_ਸਮਰੱਥਾ, \
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) { \
            CONST_ਅੰਦਰੂਨੀ(locale)->get ## N( \
                keyword_ਲੰਮਾਈ < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_ਲੰਮਾਈ), \
                sink, ਸਥਿਤੀ); \
        }, \
        *err); \
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ਅਚਲ ULocale* locale, UError_ਕੋਡ *err) { \
    ਜੇ (U_FAILURE(*err)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; \
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) { \
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR; \
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; \
    } \
    ਵਾਪਸ uenum_openFromStringEnumeration( \
        CONST_ਅੰਦਰੂਨੀ(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(ਅਧਾਰ_Name_2)
IMPL_ULOCALE_GET_KEYWORD_ਮੁੱਲ(Keyword_ਮੁੱਲ)
IMPL_ULOCALE_GET_KEYWORD_ਮੁੱਲ(Unicode_Keyword_ਮੁੱਲ)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

ਤਾਰਕਿਕ ulocale_isBogus(ਅਚਲ ULocale* locale) {
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ ਝੂਠ;
    ਵਾਪਸ CONST_ਅੰਦਰੂਨੀ(locale)->isBogus();
}

/*eof*/
