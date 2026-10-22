// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ulocale.h"
#अन्तर्भाव "unicode/locid.h"

#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<स्थिरांक icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(स्थिरांक अक्षर* localeID, int32_t length, UErrorCode* err) {
    जर (U_FAILURE(*err)) { परत nullptr; }
    जर (length < 0) {
        परत EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    परत EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(स्थिरांक अक्षर* tag, int32_t length, UErrorCode* err) {
  जर (U_FAILURE(*err)) { परत nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  जर (U_FAILURE(*err)) { परत nullptr; }
  परत EXTERNAL(l.clone());
}

शून्य
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
स्थिरांक अक्षर* ulocale_get ## N1(स्थिरांक ULocale* locale U_LIFETIME_BOUND) { \
    जर (locale == nullptr) परत nullptr; \
    परत CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_मूल्य(N) \
int32_t ulocale_get ##N ( \
    स्थिरांक ULocale* locale, स्थिरांक अक्षर* keyword, int32_t keywordLength, \
    अक्षर* मूल्य_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    जर (U_FAILURE(*err)) परत 0; \
    जर (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        परत 0; \
    } \
    परत ByteSinkUtil::viaByteSinkToTerminatedChars( \
        मूल्य_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(स्थिरांक ULocale* locale, UErrorCode *err) { \
    जर (U_FAILURE(*err)) परत nullptr; \
    जर (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        परत nullptr; \
    } \
    परत uenum_openFromStringEnumeration( \
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

तार्किक ulocale_isBogus(स्थिरांक ULocale* locale) {
    जर (locale == nullptr) परत असत्य;
    परत CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
