// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/errorcode.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/ulocale.h"
#અન્તર્ભાવ "unicode/locid.h"

#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<અચલ icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(અચલ અક્ષર* localeID, int32_t length, UErrorCode* err) {
    જો (U_FAILURE(*err)) { પાછા nullptr; }
    જો (length < 0) {
        પાછા EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    પાછા EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(અચલ અક્ષર* tag, int32_t length, UErrorCode* err) {
  જો (U_FAILURE(*err)) { પાછા nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  જો (U_FAILURE(*err)) { પાછા nullptr; }
  પાછા EXTERNAL(l.clone());
}

શૂન્ય
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
અચલ અક્ષર* ulocale_get ## N1(અચલ ULocale* locale U_LIFETIME_BOUND) { \
    જો (locale == nullptr) પાછા nullptr; \
    પાછા CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_કિંમત(N) \
int32_t ulocale_get ##N ( \
    અચલ ULocale* locale, અચલ અક્ષર* keyword, int32_t keywordLength, \
    અક્ષર* કિંમત_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    જો (U_FAILURE(*err)) પાછા 0; \
    જો (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        પાછા 0; \
    } \
    પાછા ByteSinkUtil::viaByteSinkToTerminatedChars( \
        કિંમત_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(અચલ ULocale* locale, UErrorCode *err) { \
    જો (U_FAILURE(*err)) પાછા nullptr; \
    જો (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        પાછા nullptr; \
    } \
    પાછા uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_કિંમત(Keyword_કિંમત)
IMPL_ULOCALE_GET_KEYWORD_કિંમત(Unicode_Keyword_કિંમત)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

તાર્કિક ulocale_isBogus(અચલ ULocale* locale) {
    જો (locale == nullptr) પાછા ખોટું;
    પાછા CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
