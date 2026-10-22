// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/errorcode.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ulocale.h"
#അന്തര്ഭാവ "unicode/locid.h"

#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<സ്ഥിരം icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(സ്ഥിരം അക്ഷരം* localeID, int32_t length, UErrorCode* err) {
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ nullptr; }
    എങ്കിൽ (length < 0) {
        തിരികെ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    തിരികെ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(സ്ഥിരം അക്ഷരം* tag, int32_t length, UErrorCode* err) {
  എങ്കിൽ (U_FAILURE(*err)) { തിരികെ nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  എങ്കിൽ (U_FAILURE(*err)) { തിരികെ nullptr; }
  തിരികെ EXTERNAL(l.clone());
}

ശൂന്യം
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
സ്ഥിരം അക്ഷരം* ulocale_get ## N1(സ്ഥിരം ULocale* locale U_LIFETIME_BOUND) { \
    എങ്കിൽ (locale == nullptr) തിരികെ nullptr; \
    തിരികെ CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_മൂല്യം(N) \
int32_t ulocale_get ##N ( \
    സ്ഥിരം ULocale* locale, സ്ഥിരം അക്ഷരം* keyword, int32_t keywordLength, \
    അക്ഷരം* മൂല്യം_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    എങ്കിൽ (U_FAILURE(*err)) തിരികെ 0; \
    എങ്കിൽ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        തിരികെ 0; \
    } \
    തിരികെ ByteSinkUtil::viaByteSinkToTerminatedChars( \
        മൂല്യം_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(സ്ഥിരം ULocale* locale, UErrorCode *err) { \
    എങ്കിൽ (U_FAILURE(*err)) തിരികെ nullptr; \
    എങ്കിൽ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        തിരികെ nullptr; \
    } \
    തിരികെ uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_മൂല്യം(Keyword_മൂല്യം)
IMPL_ULOCALE_GET_KEYWORD_മൂല്യം(Unicode_Keyword_മൂല്യം)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

തർക്കം ulocale_isBogus(സ്ഥിരം ULocale* locale) {
    എങ്കിൽ (locale == nullptr) തിരികെ അസത്യം;
    തിരികെ CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
