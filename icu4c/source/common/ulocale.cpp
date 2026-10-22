// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/errorcode.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ulocale.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"

#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ଅଚଳ icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ଅଚଳ ଅକ୍ଷର* localeID, int32_t length, UErrorCode* err) {
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (length < 0) {
        ଫେରନ୍ତୁ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ଫେରନ୍ତୁ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ଅଚଳ ଅକ୍ଷର* tag, int32_t length, UErrorCode* err) {
  ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ nullptr; }
  ଫେରନ୍ତୁ EXTERNAL(l.clone());
}

ଶୂନ୍ୟ
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ଅଚଳ ଅକ୍ଷର* ulocale_get ## N1(ଅଚଳ ULocale* locale U_LIFETIME_BOUND) { \
    ଯଦି (locale == nullptr) ଫେରନ୍ତୁ nullptr; \
    ଫେରନ୍ତୁ CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_ମୂଲ୍ୟ(N) \
int32_t ulocale_get ##N ( \
    ଅଚଳ ULocale* locale, ଅଚଳ ଅକ୍ଷର* keyword, int32_t keywordLength, \
    ଅକ୍ଷର* ମୂଲ୍ୟ_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    ଯଦି (U_FAILURE(*err)) ଫେରନ୍ତୁ 0; \
    ଯଦି (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ଫେରନ୍ତୁ 0; \
    } \
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars( \
        ମୂଲ୍ୟ_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ଅଚଳ ULocale* locale, UErrorCode *err) { \
    ଯଦି (U_FAILURE(*err)) ଫେରନ୍ତୁ nullptr; \
    ଯଦି (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ଫେରନ୍ତୁ nullptr; \
    } \
    ଫେରନ୍ତୁ uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_ମୂଲ୍ୟ(Keyword_ମୂଲ୍ୟ)
IMPL_ULOCALE_GET_KEYWORD_ମୂଲ୍ୟ(Unicode_Keyword_ମୂଲ୍ୟ)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

ତାର୍କିକ ulocale_isBogus(ଅଚଳ ULocale* locale) {
    ଯଦି (locale == nullptr) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ଫେରନ୍ତୁ CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
