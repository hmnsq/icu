// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ضمن "unicode/bytestream.h"
#ضمن "unicode/errorcode.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ulocale.h"
#ضمن "unicode/locid.h"

#ضمن "bytesinkutil.h"
#ضمن "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UErrorCode* err) {
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ nullptr; }
    ܐܢ (length < 0) {
        ܦܢܐ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ܦܢܐ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ثابت حرف* tag, int32_t length, UErrorCode* err) {
  ܐܢ (U_FAILURE(*err)) { ܦܢܐ nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  ܐܢ (U_FAILURE(*err)) { ܦܢܐ nullptr; }
  ܦܢܐ EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    ܐܢ (locale == nullptr) ܦܢܐ nullptr; \
    ܦܢܐ CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_VALUE(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keywordLength, \
    حرف* valueBuffer, int32_t bufferCapacity, UErrorCode *err) { \
    ܐܢ (U_FAILURE(*err)) ܦܢܐ 0; \
    ܐܢ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ܦܢܐ 0; \
    } \
    ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars( \
        valueBuffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UErrorCode *err) { \
    ܐܢ (U_FAILURE(*err)) ܦܢܐ nullptr; \
    ܐܢ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ܦܢܐ nullptr; \
    } \
    ܦܢܐ uenum_openFromStringEnumeration( \
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

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    ܐܢ (locale == nullptr) ܦܢܐ ܕܓܠܐ;
    ܦܢܐ CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
