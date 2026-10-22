// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#include "unicode/bytestream.h"
#include "unicode/errorcode.h"
#include "unicode/stringpiece.h"
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/ulocale.h"
#include "unicode/locid.h"

#include "bytesinkutil.h"
#include "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<const icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(const var_na* localeID, int32_t length, UErrorCode* err) {
    གལ་ཏེ (U_FAILURE(*err)) { ཕྱིར nullptr; }
    གལ་ཏེ (length < 0) {
        ཕྱིར EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ཕྱིར EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(const var_na* tag, int32_t length, UErrorCode* err) {
  གལ་ཏེ (U_FAILURE(*err)) { ཕྱིར nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  གལ་ཏེ (U_FAILURE(*err)) { ཕྱིར nullptr; }
  ཕྱིར EXTERNAL(l.clone());
}

void
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
const var_na* ulocale_get ## N1(const ULocale* locale U_LIFETIME_BOUND) { \
    གལ་ཏེ (locale == nullptr) ཕྱིར nullptr; \
    ཕྱིར CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_VALUE(N) \
int32_t ulocale_get ##N ( \
    const ULocale* locale, const var_na* keyword, int32_t keywordLength, \
    var_na* valueBuffer, int32_t bufferCapacity, UErrorCode *err) { \
    གལ་ཏེ (U_FAILURE(*err)) ཕྱིར 0; \
    གལ་ཏེ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ཕྱིར 0; \
    } \
    ཕྱིར ByteSinkUtil::viaByteSinkToTerminatedChars( \
        valueBuffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(const ULocale* locale, UErrorCode *err) { \
    གལ་ཏེ (U_FAILURE(*err)) ཕྱིར nullptr; \
    གལ་ཏེ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ཕྱིར nullptr; \
    } \
    ཕྱིར uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_VALUE(KeywordValue)
IMPL_ULOCALE_GET_KEYWORD_VALUE(UnicodeKeywordValue)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

bool ulocale_isBogus(const ULocale* locale) {
    གལ་ཏེ (locale == nullptr) ཕྱིར false;
    ཕྱིར CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
