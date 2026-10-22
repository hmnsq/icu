// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/errorcode.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ulocale.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ಧ್ರುವ icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ಧ್ರುವ ವರ್ಣ* localeID, int32_t length, UErrorCode* err) {
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (length < 0) {
        ಪ್ರತ್ಯಾಗಮ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ಪ್ರತ್ಯಾಗಮ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ಧ್ರುವ ವರ್ಣ* tag, int32_t length, UErrorCode* err) {
  ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
  ಪ್ರತ್ಯಾಗಮ EXTERNAL(l.clone());
}

ಶೂನ್ಯ
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ಧ್ರುವ ವರ್ಣ* ulocale_get ## N1(ಧ್ರುವ ULocale* locale U_LIFETIME_BOUND) { \
    ಯದಿ (locale == nullptr) ಪ್ರತ್ಯಾಗಮ nullptr; \
    ಪ್ರತ್ಯಾಗಮ CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(N) \
int32_t ulocale_get ##N ( \
    ಧ್ರುವ ULocale* locale, ಧ್ರುವ ವರ್ಣ* keyword, int32_t keywordLength, \
    ವರ್ಣ* ಮೌಲ್ಯ_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    ಯದಿ (U_FAILURE(*err)) ಪ್ರತ್ಯಾಗಮ 0; \
    ಯದಿ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ಪ್ರತ್ಯಾಗಮ 0; \
    } \
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars( \
        ಮೌಲ್ಯ_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ಧ್ರುವ ULocale* locale, UErrorCode *err) { \
    ಯದಿ (U_FAILURE(*err)) ಪ್ರತ್ಯಾಗಮ nullptr; \
    ಯದಿ (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ಪ್ರತ್ಯಾಗಮ nullptr; \
    } \
    ಪ್ರತ್ಯಾಗಮ uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(Keyword_ಮೌಲ್ಯ)
IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(Unicode_Keyword_ಮೌಲ್ಯ)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

ತಾರ್ಕಿಕ ulocale_isBogus(ಧ್ರುವ ULocale* locale) {
    ಯದಿ (locale == nullptr) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
