// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#समावेश "unicode/bytestream.h"
#समावेश "unicode/errorcode.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ulocale.h"
#समावेश "unicode/locid.h"

#समावेश "bytesinkutil.h"
#समावेश "cmemory.h"

U_NAMESPACE_USE
#घोषणा EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#घोषणा CONST_INTERNAL(e) (reinterpret_cast<स्थिर icu::Locale*>(e))
#घोषणा INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(स्थिर अक्षर* localeID, int32_t length, UErrorCode* err) {
    अगर (U_FAILURE(*err)) { वापस nullptr; }
    अगर (length < 0) {
        वापस EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    वापस EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(स्थिर अक्षर* tag, int32_t length, UErrorCode* err) {
  अगर (U_FAILURE(*err)) { वापस nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  अगर (U_FAILURE(*err)) { वापस nullptr; }
  वापस EXTERNAL(l.clone());
}

व्योम
ulocale_close(ULocale* locale) {
    मिटाओ INTERNAL(locale);
}

#घोषणा IMPL_ULOCALE_STRING_GETTER(N1, N2) \
स्थिर अक्षर* ulocale_get ## N1(स्थिर ULocale* locale U_LIFETIME_BOUND) { \
    अगर (locale == nullptr) वापस nullptr; \
    वापस CONST_INTERNAL(locale)->लो ## N2(); \
}

#घोषणा IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#घोषणा IMPL_ULOCALE_GET_KEYWORD_मूल्य(N) \
int32_t ulocale_get ##N ( \
    स्थिर ULocale* locale, स्थिर अक्षर* keyword, int32_t keyword_लंबाई, \
    अक्षर* मूल्य_बफर, int32_t बफर_Capacity, UErrorCode *err) { \
    अगर (U_FAILURE(*err)) वापस 0; \
    अगर (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        वापस 0; \
    } \
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars( \
        मूल्य_बफर, बफर_Capacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->लो ## N( \
                keyword_लंबाई < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_लंबाई), \
                sink, status); \
        }, \
        *err); \
}

#घोषणा IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(स्थिर ULocale* locale, UErrorCode *err) { \
    अगर (U_FAILURE(*err)) वापस nullptr; \
    अगर (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        वापस nullptr; \
    } \
    वापस uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_मूल्य(Keyword_मूल्य)
IMPL_ULOCALE_GET_KEYWORD_मूल्य(Unicode_Keyword_मूल्य)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

bool ulocale_isBogus(स्थिर ULocale* locale) {
    अगर (locale == nullptr) वापस false;
    वापस CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
